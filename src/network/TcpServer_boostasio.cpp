#include "TcpServer_boostasio.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;
using namespace zhongan;

namespace network {

TcpServer_boostasio::TcpServer_boostasio(short port, SocketReaderSpi* spi, TcpServerConnSpi* conn_spi)
: TcpServer(spi, conn_spi)
	, acceptor_(io_service_, tcp::endpoint(tcp::v4(), port))
{
	
}

bool TcpServer_boostasio::StartUp(string &err){
	common::Thread::Start();
	start_accept();
	return true;
}

void TcpServer_boostasio::start_accept() {
	TcpSocket_boostasio* new_session = new TcpSocket_boostasio(io_service_, read_spi_, this, true);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&TcpServer_boostasio::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void TcpServer_boostasio::handle_accept(TcpSocket_boostasio *new_session,
	const boost::system::error_code& error)
{
	if (!error)	{
		if (conn_spi_) conn_spi_->OnAccept(new_session);
		new_session->start();

		/*//如果session连进来后5s都不发数据，则掐断之		
		timer_.expires_from_now(boost::posix_time::milliseconds(EXPIRES_TIME));
		timer_.async_wait(boost::bind(&TcpServer_boostasio::OnTimer, this, new_session, boost::asio::placeholders::error));*/
	}
	else {
		delete new_session;
	}

	start_accept();
}


void TcpServer_boostasio::OnDisconnect(TcpSocket *tcp_sock) {
	if (conn_spi_ != NULL)
	{
		conn_spi_->OnDiscon(tcp_sock);
	}
}

void TcpServer_boostasio::OnRun()
{
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_service_);
		io_service_.run();
	}
}

}