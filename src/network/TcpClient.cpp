
#include "network/TcpClient.h"
#include "TcpClient_boostasio.h"


namespace network {

	TcpClient *TcpClient::CreateTcpClient(NETWORK_LIB_TYPE type,
		const char *ip, const char *port,
		SocketReaderSpi* read_spi , ReConnSpi* re_conn_spi ){
		
		if (BOOST_ASIO == type) {
			TcpClient_boostasio *client = new TcpClient_boostasio(
				ip, port, read_spi, re_conn_spi);
			return client;
		}
		else if (LIB_EVENT == type) {
			return NULL;
		}
		else
			return NULL;
	}

	TcpClient::~TcpClient(){

	}

	TcpClient::TcpClient(SocketReaderSpi* reader_spi, ReConnSpi* re_conn_spi)
		: read_spi_(reader_spi)
		, re_conn_spi_(re_conn_spi){

	}
}