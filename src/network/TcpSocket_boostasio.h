/*!
* \brief       抽象一个网络连接.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
* 
*
*/

#ifndef _TCP_SOCKET_BOOST_ASIO_H_  
#define _TCP_SOCKET_BOOST_ASIO_H_  

#include <iostream>
#include <vector>
#include <deque>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include "common/Mutex.h"
#include "network/TcpSocket.h"
#include "TcpMessage.h"

using namespace boost::asio::ip;
using namespace zhongan;

namespace network {



typedef std::deque<TcpMessage> TcpMessageQueue;


class  TcpSocket_boostasio : public TcpSocket, public boost::enable_shared_from_this<TcpSocket_boostasio>
{
public:
	TcpSocket_boostasio(boost::asio::io_service& io_service, 
		SocketReaderSpi* read_spi, SocketDissConnSpi* dis_conn_spi, bool is_server=false);
	virtual ~TcpSocket_boostasio();
	
	virtual bool Send(char* buf, int len, std::string &err);

	tcp::socket& socket() { return socket_; }
	void start();
	void close();

private:
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);
	void handle_close();

	boost::asio::io_service &io_service_;
	// The socket used to communicate with the client.
	tcp::socket socket_;

	

	TcpMessage read_message_; // 缓存读的数据.
	TcpMessageQueue write_message_;
	common::Mutex write_message_mutex_;

	// 是服务端还是客户端.
	bool is_server_;
	bool server_recv_data_; // 服务器在EXPIRES_TIME内是否收到过数据,如果没收到，则掐断连接.
	// 客户端计数器（<NUMBER_）。客户端每隔EXPIRES向服务器发心跳包，如果连续发NUMBER_次都没收到反馈，则掐断连接.
	int client_ii_a; 

	//服务端：如果EXPIRES_TIME 都收不到数据，则掐断该连接.
	//客户端：每隔EXPIRES_TIME向服务端发一个心跳包，若连续6次收不到服务器返回的心跳包，则掐断.
	boost::asio::deadline_timer timer_;
	void OnTimer(const boost::system::error_code&);
};

typedef boost::shared_ptr<TcpSocket_boostasio> TcpSocket_boostasioPtr;




}

#endif