#pragma once

#include "NetworkAsio/TcpSession.h"
#include "Common/Thread.h"

namespace network_asio {

class TcpServerConnSpi {
public:
	virtual void OnAccept(TcpSession* sock) = 0;
	virtual void OnDiscon(TcpSession *tcp_sock) = 0;
};

class NETWORK_ASIO_API TcpServer : private SocketDissConnSpi, public itstation::common::Thread
{
public:
	TcpServer(short port, SocketReaderSpi* spi, TcpServerConnSpi* conn_spi = NULL);


private:
	void start_accept();
	void handle_accept(TcpSession * new_session, const boost::system::error_code& error);

	virtual void OnDisconnect(TcpSession *tcp_sock);

	virtual void OnRun(); 

	boost::asio::io_service io_service_;
	tcp::acceptor acceptor_;
		
	SocketReaderSpi* spi_;
	TcpServerConnSpi* conn_spi_;
};

}