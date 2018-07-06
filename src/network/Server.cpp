#include "network/Server.h"
#include <algorithm>
#include <common/AppLog.h>


void SessionContainer::Append(TcpSession* sock)
{
	Locker locker(&sub_sess_mutex_);
	std::list<TcpSession*>::iterator iter = std::find(sub_sessions_.begin(), sub_sessions_.end(), sock);
	if (iter == sub_sessions_.end())
		sub_sessions_.push_back(sock);
	
}

void SessionContainer::Remove(TcpSession* sock)
{
	Locker locker(&sub_sess_mutex_);
	std::list<TcpSession*>::iterator iter = std::find(sub_sessions_.begin(), sub_sessions_.end(), sock);
	if (iter != sub_sessions_.end())
		sub_sessions_.erase(iter);
}

void SessionContainer::Send(char* buf, int len)
{
	Locker locker(&sub_sess_mutex_);
	std::list<TcpSession*>::iterator iter;
	for (iter = sub_sessions_.begin(); iter != sub_sessions_.end(); ++iter)
		(*iter)->Send(buf, len);
}


/////////////////////////////////////////////////////////////////////////////////////////

Server::Server(int port) : sub_sym_sessions_(NULL)
{
	tcp_server_ = new TcpServer(port, this, this);
	
	sub_sym_sessions_ = new Symbol2Sessions();
}

Server::~Server(void)
{
	delete tcp_server_;
	
	if (NULL != sub_sym_sessions_) { delete sub_sym_sessions_; }
}

void Server::OnAccept(TcpSession* sock)
{
	Locker locker(&sess_symbols_mutex);
	sess_symbols_[sock] = std::list<Symbol>();

	APP_LOG(LOG_LEVEL_INFO) << "OnAccept: " << sock->socket().remote_endpoint().address().to_string() << "\t" << sock->socket().remote_endpoint().port();
}

SessionContainer* Server::GetSessionContainer(const Symbol& sym, bool _Wri_Sub)
{
	Locker locker(&sub_sym_sessions_mutex_);	

	SessionContainer** sess = sub_sym_sessions_->Data(sym);
	if (!sess) return NULL;
	// if write case, new SessionContainer
	if (*sess == NULL && _Wri_Sub) { 
		*sess = new SessionContainer;
	}
	return *sess;
}

void Server::OnReceive(TcpSession *tcp_sock, char* buf, int len)
{
	if (len <= sizeof(ProtocolHead)) { return; }

	ProtocolHead* head = (ProtocolHead*)buf;
	switch (head->type)
	{
	case REQ_RUN_DATA:
		{
			if (len <= sizeof(RunDataReq)) { return; }
			RunDataReq* run_tick_req = (RunDataReq*)buf;
			if (run_tick_req->num <= 0 || len != run_tick_req->num * sizeof(Symbol)+ sizeof(RunDataReq)) 
			{ 
				return; 
			}

			Symbol *symbols = (Symbol*)(buf  + sizeof(RunDataReq));
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
		break;
	default:
		break;
	}
}


void Server::OnDiscon(TcpSession *tcp_sock)
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

	try{
		APP_LOG(LOG_LEVEL_INFO) << "OnDiscon: " << tcp_sock->socket().remote_endpoint().address().to_string() << "\t" << tcp_sock->socket().remote_endpoint().port();
	}
	catch (...){
		APP_LOG(LOG_LEVEL_WARN) << "OnDiscon: " << "illeagle connect";
	}
}

void Server::Subscribe(TcpSession *tcp_sock, const Symbol& sym)
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

void Server::UnSubscribe(TcpSession *tcp_sock, const Symbol& sym)
{
	Locker locker(&sess_symbols_mutex);
	Session2Symbols::iterator iter = sess_symbols_.find(tcp_sock);
	if (iter == sess_symbols_.end()) { return; }	//未连接的socket

	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter = std::find(sym_list.begin(), sym_list.end(), sym);
	if (sym_iter == sym_list.end()) { return; }  //未订阅.
	sym_list.erase(sym_iter);

	SessionContainer* sess = GetSessionContainer(sym);
	if (sess)sess->Remove(tcp_sock);
	
}

void Server::Send(const Symbol& sym, char* buf, int len)
{
	Locker locker(&sess_symbols_mutex);
	SessionContainer* sess = GetSessionContainer(sym, true);
	// 向订阅了该合约行情的客户端推送过去.
	sess->Send(buf, len);
}


