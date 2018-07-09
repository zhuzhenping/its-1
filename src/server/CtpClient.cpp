#include "CtpClient.h"
#include <iostream>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <iostream>
#include "common/AppLog.h"
#include "common/Global.h"
#include "datalib/SymbolInfoSet.h"
#include "ctp/MarketDataApiFactory.h"



CtpClient::CtpClient() : m_market_api(NULL), is_init_(false), i_(0)
{	
	XmlConfig config(Global::Instance()->GetConfigFile());
	if (!config.Load()) { 
		APP_LOG(LOG_LEVEL_ERROR) << "config.xml not exist";
		return ; 
	}
	XmlNodeVec Addresses = config.FindChileren("DataServer/ctp_address", "Address");
	for (int i = 0; i < Addresses.size(); ++i) {
		IPs_.push_back(Addresses[i].GetValue("IP"));
	}
	XmlNode node = config.FindNode("DataServer");
	BrokerID_ = node.GetValue("broker_id");
	
	m_market_api = MarketDataApiFactory::CreateMarketDataApi("CTP_FUTURE");
	data_writer_ = new DataWriter;
}

CtpClient::~CtpClient(void)
{
	delete m_market_api;
	delete data_writer_;
}

void CtpClient::OnMarketPrice(BaseTick* tick)
{
	FutureTick* cpy_tick = new FutureTick(*(FutureTick*)tick);

	Locker locker(&ticks_lock_);
	ticks_.push(cpy_tick);
	cond_.Signal();
}

int CtpClient::TickSize()
{
	Locker locker(&ticks_lock_);
	return ticks_.size();
}

void CtpClient::OnRun()
{
	while (IsRuning())
	{
		while (TickSize() == 0)
		{
			mutex_.Lock();
			cond_.Wait(&mutex_);
			mutex_.Unlock();
		}

		FutureTick* tick = NULL;
		while ((tick = PopTick()) != NULL)
		{
			APP_LOG(LOG_LEVEL_INFO)<<tick->symbol.instrument << tick->last_price;
			data_writer_->PushTick(tick);
			delete tick;
		}
	}
}

FutureTick* CtpClient::PopTick()
{
	Locker locker(&ticks_lock_);
	if (0 == ticks_.size()) { return NULL; }
	FutureTick* tick = ticks_.front();
	ticks_.pop();
	return tick;
}

bool CtpClient::Init(bool is_day, std::string& err)
{
	if (!data_writer_->Init(err, !is_day)) return false;
	Thread::Start();

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

	SymbolInfoSet* info_set = SymbolInfoSet::Instance();
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
	
	if (!m_market_api->Subscribe(sub_syms_, err)) return false;

	is_init_ = true;
	return true;
}

void CtpClient::Denit()
{
	Thread::Stop();
	Thread::Join();

	if (m_market_api) m_market_api->Deinit();
	if (data_writer_) data_writer_->Denit();
	SymbolInfoSet::Instance()->Deinit();
	is_init_ = false;
}

