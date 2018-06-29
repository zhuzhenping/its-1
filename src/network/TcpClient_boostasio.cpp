#include "TcpClient_boostasio.h"
#include <boost/thread.hpp>

using namespace boost::asio;

namespace network {

TcpClient_boostasio::TcpClient_boostasio(const char *ip, const char* port,
	SocketReaderSpi* read_spi, ReConnSpi* re_conn_spi) 
	: TcpClient(read_spi, re_conn_spi)
	, resolver_(io_service_)
	, query_(tcp::v4(), ip, port)
	, first_connect_(true)
{
	Start();

	endpoint_iterator = resolver_.resolve(query_);
	
}

TcpClient_boostasio::~TcpClient_boostasio(void){
	Stop();
	delete new_session_;
	io_service_.stop();
}


bool TcpClient_boostasio::StartUp(std::string &err){
	new_session_ = new TcpSocket_boostasio(io_service_, read_spi_, this);
	
	boost::asio::async_connect(new_session_->socket(), endpoint_iterator, 
		boost::bind(&TcpClient_boostasio::handle_connect, this, boost::asio::placeholders::error));
	
	return true;
}

void TcpClient_boostasio::TearDown(){
	Stop();
	new_session_->close();
}

void TcpClient_boostasio::handle_connect(const boost::system::error_code& ec) {
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
			re_conn_spi_->SockConn(false);
		}
		else {
			Sleep(3000);
			boost::asio::async_connect(new_session_->socket(), endpoint_iterator,
				boost::bind(&TcpClient_boostasio::handle_connect, this, boost::asio::placeholders::error));
		}
	}
}

bool TcpClient_boostasio::Send(char* buf, int len, string &err){
	return new_session_->Send(buf, len, err);
}


void TcpClient_boostasio::OnDisconnect(TcpSocket *tcp_sock){
	if (re_conn_spi_)re_conn_spi_->SockDisconn();
	
	string err;
	StartUp(err);
}

void TcpClient_boostasio::OnRun()
{
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_service_);
		io_service_.run();
	}
}


}