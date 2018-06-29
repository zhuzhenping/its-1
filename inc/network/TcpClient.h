/*!
* \brief       网络客户端.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
*
*
*/

#ifndef _TCP_CLIENT_H_  
#define _TCP_CLIENT_H_  

#include "network/TcpSocket.h"

namespace network {


class ReConnSpi
{
public:
	//网络第一次连接。 如果第一次连接失败（当网络断开时），则success为false
	virtual void SockConn(bool success) = 0;
	//网络断开.
	virtual void SockDisconn() = 0;
	//断线重连.
	virtual void SockReConn() = 0;
};


class NETWORK_API TcpClient
{
public:
	TcpClient *CreateTcpClient(NETWORK_LIB_TYPE type,
		const char *ip, const char *port,
		SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);

	
	virtual ~TcpClient(void);

	// 启动.
	virtual bool StartUp(std::string &err) = 0;
	// 停止.
	virtual void TearDown() = 0;
	// 发数据.
	virtual bool Send(char* buf, int len, std::string &err) = 0;


protected:
	TcpClient(SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);

	SocketReaderSpi* read_spi_;
	ReConnSpi* re_conn_spi_;

};

}

#endif