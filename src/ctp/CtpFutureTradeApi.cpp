#include "ctp/CtpFutureTradeApi.h"
#include <stdlib.h>
#include <sstream>
#include "common/Thread.h"
#include "common/DateTime.h"
#include "common/AppLog.h"
#include "common/SimpleDateTime.h"
#include "common/Directory.h"
using namespace std;

//namespace itstation {

//namespace marketapi {

/*------------------------------------------------------------------*
 |					CtpFutureTradeHandler模块					    |
 *------------------------------------------------------------------*/

void CtpFutureTradeHandler::OnRspError(CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last) {
	m_market_trade_api->spi_->OnError(request_id, response_infomation->ErrorMsg);
}

void CtpFutureTradeHandler::OnFrontDisconnected(int reason) {
	m_market_trade_api->m_succed_connect = false;
	m_market_trade_api->m_succed_login = false;
	m_reconnected = true;

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

	m_market_trade_api->spi_->OnDisconnect(error_msg);
}

void CtpFutureTradeHandler::OnHeartBeatWarning(int time_lapse) {
	char error_msg[64];
	sprintf_s(error_msg, 64, "heart beat warning -> have not receive heart beat in %d seconds", time_lapse);
	m_market_trade_api->spi_->OnDisconnect(error_msg);
}

void CtpFutureTradeHandler::OnFrontConnected() {
	m_market_trade_api->m_succed_connect = true;

	if (!m_reconnected) {	 //主动登录的返回
		m_market_trade_api->ReleaseWait();
	} 
	else {	//断线自动重连
		m_market_trade_api->spi_->OnConnect();
		std::string err;
		if (!m_market_trade_api->been_logout_)
		{
			if (!m_market_trade_api->ReLogin(err)) 
			{
				m_market_trade_api->spi_->OnError(m_market_trade_api->m_request_id, "error to relogin CTP");
			}
			else
			{
				//std::cout << "断线重连成功" << std::endl;
			}
		}
		
		m_reconnected = false;
	}
}

void CtpFutureTradeHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *rsp_user_login, 
		CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)  {
	if (NULL == rsp_user_login) { return; }

	if (IsErrorInfo(response_infomation)) {
		m_market_trade_api->error_msg_ = std::string("error on response ReqUserLogin ->") + response_infomation->ErrorMsg;
	} 
	else {
		//std::cout << "login succed" << std::endl;
		m_market_trade_api->m_succed_login = true;
		m_market_trade_api->m_order_ref = atol(rsp_user_login->MaxOrderRef);
		m_market_trade_api->m_front_id = rsp_user_login->FrontID;
		m_market_trade_api->m_session_id = rsp_user_login->SessionID;
	}

	m_market_trade_api->ReleaseWait();
}

void CtpFutureTradeHandler::OnRspUserLogout(CThostFtdcUserLogoutField* rsp_user_logout, 
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last)  {
	if (NULL == rsp_user_logout) { return; }

	if (IsErrorInfo(response_infomation)) {
		m_market_trade_api->error_msg_ = std::string("error on response ReqUserLogout ->") + response_infomation->ErrorMsg;
	} 
	else {
		m_market_trade_api->m_succed_login = false;
	}

	m_market_trade_api->ReleaseWait();
}

ExchangeIdType CtpFutureTradeHandler::GetExchangeId(const char* id)
{
	if (strcmp(id, "CFFEX") == 0) { return EXCHANGE_CFFEX; }
	else if (strcmp(id, "CZCE") == 0) {  return EXCHANGE_CZCE; }
	else if (strcmp(id, "DCE") == 0) { return EXCHANGE_DCE; }
	else if (strcmp(id, "SHFE") == 0) { return EXCHANGE_SHFE; }
	else if (strcmp(id, "INE") == 0) { return EXCHANGE_INE; }
	else if (strcmp(id, "SSE") == 0) { return EXCHANGE_SSE; }
	else if (strcmp(id, "SZE") == 0) { return EXCHANGE_SZE; }
	else { return EXCHANGE_OTHER; }
}

std::string CtpFutureTradeApi::ToExchangeStr(ExchangeIdType id)
{
	switch(id)
	{
	case EXCHANGE_CFFEX: return "CFFEX";
	case EXCHANGE_CZCE: return "CZCE";
	case EXCHANGE_DCE: return "DCE";
	case EXCHANGE_SHFE: return "SHFE";  //SHFE
	case EXCHANGE_INE: return "INE";
	case EXCHANGE_SSE: return "SSE";
	case EXCHANGE_SZE: return "SZE";
	default:
		return "";
	}
}

void CtpFutureTradeHandler::OnRspQryInstrument(CThostFtdcInstrumentField *rsp_inst, 
	CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (NULL == rsp_inst || NULL == m_market_trade_api->sec_info_spi_) { return; }

	//if (IsErrorInfo(response_infomation)) { return; }

	//static std::ofstream ofs("tmp1.log");
	//ofs << rsp_inst->ExchangeID << " " << rsp_inst->InstrumentID<<" "<<rsp_inst->InstrumentName<<endl;

	//因为最后一个不一定是期货，所以加以下处理
	if (rsp_inst->ProductClass == THOST_FTDC_PC_Futures) 
	{ 
		InstrumentInfoData info;
		strcpy(info.symbol.instrument, rsp_inst->InstrumentID);
		if ((info.symbol.exchange = GetExchangeId(rsp_inst->ExchangeID)) == EXCHANGE_OTHER) { return; }
		info.symbol.product = PRODUCT_FUTURE;

		strcpy(info.symbol.name, rsp_inst->InstrumentName);
		info.vol_multi = rsp_inst->VolumeMultiple;
		info.price_tick = rsp_inst->PriceTick;
		info.is_trading = rsp_inst->IsTrading;
		info.long_margin_ratio = rsp_inst->LongMarginRatio;
		info.short_margin_ratio = rsp_inst->ShortMarginRatio;

		if (NULL == pre_info_)
		{
			pre_info_ = new InstrumentInfoData(info);
		}
		else
		{
			m_market_trade_api->sec_info_spi_->OnInstrumentInfo(*pre_info_, false);		
			*pre_info_ = info;
		}
	}

	if (is_last && NULL != pre_info_)
	{
		m_market_trade_api->sec_info_spi_->OnInstrumentInfo(*pre_info_, true);
		delete pre_info_;
		pre_info_ = NULL;
	}
}

void CtpFutureTradeHandler::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (NULL == pInstrumentMarginRate || NULL == m_market_trade_api->sec_info_spi_) { return; }
	MarginInfo margin;
	strcpy(margin.instrument, pInstrumentMarginRate->InstrumentID);
	margin.LongMarginRatioByMoney = pInstrumentMarginRate->LongMarginRatioByMoney;
	margin.LongMarginRatioByVolume = pInstrumentMarginRate->LongMarginRatioByVolume;
	margin.ShortMarginRatioByMoney = pInstrumentMarginRate->ShortMarginRatioByMoney;
	margin.ShortMarginRatioByVolume = pInstrumentMarginRate->ShortMarginRatioByVolume;
	m_market_trade_api->sec_info_spi_->OnMarginInfo(margin, bIsLast);
}

///请求查询合约手续费率响应
void CtpFutureTradeHandler::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (NULL == pInstrumentCommissionRate || NULL == m_market_trade_api->sec_info_spi_) { return; }
	CommisionInfo commsion;
	strcpy(commsion.instrument, pInstrumentCommissionRate->InstrumentID);
	commsion.OpenRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;
	commsion.OpenRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;
	commsion.CloseRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;
	commsion.CloseRatioByVolume = pInstrumentCommissionRate->CloseRatioByVolume;
	commsion.CloseTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;
	commsion.CloseTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;
	m_market_trade_api->sec_info_spi_->OnCommisionInfo(commsion, bIsLast);
}

void CtpFutureTradeHandler::OnRspOrderInsert(CThostFtdcInputOrderField* market_order, 
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) 
{
	return; // 目测与OnErrRtnOrderInsert重复。以后发现不对劲 再处理
	if (NULL == market_order) { return; }

	OrderData order_data;
	strcpy(order_data.symbol.instrument, market_order->InstrumentID);
	order_data.symbol.product = PRODUCT_FUTURE;
	order_data.order_id = 0;
	strncpy(order_data.user_tag, market_order->UserID, sizeof(UserStrategyIdType));
	order_data.status = ORDER_STATUS_ERROR;
	order_data.submit_time = DateTime(NULL);
	order_data.update_time = DateTime(NULL);
	if ('0' == market_order->Direction) {
		order_data.direction = LONG_DIRECTION;
	}
	else {
		order_data.direction = SHORT_DIRECTION;
	}

	switch(market_order->CombOffsetFlag[0]) {	
	case '0':
		order_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		order_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		order_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		order_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		order_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	if (market_order->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		order_data.order_price_flag = LIMIT_PRICE_ORDER;
	} 
	else {
		order_data.order_price_flag = MARKET_PRICE_ORDER;
	}

	order_data.limit_price = market_order->LimitPrice;
	order_data.total_volume = market_order->VolumeTotalOriginal;
	order_data.trade_volume = 0;
	order_data.hedge_flag = market_order->CombOffsetFlag[0];	
	strcpy(order_data.status_msg, response_infomation->ErrorMsg);
	m_market_trade_api->spi_->OnOrderError(&order_data);
}

void CtpFutureTradeHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField* market_order, CThostFtdcRspInfoField* response_infomation) 
{
	if (NULL == market_order) { return; }

	OrderData order_data;
	strcpy(order_data.symbol.instrument, market_order->InstrumentID);
	order_data.symbol.product = PRODUCT_FUTURE;
	order_data.order_id = 0;
	strncpy(order_data.user_tag, market_order->UserID, sizeof(UserStrategyIdType));
	order_data.status = ORDER_STATUS_ERROR;
	order_data.submit_time = DateTime(NULL);
	order_data.update_time = DateTime(NULL);
	if ('0' == market_order->Direction) {
		order_data.direction = LONG_DIRECTION;
	}
	else {
		order_data.direction = SHORT_DIRECTION;
	}

	switch(market_order->CombOffsetFlag[0]) {	
	case '0':
		order_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		order_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		order_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		order_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		order_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	if (market_order->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		order_data.order_price_flag = LIMIT_PRICE_ORDER;
	} 
	else {
		order_data.order_price_flag = MARKET_PRICE_ORDER;
	}

	order_data.limit_price = market_order->LimitPrice;
	order_data.total_volume = market_order->VolumeTotalOriginal;
	order_data.trade_volume = 0;
	order_data.hedge_flag = market_order->CombOffsetFlag[0];	
	strcpy(order_data.status_msg, response_infomation->ErrorMsg);
	m_market_trade_api->spi_->OnOrderError(&order_data);
}

void CtpFutureTradeHandler::OnRtnOrder(CThostFtdcOrderField* market_order) {
	if (NULL == market_order) { return; }

	OrderData order_data;
	if ((order_data.symbol.exchange = GetExchangeId(market_order->ExchangeID)) == EXCHANGE_OTHER) { return; }
	strcpy(order_data.symbol.instrument, market_order->InstrumentID);
	order_data.symbol.product = PRODUCT_FUTURE;
	//strcpy(order_data.order_id, market_order->OrderSysID);
	if (strcmp(market_order->OrderSysID, "") == 0) { order_data.order_id = 0; }
	else
	{
		stringstream ss;
		ss << market_order->OrderSysID;
		ss >> order_data.order_id;
	}

	strncpy(order_data.user_tag, market_order->UserID, sizeof(UserStrategyIdType));
	order_data.status = ToOrderStatus(market_order->OrderStatus);

	// 快期把订单状态为无效&&OrderSysID为空的单子过滤掉了
	if (order_data.status == ORDER_STATUS_INVALID && 0 == order_data.order_id) return;

	order_data.submit_time.date = Date(market_order->InsertDate);
	order_data.submit_time.time = Time(market_order->InsertTime, 0);
	order_data.update_time.date = DateTime(NULL).date;
	order_data.update_time.time = Time(market_order->UpdateTime, 0);

	//DateTime submit_time(Date(market_order->InsertDate), Time(market_order->InsertTime, 0));
	//DateTime update_time(Date(DateTime(NULL).date), Time(market_order->UpdateTime, 0));
	/*order_data.submit_time = DateTime(SimpleDate(submit_time.date.year, submit_time.date.month, submit_time.date.day)
		, SimpleTime(submit_time.time.hour, submit_time.time.minute, submit_time.time.sec, submit_time.time.milsec));
	order_data.update_time = SimpleDateTime(SimpleDate(update_time.date.year, update_time.date.month, update_time.date.day)
		, SimpleTime(update_time.time.hour, update_time.time.minute, update_time.time.sec, update_time.time.milsec));*/
	
	if ('0' == market_order->Direction) {
		order_data.direction = LONG_DIRECTION;
	}
	else {
		order_data.direction = SHORT_DIRECTION;
	}

	switch(market_order->CombOffsetFlag[0]) {	
	case '0':
		order_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		order_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		order_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		order_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		order_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	if (market_order->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		order_data.order_price_flag = LIMIT_PRICE_ORDER;
	} 
	else {
		order_data.order_price_flag = MARKET_PRICE_ORDER;
	}
	
	order_data.limit_price = market_order->LimitPrice;
	order_data.total_volume = market_order->VolumeTotalOriginal;
	order_data.trade_volume = market_order->VolumeTraded;
	order_data.hedge_flag = market_order->CombOffsetFlag[0];		
	strcpy(order_data.status_msg, market_order->StatusMsg);

	m_market_trade_api->spi_->OnOrder(&order_data);
}

void CtpFutureTradeHandler::OnRtnTrade(CThostFtdcTradeField* market_trade) {
	if (NULL == market_trade) { return; }

	TradeData trade_data;
	if ((trade_data.symbol.exchange = GetExchangeId(market_trade->ExchangeID)) == EXCHANGE_OTHER) { return; }
	strcpy(trade_data.symbol.instrument, market_trade->InstrumentID);
	trade_data.symbol.product = PRODUCT_FUTURE;

	stringstream ss;
	ss << market_trade->OrderSysID;
	ss >> trade_data.order_id;
	stringstream ss2;
	ss2 << market_trade->TradeID;
	ss2 >> trade_data.trade_id;
	tradeid2stratid_[market_trade->TradeID] = market_trade->UserID;

	strncpy(trade_data.user_tag, market_trade->UserID, sizeof(UserStrategyIdType));

	trade_data.trade_time.date = Date(market_trade->TradeDate);
	trade_data.trade_time.time = Time(market_trade->TradeTime, 0);

	//DateTime trade_time(Date(market_trade->TradeDate), Time(market_trade->TradeTime, 0));
	//trade_data.trade_time = SimpleDateTime(SimpleDate(trade_time.date.year, trade_time.date.month, trade_time.date.day)
	//	, SimpleTime(trade_time.time.hour, trade_time.time.minute, trade_time.time.sec, trade_time.time.milsec));

	if ('0' == market_trade->Direction) {
		trade_data.direction = LONG_DIRECTION;
	}
	else {
		trade_data.direction = SHORT_DIRECTION;
	}

	switch(market_trade->OffsetFlag) {
	case '0':
		trade_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		trade_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		trade_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		trade_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		trade_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	trade_data.trade_price = market_trade->Price;
	trade_data.trade_volume = market_trade->Volume;

	m_market_trade_api->spi_->OnTrade(&trade_data);
}

void CtpFutureTradeHandler::OnRspQryTradingAccount(CThostFtdcTradingAccountField* account_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) 
{
	if (NULL == account_field) { 
		m_market_trade_api->spi_->OnQryAccount(request_id, nullptr, "", is_last);
		return; 
	}

	if (is_last && IsErrorInfo(response_infomation)) {
		m_market_trade_api->spi_->OnQryAccount(request_id, NULL, response_infomation->ErrorMsg, true);
		return;
	}

	AccountData account_data;
	strcpy(account_data.account_id, account_field->AccountID);
	strcpy(account_data.broker_id, account_field->BrokerID);
	account_data.money_type = MONEY_RMB;
	account_data.static_balance = account_field->PreBalance - account_field->PreCredit - account_field->PreMortgage
		+ account_field->Mortgage - account_field->Withdraw + account_field->Deposit;
	account_data.asset_balance = account_data.static_balance + account_field->CloseProfit + account_field->PositionProfit
		- account_field->Commission;
	account_data.enable_balance = account_field->Available;
	account_data.withdraw_balance = account_field->WithdrawQuota;
	account_data.frozen_balance = account_field->FrozenMargin + account_field->FrozenCommission;
	account_data.close_profit = account_field->CloseProfit;	
	account_data.commision = account_field->Commission;
	account_data.margin_balance = account_field->CurrMargin;
	account_data.delivery_margin = account_field->DeliveryMargin;	

	m_market_trade_api->spi_->OnQryAccount(request_id, &account_data, "", is_last);
}

void CtpFutureTradeHandler::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* position_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) {
 	if (NULL == position_field) { 
		m_market_trade_api->spi_->OnQryPosition(request_id, NULL, "", true);
		return; 
	}

	if (is_last && IsErrorInfo(response_infomation)) {
		m_market_trade_api->spi_->OnQryPosition(request_id, NULL, response_infomation->ErrorMsg, true);
		return;
	}

	PositionData position_data;
	if (tradeid2stratid_.find(position_field->TradeID) == tradeid2stratid_.end())
		strncpy(position_data.user_tag, position_field->InvestorID, sizeof(UserStrategyIdType));
	else
		strncpy(position_data.user_tag, tradeid2stratid_[position_field->TradeID].c_str(), sizeof(UserStrategyIdType));

	if ((position_data.symbol.exchange = GetExchangeId(position_field->ExchangeID)) == EXCHANGE_OTHER) { return; }
	strcpy(position_data.symbol.instrument, position_field->InstrumentID);
	position_data.symbol.product = PRODUCT_FUTURE;

	position_data.direction = position_field->Direction == THOST_FTDC_D_Buy ? LONG_DIRECTION : SHORT_DIRECTION;
	
	position_data.open_volume = position_field->Volume;	
	position_data.using_margin = position_field->Margin;

	if (strcmp(position_field->OpenDate, position_field->TradingDay) != 0) // 昨仓
	{
		position_data.yestd_volume = position_data.open_volume;
		position_data.enable_yestd_volume = position_data.open_volume;
		position_data.open_price = position_field->LastSettlementPrice;
	}
	else // 今仓
	{
		position_data.today_volume = position_data.open_volume;
		position_data.enable_today_volume = position_data.open_volume;
		position_data.open_price = position_field->OpenPrice;
	}

	m_market_trade_api->spi_->OnQryPosition(request_id, &position_data, "", is_last);
}

void CtpFutureTradeHandler::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* confirm_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) {
	if (IsErrorInfo(response_infomation)) {
		m_market_trade_api->error_msg_ = std::string("error on response ReqQrySettlementInfoConfirm ->") + response_infomation->ErrorMsg;
	}
	else {
		m_market_trade_api->m_succed_query_confirm_info = true;
		char trading_day[32];
		strncpy(trading_day, m_market_trade_api->m_ctp_market_api->GetTradingDay(), sizeof(trading_day));
		m_market_trade_api->m_has_confirmed = (confirm_field) && strcmp(confirm_field->ConfirmDate, trading_day);
	}

	m_market_trade_api->ReleaseWait();
}

void CtpFutureTradeHandler::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField* pettlement_info,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) {
	if (IsErrorInfo(response_infomation)) {
		m_market_trade_api->error_msg_ = std::string("error on response ReqQrySettlementInfo ->") + response_infomation->ErrorMsg;
	}
	else {
		m_market_trade_api->m_succed_query_settlement_info = true;
	}

	m_market_trade_api->ReleaseWait();
}

void CtpFutureTradeHandler::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* confirm_field,
		CThostFtdcRspInfoField* response_infomation, int request_id, bool is_last) {
	if (IsErrorInfo(response_infomation)) {
		m_market_trade_api->error_msg_ = std::string("error on response ReqSettlementInfoConfirm ->") + response_infomation->ErrorMsg;
	}
	else {
		m_market_trade_api->m_has_confirmed = true;
	}

	m_market_trade_api->ReleaseWait();
}

void CtpFutureTradeHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (NULL == pInputOrderAction) { return; }

	/*CancelOrderData param;
	strncpy(param.user_tag, pInputOrderAction->UserID, sizeof(UserStrategyIdType));
	stringstream ss;
	ss << pInputOrderAction->OrderSysID;
	ss >> param.order_id;
	if (IsErrorInfo(pRspInfo))
	{
		param.succed = false;
		strcpy(param.error, pRspInfo->ErrorMsg);
	}*/
	OrderData order_data;
	strcpy(order_data.symbol.instrument, pInputOrderAction->InstrumentID);
	if ((order_data.symbol.exchange = GetExchangeId(pInputOrderAction->ExchangeID)) == EXCHANGE_OTHER) { return; }
	order_data.symbol.product = PRODUCT_FUTURE;
	if (strcmp(pInputOrderAction->OrderSysID, "") == 0) { order_data.order_id = 0; }
	else
	{
		stringstream ss;
		ss << pInputOrderAction->OrderSysID;
		ss >> order_data.order_id;
	}

	strncpy(order_data.user_tag, pInputOrderAction->UserID, sizeof(UserStrategyIdType));
	order_data.status = CANCEL_ORDER_STATUS_ERROR;
	order_data.submit_time = DateTime(NULL);
	order_data.update_time = DateTime(NULL);	
	order_data.limit_price = pInputOrderAction->LimitPrice;
	if (IsErrorInfo(pRspInfo))	{
		strcpy(order_data.status_msg, pRspInfo->ErrorMsg);
	}

	m_market_trade_api->spi_->OnOrderError(&order_data);
}

void CtpFutureTradeHandler::OnErrRtnOrderAction(CThostFtdcOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo) {
	if (NULL == pInputOrderAction) { return; }
	
	OrderData order_data;
	strcpy(order_data.symbol.instrument, pInputOrderAction->InstrumentID);
	if ((order_data.symbol.exchange = GetExchangeId(pInputOrderAction->ExchangeID)) == EXCHANGE_OTHER) { return; }
	order_data.symbol.product = PRODUCT_FUTURE;
	if (strcmp(pInputOrderAction->OrderSysID, "") == 0) { order_data.order_id = 0; }
	else
	{
		stringstream ss;
		ss << pInputOrderAction->OrderSysID;
		ss >> order_data.order_id;
	}

	strncpy(order_data.user_tag, pInputOrderAction->UserID, sizeof(UserStrategyIdType));
	order_data.status = CANCEL_ORDER_STATUS_ERROR;
	order_data.submit_time.date = Date(pInputOrderAction->ActionDate);
	order_data.submit_time.time = Time(pInputOrderAction->ActionTime, 0);
	order_data.update_time = DateTime(NULL);	
	order_data.limit_price = pInputOrderAction->LimitPrice;
	if (IsErrorInfo(pRspInfo))	{
		strcpy(order_data.status_msg, pRspInfo->ErrorMsg);
	}

	m_market_trade_api->spi_->OnOrderError(&order_data);
}

void CtpFutureTradeHandler::OnRspQryOrder(CThostFtdcOrderField *market_order, CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (NULL == market_order) { 
		m_market_trade_api->spi_->OnQryOrder(request_id, NULL, "", true);
		return; 
	}

	if (is_last && IsErrorInfo(response_infomation)) {
		m_market_trade_api->spi_->OnQryOrder(request_id, NULL, response_infomation->ErrorMsg, true);
		return;
	}

	OrderData order_data;
	if ((order_data.symbol.exchange = GetExchangeId(market_order->ExchangeID)) == EXCHANGE_OTHER) { return; }
	strcpy(order_data.symbol.instrument, market_order->InstrumentID);
	order_data.symbol.product = PRODUCT_FUTURE;	
	if (strcmp(market_order->OrderSysID, "") == 0) { order_data.order_id = 0; }
	else
	{
		stringstream ss;
		ss << market_order->OrderSysID;
		ss >> order_data.order_id;
	}

	strncpy(order_data.user_tag, market_order->UserID, sizeof(UserStrategyIdType));
	order_data.status = ToOrderStatus(market_order->OrderStatus);

	order_data.submit_time.date = Date(market_order->InsertDate);
	order_data.submit_time.time = Time(market_order->InsertTime, 0);
	order_data.update_time.date = DateTime(NULL).date;
	order_data.update_time.time = Time(market_order->UpdateTime, 0);

	//DateTime submit_time(Date(market_order->InsertDate), Time(market_order->InsertTime, 0));
	//DateTime update_time(Date(DateTime(NULL).date), Time(market_order->UpdateTime, 0));
	//order_data.submit_time = SimpleDateTime(SimpleDate(submit_time.date.year, submit_time.date.month, submit_time.date.day)
	//	, SimpleTime(submit_time.time.hour, submit_time.time.minute, submit_time.time.sec, submit_time.time.milsec));
	//order_data.update_time = SimpleDateTime(SimpleDate(update_time.date.year, update_time.date.month, update_time.date.day)
	//	, SimpleTime(update_time.time.hour, update_time.time.minute, update_time.time.sec, update_time.time.milsec));

	if ('0' == market_order->Direction) {
		order_data.direction = LONG_DIRECTION;
	}
	else {
		order_data.direction = SHORT_DIRECTION;
	}

	switch(market_order->CombOffsetFlag[0]) {	
	case '0':
		order_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		order_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		order_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		order_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		order_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	if (market_order->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		order_data.order_price_flag = LIMIT_PRICE_ORDER;
	} 
	else {
		order_data.order_price_flag = MARKET_PRICE_ORDER;
	}

	order_data.limit_price = market_order->LimitPrice;
	order_data.total_volume = market_order->VolumeTotalOriginal;
	order_data.trade_volume = market_order->VolumeTraded;
	order_data.hedge_flag = market_order->CombOffsetFlag[0];		
	strcpy(order_data.status_msg, market_order->StatusMsg);

	m_market_trade_api->spi_->OnQryOrder(request_id, &order_data, "", is_last);
}

void CtpFutureTradeHandler::OnRspQryTrade(CThostFtdcTradeField *market_trade, CThostFtdcRspInfoField *response_infomation, int request_id, bool is_last)
{
	if (NULL == market_trade) {
		m_market_trade_api->spi_->OnQryTrade(request_id, NULL, "", true);
		return; 
	}

	if (is_last && IsErrorInfo(response_infomation)) {
		m_market_trade_api->spi_->OnQryTrade(request_id, NULL, response_infomation->ErrorMsg, true);
		return;
	}

	TradeData trade_data;
	if ((trade_data.symbol.exchange = GetExchangeId(market_trade->ExchangeID)) == EXCHANGE_OTHER) { return; }
	strcpy(trade_data.symbol.instrument, market_trade->InstrumentID);
	trade_data.symbol.product = PRODUCT_FUTURE;
	stringstream ss;
	ss<<market_trade->OrderSysID;
	ss>>trade_data.order_id;

	stringstream ss2;
	ss2<<market_trade->TradeID;
	ss2>>trade_data.trade_id;
	tradeid2stratid_[market_trade->TradeID] = market_trade->UserID;

	strncpy(trade_data.user_tag, market_trade->UserID, sizeof(UserStrategyIdType));

	trade_data.trade_time.date = Date(market_trade->TradeDate);
	trade_data.trade_time.time = Time(market_trade->TradeTime, 0);

	//DateTime trade_time(Date(market_trade->TradeDate), Time(market_trade->TradeTime, 0));
	//trade_data.trade_time = SimpleDateTime(SimpleDate(trade_time.date.year, trade_time.date.month, trade_time.date.day)
	//	, SimpleTime(trade_time.time.hour, trade_time.time.minute, trade_time.time.sec, trade_time.time.milsec));

	if ('0' == market_trade->Direction) {
		trade_data.direction = LONG_DIRECTION;
	}
	else {
		trade_data.direction = SHORT_DIRECTION;
	}

	switch(market_trade->OffsetFlag) {
	case '0':
		trade_data.open_close_flag = OPEN_ORDER;
		break;
	case '1':
		trade_data.open_close_flag = CLOSE_ORDER;
		break;
	case '3':
		trade_data.open_close_flag = CLOSE_TODAY_ORDER;
		break;
	case '4':
		trade_data.open_close_flag = CLOSE_YESTERDAY_ORDER;
		break;
	default:
		trade_data.open_close_flag = FORCE_CLOSE_ORDER;
		break;
	}

	trade_data.trade_price = market_trade->Price;
	trade_data.trade_volume = market_trade->Volume;

	m_market_trade_api->spi_->OnQryTrade(request_id, &trade_data, "", is_last);
}

bool CtpFutureTradeHandler::IsErrorInfo(CThostFtdcRspInfoField *response_infomation) {
	// 如果ErrorID != 0, 说明收到了错误的响应
	return (response_infomation) && (response_infomation->ErrorID != 0);
}

OrderStatus CtpFutureTradeHandler::ToOrderStatus(char status_char) {
	OrderStatus status = ORDER_STATUS_INVALID;
	switch (status_char) {
	case '0':
		status = ORDER_STATUS_ALL_TRADE;
		break;
	case '1':
	case '2':
		status = ORDER_STATUS_PART_TRADE;
		break;
	case '3':
	case '4':
		status = ORDER_STATUS_BEEN_SUBMIT;
		break;
	case '5':
		status = ORDER_STATUS_BEEN_CANCEL;
		break;
	case 'b':
		status = ORDER_STATUS_NOT_TOUCHED;
		break;
	case 'c':
		status = ORDER_STATUS_TOUCHED;
		break;
	default:
		status = ORDER_STATUS_INVALID;
	}

	return status;
}

/*------------------------------------------------------------------*
 |					CtpFutureTradeApi模块							|
 *------------------------------------------------------------------*/

CtpFutureTradeApi::CtpFutureTradeApi(void)
		:TradeApi()
		,m_ctp_market_api(NULL)
		,m_ctp_market_handler(NULL)
		,m_succed_query_confirm_info(false)
		,m_succed_query_settlement_info(false)
		,m_has_confirmed(false)
		,m_request_id(0) 
		,m_order_ref(0)
		,m_front_id(0)
		,m_session_id(0) 
		,m_broker_id("")
		,kMaxOrderNum(10000000000000LL)
		,been_logout_(false)
		,ctp_req_buf_(this)
{
}


CtpFutureTradeApi::~CtpFutureTradeApi(void)
{
}

bool CtpFutureTradeApi::Init(const std::string& front_addr_str, TradeSpi* spi, std::string& err) {
	spi_ = spi;
	//char buf[128];GetCurrentDirectoryA(128, buf);
	//std::string data_path = (std::string)buf + "\\data\\";
	std::string data_path = Global::Instance()->its_home+ "\\log\\ctp\\";
	if (!Directory::IsDirExist(data_path) && !Directory::MakeDir(data_path)) { err = "MakeDir:"+data_path+" false"; return false;}
	m_ctp_market_api = CThostFtdcTraderApi::CreateFtdcTraderApi(data_path.c_str());
	m_ctp_market_handler = new CtpFutureTradeHandler(this);

	m_ctp_market_api->RegisterSpi(m_ctp_market_handler);
	char front_addr[128] = {0};
	memcpy(front_addr, front_addr_str.c_str(), 128);
	m_ctp_market_api->RegisterFront(front_addr);
	m_ctp_market_api->RegisterFront(front_addr);
	m_ctp_market_api->RegisterFront(front_addr);
	m_ctp_market_api->SubscribePublicTopic(THOST_TERT_QUICK);				// 注册公有流.
	m_ctp_market_api->SubscribePrivateTopic(THOST_TERT_QUICK);				// 注册私有流.
	m_ctp_market_api->Init();

	if (!TimeWait(kInitWaitTime))
	{
		err = "time out for return of RegisterFront";
		return false;
	}
	if (!m_succed_connect) { err = error_msg_; return false; }

	ctp_req_buf_.Start();

	return true;
}

void CtpFutureTradeApi::Denit()
{
	m_succed_connect = false;
	m_succed_login = false;
	if (m_ctp_market_handler)m_ctp_market_handler->m_reconnected = false;

	
	ctp_req_buf_.Terminate();
	ctp_req_buf_.Join();

	if (m_ctp_market_api != NULL)
	{
		m_ctp_market_api->Release();
		m_ctp_market_api = NULL;
	}
}

bool CtpFutureTradeApi::Login(const std::string& broker_id, const std::string& user_id, const std::string& password, std::string& err)
{
	if ("" == broker_id || "" == user_id || "" == password) {
		err = "the arguemnt is empty";
		return false;
	}

	m_broker_id = broker_id;
	m_user_id = user_id;
	m_password = password;

	if (!m_succed_connect) {
		err = "has not connected to CTP front address";
		return false;
	}

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID,  broker_id.c_str(),sizeof(req.BrokerID));
	strncpy(req.UserID,  user_id.c_str(), sizeof(req.UserID));
	strncpy(req.Password,  password.c_str(),sizeof(req.Password));
	strncpy(req.UserProductInfo, "iTStation", sizeof(req.UserProductInfo));
	int result = m_ctp_market_api->ReqUserLogin(&req, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqUserLogin";
		return false;
	}

	if (!TimeWait(kLoginWaitTime))
	{
		err = "time out for return of ReqUserLogin";
		return false;
	}
	if (!m_succed_login) { err = error_msg_; return false; }

	been_logout_ = false;
	return true;
}

bool CtpFutureTradeApi::InitPreTrade(std::string& err) {
	if (!ReqQrySettlementInfoConfirm(err)) { return false; }

	if (!m_has_confirmed) {
		if (!ReqQrySettlementInfo(err)) { return false; }
		if (!ReqSettlementInfoConfirm(err)) { return false; }
	}

	return true;
}

bool CtpFutureTradeApi::Logout(std::string& err) {
	if (!m_succed_login) { return STATUS_OK; }

	been_logout_ = true;
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID,  m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(req.UserID,  m_user_id.c_str(), sizeof(TThostFtdcUserIDType));
	m_ctp_market_api->ReqUserLogout(&req, ++m_request_id);

	if (!TimeWait(kLogoutWaitTime)) {
		err = "time out for return of ReqUserLogout";
		return false;
	}

	if (m_succed_login) { err = error_msg_; return false; }

	return true;
}

bool CtpFutureTradeApi::ReLogin(std::string& err) {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, m_broker_id.c_str(),  sizeof(TThostFtdcBrokerIDType));
	strncpy(req.UserID, m_user_id.c_str(), sizeof(TThostFtdcUserIDType));
	strncpy(req.Password,  m_password.c_str(), sizeof(TThostFtdcPasswordType));
	strncpy(req.UserProductInfo, "iTStation", sizeof(req.UserProductInfo));
	int result = m_ctp_market_api->ReqUserLogin(&req, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqUserLogin";
		return false;
	}

	return true;
}

bool CtpFutureTradeApi::QryInstrument(std::string& err)
{
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}
	ctp_req_buf_.Push(E_QryInstrumentField);
	return true;
}

bool CtpFutureTradeApi::QryMargin(const std::string& symbol, std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}
	ctp_req_buf_.Push(E_QryInstrumentMarginRateField);
	Locker lock(&ctp_req_buf_.margin_mutex_);
	ctp_req_buf_.margin_symbols_.push(symbol);
	return true;
}

bool CtpFutureTradeApi::QryCommision(const std::string& symbol, std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}
	ctp_req_buf_.Push(E_QryInstrumentCommissionRateField);
	Locker lock(&ctp_req_buf_.commision_mutex_);
	ctp_req_buf_.commision_symbols_.push(symbol);
	return true;
}

int CtpFutureTradeApi::SubmitOrder(const OrderParamData& param, std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return  NAN_LOCAL_ORDER_ID; 
	}

	CThostFtdcInputOrderField order_field;
    memset(&order_field, 0, sizeof(order_field));
    order_field.CombHedgeFlag[0] = param.hedge_flag	;	 //1投机
    order_field.ContingentCondition = THOST_FTDC_CC_Immediately; //立即触发
    order_field.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    order_field.IsAutoSuspend = 0;
    order_field.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量1  最小数量2  全部成交3
	order_field.MinVolume = 1;

	order_field.IsSwapOrder = 0;	//新增
	order_field.UserForceClose = 0;	//新增

	if (LIMIT_PRICE_ORDER == param.order_price_flag) {
		order_field.OrderPriceType = THOST_FTDC_OPT_LimitPrice;		//***任意价1  限价2***
		order_field.TimeCondition =  THOST_FTDC_TC_GFD;				//***立即完成1  当日有效3***
	} 
	else {
		order_field.OrderPriceType = THOST_FTDC_OPT_AnyPrice;		//***任意价1  限价2***
		order_field.TimeCondition =  THOST_FTDC_TC_IOC;		//***立即完成1  当日有效3***
	}

    strcpy(order_field.InvestorID, m_user_id.c_str());
    strcpy(order_field.UserID, param.user_tag);
    strcpy(order_field.BrokerID, m_broker_id.c_str());

    strcpy(order_field.InstrumentID, param.symbol.instrument);
    order_field.LimitPrice = param.limit_price;

	if(LONG_DIRECTION == param.direction) {					
		order_field.Direction = THOST_FTDC_D_Buy;		//买
	}		
    else {									
        order_field.Direction = THOST_FTDC_D_Sell;		//卖	
	}

	switch(param.open_close_flag) {
	case OPEN_ORDER:
		order_field.CombOffsetFlag[0] = THOST_FTDC_OF_Open;			//开仓
		break;
	case CLOSE_ORDER:
		order_field.CombOffsetFlag[0] = THOST_FTDC_OF_Close;		//平仓
		break;
	case CLOSE_TODAY_ORDER:
		order_field.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	//平今
		break;
	case CLOSE_YESTERDAY_ORDER:
		order_field.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;	//平昨
		break;
	default:
		order_field.CombOffsetFlag[0] = THOST_FTDC_OF_Open;			//平仓
	}

    order_field.VolumeTotalOriginal = param.volume;

	Locker lock(&m_order_ref_lock); // 多策略/多线程 下单保护
	++m_order_ref;
	stringstream ss; ss<<m_order_ref;
	strncpy(order_field.OrderRef, ss.str().c_str(), sizeof(TThostFtdcOrderRefType));

	order_field.RequestID = ++m_request_id;
    int ret = m_ctp_market_api->ReqOrderInsert(&order_field, m_request_id);			//报单,修改

	if (0 != ret) {
		err = "fail to ReqOrderInsert";
		return  NAN_LOCAL_ORDER_ID;
	}
	
	return m_order_ref;
}

int CtpFutureTradeApi::QueryAccount(std::string& err) {
	 if (!m_succed_login) { 
		 err = "has not logined";
		 return -1; 
	 }
	 ctp_req_buf_.Push(E_QryTradingAccountField);
	 return 0;
 }

int CtpFutureTradeApi::QueryPosition(std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return -1; 
	}
	ctp_req_buf_.Push(E_QryInvestorPositionDetailField);
	return 0;
}

int CtpFutureTradeApi::QueryOrder(std::string& err)
{
	if (!m_succed_login) { 
		err = "has not logined";
		return -1; 
	}
	ctp_req_buf_.Push(E_QryOrder);
	return 0;
}

int CtpFutureTradeApi::QueryTrade(std::string& err)
{
	if (!m_succed_login) { 
		err = "has not logined";
		return -1; 
	}
	ctp_req_buf_.Push(E_QryTrade);
	return 0;
}

int CtpFutureTradeApi::CancelOrder(const OrderData& param, std::string& err)
{
	if (!m_succed_login) { 
		err = "has not logined";
		return -1; 
	}
	std::string exc_str = ToExchangeStr(param.symbol.exchange);
	if (exc_str == "")
	{
		err = "invalid exchange id";
		return -1; 
	}

	CThostFtdcInputOrderActionField cancel_param = {0};
	strcpy(cancel_param.BrokerID, m_broker_id.c_str());
	strcpy(cancel_param.InvestorID, m_user_id.c_str());
	cancel_param.ActionFlag = THOST_FTDC_AF_Delete;
	stringstream ss;
	ss << param.order_id;
	//strcpy(cancel_param.OrderSysID, param.order_id);
	sprintf(cancel_param.OrderSysID, "%12lld", param.order_id);
	strcpy(cancel_param.ExchangeID, exc_str.c_str());
	//cancel_param.FrontID = m_front_id;
	//cancel_param.SessionID = m_session_id;
	//strcpy(cancel_param.InstrumentID, "al1503");
	//stringstream ss2; 
	//ss2 << param.order_ref;
	//strcpy(cancel_param.OrderRef, ss2.str().c_str());
	
	int result = m_ctp_market_api->ReqOrderAction(&cancel_param, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqOrderAction";
		return -1;
	}

	return m_request_id;
}

bool CtpFutureTradeApi::ReqQrySettlementInfoConfirm(std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}

	if (m_has_confirmed) { return true; }

	CThostFtdcQrySettlementInfoConfirmField comfirm_field;
	memset(&comfirm_field, 0, sizeof(CThostFtdcQrySettlementInfoConfirmField));
	strncpy(comfirm_field.BrokerID,  m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(comfirm_field.InvestorID, m_user_id.c_str(),sizeof(TThostFtdcInvestorIDType));
	int result = m_ctp_market_api->ReqQrySettlementInfoConfirm(&comfirm_field, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqQrySettlementInfoConfirm";
		return false;
	}

	if (!TimeWait(kQueryConfirm)) {
		err = "time out for return of ReqQrySettlementInfoConfirm";
		return false;
	}

	if (!m_succed_query_confirm_info) { err = error_msg_; return false; }

	return true;
}

bool CtpFutureTradeApi::ReqQrySettlementInfo(std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}

	if (m_has_confirmed) { return true; }

	Thread::Sleep(1000);
	CThostFtdcQrySettlementInfoField info_field;
	memset(&info_field, 0, sizeof(CThostFtdcQrySettlementInfoField));
	strncpy(info_field.BrokerID,  m_broker_id.c_str(),sizeof(TThostFtdcBrokerIDType));
	strncpy(info_field.InvestorID,  m_user_id.c_str(), sizeof(TThostFtdcInvestorIDType));
	int result = m_ctp_market_api->ReqQrySettlementInfo(&info_field, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqQrySettlementInfo";
		return false;
	}

	if (!TimeWait(kQuerySettlementInfo)) {
		err = "time out for return of ReqQrySettlementInfo";
		return false;
	}

	if (!m_succed_query_settlement_info) { err = error_msg_; return false; }

	return true;
}

bool CtpFutureTradeApi::ReqSettlementInfoConfirm(std::string& err) {
	if (!m_succed_login) { 
		err = "has not logined";
		return false; 
	}

	if (m_has_confirmed) { return true; }

	Thread::Sleep(1000);
	CThostFtdcSettlementInfoConfirmField confirm_field;
	memset(&confirm_field, 0, sizeof(CThostFtdcSettlementInfoConfirmField));
	strncpy(confirm_field.BrokerID, m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(confirm_field.InvestorID,  m_user_id.c_str(), sizeof(TThostFtdcInvestorIDType));
	int result = m_ctp_market_api->ReqSettlementInfoConfirm(&confirm_field, ++m_request_id);
	if (0 != result) {
		err = "fail to ReqSettlementInfoConfirm";
		return false;
	}

	if (!TimeWait(kConfirmSettlement)) {
		err = "time out for return of ReqSettlementInfoConfirm";
		return false;
	}

	if (!m_has_confirmed) { err = error_msg_; return false; }

	return true;
}

bool CtpFutureTradeApi::CtpRequestBuffer::Consume(const RequestType& val) {
	bool bRet = false;
	switch (val) {
	case E_QryTradingAccountField:
		bRet = QueryAccount();
		break;
	case E_QryInvestorPositionDetailField:
		bRet = QueryPosition();
		break;
	case E_QryOrder:
		bRet = QueryOrder();
		break;
	case E_QryTrade:
		bRet = QueryTrade();
		break;
	case E_QryInstrumentField:
		bRet = QryInstrument();
		break;
	case E_QryInstrumentMarginRateField:
		bRet = QryMargin();
		break;
	case E_QryInstrumentCommissionRateField:
		bRet = QryCommision();
		break;
	default:
		//assert(false);
		break;
	}

	if (bRet) {
		m_nSleep = 1;
	}
	else {
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
		Sleep(m_nSleep);
	}
	return bRet;
}

bool CtpFutureTradeApi::CtpRequestBuffer::QueryAccount() {
	CThostFtdcQryTradingAccountField account_field = {0}; // 不将结构体归零，查出的账号的指针会为null
	strcpy(account_field.BrokerID, m_market_trade_api->m_broker_id.c_str());
	strcpy(account_field.InvestorID, m_market_trade_api->m_user_id.c_str());
	return 0 == m_market_trade_api->m_ctp_market_api->ReqQryTradingAccount(&account_field, ++m_market_trade_api->m_request_id);
}

bool CtpFutureTradeApi::CtpRequestBuffer::QueryPosition() {
	CThostFtdcQryInvestorPositionDetailField position_field = {0};
	memset(&position_field, 0, sizeof(position_field));
	strcpy(position_field.BrokerID, m_market_trade_api->m_broker_id.c_str());
	strcpy(position_field.InvestorID, m_market_trade_api->m_user_id.c_str());
	return 0 == m_market_trade_api->m_ctp_market_api->ReqQryInvestorPositionDetail(&position_field, ++m_market_trade_api->m_request_id);
}

bool CtpFutureTradeApi::CtpRequestBuffer::QueryOrder() {
	CThostFtdcQryOrderField order_param = {0};
	memset(&order_param, 0, sizeof(order_param));
	strcpy(order_param.BrokerID, m_market_trade_api->m_broker_id.c_str());
	strcpy(order_param.InvestorID, m_market_trade_api->m_user_id.c_str());
	return 0 == m_market_trade_api->m_ctp_market_api->ReqQryOrder(&order_param, ++m_market_trade_api->m_request_id);
}

bool CtpFutureTradeApi::CtpRequestBuffer::QueryTrade() {
	CThostFtdcQryTradeField trade_param = {0};
	memset(&trade_param, 0, sizeof(trade_param));
	strcpy(trade_param.BrokerID, m_market_trade_api->m_broker_id.c_str());
	strcpy(trade_param.InvestorID, m_market_trade_api->m_user_id.c_str());
	return 0== m_market_trade_api->m_ctp_market_api->ReqQryTrade(&trade_param, ++m_market_trade_api->m_request_id);
}

bool CtpFutureTradeApi::CtpRequestBuffer::QryInstrument() {
	CThostFtdcQryInstrumentField req = {0};
	memset(&req, 0, sizeof(req));
	return 0 == m_market_trade_api->m_ctp_market_api->ReqQryInstrument(&req, ++m_market_trade_api->m_request_id);
}

bool CtpFutureTradeApi::CtpRequestBuffer::QryMargin() {
	CThostFtdcQryInstrumentMarginRateField req = {0};
	strncpy(req.BrokerID,  m_market_trade_api->m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(req.InvestorID,  m_market_trade_api->m_user_id.c_str(), sizeof(TThostFtdcInvestorIDType));
	req.HedgeFlag = THOST_FTDC_HF_Speculation;
	Locker lock(&margin_mutex_);
	if (margin_symbols_.empty()) return true;
	strcpy(req.InstrumentID, margin_symbols_.front().c_str());
	bool ret = 0 == m_market_trade_api->m_ctp_market_api->ReqQryInstrumentMarginRate(&req, ++m_market_trade_api->m_request_id);
	if (ret) margin_symbols_.pop();
	return ret;
}

bool CtpFutureTradeApi::CtpRequestBuffer::QryCommision() {
	CThostFtdcQryInstrumentCommissionRateField req = {0};
	strncpy(req.BrokerID,  m_market_trade_api->m_broker_id.c_str(), sizeof(TThostFtdcBrokerIDType));
	strncpy(req.InvestorID, m_market_trade_api->m_user_id.c_str(), sizeof(TThostFtdcInvestorIDType));
	Locker lock(&commision_mutex_);
	if (commision_symbols_.empty()) return true;
	strcpy(req.InstrumentID, commision_symbols_.front().c_str());
	bool ret = 0 == m_market_trade_api->m_ctp_market_api->ReqQryInstrumentCommissionRate(&req, ++m_market_trade_api->m_request_id);
	if (ret) commision_symbols_.pop();
	return ret;
}

