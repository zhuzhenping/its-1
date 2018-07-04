#ifndef ITSTATION_DATASERVER_TICK_TCP_SERVER_H_
#define ITSTATION_DATASERVER_TICK_TCP_SERVER_H_

#include "datalib/DataServerStruct.h"
#include "network/TcpServer.h"
#include "common/SpinLock.h"
#include "datalib/Symbol2T.h"
#include "datalib/Protocol.h"
#include <list>
#include <map>

//namespace itstation {

	

class SessionContainer {
public:
	SessionContainer() : tick_(NULL) {}

	void Append(TcpSession* sock);
	void Remove(TcpSession* sock);

	void SetTick(FutureTick* tick);
	FutureTick* GetTick();

	//void SendTick(TcpSession* sock);

	void Send(char* buf, int len);

private:
	FutureTick* tick_;
	SpinLock tick_mutex_;

	std::list<TcpSession*> sub_sessions_;
	SpinLock sub_sess_mutex_;
	
};

typedef Symbol2T<SessionContainer> Symbol2Sessions;
typedef std::map<TcpSession*, std::list<Symbol> > Session2Symbols;


class TickServer : public TcpServerConnSpi, public SocketReaderSpi
{
public:
	TickServer(int port);
	virtual ~TickServer(void);

	bool Init(std::string& err);
	bool StartUp(std::string& err);	//每天调用一次.


	int SendTick(const Symbol& sym, char* buf, int len);

	

private:
	void Subscribe(TcpSession *tcp_sock, const Symbol& sym);
	void UnSubscribe(TcpSession *tcp_sock, const Symbol& sym);

	virtual void OnAccept(TcpSession* sock);
	virtual void OnDiscon(TcpSession *tcp_sock);
	virtual void OnReceive(TcpSession *tcp_sock, char* buf, int len);

	SessionContainer* GetSessionContainer(const Symbol& sym);

private:
	TcpServer* tcp_server_;

	std::list<TcpSession*> all_symbol_sessions_;	//订阅所有品种的session集.

	Symbol2Sessions* sub_sym_sessions_;// 订阅了某个symbol的session集.
	SpinLock sub_sym_sessions_mutex_;

	Session2Symbols sess_symbols_;	//每个socket维护一个订阅列表，当socket断开时能快速取消订阅该socket的行情.
	SpinLock sess_symbols_mutex;
};


#endif

