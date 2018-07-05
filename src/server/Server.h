#ifndef SERVER_H_
#define SERVER_H_


#include "network/TcpServer.h"
#include "datalib/Protocol.h"
//#include "DataService.h"

class Server : public TcpServerConnSpi, public SocketReaderSpi
{
public:
	Server(int port);
	virtual ~Server(void);

private:
	virtual void OnAccept(TcpSession* sock);
	virtual void OnDiscon(TcpSession *tcp_sock);
	virtual void OnReceive(TcpSession *tcp_sock, char* buf, int len);


private:
	TcpServer* tcp_server_;

	//DataService *data_service_;
};


#endif

