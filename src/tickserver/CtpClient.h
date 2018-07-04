#ifndef ITSTATION_DATASERVER_CTP_DATA_SERVER_H_
#define ITSTATION_DATASERVER_CTP_DATA_SERVER_H_

#include <iostream>
#include <string>
#include <map>
#include "ctp/MarketDataApi.h"
#include "datalib/DataServerStruct.h"
#include "TickServer.h"

//namespace itstation {

class CtpClient : public MarketDataSpi
{
public:
	CtpClient();
	~CtpClient(void);

	bool StartUp(bool is_day, std::string& err);

	bool InitTcp(std::string& err);

	void DoAfterMarket(bool is_day);


private:
	virtual void OnMdError(const std::string& request_name, const std::string& error_msg, const std::string& request_id) {
		std::cout<<"请求错误:request_name = "<<request_name<< "	request_id = "<<request_id
			<<"	error_msg = "<<error_msg<<std::endl<<std::endl;
	}

	virtual void OnMdDisconnect(const std::string& reson) {
		std::cout<<"断开连接:"<<reson<<std::endl<<std::endl;
	}

	virtual void OnMdConnect() {
		std::cout<<"重新连接."<<std::endl;
	}

	virtual void OnMarketPrice(BaseTick* market_price);

private:
	MarketDataApi *m_market_api;

	vector<string> IPs_;
	int i_; // 所用IP的索引.
	string BrokerID_;
	int TcpServer_port;

	TickServer* tick_tcp_server_;
	bool is_init_;
	std::string sub_syms_;
};



#endif
