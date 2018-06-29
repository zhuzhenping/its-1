
#include "network/TcpSocket.h"
#include "TcpSocket_boostasio.h"

namespace network {


	TcpSocket::~TcpSocket(){

	}

	TcpSocket::TcpSocket(SocketReaderSpi* reader_spi, SocketDissConnSpi* disconn_spi) : read_spi_(reader_spi), disconn_spi_(disconn_spi){

	}
}