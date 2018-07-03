#ifndef MYSECURITYINFOSPI_H
#define MYSECURITYINFOSPI_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QSharedPointer>

#include <stdlib.h>
#include <iostream>
#include "ctp/MarketTradeApiFactory.h"
#include <fstream>
#include <vector>
#include <map>
#include <stdio.h>
#include "common/AppLog.h"
#include "ctp/MarketTradeApi.h"
//#include "DataLib/MarketTask.h"

using namespace std;
/*
using namespace itstation;
using namespace itstation::marketapi;
using namespace itstation::common;
using namespace network_asio;*/


//#define JUST_TEST
#ifdef JUST_TEST
#define WAITING_TIME 10000
#else
#define WAITING_TIME GetWaitingMillseconds()
#endif

//namespace itstation {

class SymbolChineseName
{
public:
	SymbolChineseName();
	~SymbolChineseName() { }

	std::string ChiName(const std::string& inst, const std::string& name);

private:
	std::string PrefixStr(const std::string& name);
	std::string CodeStr(const std::string& name);

	std::map<string, string> name_dict_;
};

class MyMarketTradeCallback : public TradeSpi
{
public:
	virtual void OnError(const int request_id, const std::string& error_msg);
	virtual void OnDisconnect(const std::string& reson);
	virtual void OnConnect();

	virtual void OnOrderError(OrderData* order_data) {}
	virtual void OnOrder(OrderData* order_data) {}
	virtual void OnTrade(TradeData* order_data) {}
	virtual void OnCancelOrder(OrderData* order_data) {}
	virtual void OnQryOrder(int req_id, OrderData* order_data, const std::string& err, bool is_last) {}
	virtual void OnQryTrade(int req_id, TradeData* order_data, const std::string& err, bool is_last) {}
	virtual void OnQryAccount(int req_id, AccountData* acco_data, const std::string& err, bool is_last) {}
	virtual void OnQryPosition(int req_id, PositionData* pos_data, const std::string& err, bool is_last) {}
};

class MySecurityInfoSpi : /*public QObject, public TimedStateTaskManager,  */public SecurityInfoSpi
{
	//Q_OBJECT

public:
	MySecurityInfoSpi(ProductIdType product);
	virtual ~MySecurityInfoSpi();

	bool init();
	void denit() { trade_api->Deinit(); }

private:
	/*virtual bool DoDayOpen() ;
	virtual bool DoDayClose() { APP_LOG(LOG_LEVEL_INFO) << "DoDayClose"; return true; }
	virtual bool DoNightOpen() ;
	virtual bool DoNightClose() { APP_LOG(LOG_LEVEL_INFO) << "DoNightClose"; return true; }
	
	bool SlotTimeOut() ;*/
	void WriteFile();
	

	virtual void OnInstrumentInfo(const InstrumentInfoData& info, bool is_last)
	{
		QSharedPointer<BaseInstrumentInfo> pinfo(new InstrumentInfoData(*(InstrumentInfoData*)&info));
		if (info.symbol.exchange != EXCHANGE_INE)infos_[info.symbol.Str()] = pinfo;

		if (is_last) {
			WriteFile();
			infos_.clear();

			
		}		
	}

private:
	FILE* fp_;
	ProductIdType product_;

	map<string, QSharedPointer<BaseInstrumentInfo> > infos_;	//为了按顺序排列，改成map先存.

	QString broker_id,user_id,pwd,front_addr;
	int year;
	std::string trade_time_path;
	QStringList holidays;
	//MySecurityInfoSpi* info_spi;
	TradeSpi* market_trade_callback;
	TradeApi *trade_api;
	
};



#endif // MYSECURITYINFOSPI_H
