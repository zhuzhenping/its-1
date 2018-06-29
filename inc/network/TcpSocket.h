/*!
* \brief       抽象一个网络连接.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
* 
*
*/

#ifndef _TCP_SOCKET_H_  
#define _TCP_SOCKET_H_  

#include <string>
#include "common/Global.h"

namespace network {

// 基于哪个库进行封装.
enum NETWORK_LIB_TYPE {
	LIB_EVENT,
	BOOST_ASIO,
};

class TcpSocket;

// 收到数据.
class SocketReaderSpi {
public:
	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len) = 0;
};
// 网络连接断开.
class SocketDissConnSpi {
public:
	virtual void OnDisconnect(TcpSocket *tcp_sock) = 0;
};

class NETWORK_API TcpSocket
{
public:

	virtual ~TcpSocket();
	
	// 发数据.
	virtual bool Send(char* buf, int len, std::string &err) = 0;


protected:
	TcpSocket(SocketReaderSpi* reader_spi, SocketDissConnSpi* disconn_spi);

	SocketReaderSpi *read_spi_;
	SocketDissConnSpi *disconn_spi_;

};



}

#endif