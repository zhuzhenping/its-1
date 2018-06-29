#ifndef TCP_SERVER_BOOST_ASIO_H
#define TCP_SERVER_BOOST_ASIO_H


#include "common/Thread.h"
#include "network/TcpServer.h"
#include "TcpSocket_boostasio.h"

namespace network {


class NETWORK_API TcpServer_boostasio : public TcpServer, SocketDissConnSpi, public common::Thread
{
public:
	TcpServer_boostasio(short port, SocketReaderSpi* spi, TcpServerConnSpi* conn_spi = NULL);

	virtual bool StartUp(string &err);

private:
	void start_accept();
	void handle_accept(TcpSocket_boostasio * new_session, const boost::system::error_code& error);

	virtual void OnDisconnect(TcpSocket *tcp_sock);

	virtual void OnRun(); 

	boost::asio::io_service io_service_;
	tcp::acceptor acceptor_;
		
	
};

}

#endif