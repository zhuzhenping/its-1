#ifndef ITS_TRADE_EIGINE_H_
#define ITS_TRADE_EIGINE_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include "common/SpinLock.h"
#include "account/CTPMarginCommision.h"

//namespace itstation {


// 交易事件.
struct TradeEventData
{
	enum EventType
	{
		ORDER_EVENT,
		TRADE_EVENT,
		POSITION_EVENT,
		ACCOUNT_EVENT
	};
	UserStrategyIdType flag;
	EventType type;
	void* data;
};
class TradeEventSpi 
{
public:
	virtual void OnTradeEvent(TradeEventData& event) = 0;
};
// 交易出错提示.
class TradeErrorSpi 
{
public:
	virtual void OnTradeError(const std::string &) = 0;
};

class ACCOUNT_API TradeEngine : public TradeSpi, public CTPMarginCommision
{
public:
	static TradeEngine* Instance();
	void SetSpi(TradeEventSpi* trade_spi, TradeErrorSpi *err_spi=NULL);
	bool Init(std::string& err);
	void Denit();
	
	///交易接口：返回本地委托编号LocalOrderID>0.如果出错，返回0
	// 买开(LimitPrince)
	int Buy(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag = NULL);
	// 买开(MarketPrince).
	int BuyMar(const Symbol& symbol, int volume, const UserStrategyIdType user_tag = NULL);	
	// 买平. 
	int BuyCover(const Symbol& symbol, double price, int volume, bool is_td/*是否今仓*/, const UserStrategyIdType user_tag = NULL);
	// 买平. 优先平今仓，再平昨仓.
	int BuyCover(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag = NULL);	
	// 卖开(LimitPrince)
	int SellShort(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag = NULL);
	// 卖开(MarketPrince)
	int SellShortMar(const Symbol& symbol,int volume, const UserStrategyIdType user_tag = NULL);
	// 卖平
	int Sell(const Symbol& symbol, double price, int volume, bool is_td/*是否今仓*/, const UserStrategyIdType user_tag = NULL);
	// 卖平. 优先平今仓，再平昨仓.
	int Sell(const Symbol& symbol, double price, int volume, const UserStrategyIdType user_tag = NULL);		
	// 平仓：先平今仓，再平昨仓.
	
	// 根据本地委托编号撤单.
	void CancelOrder(int LocalOrderID);
	// 根据合约名撤单.
	void CancelOrder(const Symbol& sym, const UserStrategyIdType user_tag = NULL);

	///基础交易接口：
	// 返回本地委托编号LocalOrderID>0.如果出错，返回0
	int SubmitOrder(const OrderParamData& param);
	// 撤单.
	void CancelOrder(const OrderData& param);	

	///查询接口.
	// 查资金账户.
	void GetAccount(AccountData &);
	// 查委托.
	void GetAllOrder(std::vector<OrderData>& orders, const UserStrategyIdType user_tag = NULL);
	void GetValidOrder(std::vector<OrderData>& orders, const UserStrategyIdType user_tag = NULL);//可撤单.
	bool GetOrderByLocalId(OrderData& order, NumberIdType local_order_id, const UserStrategyIdType user_tag = NULL);//根据本地编号.
	void GetOrderBySymbol(std::vector<OrderData>& orders, const Symbol& sym, const UserStrategyIdType user_tag = NULL);
	// 查成交.
	void GetAllTrade(std::vector<TradeData>& trades, const UserStrategyIdType user_tag = NULL);
	void GetTradeBySymbol(std::vector<TradeData>& trades, const Symbol& sym, const UserStrategyIdType user_tag = NULL);
	// 查持仓 (注意：持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算).
	void GetAllLongPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag = NULL);
	void GetAllShortPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag = NULL);
	void GetAllPosition(std::vector<PositionData>& pos, const UserStrategyIdType user_tag = NULL);
	// user_tag如果为NULL，查long_positions_的第一个.
	bool GetLongPositionBySymbol(PositionData &pos, const Symbol& sym, const UserStrategyIdType user_tag = NULL);
	bool GetShortPositionBySymbol(PositionData &pos, const Symbol& sym, const UserStrategyIdType user_tag = NULL);

	///tick到来，如果行情变化，则要更新持仓的最新价、浮动盈亏.
	// 当行情变化时，更新账户的总浮动盈亏.
	void UpdateAccountProfit(PriceType &profit, bool init = false);
	PriceType CalcFloatProfit(const Symbol &, OrderDirection, PriceType open_price, PriceType last_price, VolumeType open_volume);
	// 更新有持仓的合约的最新价。注意：考虑到效率，持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	void SetPosiLastPrice(const Symbol &, PriceType);
	PriceType GetPosiPrePrice(const Symbol &sym);

	virtual ~TradeEngine();
	// 异步查询CTP的资金账号.
	void QryCtpAccount();
private:
	TradeEngine();

	virtual void OnError(const int request_id, const std::string& error_msg);
	virtual void OnOrderError(OrderData* order_data);
	virtual void OnDisconnect(const std::string& reson);
	virtual void OnConnect();
	virtual void OnOrder(OrderData* order_data);
	virtual void OnTrade(TradeData* trade_data);
	virtual void OnCancelOrder(OrderData* canceled_data);
	virtual void OnQryOrder(int req_id, OrderData* order_data, const std::string& err, bool is_last);
	virtual void OnQryTrade(int req_id, TradeData* trade_data, const std::string& err, bool is_last);
	virtual void OnQryAccount(int req_id, AccountData* trade_data, const std::string& err, bool is_last);
	virtual void OnQryPosition(int req_id, PositionData* trade_data, const std::string& err, bool is_last);
	
	void OnOrder(OrderData *order_data, bool is_qry, bool is_last = false);

	///根据成交信息实时更新持仓.
	void UpdatePoswWithTrade(TradeData* trade_data);
	void OpenLong(TradeData* trade_data);
	void CloseShort(TradeData* trade_data);
	void OpenShort(TradeData* trade_data);
	void CloseLong(TradeData* trade_data);

	///判断是不是有效订单  没有撤单或者失败.
	bool IsOrderValid(OrderStatus status);	
	///当用户查持仓时，计算持仓的最新价、持仓盈亏、持仓市值.
	void CalcPosiInfoByLastPrice(PositionData &);
	///刷新界面
	void SendTradeEventData(TradeEventData::EventType, const UserStrategyIdType, void *);


	///---------------更新资金账户----------------------------------------------
	///开仓时，冻结保证金+冻结手续费 = 下单冻结.
	void FrozenAccountBalance(const OrderData *order_data);
	///撤销开仓单时，取消下单冻结.
	void CancelFrozenAccountBalance(const OrderData *order_data);
	///中金所开仓、平仓时，即使不成交也收1块手续费.
	void MinusAccountCommision(const OrderData *order_data);
	///成交时扣除手续费.
	void MinusAccountCommision(TradeData *trade_data);
	///开仓占用保证金.
	void MarginAccountBalance(const TradeData *trade_data);
	///平仓取消占用.
	void CancelMarginAccountBalance(const TradeData *trade_data);	
	///平仓时更新平仓盈亏.
	void UpdateAccountCloseProfit(const TradeData *trade_data);


private:
	TradeEventSpi* event_spi_;
	TradeErrorSpi* err_spi_;
	static TradeEngine* self_;
	bool is_init_;

	///资金账户 ：1、下单时冻结保证金，撤单时取消冻结 2、成交时扣除手续费 3、开仓占用保证金，平仓取消占用 4、平仓时更新平仓盈亏.
	AccountData account_data_;
	std::map<NumberIdType, OrderData> valid_orders_;
	std::map<NumberIdType, OrderData> all_orders_;
	std::list<OrderData> rejected_orders_;
	std::vector<TradeData> trades_;
	std::map<std::string, std::map<Symbol, PositionData> > long_positions_; // 根据user_tag进行分类.
	std::map<std::string, std::map<Symbol, PositionData> > short_positions_;
	///持仓合约的现价。持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算.
	std::map<Symbol, PriceType> last_prices_;

	SpinLock account_mutex_;
	SpinLock order_mutex_;
	SpinLock trade_mutex_;
	SpinLock pos_mutex_;
	SpinLock last_price_mutex_;

	bool re_qry_pos_; // init时，如果发生ontrade，需要重新查持仓.
	
};


#endif