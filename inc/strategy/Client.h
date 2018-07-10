#ifndef TICK_CLIENT_H
#define TICK_CLIENT_H

#include <vector>
#include <list>
#include "network/TcpClient.h"
#include "common/SpinLock.h"
#include "datalib/Protocol.h"
#include "datalib/DataServerStruct.h"
#include "datalib/Symbol2T.h"

class ClientSpi {
public:
	virtual void OnData(Bars *, bool is_kline_update = false) = 0;
	virtual void OnError(const string &) = 0;
};

class Client : public SocketReaderSpi, public ReConnSpi
{
public:
	Client(ClientSpi* spi);
	virtual ~Client();

	void Init();
	void Denit();

	void SubData(const Symbol& sym);
	void SubData(const std::vector<Symbol>& syms);
	void UnSubData(const Symbol& sym);
	void UnSubData(const std::vector<Symbol>& syms);

private:
	//receive tick or kline.
	virtual void OnReceive(TcpSession *tcp_sock, char* buf, int len);
	//try to connect, but if connect fail, success==false
	virtual void SockConn(bool success);
	virtual void SockDisconn();
	virtual void SockReConn();

	void new_bars(const Symbol &sym);
	void delete_bars(const Symbol &sym);

private:
	SpinLock lock_;
	TcpClient* tcp_client_;

	list<Symbol> sub_syms_;// for reconnect to sub

	Symbol2T<Bars> bars_;//actual strategy data!
	SpinLock bars_mutex_;

	// 储存所有服务器、端口.
	vector<string> IPs_;
	int i_; // 所用IP的索引.
	string port_;

	bool is_init_;
	ClientSpi* spi_;
};


#endif

