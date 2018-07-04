#include "CtpClient.h"
#include <iostream>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <iostream>
#include "common/AppLog.h"
#include "common/Global.h"
#include "datalib/SymbolInfoSet.h"
#include "ctp/MarketDataApiFactory.h"



CtpClient::CtpClient() : m_market_api(NULL), is_init_(false), tick_tcp_server_(NULL)
{
	
	XmlConfig config(Global::GetInstance()->GetConfigFile());
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
	TcpServer_port = atoi(node.GetValue("tick_port").c_str());
	
}

CtpClient::~CtpClient(void)
{
	//TODO:
	//if (m_market_api) { delete m_market_api; }
	//delete tick_tcp_server_;	
}

void CtpClient::OnMarketPrice(BaseTick* tick)
{
	if (is_init_) { tick_tcp_server_->SendTick(tick->symbol, (char*)tick, sizeof(FutureTick)); }

	cout<<tick->symbol.instrument<<"  " << tick->date_time.Str() << "  "<<tick->last_price<<endl;
}

bool  CtpClient::InitTcp(std::string& err)
{
	
	if (NULL == tick_tcp_server_)
	{
		
		cout << "listen on " << TcpServer_port << endl;
		tick_tcp_server_ = new TickServer(TcpServer_port);
		if (!tick_tcp_server_->Init(err))
		{
			delete tick_tcp_server_;
			tick_tcp_server_ = NULL;
			return false;
		}
	}

	return true;
}

bool CtpClient::StartUp(bool is_day, std::string& err)
{
	if (!tick_tcp_server_->StartUp(err))
	{
		return false;
	}
	

	if (NULL == m_market_api)
	{
		m_market_api = MarketDataApiFactory::CreateMarketDataApi("CTP_FUTURE");
		if (NULL == m_market_api){
			err = "create ctp api error";
			return false;
		}
	}

	i_ %= IPs_.size();
	string IP = IPs_[i_++];

	if (!m_market_api->Init(IP, this, err)) {
		err = std::string("CTP API Init error : ") + err;
		m_market_api->Deinit();
		return false;
	}

	if (!m_market_api->Login(BrokerID_, "", "", err)) {
		err = std::string("CTP API Login error : ") + err;
		m_market_api->Deinit();
		return false;
	}

	SymbolInfoSet* info_set = SymbolInfoSet::GetInstance();
	if (!info_set->Init(err))
	{
		return false;
	}

	sub_syms_ = "";
	if (is_day)
	{
		const std::vector<Symbol>& syms = info_set->FutureSymbols();
		for (int i=0; i<syms.size(); ++i)
		{
			if (i != 0) { sub_syms_ += ","; }
			sub_syms_ += syms[i].instrument;
		}
	} 
	else
	{
		const std::vector<Symbol>& syms = info_set->NightFutureSymbols();
		for (int i=0; i<syms.size(); ++i)
		{
			if (i != 0) { sub_syms_ += ","; }
			sub_syms_ += syms[i].instrument;
		}
	}
	

	if (!m_market_api->Subscribe(sub_syms_, err)) {
		err = std::string("SubscribeMarketPrice error : ") + err;
		return false;
	}

	is_init_ = true;
	return true;
}

void CtpClient::DoAfterMarket(bool is_day)
{
	if (NULL != m_market_api)
	{
		std::string err;
		//if (!m_market_api->Logout(err)) 
		//{
		//	printf("Logout error : %s\n",err.c_str());
		//}
		m_market_api->Deinit();
	}

	SymbolInfoSet::GetInstance()->Deinit();
	is_init_ = false;
}

