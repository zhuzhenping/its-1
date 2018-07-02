#include "ctp/CtpFutureMarketDataApi.h"
#include <algorithm>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QDate>
#include <QtCore/QStringList>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "common/DateTime.h"
#include "common/AppLog.h"
#include "common/Directory.h"
#include "common/StatusDefine.h"
#include "datalib/SymbolInfoSet.h"

////namespace itstation {
////namespace itstation {

/*------------------------------------------------------------------*
 |					CtpFutureMarketDataHandler模块					    |
 *------------------------------------------------------------------*/

void CtpFutureMarketDataHandler::OnRspError(CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	char request_id_str[16];
	sprintf_s(request_id_str, 16, "%d", request_id);
	api_->spi_->OnMdError("common", response_infomation->ErrorMsg, request_id_str);
}

void CtpFutureMarketDataHandler::OnFrontDisconnected(int reason)
{
	if (!api_->m_succed_connect)
		reconn_ = false;
	else
		reconn_ = true;
	api_->m_succed_connect = false;
	api_->m_succed_login = false;

	std::string error_msg = "CTP front disconnect -> ";
	switch(reason) {
	case 0x1001:
		error_msg += "socket read error";
		break;
	case 0x1002:
		error_msg += "socket write error";
		break;
	case 0x2001:
		error_msg += "heart beat timeout";
		break;
	case 0x2002:
		error_msg += "send heart beat error";
		break;
	case 0x2003:
		error_msg += "receive error message";
		break;
	default:
		error_msg += "other error";
	}

	api_->spi_->OnMdDisconnect(error_msg);
}

void CtpFutureMarketDataHandler::OnHeartBeatWarning(int time_lapse)
{
	char error_msg[64];
	sprintf_s(error_msg, 64, "heart beat warning -> have not receive heart beat in %d seconds", time_lapse);
	api_->spi_->OnMdDisconnect(error_msg);
}

void CtpFutureMarketDataHandler::OnFrontConnected()
{
	api_->m_succed_connect = true;

	if (!reconn_) {	 //主动登录的返回
		api_->ReleaseWait();
	} 
	else {	//断线自动重连
		if (!api_->ReLogin(api_->error_msg_)) {
			cout << "ReLogin error:\t"<<api_->error_msg_<<endl;		
			APP_LOG(LOG_LEVEL_ERROR) << "ReLogin error:\t"<<api_->error_msg_;
		}
	}
}

void CtpFutureMarketDataHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *rsp_user_login, 
		CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (IsErrorInfo(response_infomation)) {
		api_->error_msg_ = std::string("error on response ReqUserLogin ->") + response_infomation->ErrorMsg;
	} 
	else {
		api_->m_succed_login = true;
	}

	if (!reconn_)
		api_->ReleaseWait();
	else { // 是断线重连，就得重新订阅行情
		if (!api_->ReReqMarketPrice(api_->error_msg_)) {
			cout << "ReReqMarketPrice error:\t"<<api_->error_msg_<<endl;		
			APP_LOG(LOG_LEVEL_ERROR) << "ReReqMarketPrice error:\t"<<api_->error_msg_;
		}
		reconn_ = false;
	}
}

void CtpFutureMarketDataHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *specific_instrument, 
		CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (is_last && IsErrorInfo(response_infomation)) {
		char request_id_str[128];
		sprintf_s(request_id_str, 128, "%d", request_id);
		api_->spi_->OnMdError("SubscribeMarketData", response_infomation->ErrorMsg, request_id_str);
	}
}

void CtpFutureMarketDataHandler::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *specific_instrument, 
		CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (is_last && IsErrorInfo(response_infomation)) {
		char request_id_str[128];
		sprintf_s(request_id_str, 128, "%d", request_id);
		api_->spi_->OnMdError("UnSubscribeMarketData", response_infomation->ErrorMsg, request_id_str);
	}
}

void CtpFutureMarketDataHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *depth_market_data)
{
#if 0
	if (strlen(depth_market_data->InstrumentID) > 8) { // 期货合约名的长度一般为6位或5位
		OptionTick tick;
		tick.symbol.product = PRODUCT_OPTION;
		strcpy(tick.symbol.instrument, depth_market_data->InstrumentID);
		//期权合约所属交易所的信息 ?
		Time tick_time(depth_market_data->UpdateTime, depth_market_data->UpdateMillisec);
		Date tick_date(depth_market_data->TradingDay);
		DateTime tmp_time(tick_date, tick_time);
		if (tmp_time < api_->low_time_ || tmp_time > api_->high_time_)
		{
			cout << "clean tick " << tick.symbol.instrument << " : " << tmp_time.Str() << " (" <<  api_->low_time_.Str() << ", " << api_->high_time_.Str() << ")"<< endl;
			APP_LOG(LOG_LEVEL_INFO)<<"clean tick " << tick.symbol.instrument << " : " << tmp_time.Str();
			return;
		}
		tick.date_time = SimpleDateTime(SimpleDate(tick_date.year, tick_date.month, tick_date.day)
			, SimpleTime(tick_time.hour, tick_time.minute, tick_time.sec, tick_time.milsec));

		tick.last_price = depth_market_data->LastPrice;
		tick.volume = depth_market_data->Volume;		
		tick.amount = depth_market_data->Turnover;
		tick.pre_close = depth_market_data->PreClosePrice;
		tick.today_open = depth_market_data->OpenPrice;
		tick.today_high = depth_market_data->HighestPrice;
		tick.today_low = depth_market_data->LowestPrice;
		tick.open_interest = depth_market_data->OpenInterest;
		tick.ask_price = depth_market_data->AskPrice1;
		tick.ask_volume = depth_market_data->AskVolume1;
		tick.bid_price = depth_market_data->BidPrice1;
		tick.bid_volume = depth_market_data->BidVolume1;
		//计算期权的风险指标
		api_->spi_->OnMarketPrice(&tick);
		return;
	}
#endif

	FutureTick tick;
	strcpy(tick.symbol.instrument, depth_market_data->InstrumentID);
	tick.symbol.product = PRODUCT_FUTURE;
	tick.symbol.exchange = SymbolInfoSet::GetInstance()->GetFutureExchange(depth_market_data->InstrumentID);

	Date tick_date(depth_market_data->ActionDay);
	Time tick_time(depth_market_data->UpdateTime, depth_market_data->UpdateMillisec);	
	//对于夜盘，上期所、郑商所的ActionDay为当前日期，大商所actionDay为交易日
	if ((tick_time.hour >= 20 || tick_time.hour < 3) && tick.symbol.exchange == EXCHANGE_DCE)
	{
		tick_date.AddDays(-1);
		while (tick_date.WeekDay() > 5 || tick_date.IsHoliday())
			tick_date.AddDays(-1);
		if (tick_time.hour < 3) tick_date.AddDays(1);
	}

	DateTime tmp_time(tick_date, tick_time);
	if (tmp_time < api_->low_time_ || tmp_time > api_->high_time_)
	{
		//DateTime now(NULL);		
		//APP_LOG_DBG<<"dirty data " << tick.symbol.instrument << " : tick time " << tmp_time.Str() <<"\t actual time " << now.Str();
		//return;
	}

	tick.date_time = SimpleDateTime(SimpleDate(tick_date.year, tick_date.month, tick_date.day)
		, SimpleTime(tick_time.hour, tick_time.minute, tick_time.sec, tick_time.milsec));
	
	tick.last_price = depth_market_data->LastPrice;
	tick.volume = depth_market_data->Volume;
	tick.open_interest = depth_market_data->OpenInterest;
	tick.ask_price = depth_market_data->AskPrice1 > 1e100 ? NAN : depth_market_data->AskPrice1;
	tick.ask_volume = depth_market_data->AskVolume1;
	tick.bid_price = depth_market_data->BidPrice1 > 1e100 ? NAN : depth_market_data->BidPrice1;
	tick.bid_volume = depth_market_data->BidVolume1;
	tick.pre_close = depth_market_data->PreClosePrice;
	tick.today_open = depth_market_data->OpenPrice > 1e100 ? NAN : depth_market_data->OpenPrice;
	tick.today_high = depth_market_data->HighestPrice > 1e100 ? NAN : depth_market_data->HighestPrice;
	tick.today_low = depth_market_data->LowestPrice > 1e100 ? NAN : depth_market_data->LowestPrice;
	tick.amount = depth_market_data->Turnover;
	if (tick.symbol.exchange == EXCHANGE_CZCE) { // 乘以合约数量乘数.
		tick.amount *= SymbolInfoSet::GetInstance()->GetVolMulti(tick.symbol);
	}
	tick.pre_settlement = depth_market_data->PreSettlementPrice;

	tick.pre_open_interest = depth_market_data->PreOpenInterest;
	tick.up_limit = depth_market_data->UpperLimitPrice;
	tick.drop_limit = depth_market_data->LowerLimitPrice;

	api_->spi_->OnMarketPrice(&tick);
}

bool CtpFutureMarketDataHandler::IsErrorInfo(CThostFtdcRspInfoField *response_infomation) {
	// 如果ErrorID != 0, 说明收到了错误的响应
	return (response_infomation) && (response_infomation->ErrorID != 0);
}

/*------------------------------------------------------------------*
 |					CtpFutureMarketDataApi模快						|
 *------------------------------------------------------------------*/

CtpFutureMarketDataApi::CtpFutureMarketDataApi(void)
		:m_ctp_market_api(NULL)
		,m_ctp_market_handler(NULL)
		,m_request_id(0) 
{
	m_ctp_market_handler = new CtpFutureMarketDataHandler(this); 
}


CtpFutureMarketDataApi::~CtpFutureMarketDataApi(void) 
{
	if (NULL != m_ctp_market_handler) { delete m_ctp_market_handler; }
	if (m_succed_connect || m_succed_login)
		Deinit();
}

bool CtpFutureMarketDataApi::Init(const std::string& front_addr_str, MarketDataSpi* spi, std::string& err) 
{
	if (m_succed_connect) return true;
	spi_ = spi;

	char* its_home = getenv("ITS_HOME");
	if (NULL == its_home){ err = "ITS_HOME == NULL"; return false;}

	if (holidays_ == NULL) 
	{
		QDate cur_date = QDate::currentDate();
		std::string conf_path = std::string(its_home) + "/cfg/TradingTime.xml";
		if (!QFile::exists(conf_path.c_str())) {err = conf_path + " is not exist";return false;}
		XmlConfig config2(conf_path);
		if (!config2.Load()) { return false; }
		XmlNodeVec node_vec = config2.FindChileren("Holidays");
		for (int i=0; i < node_vec.size(); ++i)
		{
			if (atoi(node_vec[i].GetValue("Year").c_str()) == cur_date.year())
			{
				holidays_ = new QStringList(QString(node_vec[i].GetValue("Days").c_str()).split(','));
				break;
			}
		}
	}

	if (m_ctp_market_api == NULL) {
		std::string data_path = Global::GetInstance()->its_home+ "\\log\\ctp\\";
		if (!Directory::IsDirExist(data_path) && !Directory::MakeDir(data_path)) { err = "MakeDir:"+data_path+" false"; return false;}
		m_ctp_market_api = CThostFtdcMdApi::CreateFtdcMdApi(data_path.c_str());
		if (NULL == m_ctp_market_handler) { m_ctp_market_handler = new CtpFutureMarketDataHandler(this); }

		m_ctp_market_api->RegisterSpi(m_ctp_market_handler);
		char front_addr[128];
		memcpy(front_addr, front_addr_str.c_str(), 128);
		m_ctp_market_api->RegisterFront(front_addr);
		m_ctp_market_api->RegisterFront(front_addr);
		m_ctp_market_api->RegisterFront(front_addr);
		m_ctp_market_api->Init();

		if (!TimeWait(kInitWaitTime))
		{
			err = "time out for return of RegisterFront";
			return false;
		}
		if (!m_succed_connect) { 
			err = error_msg_;
			return false; 
		}
	}

	return true;
}

void CtpFutureMarketDataApi::Deinit()
{
	//if (!m_succed_connect && !m_succed_login) return;
	m_succed_connect = false;
	m_succed_login = false;
	if (m_ctp_market_api != NULL)
	{
		m_ctp_market_api->Release();
		m_ctp_market_api = NULL;
	}

	if (holidays_ != NULL) {
		delete holidays_;
		holidays_ = NULL;
	}
}

bool CtpFutureMarketDataApi::Login(const std::string& broker_id, const std::string& user_id, const std::string& password, std::string& err) 
{
	if ("" == broker_id /*|| "" == user_id || "" == password*/) 
	{
		err = "the arguemnt is empty";
		return false;
	}

	m_broker_id = broker_id;
	m_user_id = user_id;
	m_password = password;

	if (!m_succed_connect) 
	{
		err = "has not connected to CTP front address";
		return false;
	}
	if (m_succed_login) return true;	

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(req.UserID, user_id.c_str(), sizeof(TThostFtdcUserIDType));
	strncpy(req.Password, password.c_str(), sizeof(TThostFtdcPasswordType));
	int result = m_ctp_market_api->ReqUserLogin(&req, ++m_request_id);
	if (0 != result) 
	{
		err = "fail to ReqUserLogin";
		return false;
	}

	if (!TimeWait(kLoginWaitTime)) 
	{
		err = "time out for return of ReqUserLogin";
		return false;
	}

	if (!m_succed_login) { 
		err = error_msg_;
		return false; 
	}

	if (!SymbolInfoSet::GetInstance()->Init(err)) return false;

	QDateTime now_t = QDateTime::currentDateTime();
	low_time_.date = Date(now_t.date().year(), now_t.date().month(), now_t.date().day());
	high_time_.date = low_time_.date;
	if (now_t.time() >= QTime(2, 45) && now_t.time() < QTime(15, 30))
	{
		low_time_.time = Time(8, 55, 0);
		high_time_.time = Time(15, 20, 0);
	}
	else
	{
		low_time_.time = Time(20, 55, 0);
		high_time_.time = Time(2, 35, 0);
		if (now_t.time() < QTime(2, 45))
		{
			QDate date = now_t.date().addDays(-1);
			low_time_.date = Date(date.year(), date.month(), date.day());
		}
		else
		{
			QDate date = now_t.date().addDays(1);
			high_time_.date = Date(date.year(), date.month(), date.day());
		}
	}

	return true;
}

bool CtpFutureMarketDataApi::Logout(std::string& err)
{
	if (!m_succed_login) { return true; }

	m_symbols.clear();
	CThostFtdcUserLogoutField req = {0};
	strncpy(req.BrokerID,  m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	int result = m_ctp_market_api->ReqUserLogout(&req, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqUserLogout";
		return false;
	}

	m_succed_login = false;
	return true;
}

bool CtpFutureMarketDataApi::Subscribe(const std::string& symbol, std::string& err) 
{
	 std::vector<std::string> inst_vec;
	 Split(symbol, ",", &inst_vec);
	 int len = inst_vec.size();
	 char** insts  = new char*[len];
	 for (int i=0; i < len; ++i)
	 {
		 insts[i] = new char[inst_vec[i].size() + 1];
		 strcpy(insts[i], inst_vec[i].c_str());
	 }


	int result = m_ctp_market_api->SubscribeMarketData(insts, len);
	if (0 != result) 
	{
		for (int i=0; i < len; ++i)
		{
			delete [] insts[i];
		}
		delete [] insts;

		err = "fail to SubscribeMarketData";
		return false;
	}

	for (int i=0; i < len; ++i)
	{
		m_symbols.push_back(inst_vec[i]);
		delete [] insts[i];
	}
	delete [] insts;
	return true;
}

bool CtpFutureMarketDataApi::UnSubscribe(const std::string& symbol, std::string& err) 
{
	std::list<std::string>::iterator ite;
	ite = std::find(m_symbols.begin(), m_symbols.end(), symbol);
	if (ite == m_symbols.end()) { return false; }	//没订阅过，直接返回

	char* symbols[1] = {const_cast<char*>(symbol.c_str())};
	int result = m_ctp_market_api->UnSubscribeMarketData(symbols, 1);
	if (0 != result) 
	{
		err = "fail to UnSubscribeMarketData";
		return false;
	}

	//m_symbols.remove(symbol);
	m_symbols.erase(ite);
	return true;
}

void CtpFutureMarketDataApi::Join() 
{
	m_ctp_market_api->Join();
}

void CtpFutureMarketDataApi::Trim(std::string &s) 
{
	if (s.empty()) 
	{
		return;
	}

	s.erase(0,s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
}

void CtpFutureMarketDataApi::Split(const std::string& s, const std::string delim,std::vector<std::string>* ret)
{
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		std::string part_str = s.substr(last,index-last);
		Trim(part_str);
		if (!part_str.empty() && std::find(m_symbols.end(), m_symbols.end(), part_str) == m_symbols.end()) 
		{ 
			ret->push_back(part_str); 
		}
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
	if (index - last > 0)  
	{  
		std::string part_str = s.substr(last,index-last);
		Trim(part_str);
		if (!part_str.empty() && std::find(m_symbols.end(), m_symbols.end(), part_str) == m_symbols.end()) 
		{ 
			ret->push_back(part_str); 
		}
	}  
}


bool CtpFutureMarketDataApi::ReLogin(std::string& err) 
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID,  m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(req.UserID, m_user_id.c_str(), sizeof(TThostFtdcUserIDType));
	strncpy(req.Password, m_password.c_str(), sizeof(TThostFtdcPasswordType));
	int result = m_ctp_market_api->ReqUserLogin(&req, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqUserLogin";
		return false;
	}

	return true;
}

bool CtpFutureMarketDataApi::ReReqMarketPrice(std::string& err) 
{
	char* symbols[100];
	int num = 0;
	std::list<std::string>::const_iterator kite;
	for(kite = m_symbols.begin(); kite != m_symbols.end() && num < 100; ++kite) {
		symbols[num] = const_cast<char*>((*kite).c_str());
		++num;
	}

	int result = m_ctp_market_api->SubscribeMarketData(symbols, num);
	if (0 != result) {
		err = "fail to SubscribeMarketData";
		return false;
	}

	return true;
}

