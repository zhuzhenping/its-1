#include "Server.h"
#include <algorithm>
#include <common/AppLog.h>
#include "datalib/SymbolInfoSet.h"



Server::Server(int port)
{
	tcp_server_ = new TcpServer(port, this, this);
	
}

Server::~Server(void)
{
	delete tcp_server_;

}

void Server::OnAccept(TcpSession* sock)
{
	APP_LOG(LOG_LEVEL_INFO) << "OnAccept: " << sock->socket().remote_endpoint().address().to_string() << "\t" << sock->socket().remote_endpoint().port();
}


void Server::OnReceive(TcpSession *tcp_sock, char* buf, int len)
{
	if (len < sizeof(ProtocolHead)) { /*有误*/ return; }
	ProtocolHead *req_head = (ProtocolHead*)buf;
	switch(req_head->type) {
	case REQ_RUN_TICK:
		{
			if (len != sizeof(LoginRequest)) return;
			LoginRequest *req = (LoginRequest*)buf;
			//if(login_service_)login_service_->HandleReq(req, tcp_sock);
			APP_LOG(LOG_LEVEL_INFO) << "REQ_RUN_TICK";
		}
		break;
	default:
		break;
	}
}


void Server::OnDiscon(TcpSession *sock)
{
	try{
		APP_LOG(LOG_LEVEL_INFO) << "OnDiscon: " << sock->socket().remote_endpoint().address().to_string() << "\t" << sock->socket().remote_endpoint().port();
	}
	catch (...){
		APP_LOG(LOG_LEVEL_INFO) << "OnDiscon: " << "illeagle connect";
	}

	
}
