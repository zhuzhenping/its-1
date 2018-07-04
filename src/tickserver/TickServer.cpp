#include "TickServer.h"
#include <algorithm>
#include <common/AppLog.h>
#include "datalib/SymbolInfoSet.h"


void SessionContainer::Append(TcpSession* sock)
{
	if (tick_){
		Locker lock(&tick_mutex_);
		sock->Send((char*)tick_, sizeof(FutureTick));		
	}

	{
		Locker locker(&sub_sess_mutex_);
		std::list<TcpSession*>::iterator iter = std::find(sub_sessions_.begin(), sub_sessions_.end(), sock);
		if (iter == sub_sessions_.end())
			sub_sessions_.push_back(sock);
	}
	
}

void SessionContainer::Remove(TcpSession* sock)
{
	Locker locker(&sub_sess_mutex_);
	std::list<TcpSession*>::iterator iter = std::find(sub_sessions_.begin(), sub_sessions_.end(), sock);
	if (iter != sub_sessions_.end())
	{
		sub_sessions_.erase(iter);
	}
}

void SessionContainer::SetTick(FutureTick* tick) 
{
	Locker locker(&tick_mutex_);
	if (NULL != tick_) { free(tick_); }
	tick_ = tick;
}

FutureTick* SessionContainer::GetTick() {
	Locker locker(&tick_mutex_);
	if (tick_){
		return new FutureTick(*tick_);
	}
	else
		return NULL;
}

void SessionContainer::Send(char* buf, int len)
{
	Locker locker(&sub_sess_mutex_);
	std::list<TcpSession*>::iterator iter;
	for (iter = sub_sessions_.begin(); iter != sub_sessions_.end(); ++iter)
	{
		(*iter)->Send(buf, len);
	}
}


/*

void SessionContainer::SendTick(TcpSession* sock) 
{ 
	Locker locker(&tick_mutex_);
	if (NULL != tick_)
	{

		sock->Send((char*)tick_, sizeof(FutureTick));
	}
}
*/

/////////////////////////////////////////////////////////////////////////////////////////

TickServer::TickServer(int port) : sub_sym_sessions_(NULL)
{
	tcp_server_ = new TcpServer(port, this, this);
	
	sub_sym_sessions_ = new Symbol2Sessions();
}

TickServer::~TickServer(void)
{
	delete tcp_server_;

	
	if (NULL != sub_sym_sessions_) { delete sub_sym_sessions_; }
}

bool TickServer::Init(std::string& err)
{
	SymbolInfoSet* sym_info = SymbolInfoSet::Instance();

	const std::vector<Symbol>& futures = sym_info->FutureSymbols();
	for (int i=0; i<futures.size(); ++i)
	{
		SessionContainer** sess = sub_sym_sessions_->Data(futures[i]);
		if (sess == NULL) 
		{ 
			APP_LOG(LOG_LEVEL_WARN) << "InitContainer error: " << futures[i].Str() << " not in container";
			continue; 
		}
		*sess = new SessionContainer();
	}
	return true;
}

bool TickServer::StartUp(std::string& err)
{
	Locker locker(&sub_sym_sessions_mutex_);
	SymbolInfoSet* sym_info = SymbolInfoSet::Instance();

	Symbol2Sessions* tmp_container = new Symbol2Sessions();
	const std::vector<Symbol>& futures = sym_info->FutureSymbols();
	for (int i=0; i<futures.size(); ++i)
	{
		SessionContainer** sess = tmp_container->Data(futures[i]);
		if (sess == NULL) { continue; }
		SessionContainer** pre_tick_reg = sub_sym_sessions_->Data(futures[i]);
		if (pre_tick_reg != NULL && *pre_tick_reg != NULL)
		{
			*sess = *pre_tick_reg;
			*pre_tick_reg = NULL;
		}
		else
		{
			*sess = new SessionContainer();
		}
	}
	sub_sym_sessions_->Clear();
	delete sub_sym_sessions_;
	sub_sym_sessions_ = tmp_container;

	return true;
}

void TickServer::OnAccept(TcpSession* sock)
{
	Locker locker(&sess_symbols_mutex);
	sess_symbols_[sock] = std::list<Symbol>();
}

SessionContainer* TickServer::GetSessionContainer(const Symbol& sym)
{
	Locker locker(&sub_sym_sessions_mutex_);	

	SessionContainer** sess = sub_sym_sessions_->Data(sym);
	if (sess == NULL || *sess == NULL) { return NULL; }
	return *sess;
}

void TickServer::OnReceive(TcpSession *tcp_sock, char* buf, int len)
{
	if (len <= sizeof(ProtocolHead)) { return; }

	ProtocolHead* head = (ProtocolHead*)buf;
	switch (head->type)
	{
	case REQ_RUN_TICK:
		{
			if (len <= sizeof(RunTickRequt)) { return; }
			RunTickRequt* run_tick_req = (RunTickRequt*)buf;
			if (run_tick_req->num <= 0 || len != run_tick_req->num * sizeof(Symbol)+ sizeof(RunTickRequt)) 
			{ 
				return; 
			}

			Symbol *symbols = (Symbol*)(buf  + sizeof(RunTickRequt));
			if (run_tick_req->is_sub)
			{
				for (int i=0; i < run_tick_req->num; ++i)
				{
					Subscribe(tcp_sock, symbols[i]);
				}
			} 
			else
			{
				for (int i=0; i < run_tick_req->num; ++i)
				{
					UnSubscribe(tcp_sock, symbols[i]);
				}
			}

		}

	default:
		break;
	}
}


void TickServer::OnDiscon(TcpSession *tcp_sock)
{
	Locker locker(&sess_symbols_mutex);
	Session2Symbols::iterator iter = sess_symbols_.find(tcp_sock);
	if (iter != sess_symbols_.end()) {
		list<Symbol> &sym_list = iter->second;
		for (list<Symbol>::iterator sym_iter = sym_list.begin(); sym_iter!=sym_list.end(); ++sym_iter){
			SessionContainer* sess_cont = GetSessionContainer(*sym_iter);
			if(sess_cont)sess_cont->Remove(tcp_sock);
		}
		sess_symbols_.erase(iter);
	}

	std::list<TcpSession*>::iterator sock_iter = std::find(all_symbol_sessions_.begin(), all_symbol_sessions_.end(), tcp_sock);
	if (sock_iter != all_symbol_sessions_.end())
	{
		all_symbol_sessions_.erase(sock_iter);
	}
}

void TickServer::Subscribe(TcpSession *tcp_sock, const Symbol& sym)
{
	Locker locker(&sess_symbols_mutex);
	Session2Symbols::iterator iter = sess_symbols_.find(tcp_sock);
	if (iter == sess_symbols_.end()) { return; }	//未连接的socket

	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter = std::find(sym_list.begin(), sym_list.end(), sym);
	if (sym_iter != sym_list.end()) { return; }  //不重复订阅.

	SessionContainer* sess = GetSessionContainer(sym);
	if (sess == NULL) { return; }

	sess->Append(tcp_sock);
	sym_list.push_back(sym);
}

void TickServer::UnSubscribe(TcpSession *tcp_sock, const Symbol& sym)
{
	Locker locker(&sess_symbols_mutex);
	Session2Symbols::iterator iter = sess_symbols_.find(tcp_sock);
	if (iter == sess_symbols_.end()) { return; }	//未连接的socket

	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter = std::find(sym_list.begin(), sym_list.end(), sym);
	if (sym_iter == sym_list.end()) { return; }  //未订阅
	sym_list.erase(sym_iter);

	SessionContainer* sess = GetSessionContainer(sym);
	if (sess)sess->Remove(tcp_sock);
	
}

int TickServer::SendTick(const Symbol& sym, char* buf, int len)
{
	Locker locker(&sess_symbols_mutex);
	SessionContainer* sess = GetSessionContainer(sym);
	if (sess == NULL) 
	{ 
		APP_LOG(LOG_LEVEL_WARN) << "unknow symbol: " << sym.Str();
		return -1; 
	}

	FutureTick* tick = (FutureTick*)malloc(len);
	memcpy(tick, buf, len);
	sess->SetTick(tick);

	// 向订阅了该合约行情的客户端推送过去.
	sess->Send(buf, len);

	return 0;
}


