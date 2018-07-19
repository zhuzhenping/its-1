#ifndef ITSTATION_DATASERVER_CTP_DATA_SERVER_H_
#define ITSTATION_DATASERVER_CTP_DATA_SERVER_H_

#include <iostream>
#include <string>
#include <map>
#include <queue>
#include "common/AppLog.h"
#include "ctp/MarketDataApi.h"
#include "datalib/DataServerStruct.h"
#include "DataWriter.h"

//namespace itstation {

class CtpClient : public MarketDataSpi, public Thread
{
public:
	CtpClient();
	~CtpClient(void);

	bool Init(bool is_day, std::string& err);

	void Denit();


private:
	virtual void OnMdError(const std::string& request_name, const std::string& error_msg, const std::string& request_id) {
		APP_LOG(LOG_LEVEL_INFO) << request_name << error_msg << request_id;
	}

	virtual void OnMdDisconnect(const std::string& reson) {
		APP_LOG(LOG_LEVEL_INFO) << "OnMdDisconnect";
	}

	virtual void OnMdConnect() {
		APP_LOG(LOG_LEVEL_INFO) << "OnMdConnect";
	}

	virtual void OnMarketPrice(BaseTick* market_price);

	void OnRun();
	int TickSize();
	FutureTick* PopTick();

private:
	MarketDataApi *m_market_api;
	DataWriter *data_writer_;

	vector<string> IPs_;
	int i_; // 所用IP的索引.
	string BrokerID_;

	bool is_init_;
	std::string sub_syms_;

	std::queue<FutureTick*> ticks_;
	SpinLock ticks_lock_;
	Mutex mutex_;
	Condition cond_;
};



#endif
