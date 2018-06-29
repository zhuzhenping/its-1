
#include "network/TcpServer.h"
#include "TcpServer_boostasio.h"


namespace network {

	TcpServer *TcpServer::CreateTcpServer(NETWORK_LIB_TYPE type,
		int port, SocketReaderSpi* read_spi, TcpServerConnSpi* conn_spi){
		
		if (BOOST_ASIO == type) {
			return NULL;
		}
		else if (LIB_EVENT == type) {
			return NULL;
		}
		else
			return NULL;
	}

	TcpServer::~TcpServer(){

	}

	TcpServer::TcpServer(SocketReaderSpi* read_spi, TcpServerConnSpi* conn_spi)
		: read_spi_(read_spi)
		, conn_spi_(conn_spi){

	}
}