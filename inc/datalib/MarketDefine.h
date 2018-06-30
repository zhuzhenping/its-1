#ifndef MARKET_DEFINE_H_
#define MARKET_DEFINE_H_

#include <string>
#include <map>
#include "common/DateTime.h"
//#ifndef Q_MOC_RUN	//为了解除与qmake的冲突
//#include "boost/date_time/posix_time/posix_time.hpp" 
//#endif

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#ifdef WIN32
#include <float.h>

#define is_nan(x) _isnan(x)

#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const double *) __nan)
#endif

//#elif defined __GNUG__
#else
#include <math.h>
#include <string.h>
#define is_nan(x) isnan(x)
#endif

//namespace itstation {

#define EXCHANGE_SSE		'0'	// 上海证券交易所.
#define EXCHANGE_SZE		'1'	// 深圳证券交易所.	 
#define EXCHANGE_CFFEX		'2'	// 中国金融期货交易所.
#define EXCHANGE_DCE		'3'	// 大连商品交易所. 
#define EXCHANGE_CZCE		'4'	// 中国郑州商品交易所. 
#define EXCHANGE_SHFE		'5'	// 上海期货交易所.
#define EXCHANGE_SGE		'6'	// 上海黄金交易所.
#define EXCHANGE_INE		'7'	// 上海国际能源交易中心.
#define EXCHANGE_HK			'8'	// 港交所.
#define EXCHANGE_OTHER		'n' //
typedef char ExchangeIdType;	//交易所类型

#define PRODUCT_STOCK		'0'
#define PRODUCT_FUTURE		'1'
#define PRODUCT_OPTION		'2'
#define PRODUCT_BOND		'3'
#define PRODUCT_FUND		'4'
#define PRODUCT_INDEX		'5' 
#define PRODUCT_SPOT		'6' 
#define PRODUCT_OTHER		'n' 
typedef char ProductIdType;		//品种类型

#define DIMENSION_SECOND	'0'
#define DIMENSION_MINUTE	'1'	
#define DIMENSION_HOUR		'2'	
#define DIMENSION_DAY		'3'
#define DIMENSION_WEEK		'4'
#define DIMENSION_MONTH		'5'
#define DIMENSION_YEAR		'6'
#define DIMENSION_TICK		'9'
typedef char  DimensionType;	//K线周期

#define ORDER_STATUS_INVALID					'0'		/**< 订单无效,对应期货的未知 */
#define ORDER_STATUS_WAIT_SUBMIT				'1'		/**< 待报 */
#define ORDER_STATUS_BEEN_SUBMIT				'2'		/**< 已报(股票);未成交(期货) */
#define ORDER_STATUS_PART_TRADE					'3'		/**< 部分成交 */
#define ORDER_STATUS_ALL_TRADE					'4'		/**< 全部成交 */
#define ORDER_STATUS_BEEN_SUBMIT_WAIT_CANCEL	'5'		/**< 已报待撤(股票) */
#define ORDER_STATUS_PART_TRADE_WAIT_CANCEL		'6'		/**< 部成待撤(股票) */
#define ORDER_STATUS_BEEN_CANCEL				'7'		/**< 部撤、已撤（股票 场内撤单）；撤单(期货) */
#define ORDER_STATUS_NOT_TOUCHED				'8'		/**< 条件单尚未触发 */
#define ORDER_STATUS_TOUCHED					'9'		/**< 条件单已触发 */
#define ORDER_STATUS_ERROR						'n'		/**< 下单错误 */
#define CANCEL_ORDER_STATUS_ERROR				'm'		/**< 撤单错误 */
typedef char OrderStatus;								/**< 订单状态 */

#define ALL_DIRECTION		'0'		/**< 查：净持仓；平仓：多空双向 */
#define LONG_DIRECTION		'1'		/**< 买多 */
#define SHORT_DIRECTION		'2'		/**< 卖空 */
typedef char OrderDirection;		/**< 买卖方向 */

#define OPEN_ORDER				'0'		/**< 开仓 */
#define CLOSE_ORDER				'1'		/**< 平仓 */
#define CLOSE_TODAY_ORDER		'3'		/**< 平今，只对期货 */
#define CLOSE_YESTERDAY_ORDER	'4'		/**< 平昨，只对期货 */
#define FORCE_CLOSE_ORDER		'5'		/**< 强平 */
typedef char OpenCloseFlag;				/**< 开平标志 */

#define LIMIT_PRICE_ORDER			'0'		/**< 限价 */
#define MARKET_PRICE_ORDER			'1'		/**< 市价,股票目前不支持 */
#define SZ_DS_BEST_PRICE_ORDER		'2'		/**< 深对手方最优价格委托 */
#define SZ_BF_BEST_PRICE_ORDER		'3'		/**< 深本方最优价格委托 */
#define SZ_CANCEL_PRICE_ORDER		'4'		/**< 深即时成交剩余撤销委托 */
#define HS_CANCEL_PRICE_ORDER		'5'		/**< 沪深最优五档并撤销委托 */
#define SZ_ALL_TRADE_PRICE_ORDER	'6'		/**< 深全额成交或撤销委托 */
#define SH_BEST_CHA_PRICE_ORDER		'7'		/**< 沪最优五档并转限价委托 */
typedef char OrderPriceFlag;				/**< 委托类型 */

#define MONEY_RMB				'0'		/**< 人民币 */
#define MONEY_HK				'1'		/**< 港币 */
#define MONEY_DOLLAR			'2'		/**< 美元 */
typedef char MoneyType;					/**< 币值类型 */

#define NAN_PRICE_UNIT '\x80'	//代表空值
typedef signed char PriceMoveUnit;	//买、卖5档相对于最新价的偏移单位（以最小跳动为单位），卖盘为大于0，买盘为小于0

#define COMM_TICK_DATA			'0'
#define COMM_TICK_SET			'1'
#define COMM_KLINE_DATA			'2'
#define COMM_KLINE_SET			'3'
typedef char CommDataType;		//通信数据类型

#define HF_SPECULATION '1'		///投机
#define HF_ARBITRAGE '2'		///套利
#define HF_HEGE '3'				///套保
typedef char HedgeFlagType;

#define STK_STATUS_N '0'		///通常状态
#define STK_STATUS_T '1'		///停牌状态
#define STK_STATUS_Z '2'		///退市整理期
#define STK_STATUS_UNKNOWN 'n'	///未知
typedef char StockStatusType;	///股票状态

#define ERROR_MSG_LEN		 80		/**<回报错误信息长度 */
#define STRUCT_ALIGN_LEN      1		/**< 结构体对齐长度 */
#define PRICE_LEVEL_NUM       5
//#define COMM_IDENTIFY '\xff'	//通信标志
#define NAN_SYMBOL Symbol(PRODUCT_OTHER, EXCHANGE_OTHER, "")
#define NAN_LOCAL_ORDER_ID -1

typedef char TimeFieldType;	//时间域，为年份时表示和1900年只差
typedef char String32[32];
typedef char String16[16];
typedef char InstrumentIdType[23];
typedef char InstrumentNameType[24];
typedef char StatusMsgType[81];
typedef char UserIdType[20];
typedef char PwdType[20];
typedef long long NumberIdType;
//typedef float MovePointType;		//最小跳动
typedef float REAL;
typedef double PriceType;	//价格类型
typedef int VolumeType;	//量类型 期货：手，股票：股
typedef String16 UserStrategyIdType; // 子账号&策略号
#define UserManualId "999999"

#pragma pack(STRUCT_ALIGN_LEN)

//通信数据头 (2B)
//struct CommDataHead 
//{
//	char identify;
//	CommDataType type;
//	CommDataHead() {
//		identify = COMM_IDENTIFY;
//		type = COMM_TICK_DATA;
//	}
//};

//交易品种
struct Symbol 
{
	ProductIdType product;
	ExchangeIdType exchange;
	InstrumentIdType instrument;
	Symbol(char prod = PRODUCT_OTHER, char exch = EXCHANGE_OTHER, const char* inst = "") : product(prod), exchange(exch) { memcpy(instrument, inst, sizeof(InstrumentIdType)); }
	Symbol(const Symbol& other) : product(other.product), exchange(other.exchange) { memcpy(instrument, other.instrument, sizeof(InstrumentIdType)); }
	Symbol& operator=(const Symbol& other) {
		if (this != &other) {
			product = other.product;
			exchange = other.exchange;
			memcpy(instrument, other.instrument, sizeof(InstrumentIdType));
		}
		return *this;
	}

	//Symbol(const char* str){
	//	product = PRODUCT_FUTURE;
	//	exchange = EXCHANGE_CFFEX;
	//	memcpy(instrument, str, sizeof(InstrumentIdType));
	//}

	Symbol& operator=(const char* inst)
	{
		memcpy(instrument, inst, sizeof(InstrumentIdType));
		return *this;
	}

	std::string Str() const 
	{
		char str[12] = {0};
		sprintf(str, "%c%c_%s", product, exchange, instrument);
		return str;
	}

	bool operator==(const Symbol& other) const 
	{
		if (product == PRODUCT_FUTURE)
		{
			return (product == other.product) && (strcmp(instrument, other.instrument) == 0);
		}
		return (product == other.product) && (exchange == other.exchange) && (strcmp(instrument, other.instrument) == 0);
	}

	bool operator!=(const Symbol& other) const 
	{
		return !(*this == other);
	}

	bool operator<(const Symbol& other) const 
	{
		if (product == PRODUCT_FUTURE)
		{
			return product < other.product  || (product == other.product && strcmp(instrument, other.instrument) < 0);
		}
		return product < other.product  || (product == other.product && exchange < other.exchange) ||
			(product == other.product && exchange == other.exchange && strcmp(instrument, other.instrument) < 0);
	}

	bool operator>(const Symbol& other) const 
	{
		if (product == PRODUCT_FUTURE)
		{
			return product > other.product  || (product == other.product && strcmp(instrument, other.instrument) > 0);
		}
		return product > other.product  || (product == other.product && exchange > other.exchange) ||
			(product == other.product && exchange == other.exchange && strcmp(instrument, other.instrument) > 0);
	}
	bool operator<=(const Symbol& other) const { return !(*this > other); }
	bool operator>=(const Symbol& other) const { return !(*this < other); }
};
//交易品种(带合约名)
struct SymbolEx : public Symbol 
{
	InstrumentNameType name;

	SymbolEx(char *_name = "") : Symbol() { memcpy(name, _name, sizeof(InstrumentNameType)); }

	void FromStr(const char* str)
	{
		InstrumentIdType inst = {0};
		InstrumentNameType na = {0};
		sscanf(str, "%c,%[^,],%s", &exchange, inst, na);
		strcpy(instrument, inst);
		strcpy(name, na);
	}
	std::string ToStr() const
	{
		char tmp[64] = {0};
		sprintf(tmp, "%c,%s,%s", exchange, instrument, name);
		return tmp;
	}
};

/*
 * @brief OrderParamData 委托参数结构体 TODO:改名为SubmitParam
 */
struct OrderParamData
{
	Symbol symbol;							/**< 代码 */
	OrderDirection direction;				/**< 买卖方向 */
	OpenCloseFlag open_close_flag;			/**< 开平标志(专属期货) */
	OrderPriceFlag order_price_flag;		/**< 委托类型 */
	PriceType limit_price;					/**< 委托限价 */
	VolumeType volume;						/**< 总的委托量 */
	HedgeFlagType hedge_flag;				/**< 投机套保标志(专属期货) */
	UserStrategyIdType user_tag;			/**< 用户的自定义标志 */
	
	int	order_group;						/**< 委托批号(专属JZ股票) */
	int entrust_id;							/**< 合同号(专属JSD股票) */

	OrderParamData() { memset(this, 0, sizeof(OrderParamData)); }
};

/*
 * @brief OrderData 委托回报结构体
 */
struct OrderData 
{
	Symbol symbol;							/**< 行情代码 */
	NumberIdType order_id;					/**< 订单号 */
	NumberIdType local_order_id;			/**< 本地订单号 */
	UserStrategyIdType user_tag;			/**< 用户的自定义标志 */
	OrderStatus status;						/**< 订单状态 */
	DateTime submit_time;			/**< 委托时间 */
	DateTime update_time;			/**< 最近更新时间 */
	OrderDirection direction;				/**< 买卖方向 */
	OpenCloseFlag open_close_flag;			/**< 开平标志(专属期货) */
	OrderPriceFlag order_price_flag;		/**< 委托类型 */
	PriceType limit_price;					/**< 委托限价 */
	VolumeType total_volume;				/**< 总的委托量 */
	VolumeType trade_volume;				/**< 已成交的量 */
	HedgeFlagType hedge_flag;				/**< 投机套保标志(专属期货) */
	StatusMsgType status_msg;				/**< 详细状态信息 */

	// 为国君证券
	int	order_group;						/**< 委托批号 */

	OrderData()
		: status(ORDER_STATUS_INVALID)
		, direction(LONG_DIRECTION)
		, open_close_flag(OPEN_ORDER)
		, order_price_flag(LIMIT_PRICE_ORDER)
		, limit_price(0)
		, total_volume(0)
		, trade_volume(0)
		, order_group(-1)
	{
		memset(user_tag, 0, sizeof(UserStrategyIdType));
		memset(status_msg, 0, sizeof(StatusMsgType));
	}
};

//struct CancelOrderParamData
//{
//	long long order_id;
//	ExchangeIdType exchange;
//	CancelOrderParamData() : order_id(0), exchange(EXCHANGE_SSE) {}
//};

struct CancelOrderData
{
	UserStrategyIdType user_tag;			/**< 用户的自定义标志 */
	NumberIdType order_id;
	bool succed;
	char error[64];
	CancelOrderData() : succed(true) { memset(user_tag, 0, sizeof(UserStrategyIdType)); memset(error, 0, 64); }
};

struct BaseInstrumentInfo 
{
	SymbolEx symbol;						/**< 行情代码 */
	double	price_tick;						/**< 最小变动价位 */

	BaseInstrumentInfo() : price_tick(0.0) {}
	virtual void FromStr(const char* str) = 0;
	virtual std::string ToStr() const = 0;
};

struct StockInstrumentInfo : public BaseInstrumentInfo 
{
	PriceType	max_rise_value;				/**< 涨停价 */
	PriceType	max_down_value;				/**< 跌停价 */
	VolumeType	max_qty;					/**< 最高数量 单位:手 */
	VolumeType	min_qty;					/**< 最低数量 单位:手 */
	VolumeType	buy_unit;					/**< 买入最小单位 */
	VolumeType	sell_unit;					/**< 卖出最小单位 */
	StockStatusType	status;	

	void FromStr(const char* str)
	{
		symbol.product = PRODUCT_STOCK;
		InstrumentIdType instrument = {0};
		InstrumentNameType name = {0};
		sscanf(str, "%c,%[^,],%[^,],%lf,%lf,%lf,%d,%d,%d,%d,%c", &symbol.exchange, instrument, name,
			&price_tick, &max_rise_value, &max_down_value, &max_qty, &min_qty, &buy_unit, &sell_unit, &status);
		strcpy(symbol.instrument, instrument);
		strcpy(symbol.name, name);
	}
	std::string ToStr() const
	{
		char tmp[64] = {0};
		sprintf(tmp, "%c,%s,%s,%g,%g,%g,%d,%d,%d,%d,%c", symbol.exchange, symbol.instrument, symbol.name, 
			price_tick, max_rise_value, max_down_value, max_qty, min_qty, buy_unit, sell_unit, status);
		return tmp;
	}
};

struct InstrumentInfo {
	SymbolEx symbol;						
	int		vol_multi;						/**< 合约数量乘数 */
	double	price_tick;						/**< 最小变动价位 */

	InstrumentInfo() { memset(this, 0, sizeof(InstrumentInfo)); }
	void FromStr(const char* str)
	{
		symbol.product = PRODUCT_FUTURE;
		sscanf(str, "%c,%[^,],%[^,],%d,%lf", 
			&symbol.exchange, symbol.instrument, symbol.name, 
			&vol_multi, &price_tick);
	}
	std::string ToStr() const
	{
		char tmp[256] = {0};
		sprintf(tmp, "%c,%s,%s,%d,%g", symbol.exchange, symbol.instrument, 
			symbol.name, vol_multi, price_tick);
		return tmp;
	}
	bool operator<(const InstrumentInfo& other) const
	{
		return symbol < other.symbol;
	}
};

// 期货合约信息
struct InstrumentInfoData : public BaseInstrumentInfo {
	int		vol_multi;						/**< 合约数量乘数 */
	bool	is_trading;						/**< 当前是否交易 */
	// 成交额 * 保证金率 => 保证金 ;  投机: 上期所按大额单边收取保证金，其他交易所收双边; 
	double	long_margin_ratio;				/**< 多头保证金率 */
	double	short_margin_ratio;				/**< 空头保证金率 */


	InstrumentInfoData() : BaseInstrumentInfo(), vol_multi(0), is_trading(true), long_margin_ratio(0.0), short_margin_ratio(0.0) {}
	void FromStr(const char* str)
	{
		symbol.product = PRODUCT_FUTURE;
		InstrumentIdType instrument = {0};
		InstrumentNameType name = {0};
		sscanf(str, "%c,%[^,],%[^,],%d,%lf,%d,%lf,%lf", &symbol.exchange, instrument, 
			name, &vol_multi, &price_tick, &is_trading, &long_margin_ratio, &short_margin_ratio);
		strcpy(symbol.instrument, instrument);
		strcpy(symbol.name, name);
	}
	std::string ToStr() const
	{
		char tmp[64] = {0};
		sprintf(tmp, "%c,%s,%s,%d,%g,%d,%g,%g", symbol.exchange, symbol.instrument, 
			symbol.name, vol_multi, price_tick, is_trading, long_margin_ratio, short_margin_ratio);
		return tmp;
	}
};

// 保证金
struct MarginInfo {
	InstrumentIdType instrument;
	///多头保证金率
	double	LongMarginRatioByMoney;
	///多头保证金费
	double	LongMarginRatioByVolume;
	///空头保证金率
	double	ShortMarginRatioByMoney;
	///空头保证金费
	double	ShortMarginRatioByVolume;

	MarginInfo() { memset(this, 0, sizeof(MarginInfo)); }
	std::string FromStr(const char* str)
	{
		InstrumentIdType inst = {0};
		String16 date = {0};
		sscanf(str, "%[^,],%[^,],%lf,%lf,%lf,%lf", date, inst, &LongMarginRatioByMoney, &LongMarginRatioByVolume, 
			&ShortMarginRatioByMoney, &ShortMarginRatioByVolume);
		strcpy(instrument, inst);
		return date;
	}
	std::string ToStr() const
	{
		char tmp[64] = {0};
		sprintf(tmp, "%s,%g,%g,%g,%g", instrument, LongMarginRatioByMoney, LongMarginRatioByVolume,
			ShortMarginRatioByMoney, ShortMarginRatioByVolume);
		return tmp;
	}
};

struct OptionInfo
{
	SymbolEx symbol;					
	InstrumentIdType underling;
	char combt;
	int		vol_multi;						/**< 合约数量乘数 */
	double	price_tick;						/**< 最小变动价位 */
	int open_date;
	int expire_date;
	double strike_price;

	void FromStr(const char* str)
	{
		symbol.product = PRODUCT_OPTION;
		sscanf(str, "%c,%[^,],%[^,],%d,%lf,%lf,%d,%d%[^,]", 
			&symbol.exchange, symbol.instrument, symbol.name,
			&vol_multi, &price_tick, &strike_price, &open_date, &expire_date, underling);
	}
	std::string ToStr() const
	{
		char tmp[256] = {0};
		sprintf(tmp, "%c,%s,%s,%d,%g,%g,%d,%d,%s", symbol.exchange, symbol.instrument, 
			symbol.name, vol_multi, price_tick, strike_price, open_date, expire_date, underling);
		return tmp;
	}
	bool operator<(const OptionInfo& other) const
	{
		return symbol < other.symbol;
	}
};

// 手续费
struct CommisionInfo {
	InstrumentIdType instrument;
	///开仓手续费率
	double	OpenRatioByMoney;
	///开仓手续费
	double	OpenRatioByVolume;
	///平仓手续费率
	double	CloseRatioByMoney;
	///平仓手续费
	double	CloseRatioByVolume;
	///平今手续费率
	double	CloseTodayRatioByMoney;
	///平今手续费
	double	CloseTodayRatioByVolume;

	CommisionInfo() { memset(this, 0, sizeof(CommisionInfo)); }
	std::string FromStr(const char* str)
	{
		String16 date = {0};
		InstrumentIdType inst = {0};		
		sscanf(str, "%[^,],%[^,],%lf,%lf,%lf,%lf,%lf,%lf", date, inst, &OpenRatioByMoney, &OpenRatioByVolume, 
			&CloseRatioByMoney, &CloseRatioByVolume, &CloseTodayRatioByMoney, &CloseTodayRatioByVolume);
		strcpy(instrument, inst);
		return date;
	}
	std::string ToStr() const
	{
		char tmp[64] = {0};
		sprintf(tmp, "%s,%g,%g,%g,%g,%g,%g", instrument, OpenRatioByMoney, OpenRatioByVolume,
			CloseRatioByMoney, CloseRatioByVolume, CloseTodayRatioByMoney, CloseTodayRatioByVolume);
		return tmp;
	}
};
#pragma pack()

/*
 * @brief PositionData 持仓结构体
 * @note 持仓的最新价、持仓盈亏、持仓市值并非实时更新，而是用户取时再计算
 */
struct PositionData {
	UserStrategyIdType user_tag;			/**< 用户的自定义标志 */
	Symbol symbol;							/**< 行情代码 */
	PriceType last_price;					/**< 最新价 */
	PriceType open_price;					/**< 期货：开仓均价(不含手续费)；股票： 成本价*/
	PriceType position_profit;				/**< 持仓盈亏(浮动盈亏)  */
	VolumeType open_volume;					/**< 总持仓量=最终的净持仓总量（单位：手(期货)、股(股票)） */
	PriceType position_cost;				/**< 持仓市值（价值） 期货：合约市值（有杠杆） 股票：持有基金、股票市值  */
	// 期货部分
	OrderDirection direction;				/**< 买卖方向 */	
	VolumeType today_volume;				/**< 今仓 */
	VolumeType yestd_volume;				/**< 昨仓 */
	VolumeType enable_today_volume;			/**< 可用今仓 */
	VolumeType enable_yestd_volume;			/**< 可用昨仓 */
	PriceType using_margin;					/**< 占用保证金 */
	// 以下为股票部分
	VolumeType current_volume;				/**< 当前数量 */
	VolumeType enable_volume;				/**< 可用数量 */	

	PositionData() { memset(this, 0, sizeof(PositionData)); }
};

/*
 * @brief TradeData 成交回报结构体
 */
struct TradeData {
	Symbol symbol;							/**< 行情代码 */
	NumberIdType order_id;					/**< 订单号 */
	NumberIdType local_order_id;			/**< 本地订单号 */
	NumberIdType trade_id;					/**< 成交号 */
	UserStrategyIdType user_tag;			/**< 用户的自定义标志 */
	DateTime trade_time;			/**< 成交时间 */
	OrderDirection direction;				/**< 买卖方向 */
	OpenCloseFlag open_close_flag;			/**< 开平标志 */
	PriceType trade_price;					/**< 成交价格 */
	VolumeType trade_volume;				/**< 成交数量 */
	PriceType trade_commision;				/**< 手续费(股票自己算) */

	TradeData() { memset(this, 0, sizeof(TradeData)); }
};

/*
 * @brief AccountData 资金账户回报结构体
 */
struct	AccountData 
{
	String16 account_id;				/**< 资金账号 */
	String16 broker_id;					/**< 经纪公司账号 */
	MoneyType money_type;				/**< 币值类型 */
	PriceType asset_balance;			/**< 总资产,对应期货的动态权益 */
	PriceType enable_balance;			/**< 可用资金 */
	PriceType withdraw_balance;			/**< 可取资金 */
	PriceType frozen_balance;			/**< 冻结资金（金证：资金余额-可用资金） */
	PriceType position_profit;			/**< 浮动盈亏（根据持仓自己算） */
	PriceType close_profit;				/**< 平仓盈亏（股票自己算）*/
	PriceType commision;				/**< 手续费（股票自己算） */

	//以下为股票字段
	PriceType market_value;				/**< 股票市值 */
	PriceType fund_balance;				/**< 资金可用余额 */

	//以下为期货字段
	PriceType static_balance;			/**< 静态权益 */	
	PriceType margin_balance;			/**< 占用保证金 */
	PriceType delivery_margin;			/**< 交割保证金 */

	AccountData() { memset(this, 0, sizeof(AccountData)); money_type = MONEY_RMB; }
};

/*
 * @brief MoneyFlowData 资金流水结构体
 */
struct	MoneyFlowData {
	typedef char StringType[16];

	StringType account_id;			/**< 资金账号 */
	StringType broker_id;			/**< 经纪公司账号 */
	char flow_id[20];				/**< 流水号 */
	MoneyType money_type;			/**< 币值类型， 1-人民 2-美元 3-港币 */
	char business_abstract[32];		/**< 业务摘要 */
	char abstract_type[12];			/**< 摘要类别 */
	double change_value;			/**< 发生金额 */
	double reserve_value;			/**< 余额 */
	Symbol symbol;					/**< 行情代码 */
	OrderDirection direction;		/**< 买卖方向 */
	double trade_price;				/**< 成交价格 */
	int trade_volume;				/**< 成交数量 */
	double trade_value;				/**< 成交金额 */
	DateTime trade_time;	/**< 成交时间 */
	double commission;				/**< 佣金 */
	double transfer_fee;			/**< 过户费 */
	double stamp_tax;				/**< 印花税 */
	double other_fee;				/**< 其他费用 */
};

#pragma pack()


#endif	//EYEGLE_STRUCT_H_
