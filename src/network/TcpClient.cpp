#include "network/TcpClient.h"
#include <boost/thread.hpp>

using namespace boost::asio;

//namespace network_asio {

TcpClient::TcpClient(const char *ip, const char* port, SocketReaderSpi* read_spi, ReConnSpi* re_conn_spi) 
	: read_spi_(read_spi)
	, re_conn_spi_(re_conn_spi)
	, resolver_(io_service_)
	, query_(tcp::v4(), ip, port)
	, first_connect_(true)
{
	Start();

	endpoint_iterator = resolver_.resolve(query_);
	
}

TcpClient::~TcpClient(void){
	Stop();
	delete new_session_;
	io_service_.stop();
}


void TcpClient::StartUp(){
	new_session_ = new TcpSession(io_service_, read_spi_, this);
	
	boost::asio::async_connect(new_session_->socket(), endpoint_iterator, 
		boost::bind(&TcpClient::handle_connect, this, boost::asio::placeholders::error));
	
}

void TcpClient::TearDown(){
	Stop();
	new_session_->socket().close();
	/*delete new_session_;
	new_session_ = NULL;*/
}

void TcpClient::handle_connect(const boost::system::error_code& ec) {
	if (!ec) {
		new_session_->start();
		if (re_conn_spi_) {
			if (first_connect_) {
				re_conn_spi_->SockConn(true);
				first_connect_ = false;
			}
			else{
				re_conn_spi_->SockReConn();
			}
		}
	}
	else {
		if (first_connect_) {
			if(re_conn_spi_)re_conn_spi_->SockConn(false);
		}
		else {
			Sleep(3000);
			boost::asio::async_connect(new_session_->socket(), endpoint_iterator,
				boost::bind(&TcpClient::handle_connect, this, boost::asio::placeholders::error));
		}
	}
}

void TcpClient::Send(char* buf, int len){
	new_session_->Send(buf, len);
}


void TcpClient::OnDisconnect(TcpSession *tcp_sock){
	if (re_conn_spi_)re_conn_spi_->SockDisconn();
	
	StartUp();
}

void TcpClient::OnRun()
{
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_service_);
		io_service_.run();
	}
}

