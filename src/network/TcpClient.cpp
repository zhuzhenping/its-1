#include "network/TcpClient.h"
#include <boost/thread.hpp>

using namespace boost::asio;

//namespace network_asio {

TcpClient::TcpClient(const char *ip, const char* port, SocketReaderSpi* read_spi, ReConnSpi* re_conn_spi) 
	: Thread()
	, read_spi_(read_spi)
	, re_conn_spi_(re_conn_spi)
	, resolver_(io_service_)
	, query_(tcp::v4(), ip, port)
	, first_connect_(true)
{
	endpoint_iterator_ = resolver_.resolve(query_);
}

TcpClient::~TcpClient(void){	
	io_service_.stop();
}


void TcpClient::Init(){	
	new_session_ = new TcpSession(io_service_, read_spi_, this);

	Thread::Start();
	boost::asio::async_connect(new_session_->socket(), endpoint_iterator_, 
		boost::bind(&TcpClient::handle_connect, this, boost::asio::placeholders::error));
}

void TcpClient::Denit(){
	Thread::Stop();
	Thread::Join();
	//new_session_->socket().close();//会导致TcpSession中delete this
	delete new_session_;
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
			/*if (init_) {
				Sleep(3000);				
				boost::asio::async_connect(new_session_->socket(), endpoint_iterator_,
					boost::bind(&TcpClient::handle_connect, this, boost::asio::placeholders::error));
			}*/
		}
	}
}

void TcpClient::Send(const char* buf, int len){
	new_session_->Send(buf, len);
}


void TcpClient::OnDisconnect(TcpSession *tcp_sock){
	if (re_conn_spi_)re_conn_spi_->SockDisconn();
	// if net disconnect, to auto connect when net ok
	//Init();
}

void TcpClient::OnRun()
{
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_service_);
		io_service_.run();
	}
}

