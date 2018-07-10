#include "account/DataEngine.h"
#include <iostream>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <iostream>
#include "common/AppLog.h"
#include "common/Global.h"
#include "datalib/SymbolInfoSet.h"
#include "ctp/MarketDataApiFactory.h"

using namespace std;

DataEngine *DataEngine::self_ = NULL;

DataEngine *DataEngine::Instance() {
	static SpinLock lock;
	Locker locker(&lock);
	if (NULL == self_)
	{
		self_ = new DataEngine();
	}
	return self_;
}

void DataEngine::SetSpi(DataEventSpi *spi) {
	spi_ = spi;
}

DataEngine::DataEngine() : m_market_api(NULL) {
	XmlConfig config(Global::Instance()->GetConfigDir()+"config.xml");
	if (!config.Load()) { 
		APP_LOG(LOG_LEVEL_ERROR) << "config.xml not exist";
		return ; 
	}

	XmlNodeVec Addresses = config.FindChileren("DataServer/ctp_address", "Address");
	for (int i = 0; i < Addresses.size(); ++i) {
		IPs_.push_back(Addresses[i].GetValue("IP"));
	}
	i_ = 0;

	XmlNode node = config.FindNode("DataServer");
	BrokerID_ = node.GetValue("broker_id");
}

DataEngine::~DataEngine() {

}


bool DataEngine::Init(string symbols) {
	if (NULL == m_market_api)
	{
		m_market_api = MarketDataApiFactory::CreateMarketDataApi("CTP_FUTURE");
		if (NULL == m_market_api){
			APP_LOG(LOG_LEVEL_ERROR) <<  "CreateMarketDataApi fail";
			return false;
		}
	}

	i_ %= IPs_.size();
	string IP = IPs_[i_++];

	string err;
	if (!m_market_api->Init(IP, this, err)) {
		APP_LOG(LOG_LEVEL_ERROR) << std::string("CTP API Init error : ") + err;
		m_market_api->Denit();
		return false;
	}

	if (!m_market_api->Login(BrokerID_, "", "", err)) {
		APP_LOG(LOG_LEVEL_ERROR) << std::string("CTP API Login error : ") + err;
		m_market_api->Denit();
		return false;
	}

	if (!m_market_api->Subscribe(symbols, err)) {
		APP_LOG(LOG_LEVEL_ERROR) << std::string("SubscribeMarketPrice error : ") + err;
		return false;
	}

	is_init_ = true;
	return true;
}

void DataEngine::OnMdError(const std::string& request_name, const std::string& error_msg, const std::string& request_id) {
	APP_LOG(LOG_LEVEL_ERROR) 
		<< "request_name = "<<request_name
		<< "	request_id = "<<request_id
		<< "	error_msg = "<<error_msg;
}

void DataEngine::OnMdDisconnect(const std::string& reson) {
	APP_LOG(LOG_LEVEL_ERROR) <<"OnMdDisconnect:"<<reson;
}

void DataEngine::OnMdConnect() {
	APP_LOG(LOG_LEVEL_ERROR) << "OnMdConnect Reconnect";
}

void DataEngine::OnMarketPrice(BaseTick* tick) {
	//cout<<tick->symbol.instrument<<"  " << tick->date_time.Str() << "  "<<tick->last_price<<endl;
	if (spi_) {
		spi_->OnTick((FutureTick*)tick);
	}
}