#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H


#include "network/TcpSession.h"
#include "common/Thread.h"

//namespace network_asio {

class ReConnSpi
{
public:
	//try to connect, but if connect fail, success==false
	virtual void SockConn(bool success) = 0;
	//网络断开
	virtual void SockDisconn() = 0;
	//断线重连
	virtual void SockReConn() = 0;
};

class NETWORK_API TcpClient : private SocketDissConnSpi, public Thread
{
public:
	TcpClient(const char *ip, const char *port, SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);
	virtual ~TcpClient(void);

	// all in asyn
	void Init();
	void Denit();
	void Send(const char* buf, int len);

private:
	virtual void OnDisconnect(TcpSession *tcp_sock);

	virtual void OnRun(); 

	void handle_connect(const boost::system::error_code& err);

	boost::asio::io_service io_service_;
	tcp::resolver resolver_;
	tcp::resolver::query query_;
	tcp::resolver::iterator endpoint_iterator_;
	
	bool first_connect_;
	TcpSession *new_session_;

	SocketReaderSpi* read_spi_;
	ReConnSpi* re_conn_spi_;
};

#endif // TCP_CLIENT_H