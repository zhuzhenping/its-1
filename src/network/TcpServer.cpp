#include "NetworkAsio/TcpServer.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

namespace network_asio {

TcpServer::TcpServer(short port, SocketReaderSpi* spi, TcpServerConnSpi* conn_spi)
	: spi_(spi)
	, conn_spi_(conn_spi)
	, acceptor_(io_service_, tcp::endpoint(tcp::v4(), port))
{
	Thread::Start();
	start_accept();
}

void TcpServer::start_accept() {
	TcpSession* new_session = new TcpSession(io_service_, spi_, this, true);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&TcpServer::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void TcpServer::handle_accept(TcpSession *new_session,
	const boost::system::error_code& error)
{
	if (!error)	{
		if (conn_spi_) conn_spi_->OnAccept(new_session);
		new_session->start();

		/*//如果session连进来后5s都不发数据，则掐断之		
		timer_.expires_from_now(boost::posix_time::milliseconds(EXPIRES_TIME));
		timer_.async_wait(boost::bind(&TcpServer::OnTimer, this, new_session, boost::asio::placeholders::error));*/
	}
	else {
		delete new_session;
	}

	start_accept();
}


void TcpServer::OnDisconnect(TcpSession *tcp_sock) {
	if (conn_spi_ != NULL)
	{
		conn_spi_->OnDiscon(tcp_sock);
	}
}

void TcpServer::OnRun()
{
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_service_);
		io_service_.run();
	}
}

}