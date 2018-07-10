#ifndef STRATEGY_H
#define STRATEGY_H

//#include "dataserver/DataApiDefine.h"
#include "account/TradeEngine.h"
//#include "account/DataEngine.h"
#include "strategy/Client.h"
#include "network/Timer.h"
//#include "ctp/MarketDataApi.h"
//#include "IndexLib/iMA.h"


using namespace std;

class STRATEGY_API Strategy : public TradeEventSpi, public TradeErrorSpi, public TimerSpi, public ClientSpi//, public DataEventSpi
{
public:
	Strategy();
	virtual ~Strategy();
	
	bool Init(string &);
	void Denit();

private:
	/*
	// OnTick
	virtual void OnUpdateKline(const Bars *bars, bool is_new);
	// OnBar
	virtual void OnKlineFinish(const Bars *bars);
	// Init: OnTick & OnBar
	virtual void OnInitKline(const Bars *bars);

	bool SubBars(const Symbol& symbol, const std::string& period);
	*/

	virtual void OnTimer();
	virtual void OnData(Bars *, bool);
	virtual void OnError(const string &);

	// OnOrder OnTrade
	virtual void OnTradeEvent(TradeEventData& event);
	// OnError
	virtual void OnTradeError(const string &);
	
	//virtual void OnTick(FutureTick *);

	void UpdateAccount();
	void UpdateValidOrders();
	void UpdateTrades();
	void UpdatePositions();

	// 指标.
	// iMA s_ma_; // 5min
	// iMA l_ma_; // 10min

	// 策略编号.
	UserStrategyIdType user_tag_;
	std::vector<PositionData> positions_;
	std::vector<PriceType> float_profit_; // 持仓的浮动盈亏.

	SpinLock pos_mutex_;
	int state_; //0:没开仓、1:开多仓、2:开空仓.
	double price_offset_; //对手价+N
	int submit_hands_;//下单手数.
	string cancel_interval_;//撤单间隔时间.

	// 账户资金到某个设定金额，自动全部平仓： 资金止盈，总的资金量达到设定值.
	double target_profit_value_;
	Symbol symbol_;
	double last_price_;

	TradeEngine *trade_engine_;
	//DataEngine *data_engine_;

	TimerApi *timer_;
	Client *client_;
};

#endif