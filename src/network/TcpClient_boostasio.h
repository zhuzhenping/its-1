#ifndef TCP_CLIENT_BOOST_ASIO_H
#define TCP_CLIENT_BOOST_ASIO_H

#include "TcpSocket_boostasio.h"
#include "common/Thread.h"
#include "network/TcpClient.h"

namespace network {



class  TcpClient_boostasio : public TcpClient, SocketDissConnSpi, public common::Thread
{
public:
	TcpClient_boostasio(const char *ip, const char *port, 
		SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);
	virtual ~TcpClient_boostasio(void);

	// 全部异步的方式.
	bool StartUp(std::string &err);
	void TearDown();
	virtual bool Send(char* buf, int len, std::string &err);

private:
	virtual void OnDisconnect(TcpSocket *tcp_sock);

	virtual void OnRun(); 

	void handle_connect(const boost::system::error_code& err);

	boost::asio::io_service io_service_;
	tcp::resolver resolver_;
	tcp::resolver::query query_;
	tcp::resolver::iterator endpoint_iterator;
	
	bool first_connect_;
	TcpSocket_boostasio *new_session_;

	

};

}

#endif