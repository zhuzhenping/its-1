#include "network/Client.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include "common/Thread.h"
#include "common/XmlConfig.h"
#include "common/AppLog.h"


Client::Client(ClientSpi* spi) 
	: spi_(spi), is_init_(false), tcp_client_(NULL)
{
	XmlConfig config(Global::Instance()->GetConfigFile());
	if (!config.Load()) { 
		APP_LOG(LOG_LEVEL_ERROR) << "config.xml not exist";
		return ; 
	}

	XmlNodeVec Addresses = config.FindChileren("DataServer/my_address", "Address");
	for (int i = 0; i < Addresses.size(); ++i) {		
		IPs_.push_back(Addresses[i].GetValue("IP"));
	}
	i_ = 0;
	XmlNode node = config.FindNode("DataServer");
	port_ = node.GetValue("port");
}

Client::~Client(void)
{
	tcp_client_->Denit();
	delete tcp_client_;
}

void Client::OnReceive(TcpSession *tcp_sock, char* buf, int len)
{
	if (len < sizeof(ProtocolHead)) { return; }
	ProtocolHead *req_head = (ProtocolHead*)buf;
	switch(req_head->type) {
	case RSP_RUN_TICK:
		{
			RunTickRsp *rsp = (RunTickRsp*)buf;
			Bars **bars = bars_.Data(rsp->tick.symbol); 
			(*bars)->tick = rsp->tick;
			if (spi_) spi_->OnData(*bars);
		}
		break;
	case RSP_RUN_KLINE:
		{
			RunKlineRsp *rsp = (RunKlineRsp*)buf;
			Bars **bars = bars_.Data(rsp->kline.symbol); 
			(*bars)->klines.Append(rsp->kline);
			if (spi_) spi_->OnData(*bars);
		}
		break;
	case RSP_HIS_TICK:
		{
			//RunTickRsp *tick_rsp = (RunTickRsp*)buf;
		}
		break;
	case RSP_HIS_KLINE:
		{
			HisKlineRsp *rsp = (HisKlineRsp*)buf;
			Symbol &sym = rsp->klines[0].symbol;
			Bars **bars = bars_.Data(sym);
			for (int i = 0; i < rsp->num; ++i){
				(*bars)->klines.Append(rsp->klines[i]);
			}
			if (spi_) spi_->OnData(*bars);
		}
		break;
	default:
		break;
	}
}

void Client::SockConn(bool success){
	is_init_ = success;

	if (success){ //网络连上.
		std::vector<Symbol> syms(sub_syms_.begin(), sub_syms_.end());
		sub_syms_.clear();
		SubData(syms);
	}
	else{//没连上：换服务器地址去连；或者网络断了或者服务器地址都不正确.
		if (i_ == IPs_.size()){
			if (spi_)spi_->OnError("net disconnect");
		}
		else {
			i_ %= IPs_.size();
			string IP = IPs_[i_++];
			if (tcp_client_) delete tcp_client_;
			tcp_client_ = new TcpClient(IP.c_str(), port_.c_str(), this, this);
			tcp_client_->Init();
		}
	}
}

void Client::SockDisconn(){
	is_init_ = false;
	if (spi_)spi_->OnError("net disconnect");
}

void Client::SockReConn()
{
	is_init_ = true;
	std::vector<Symbol> syms(sub_syms_.begin(), sub_syms_.end());
	sub_syms_.clear();
	SubData(syms);
}

void Client::Init()
{
	if (!is_init_){
		i_ %= IPs_.size();
		string IP = IPs_[i_++];
		if (tcp_client_) delete tcp_client_;
		tcp_client_ = new TcpClient(IP.c_str(), port_.c_str(), this, this);
		tcp_client_->Init();
	}
}

void Client::Denit(){
	is_init_ = false;
	tcp_client_->Denit();
}


void Client::SubData(const Symbol& sym)
{
	//has subed.
	if (std::find(sub_syms_.begin(), sub_syms_.end(), sym) != sub_syms_.end())return;
	sub_syms_.push_back(sym);

	//get his klines
	int his_len = sizeof(HisDataReq) + sizeof(Symbol);
	HisDataReq *his_req = (HisDataReq*)malloc(his_len);
	his_req->type = REQ_HIS_DATA;
	his_req->num = 1;
	memcpy((char*)his_req + sizeof(HisDataReq), &sym, sizeof(Symbol));
	//sub runtime tick and kline
	int len = sizeof(RunDataReq) + sizeof(Symbol);
	RunDataReq* req = (RunDataReq*)malloc(len);
	req->type = REQ_RUN_DATA;
	req->num = 1;
	req->is_sub = true;
	memcpy((char*)req + sizeof(RunDataReq), &sym, sizeof(Symbol));
	if (is_init_){
		Locker locker(&lock_);
		tcp_client_->Send((char*)his_req, his_len);
		tcp_client_->Send((char*)req, len);
	}	
	free(req);

	//
	new_bars(sym);
}

void Client::new_bars(const Symbol &sym){
	Bars** bars = bars_.Data(sym);
	if (bars) { 
		if (NULL == *bars)
			*bars = new Bars;
	}
}
void Client::delete_bars(const Symbol &sym) {
	Bars** bars = bars_.Data(sym);
	if (bars) { 
		delete *bars;
		*bars = NULL;
	}
}
void Client::SubData(const std::vector<Symbol>& syms)
{
	//get his klines
	int his_len = sizeof(HisDataReq) + sizeof(Symbol) *syms.size();
	HisDataReq *his_req = (HisDataReq*)malloc(his_len);
	his_req->type = REQ_HIS_DATA;
	his_req->num = syms.size();
	char* his_cp_pos = (char*)his_req + sizeof(HisDataReq);
	//sub
	int len = sizeof(RunDataReq) + sizeof(Symbol) * syms.size();
	RunDataReq* req = (RunDataReq*)malloc(len);
	req->type = REQ_RUN_DATA;
	req->num = syms.size();
	req->is_sub = true;
	char* cp_pos = (char*)req + sizeof(RunDataReq);
	// 已经订阅过的 不必再订阅；没有订阅过的 放进sub_syms_
	std::list<Symbol> tmp_syms;
	for (int i=0; i<syms.size(); ++i)
	{
		if (std::find(sub_syms_.begin(), sub_syms_.end(), syms[i]) != sub_syms_.end())
		{
			req->num--;
			len -= sizeof(Symbol);
			continue;
		}
		memcpy(his_cp_pos, &syms[i], sizeof(Symbol));
		his_cp_pos += sizeof(Symbol);
		memcpy(cp_pos, &syms[i], sizeof(Symbol));
		cp_pos += sizeof(Symbol);

		tmp_syms.push_back(syms[i]);

		new_bars(syms[i]);
	}
	sub_syms_.insert(sub_syms_.end(), tmp_syms.begin(), tmp_syms.end());

	if (is_init_){
		Locker locker(&lock_);
		tcp_client_->Send((char*)his_req, his_len);
		tcp_client_->Send((char*)req, len);
	}
	free(req);
}

void Client::UnSubData(const Symbol& sym)
{
	list<Symbol>::iterator iter = std::find(sub_syms_.begin(), sub_syms_.end(), sym);
	if (iter != sub_syms_.end()) { sub_syms_.erase(iter); }

	RunDataReq* req = (RunDataReq*)malloc(sizeof(RunDataReq) + sizeof(Symbol));
	req->type = REQ_RUN_DATA;
	req->num = 1;
	req->is_sub = false;
	memcpy((char*)req + sizeof(RunDataReq), &sym, sizeof(Symbol));
	if (is_init_){
		Locker locker(&lock_);
		tcp_client_->Send((char*)req, sizeof(RunDataReq) + sizeof(Symbol));
	}	
	free(req);

	//delete bars
	delete_bars(sym);
}

void Client::UnSubData(const std::vector<Symbol>& syms)
{
	int len = sizeof(RunDataReq) + sizeof(Symbol) * syms.size();
	RunDataReq* req = (RunDataReq*)malloc(len);
	req->type = REQ_RUN_DATA;
	req->num = syms.size();
	req->is_sub = false;
	char* cp_pos = (char*)req + sizeof(RunDataReq);
	for (int i=0; i<syms.size(); ++i)
	{
		memcpy(cp_pos, &syms[i], sizeof(Symbol));
		cp_pos += sizeof(Symbol);
		list<Symbol>::iterator iter = std::find(sub_syms_.begin(), sub_syms_.end(), syms[i]);
		if (iter != sub_syms_.end()) { 
			sub_syms_.erase(iter); 
			delete_bars(*iter);
		}
	}
	if (is_init_){
		Locker locker(&lock_);
		tcp_client_->Send((char*)req, sizeof(RunDataReq) + req->num * sizeof(Symbol));
	}	
	free(req);
}
