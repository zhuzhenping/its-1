#ifndef ITS_DATA_EIGINE_H_
#define ITS_DATA_EIGINE_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include "common/SpinLock.h"
#include "ctp/MarketDataApi.h"

class DataEventSpi {
public:
	virtual void OnTick(FutureTick *) = 0;
	virtual void OnKline(Kline *) = 0;
};

class ACCOUNT_API DataEngine : public MarketDataSpi
{
public:
	static DataEngine *Instance();
	void SetSpi(DataEventSpi *spi);
	virtual ~DataEngine();

	bool init(string symbols);

private:
	DataEngine();

	virtual void OnMdError(const std::string& request_name, const std::string& error_msg, const std::string& request_id);
	virtual void OnMdDisconnect(const std::string& reson);
	virtual void OnMdConnect();
	virtual void OnMarketPrice(BaseTick* market_price);

private:
	static DataEngine *self_;
	DataEventSpi *spi_;

	MarketDataApi *m_market_api;
	bool is_init_;

	vector<string> IPs_;
	int i_; // ����IP������.
	string BrokerID_;
};


#endif