#include <iostream>
#include <sstream>
#include <algorithm>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include "common/Thread.h"
#include "datalib/SimpleMath.h"
#include "datalib/SymbolInfoSet.h"
#include "account/TradeEngine.h"
#include "common/XmlConfig.h"
#include "common/AppLog.h"

using namespace std;

#define USER_TAG (user_tag==NULL?s_user_id.c_str():user_tag)

//namespace itstation {

TradeEngine* TradeEngine::self_ = NULL;
static std::string s_user_id;

TradeEngine::TradeEngine() : CTPMarginCommision(), err_spi_(NULL), is_init_(false), event_spi_(NULL), re_qry_pos_(false)
{

}

TradeEngine::~TradeEngine()
{

}

TradeEngine* TradeEngine::Instance()
{
	static SpinLock lock;
	Locker locker(&lock);
	if (NULL == self_)
	{
		self_ = new TradeEngine();
	}
	return self_;
}
void TradeEngine::Denit(){
	if (api_) api_->Denit();
	CTPMarginCommision::Denit();
}
bool TradeEngine::Init(std::string& err)
{
	if (is_init_) { return true; }
	re_qry_pos_ = false;
	CTPMarginCommision::Init();

	XmlConfig config(Global::Instance()->GetConfigFile());
	if (!config.Load()) 
	{
		err = "load config error";
		return false;
	}

	XmlNode node = config.FindNode("CtpTradeAccount");		
	std::string broker_id = node.GetValue("broker_id");
	s_user_id = node.GetValue("user_id");
	std::string pwd = node.GetValue("pwd");
	std::string front_addr = node.GetValue("front_addr");

		
	if (!api_->Init(front_addr, this, err)) 
	{
		err = std::string("CTP API Init error : ") + err;
		return false;
	}

	if (!api_->Login(broker_id, s_user_id, pwd, err)) {
		err = std::string("CTP API Login error : ") + err;
		return false;
	}
	Thread::Sleep(1000);
	if (!api_->InitPreTrade(err)) {
		err = std::string("CTP API confirm risk error : ") + err;
		return false;
	}
	if (api_->QueryAccount(err))
	{
		err = std::string("CTP API query account error : ") + err;
		return false;
	}
	if (api_->QueryOrder(err))
	{
		err = std::string("CTP API query order error : ") + err;
		return false;
	}
	if (api_->QueryTrade(err))
	{
		err = std::string("CTP API query trade error : ") + err;
		return false;
	}
	if (api_->QueryPosition(err))
	{
		err = std::string("CTP API query position error : ") + err;
		return false;
	}
		
	Thread::Sleep(3000);
	if (re_qry_pos_) {
		if (api_->QueryPosition(err))
		{
			err = std::string("CTP API query position error : ") + err;
			return false;
		}
	}

	is_init_ = true;
	return true;
}

void TradeEngine::QryCtpAccount(){
	string err;
	api_->QueryAccount(err);
}

void TradeEngine::SetSpi(TradeEventSpi* trade_spi, TradeErrorSpi *err_spi)
{
	event_spi_ = trade_spi;
	err_spi_ = err_spi;
}

int TradeEngine::SubmitOrder(const OrderParamData& param)
{
	if (!is_init_) { 
		if(err_spi_)err_spi_->OnTradeError("CTP接口未初始化"); 
		return NAN_LOCAL_ORDER_ID; 
	}
	std::string err;
	int LocalOrderID = api_->SubmitOrder(param, err);
	if (LocalOrderID == NAN_LOCAL_ORDER_ID)
	{
		err = std::string("委托失败: ") + err;
		if(err_spi_)err_spi_->OnTradeError(err); 
	}
	QryMargin(param.symbol.instrument);
	QryCommision(param.symbol.instrument);
	//cout<<"LocalOrderID:"<<LocalOrderID<<endl;
	return LocalOrderID;
}

void TradeEngine::CancelOrder(const OrderData& param)
{
	if (!is_init_) { if(err_spi_)err_spi_->OnTradeError("CTP接口未初始化"); return; }
	std::string err;
	if (api_->CancelOrder(param, err) <= 0)
	{
		err = std::string("撤单失败: ") + err;
		if(err_spi_)err_spi_->OnTradeError(err); 
	}
}

void TradeEngine::CancelOrder(const Symbol& sym, const UserStrategyIdType user_tag)
{
	if (!is_init_) { if(err_spi_)err_spi_->OnTradeError("CTP接口未初始化"); return; }
	Locker locker(&order_mutex_);
	for (std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.begin(); iter != valid_orders_.end(); ++iter)
	{
		if (!strcmp(iter->second.user_tag, USER_TAG) && iter->second.symbol == sym)
		{
			CancelOrder(iter->second);
		}
	}
}

void TradeEngine::CancelOrder(int LocalOrderID){
	if (!is_init_) { if(err_spi_)err_spi_->OnTradeError("CTP接口未初始化"); return; }
	Locker locker(&order_mutex_);
	for (std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.begin(); iter != valid_orders_.end(); ++iter)
	{
		if (iter->second.local_order_id == LocalOrderID)
		{
			CancelOrder(iter->second);
		}
	}
}

int TradeEngine::Buy(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = LONG_DIRECTION;
	param.open_close_flag = OPEN_ORDER;
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}

int TradeEngine::BuyMar(const Symbol& symbol, int volume, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = 0.0;
	param.volume = volume;
	param.order_price_flag = MARKET_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = LONG_DIRECTION;
	param.open_close_flag = OPEN_ORDER;
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}

int TradeEngine::Sell(const Symbol& symbol, double price, int volume, bool is_td, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = SHORT_DIRECTION;
	param.open_close_flag = CLOSE_ORDER;
	if (is_td) { param.open_close_flag = CLOSE_TODAY_ORDER; }
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}

int TradeEngine::SellShort(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = SHORT_DIRECTION;
	param.open_close_flag = OPEN_ORDER;
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}


int TradeEngine::SellShortMar( const Symbol& symbol,int volume, const UserStrategyIdType user_tag /*= NULL*/ )
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = 0.0;
	param.volume = volume;
	param.order_price_flag = MARKET_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = SHORT_DIRECTION;
	param.open_close_flag = OPEN_ORDER;
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}

	
int TradeEngine::Sell(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	//param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = SHORT_DIRECTION;
	//param.open_close_flag = CLOSE_ORDER;
	//if (is_td) { param.open_close_flag = CLOSE_TODAY_ORDER; }
	param.hedge_flag = HF_SPECULATION;

	int enable_today_volume = 0;
	int enable_yestd_volume = 0;
	pos_mutex_.Lock();
	std::map<std::string, std::map<Symbol, PositionData> >::iterator it = long_positions_.find(USER_TAG);
	if (it != long_positions_.end()) {			
		for (std::map<Symbol, PositionData>::iterator iter = it->second.begin(); iter != it->second.end(); ++iter) {
			if (symbol == iter->first){
				enable_today_volume = iter->second.enable_today_volume;
				enable_yestd_volume = iter->second.enable_yestd_volume;
			}
		}
	}
	pos_mutex_.Unlock();

	if (enable_today_volume >= volume) {
		param.open_close_flag = CLOSE_TODAY_ORDER;
		param.volume = volume;			
		return SubmitOrder(param);
	}
	else if (enable_today_volume > 0) {
		param.open_close_flag = CLOSE_TODAY_ORDER;
		param.volume = enable_today_volume;
		if (0 ==SubmitOrder(param)) return 0;
		volume -= enable_today_volume;
	}

	if (volume > 0 && enable_yestd_volume > 0) {
		param.open_close_flag = CLOSE_ORDER;
		param.volume = min(enable_yestd_volume, volume);
		return SubmitOrder(param);
	}
}


int TradeEngine::BuyCover(const Symbol& symbol, double price, int volume, bool is_td, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = LONG_DIRECTION;
	param.open_close_flag = CLOSE_ORDER;
	if (is_td) { param.open_close_flag = CLOSE_TODAY_ORDER; }
	param.hedge_flag = HF_SPECULATION;
	return TradeEngine::Instance()->SubmitOrder(param);
}

int TradeEngine::BuyCover(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag)
{
	OrderParamData param;
	param.symbol = symbol;
	param.limit_price = price;
	//param.volume = volume;
	param.order_price_flag = LIMIT_PRICE_ORDER;
	strncpy(param.user_tag, USER_TAG, sizeof(UserStrategyIdType));
	param.direction = LONG_DIRECTION;
	//param.open_close_flag = CLOSE_ORDER;
	//if (is_td) { param.open_close_flag = CLOSE_TODAY_ORDER; }
	param.hedge_flag = HF_SPECULATION;

		
	int enable_today_volume = 0;
	int enable_yestd_volume = 0;
	pos_mutex_.Lock();
	std::map<std::string, std::map<Symbol, PositionData> >::iterator it = short_positions_.find(USER_TAG);
	if (it != short_positions_.end()) {			
		for (std::map<Symbol, PositionData>::iterator iter = it->second.begin(); iter != it->second.end(); ++iter) {
			if (symbol == iter->first){
				enable_today_volume = iter->second.enable_today_volume;
				enable_yestd_volume = iter->second.enable_yestd_volume;
			}
		}
	}
	pos_mutex_.Unlock();

	if (enable_today_volume >= volume) {
		param.open_close_flag = CLOSE_TODAY_ORDER;
		param.volume = volume;			
		return SubmitOrder(param);
	}
	else if (enable_today_volume > 0) {
		param.open_close_flag = CLOSE_TODAY_ORDER;
		param.volume = enable_today_volume;
		if (0 ==SubmitOrder(param)) return 0;
		volume -= enable_today_volume;
	}

	if (volume > 0 && enable_yestd_volume > 0) {
		param.open_close_flag = CLOSE_ORDER;
		param.volume = min(enable_yestd_volume, volume);
		return SubmitOrder(param);
	}
}

	

void TradeEngine::OnError(const int request_id, const std::string& error_msg) 
{
	std::string msg = std::string("CTP请求错误：") + error_msg;
	if (err_spi_)err_spi_->OnTradeError(msg);
}

bool TradeEngine::IsOrderValid(OrderStatus status)
{
	return status == ORDER_STATUS_BEEN_SUBMIT || status == ORDER_STATUS_PART_TRADE;
}

PriceType TradeEngine::CalcFloatProfit(const Symbol &symbol, OrderDirection direction, PriceType open_price, PriceType last_price, VolumeType open_volume) {
	double val = 0.;
	if (direction == LONG_DIRECTION)
		val = info_set_->GetVolMulti(symbol) * open_volume * (last_price - open_price);
	else
		val = info_set_->GetVolMulti(symbol) * open_volume * (open_price - last_price);

	return val;
}

void TradeEngine::SetPosiLastPrice(const Symbol &symbol, PriceType last_price) {
	///更新有持仓的合约的最新价。注意：考虑到效率，持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	Locker lock(&last_price_mutex_);
	last_prices_[symbol] = last_price;
}

PriceType TradeEngine::GetPosiPrePrice(const Symbol &sym) {
	Locker lock(&last_price_mutex_);
	return last_prices_[sym];
}

void TradeEngine::OnOrderError(OrderData* order_data)
{
	OnOrder(order_data);
}

void TradeEngine::OnDisconnect(const std::string& reson) 
{
	is_init_ = false;
	if(err_spi_)err_spi_->OnTradeError( "与CTP断开连接"); 
}

void TradeEngine::OnConnect() 
{
	is_init_ = true;
	if(err_spi_)err_spi_->OnTradeError("与CTP重新连接成功"); 
}

void TradeEngine::FrozenAccountBalance(const OrderData *order_data) {
	if (order_data->open_close_flag != OPEN_ORDER) return;
	// 冻结保证金.
	double frozen_balance = CalcMargin(order_data->symbol, order_data->direction, order_data->limit_price, order_data->total_volume)
		+ CalcCommision(order_data->symbol, order_data->open_close_flag, order_data->limit_price, order_data->total_volume);

	Locker lock(&account_mutex_);
	account_data_.frozen_balance += frozen_balance;
	account_data_.enable_balance -= frozen_balance;
	account_data_.withdraw_balance -= frozen_balance;
}

void TradeEngine::CancelFrozenAccountBalance(const OrderData *order_data) {
	if (order_data->open_close_flag != OPEN_ORDER) return;
	// 取消冻结保证金.
	double frozen_balance = CalcMargin(order_data->symbol, order_data->direction, order_data->limit_price, order_data->total_volume)
		+ CalcCommision(order_data->symbol, order_data->open_close_flag, order_data->limit_price, order_data->total_volume);

	Locker lock(&account_mutex_);
	account_data_.frozen_balance -= frozen_balance;
	account_data_.enable_balance += frozen_balance;
	account_data_.withdraw_balance += frozen_balance;
}

void TradeEngine::MinusAccountCommision(const OrderData *order_data) {
	Locker lock(&account_mutex_);
	account_data_.asset_balance -= 1.0;
	account_data_.enable_balance -= 1.0;
	account_data_.withdraw_balance -= 1.0;
}

void TradeEngine::MinusAccountCommision(TradeData *trade_data) {
	trade_data->trade_commision = CalcCommision(trade_data->symbol, trade_data->open_close_flag, trade_data->trade_price, trade_data->trade_volume);
	// 扣除手续费.
	Locker lock(&account_mutex_);
	account_data_.asset_balance -= trade_data->trade_commision;
	account_data_.enable_balance -= trade_data->trade_commision;
	account_data_.withdraw_balance -= trade_data->trade_commision;
}

void TradeEngine::OnOrder(OrderData *order_data, bool is_qry, bool is_last) {
	if (NULL == order_data) return;
	///查下保证金率、手续费率.
	QryMargin(order_data->symbol.instrument);
	QryCommision(order_data->symbol.instrument);

	order_mutex_.Lock();
	if (order_data->order_id == 0) // 废单.
	{		
		if (!is_qry){
			APP_LOG(LOG_LEVEL_ERROR) << order_data->symbol.Str() << "\t" << order_data->status_msg;
		}		
		rejected_orders_.push_back(*order_data); 
		order_mutex_.Unlock();
		/*if (!is_qry || (is_qry && is_last))
			SendTradeEventData(TradeEventData::ORDER_EVENT, order_data->user_tag, order_data);*/
		return;
	}

	//更新持仓的可用数量.
	if (!is_qry && (order_data->open_close_flag == CLOSE_ORDER || order_data->open_close_flag == CLOSE_TODAY_ORDER || order_data->open_close_flag == CLOSE_YESTERDAY_ORDER))
	{
		std::map<NumberIdType, OrderData>::iterator pre_order = all_orders_.find(order_data->order_id);
		//第一个有效订单，冻结持仓.
		if (pre_order == all_orders_.end() && IsOrderValid(order_data->status))
		{
			Locker locker(&pos_mutex_);
			if (order_data->direction == LONG_DIRECTION)
			{
				if (short_positions_.find(order_data->user_tag) != short_positions_.end()) {
					std::map<Symbol, PositionData>::iterator pos_iter = short_positions_[order_data->user_tag].find(order_data->symbol);
					if (pos_iter != short_positions_[order_data->user_tag].end())
					{
						if (order_data->open_close_flag == CLOSE_TODAY_ORDER)
						{
							pos_iter->second.enable_today_volume -= order_data->total_volume;
							if (pos_iter->second.enable_today_volume < 0) { pos_iter->second.enable_today_volume = 0; }
						} 
						else
						{
							pos_iter->second.enable_yestd_volume -= order_data->total_volume;
							if (pos_iter->second.enable_yestd_volume < 0) { pos_iter->second.enable_yestd_volume = 0; }
						}
					}
				}				
			}
			else // 空单 SHORT_DIRECTION
			{
				if (long_positions_.find(order_data->user_tag) != long_positions_.end()) {
					std::map<Symbol, PositionData>::iterator pos_iter = long_positions_[order_data->user_tag].find(order_data->symbol);
					if (pos_iter != long_positions_[order_data->user_tag].end())
					{
						if (order_data->open_close_flag == CLOSE_TODAY_ORDER)
						{
							pos_iter->second.enable_today_volume -= order_data->total_volume;
							if (pos_iter->second.enable_today_volume < 0) { pos_iter->second.enable_today_volume = 0; }
						} 
						else
						{
							pos_iter->second.enable_yestd_volume -= order_data->total_volume;
							if (pos_iter->second.enable_yestd_volume < 0) { pos_iter->second.enable_yestd_volume = 0; }
						}
					}
				}
			}
		}
		//有效订单被撤销.
		else if (pre_order != all_orders_.end() && IsOrderValid(pre_order->second.status) && order_data->status == ORDER_STATUS_BEEN_CANCEL)
		{
			Locker locker(&pos_mutex_);
			if (order_data->direction == LONG_DIRECTION)
			{
				if (short_positions_.find(order_data->user_tag) != short_positions_.end()) {
					std::map<Symbol, PositionData>::iterator pos_iter = short_positions_[order_data->user_tag].find(order_data->symbol);
					if (pos_iter != short_positions_[order_data->user_tag].end())
					{
						if (order_data->open_close_flag == CLOSE_TODAY_ORDER)
						{
							pos_iter->second.enable_today_volume += order_data->total_volume;
							//if (pos_iter->second.enable_today_volume > pos_iter->second.open_volume) { pos_iter->second.enable_today_volume = pos_iter->second.open_volume; }
						} 
						else
						{
							pos_iter->second.enable_yestd_volume += order_data->total_volume;
							//if (pos_iter->second.enable_yestd_volume > pos_iter->second.open_volume) { pos_iter->second.enable_yestd_volume = pos_iter->second.open_volume; }
						}
					}
				}				
			}
			else
			{
				if (long_positions_.find(order_data->user_tag) != long_positions_.end()) {
					std::map<Symbol, PositionData>::iterator pos_iter = long_positions_[order_data->user_tag].find(order_data->symbol);
					if (pos_iter != long_positions_[order_data->user_tag].end())
					{
						if (order_data->open_close_flag == CLOSE_TODAY_ORDER)
						{
							pos_iter->second.enable_today_volume += order_data->total_volume;
							//if (pos_iter->second.enable_today_volume > pos_iter->second.open_volume) { pos_iter->second.enable_today_volume = pos_iter->second.open_volume; }
						} 
						else
						{
							pos_iter->second.enable_yestd_volume += order_data->total_volume;
							//if (pos_iter->second.enable_yestd_volume > pos_iter->second.open_volume) { pos_iter->second.enable_yestd_volume = pos_iter->second.open_volume; }
						}
					}
				}
			}
		}

		//SendTradeEventData(TradeEventData::POSITION_EVENT, order_data->user_tag, NULL);
	}

	all_orders_[order_data->order_id] = *order_data;
	//if (order_data->symbol.exchange == EXCHANGE_CFFEX) MinusAccountCommision(order_data);
	std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.find(order_data->order_id);
	if (iter == valid_orders_.end() && IsOrderValid(order_data->status))
	{
		valid_orders_[order_data->order_id] = *order_data;
		// 开仓时冻结保证金.
		if (!is_qry) FrozenAccountBalance(order_data);	
	}
	else if (iter != valid_orders_.end() && !IsOrderValid(order_data->status))
	{
		valid_orders_.erase(iter);
		// 取消开仓单时取消冻结保证金.
		if (!is_qry) CancelFrozenAccountBalance(order_data);
	}
	order_mutex_.Unlock();

	if (!is_qry || (is_qry && is_last))
		SendTradeEventData(TradeEventData::ORDER_EVENT, order_data->user_tag, order_data);
}

void TradeEngine::OnOrder(OrderData* order_data)
{
	OnOrder(order_data, false);
}

void TradeEngine::OnTrade(TradeData* trade_data)
{
	re_qry_pos_ = true;

	trade_mutex_.Lock();
	trades_.push_back(*trade_data);
	trade_mutex_.Unlock();

	// 扣除手续费.
	MinusAccountCommision(trade_data);
	// 根据成交信息实时更新持仓.
	UpdatePoswWithTrade(trade_data);

	SendTradeEventData(TradeEventData::TRADE_EVENT, trade_data->user_tag, trade_data);
}

void TradeEngine::OpenLong(TradeData* trade_data) {
	if (long_positions_.find(trade_data->user_tag) != long_positions_.end()) { 
		std::map<Symbol, PositionData>::iterator iter = long_positions_[trade_data->user_tag].find(trade_data->symbol);
		if (iter == long_positions_[trade_data->user_tag].end())
		{
			PositionData pos_data;
			pos_data.direction = LONG_DIRECTION;
			pos_data.symbol = trade_data->symbol;
			pos_data.open_price = trade_data->trade_price;
			pos_data.open_volume = trade_data->trade_volume;
			pos_data.today_volume = trade_data->trade_volume;
			pos_data.enable_today_volume = trade_data->trade_volume;
			pos_data.last_price = trade_data->trade_price;
			strcpy(pos_data.user_tag, trade_data->user_tag);
			pos_data.using_margin = CalcMargin(pos_data.symbol, pos_data.direction, pos_data.open_price, pos_data.open_volume);
			long_positions_[trade_data->user_tag][pos_data.symbol] = pos_data;
		} 
		else
		{
			int sum_vom = iter->second.open_volume + trade_data->trade_volume;
			iter->second.open_price = (iter->second.open_price * iter->second.open_volume
				+ trade_data->trade_price * trade_data->trade_volume) / sum_vom;
			iter->second.open_volume = sum_vom;
			iter->second.enable_today_volume += trade_data->trade_volume;
			iter->second.today_volume += trade_data->trade_volume;
			iter->second.using_margin = CalcMargin(iter->second.symbol, iter->second.direction, iter->second.open_price, iter->second.open_volume);
		}
	}
	else {
		PositionData pos_data;
		pos_data.direction = LONG_DIRECTION;
		pos_data.symbol = trade_data->symbol;
		pos_data.open_price = trade_data->trade_price;
		pos_data.open_volume = trade_data->trade_volume;
		pos_data.today_volume = trade_data->trade_volume;
		pos_data.enable_today_volume = trade_data->trade_volume;
		pos_data.last_price = trade_data->trade_price;
		strcpy(pos_data.user_tag, trade_data->user_tag);
		pos_data.using_margin = CalcMargin(pos_data.symbol, pos_data.direction, pos_data.open_price, pos_data.open_volume);
		long_positions_[trade_data->user_tag][pos_data.symbol] = pos_data;
	}
}

void TradeEngine::OpenShort(TradeData* trade_data) {
	if (short_positions_.find(trade_data->user_tag) != short_positions_.end()) {
		std::map<Symbol, PositionData>::iterator iter = short_positions_[trade_data->user_tag].find(trade_data->symbol);
		if (iter == short_positions_[trade_data->user_tag].end())
		{
			PositionData pos_data;
			pos_data.direction = SHORT_DIRECTION;
			pos_data.symbol = trade_data->symbol;
			pos_data.open_price = trade_data->trade_price;
			pos_data.open_volume = trade_data->trade_volume;
			pos_data.enable_today_volume = trade_data->trade_volume;
			pos_data.today_volume = trade_data->trade_volume;
			pos_data.last_price = trade_data->trade_price;
			strcpy(pos_data.user_tag, trade_data->user_tag);
			pos_data.using_margin = CalcMargin(pos_data.symbol, pos_data.direction, pos_data.open_price, pos_data.open_volume);
			short_positions_[trade_data->user_tag][pos_data.symbol] = pos_data;
		} 
		else
		{
			int sum_vom = iter->second.open_volume + trade_data->trade_volume;
			iter->second.open_price = (iter->second.open_price * iter->second.open_volume
				+ trade_data->trade_price * trade_data->trade_volume) / sum_vom;
			iter->second.open_volume = sum_vom;
			iter->second.enable_today_volume += trade_data->trade_volume;
			iter->second.today_volume += trade_data->trade_volume;
			iter->second.using_margin = CalcMargin(iter->second.symbol, iter->second.direction, iter->second.open_price, iter->second.open_volume);
		}
	}
	else {
		PositionData pos_data;
		pos_data.direction = SHORT_DIRECTION;
		pos_data.symbol = trade_data->symbol;
		pos_data.open_price = trade_data->trade_price;
		pos_data.open_volume = trade_data->trade_volume;
		pos_data.enable_today_volume = trade_data->trade_volume;
		pos_data.today_volume = trade_data->trade_volume;
		pos_data.last_price = trade_data->trade_price;
		strcpy(pos_data.user_tag, trade_data->user_tag);
		pos_data.using_margin = CalcMargin(pos_data.symbol, pos_data.direction, pos_data.open_price, pos_data.open_volume);
		short_positions_[trade_data->user_tag][pos_data.symbol] = pos_data;
	}
}

void TradeEngine::CloseShort(TradeData* trade_data) {
	if (short_positions_.find(trade_data->user_tag) != short_positions_.end()) {
		std::map<Symbol, PositionData>::iterator iter = short_positions_[trade_data->user_tag].find(trade_data->symbol);
		if (iter == short_positions_[trade_data->user_tag].end())
		{
			stringstream ss;
			ss << "未找到持仓记录以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume;
			if(err_spi_)err_spi_->OnTradeError( ss.str()); 
			return;
		}

		if (iter->second.open_volume < trade_data->trade_volume)
		{
			stringstream ss;
			ss << "持仓量不足以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume
				<< " (持仓量:" << iter->second.open_volume << ")";
			if(err_spi_)err_spi_->OnTradeError( ss.str()); 
			short_positions_[trade_data->user_tag].erase(iter);
			return;
		}

		if (iter->second.open_volume == trade_data->trade_volume)
		{
			short_positions_[trade_data->user_tag].erase(iter);
			return;
		}

		int vol = iter->second.open_volume - trade_data->trade_volume;
		iter->second.open_price = (iter->second.open_price * iter->second.open_volume - trade_data->trade_price * trade_data->trade_volume) / vol;
		iter->second.open_volume = vol;
		iter->second.today_volume -= trade_data->trade_volume;
		iter->second.enable_today_volume -= trade_data->trade_volume;
		if (trade_data->open_close_flag == CLOSE_ORDER || trade_data->open_close_flag == CLOSE_YESTERDAY_ORDER)
		{
			iter->second.yestd_volume -= trade_data->trade_volume;
		}
		iter->second.using_margin = CalcMargin(iter->second.symbol, iter->second.direction, iter->second.open_price, iter->second.open_volume);
	}
	else if (s_user_id == trade_data->user_tag) { // 从快期等外部软件平仓
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = short_positions_.begin(); iter0 != short_positions_.end(); ++iter0) {
			std::map<Symbol, PositionData>::iterator iter = iter0->second.find(trade_data->symbol);
			if (iter != iter0->second.end())
				iter0->second.erase(iter);	
		}
	}
	else {
		stringstream ss;
		ss << "未找到持仓记录以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume;
		if(err_spi_)err_spi_->OnTradeError( ss.str()); 
		return;
	}
}

void TradeEngine::CloseLong(TradeData* trade_data) {
	if (long_positions_.find(trade_data->user_tag) != long_positions_.end()) {
		std::map<Symbol, PositionData>::iterator iter = long_positions_[trade_data->user_tag].find(trade_data->symbol);
		if (iter == long_positions_[trade_data->user_tag].end())
		{
			stringstream ss;
			ss << "未找到持仓记录以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume;
			if(err_spi_)err_spi_->OnTradeError(ss.str()); 
			return;
		}

		if (iter->second.open_volume < trade_data->trade_volume)
		{
			stringstream ss;
			ss << "持仓量不足以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume
				<< " (持仓量:" << iter->second.open_volume << ")";
			if(err_spi_)err_spi_->OnTradeError(ss.str()); 
			long_positions_[trade_data->user_tag].erase(iter);
			return;
		}

		if (iter->second.open_volume == trade_data->trade_volume)
		{
			long_positions_[trade_data->user_tag].erase(iter);
			return;
		}

		int vol = iter->second.open_volume - trade_data->trade_volume;
		iter->second.open_price = (iter->second.open_price * iter->second.open_volume - trade_data->trade_price * trade_data->trade_volume) / vol;
		iter->second.open_volume = vol;
		iter->second.today_volume -= trade_data->trade_volume;
		iter->second.enable_today_volume -= trade_data->trade_volume;
		if (trade_data->open_close_flag == CLOSE_ORDER || trade_data->open_close_flag == CLOSE_YESTERDAY_ORDER)
		{
			iter->second.yestd_volume -= trade_data->trade_volume;
		}
		iter->second.using_margin = CalcMargin(iter->second.symbol, iter->second.direction, iter->second.open_price, iter->second.open_volume);
	}
	else if (s_user_id == trade_data->user_tag) { // 从快期等外部软件平仓
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = long_positions_.begin(); iter0 != long_positions_.end(); ++iter0) {
			std::map<Symbol, PositionData>::iterator iter = iter0->second.find(trade_data->symbol);
			if (iter != iter0->second.end())
				iter0->second.erase(iter);			
		}
	}
	else {
		stringstream ss;
		ss << "未找到持仓记录以更新平仓成交回报：" << trade_data->symbol.instrument << " 平 " << trade_data->trade_volume;
		if(err_spi_)err_spi_->OnTradeError(ss.str()); 
		return;
	}
}

void TradeEngine::MarginAccountBalance(const TradeData *trade_data) {
	// 占用保证金.
	double margin_balance = CalcMargin(trade_data->symbol, trade_data->direction, trade_data->trade_price, trade_data->trade_volume);
	Locker lock(&account_mutex_);
	account_data_.margin_balance += margin_balance;
	account_data_.enable_balance -= margin_balance;
	account_data_.withdraw_balance -= margin_balance;
}

void TradeEngine::CancelMarginAccountBalance(const TradeData *trade_data) {
	// 取消保证金占用.
	double margin_balance = CalcMargin(trade_data->symbol, trade_data->direction, trade_data->trade_price, trade_data->trade_volume);
	Locker lock(&account_mutex_);
	account_data_.margin_balance -= margin_balance;
	account_data_.enable_balance += margin_balance;
	account_data_.withdraw_balance += margin_balance;
}

void TradeEngine::UpdateAccountCloseProfit(const TradeData *trade_data) {
	// 计算平仓盈亏.
	Locker lock(&last_price_mutex_);
	account_data_.close_profit += CalcFloatProfit(trade_data->symbol, trade_data->direction, trade_data->trade_price, last_prices_[trade_data->symbol], trade_data->trade_volume);
}

void TradeEngine::SendTradeEventData(TradeEventData::EventType type, const UserStrategyIdType user_tag, void *data) {
	if (NULL == event_spi_ || !is_init_) return;
	TradeEventData eve;
	eve.type = type;
	strcpy(eve.flag, user_tag);	
	eve.data = data;
	event_spi_->OnTradeEvent(eve);
}

void TradeEngine::CalcPosiInfoByLastPrice(PositionData &posi_data) {
	last_price_mutex_.Lock();
	posi_data.last_price = last_prices_[posi_data.symbol];
	last_price_mutex_.Unlock();
	if (PriceUnEqual(posi_data.last_price, 0.)) {
		posi_data.position_profit = CalcFloatProfit(posi_data.symbol, posi_data.direction, posi_data.open_price, posi_data.last_price, posi_data.open_volume);
		posi_data.position_cost = posi_data.last_price * info_set_->GetVolMulti(posi_data.symbol) * posi_data.open_volume;
	}
}

void TradeEngine::UpdatePoswWithTrade(TradeData* trade_data)
{
	// 根据成交信息实时更新持仓.
	Locker locker(&pos_mutex_);
	if (trade_data->direction == LONG_DIRECTION) // 多.
	{
		if (trade_data->open_close_flag == OPEN_ORDER) // 开.
		{
			// 开多 .
			OpenLong(trade_data);
			// 占用保证金.
			MarginAccountBalance(trade_data);			
		}
		else  // 平.
		{
			// 平空.
			CloseShort(trade_data);
			// 取消保证金占用.
			CancelMarginAccountBalance(trade_data);
			// 更新账户平仓盈亏.
			UpdateAccountCloseProfit(trade_data);
		}
	}
	else // 空.
	{
		if (trade_data->open_close_flag == OPEN_ORDER) // 开 .
		{
			// 开空.
			OpenShort(trade_data);
			// 占用保证金.
			MarginAccountBalance(trade_data);
		}
		else // 平.
		{
			// 平多.
			CloseLong(trade_data);
			// 取消保证金占用.
			CancelMarginAccountBalance(trade_data);
			// 更新账户平仓盈亏.
			UpdateAccountCloseProfit(trade_data);
		}
	}	
}

void TradeEngine::OnCancelOrder(OrderData* order_data)
{
	SendTradeEventData(TradeEventData::ORDER_EVENT, order_data->user_tag, order_data);

	stringstream ss;
	ss << "撤单失败 : " << order_data->status_msg << " 策略号: " << order_data->user_tag;
	if(err_spi_)err_spi_->OnTradeError(ss.str()); 
}

void TradeEngine::OnQryOrder(int req_id, OrderData* order_data, const std::string& err, bool is_last)
{
	OnOrder(order_data, true, is_last);
}

void TradeEngine::OnQryTrade(int req_id, TradeData* trade_data, const std::string& err, bool is_last)
{
	if (NULL == trade_data) { return; }
	///查下保证金率、手续费率.
	QryMargin(trade_data->symbol.instrument);
	QryCommision(trade_data->symbol.instrument);

	Locker locker(&trade_mutex_);
	trades_.push_back(*trade_data);
}

void TradeEngine::OnQryAccount(int req_id, AccountData* acco_data, const std::string& err, bool is_last)
{
	if (NULL == acco_data)
	{
		if(err_spi_)err_spi_->OnTradeError(std::string("查询资金失败: ") + err); 
		return;
	}

	Locker locker(&account_mutex_);
	account_data_ = *acco_data;

	SendTradeEventData(TradeEventData::ACCOUNT_EVENT, UserManualId, acco_data);
}

void TradeEngine::OnQryPosition(int req_id, PositionData* pos_data, const std::string& err, bool is_last)
{
	if (NULL == pos_data || pos_data->open_volume <= 0) { 
		if (is_last)
			SendTradeEventData(TradeEventData::POSITION_EVENT, pos_data->user_tag, pos_data);
		return; 
	}
	///查下保证金率、手续费率.
	QryMargin(pos_data->symbol.instrument);
	QryCommision(pos_data->symbol.instrument);

	///更新持仓.
	Locker locker(&pos_mutex_);
	if (pos_data->direction == LONG_DIRECTION) // 多.
	{
		if (long_positions_.find(pos_data->user_tag) != long_positions_.end()) {
			std::map<Symbol, PositionData>::iterator iter = long_positions_[pos_data->user_tag].find(pos_data->symbol);
			if (iter == long_positions_[pos_data->user_tag].end()) // 新的持仓到来.
			{
				long_positions_[pos_data->user_tag][pos_data->symbol] = *pos_data;
			} 
			else // 查持仓明细 ： 同一个合约的持仓可能会按成交编号分割.
			{ 
				int sum_vom = iter->second.open_volume + pos_data->open_volume;
				iter->second.open_price = (iter->second.open_price * iter->second.open_volume
					+ pos_data->open_price * pos_data->open_volume) / sum_vom;
				iter->second.open_volume = sum_vom;				
				iter->second.today_volume += pos_data->today_volume;
				iter->second.yestd_volume += pos_data->yestd_volume;
				iter->second.enable_today_volume += pos_data->enable_today_volume;
				iter->second.enable_yestd_volume += pos_data->enable_yestd_volume;				
				iter->second.using_margin += pos_data->using_margin;
			}
		}
		else { // 新的持仓到来.
			long_positions_[pos_data->user_tag][pos_data->symbol] = *pos_data;
		}
	}
	else // 空.
	{
		if (short_positions_.find(pos_data->user_tag) != short_positions_.end()) {
			std::map<Symbol, PositionData>::iterator iter = short_positions_[pos_data->user_tag].find(pos_data->symbol);
			if (iter == short_positions_[pos_data->user_tag].end()) // 新的持仓到来.
			{
				short_positions_[pos_data->user_tag][pos_data->symbol] = *pos_data;
			} 
			else // 查持仓明细 ： 同一个合约的持仓可能会按成交编号分割.
			{
				int sum_vom = iter->second.open_volume + pos_data->open_volume;
				iter->second.open_price = (iter->second.open_price * iter->second.open_volume
					+ pos_data->open_price * pos_data->open_volume) / sum_vom;
				iter->second.open_volume = sum_vom;
				iter->second.today_volume += pos_data->today_volume;
				iter->second.yestd_volume += pos_data->yestd_volume;
				iter->second.enable_today_volume += pos_data->enable_today_volume;
				iter->second.enable_yestd_volume += pos_data->enable_yestd_volume;
				iter->second.last_price = pos_data->last_price;
				iter->second.using_margin += pos_data->using_margin;
			}
		}
		else { // 新的持仓到来
			short_positions_[pos_data->user_tag][pos_data->symbol] = *pos_data;
		}
	}

	if (is_last)
		SendTradeEventData(TradeEventData::POSITION_EVENT, pos_data->user_tag, pos_data);
}

///非初始化时,profit表示差额(本次浮动盈亏与上个浮动盈亏之差),为输入参数.
///初始化时，profit表示浮动盈亏，为输出参数.
void TradeEngine::UpdateAccountProfit(PriceType &profit, bool init) {
	Locker locker(&account_mutex_);
	if (!init) { // 非初始化.
		account_data_.position_profit += profit;
		account_data_.asset_balance += profit;
		account_data_.enable_balance += profit;
		account_data_.withdraw_balance += profit;
	}
	else {
		account_data_.position_profit = account_data_.asset_balance - account_data_.static_balance;
		profit = account_data_.position_profit;
	}
	//SendTradeEventData(TradeEventData::ACCOUNT_EVENT, UserManualId, NULL);
}

void TradeEngine::GetAccount(AccountData &account)
{
	Locker locker(&account_mutex_);
	account = account_data_;
}

void TradeEngine::GetAllOrder(std::vector<OrderData>& orders, const UserStrategyIdType user_tag) {
	Locker locker(&order_mutex_);
	for (std::list<OrderData>::iterator iter = rejected_orders_.begin(); iter != rejected_orders_.end(); ++iter)
	{
		if (NULL == user_tag || !strcmp(iter->user_tag, user_tag)) orders.push_back(*iter);
	}
	for (std::map<NumberIdType, OrderData>::iterator iter = all_orders_.begin(); iter != all_orders_.end(); ++iter)
	{
		if (NULL == user_tag || !strcmp(iter->second.user_tag, user_tag)) orders.push_back(iter->second);
	}
}

void TradeEngine::GetValidOrder(std::vector<OrderData>& orders, const UserStrategyIdType user_tag)
{
	Locker locker(&order_mutex_);

	for (std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.begin(); iter != valid_orders_.end(); ++iter)
	{
		if (NULL == user_tag || !strcmp(iter->second.user_tag, user_tag)) orders.push_back(iter->second);
	}
}


bool TradeEngine::GetOrderByLocalId(OrderData& order, NumberIdType local_order_id, const UserStrategyIdType user_tag)
{
	Locker locker(&order_mutex_);
	for (std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.begin(); iter != valid_orders_.end(); ++iter)
	{
		if (iter->second.local_order_id == local_order_id && (!strcmp(iter->second.user_tag, user_tag) || NULL == user_tag)){
			order = iter->second;
			return true;
		}
	}
	return false;
}

void TradeEngine::GetOrderBySymbol(std::vector<OrderData>& orders, const Symbol& sym, const UserStrategyIdType user_tag)
{
	Locker locker(&order_mutex_);
	for (std::map<NumberIdType, OrderData>::iterator iter = valid_orders_.begin(); iter != valid_orders_.end(); ++iter)
	{
		if (iter->second.symbol == sym && (!strcmp(iter->second.user_tag, user_tag) || NULL == user_tag))
			orders.push_back(iter->second);
	}
}

void TradeEngine::GetAllTrade(std::vector<TradeData>& trades, const UserStrategyIdType user_tag)
{
	Locker locker(&trade_mutex_);
	for (std::vector<TradeData>::const_iterator iter = trades_.begin(); iter != trades_.end(); ++iter) {
		if (user_tag == NULL || !strcmp(iter->user_tag, user_tag)) trades.push_back(*iter);
	}
}

void TradeEngine::GetTradeBySymbol(std::vector<TradeData>& trades, const Symbol& sym, const UserStrategyIdType user_tag)
{
	Locker locker(&trade_mutex_);
	for (std::vector<TradeData>::const_iterator iter = trades_.begin(); iter != trades_.end(); ++iter) {
		if (user_tag == NULL || !strcmp(iter->user_tag, user_tag)) 
			trades.push_back(*iter);
	}
}

void TradeEngine::GetAllLongPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag)
{
	Locker locker(&pos_mutex_);
	///持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	if (nullptr == user_tag) { // 取所有子策略的.
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = long_positions_.begin(); iter0 != long_positions_.end(); ++iter0) {
			for (std::map<Symbol, PositionData>::iterator iter = iter0->second.begin(); iter != iter0->second.end(); ++iter) 	{
				CalcPosiInfoByLastPrice(iter->second);
				pos.push_back(iter->second);
			}
		}
		return;
	}

	if (long_positions_.find(user_tag) == long_positions_.end()) return; // 没有该子策略的持仓.
	for (std::map<Symbol, PositionData>::iterator iter = long_positions_[user_tag].begin(); iter != long_positions_[user_tag].end(); ++iter)
	{
		CalcPosiInfoByLastPrice(iter->second);
		pos.push_back(iter->second);
	}
}

void TradeEngine::GetAllShortPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag)
{
	Locker locker(&pos_mutex_);
	///持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	if (nullptr == user_tag) { // 取所有子策略的.
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = short_positions_.begin(); iter0 != short_positions_.end(); ++iter0) {
			for (std::map<Symbol, PositionData>::iterator iter = iter0->second.begin(); iter != iter0->second.end(); ++iter) 	{
				CalcPosiInfoByLastPrice(iter->second);
				pos.push_back(iter->second);
			}
		}
		return;
	}

	if (short_positions_.find(user_tag) == short_positions_.end()) return; // 没有该子策略的持仓.
	for (std::map<Symbol, PositionData>::iterator iter = short_positions_[user_tag].begin(); iter != short_positions_[user_tag].end(); ++iter)
	{
		CalcPosiInfoByLastPrice(iter->second);
		pos.push_back(iter->second);
	}
}

void TradeEngine::GetAllPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag)
{
	Locker locker(&pos_mutex_);
	///持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	if (nullptr == user_tag) { // 取所有子策略的.
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = long_positions_.begin(); iter0 != long_positions_.end(); ++iter0) {
			for (std::map<Symbol, PositionData>::iterator iter = iter0->second.begin(); iter != iter0->second.end(); ++iter) 	{
				CalcPosiInfoByLastPrice(iter->second);
				pos.push_back(iter->second);
			}
		}
		for (std::map<std::string, std::map<Symbol, PositionData> >::iterator iter0 = short_positions_.begin(); iter0 != short_positions_.end(); ++iter0) {
			for (std::map<Symbol, PositionData>::iterator iter = iter0->second.begin(); iter != iter0->second.end(); ++iter) 	{
				CalcPosiInfoByLastPrice(iter->second);
				pos.push_back(iter->second);
			}
		}
		return;
	}

	if (long_positions_.find(user_tag) != long_positions_.end()) {
		for (std::map<Symbol, PositionData>::iterator iter = long_positions_[user_tag].begin(); iter != long_positions_[user_tag].end(); ++iter)
		{
			CalcPosiInfoByLastPrice(iter->second);
			pos.push_back(iter->second);
		}
	}
	if (short_positions_.find(user_tag) != short_positions_.end()) {
		for (std::map<Symbol, PositionData>::iterator iter = short_positions_[user_tag].begin(); iter != short_positions_[user_tag].end(); ++iter)
		{
			CalcPosiInfoByLastPrice(iter->second);
			pos.push_back(iter->second);
		}
	}
}
// 多头头寸
bool TradeEngine::GetLongPositionBySymbol(PositionData &pos, const Symbol& sym, const UserStrategyIdType user_tag)
{
	Locker locker(&pos_mutex_);
	if (long_positions_.empty()) return false;

	std::map<std::string, std::map<Symbol, PositionData> >::iterator pos_it;
	if (user_tag != NULL)
		pos_it = long_positions_.find(user_tag);
	else
		pos_it = long_positions_.begin();
		
	if (pos_it != long_positions_.end())
	{
		std::map<Symbol, PositionData>::iterator it = pos_it->second.find(sym);
		if (it != pos_it->second.end())
		{
			pos = it->second;
			return true;
		}
	}
		
	return false;
}

// 空头头寸
bool TradeEngine::GetShortPositionBySymbol(PositionData &pos, const Symbol& sym, const UserStrategyIdType user_tag)
{
	Locker locker(&pos_mutex_);
	if (short_positions_.empty()) return false;

	std::map<std::string, std::map<Symbol, PositionData> >::iterator pos_it;
	if (user_tag != NULL)
		pos_it = short_positions_.find(user_tag);
	else
		pos_it = short_positions_.begin();

	if (pos_it != short_positions_.end())
	{
		std::map<Symbol, PositionData>::iterator it = pos_it->second.find(sym);
		if (it != pos_it->second.end())
		{
			pos = it->second;
			return true;
		}
	}

	return false;
}

