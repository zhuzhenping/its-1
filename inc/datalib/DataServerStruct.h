#ifndef DATA_SERVER_STRUCT_H_
#define DATA_SERVER_STRUCT_H_

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <sstream>
#include "datalib/MarketDefine.h"
#include "common/SimpleDateTime.h"
#include "datalib/Array.h"

//namespace itstation {

#define FLOAT_NAN -999999
#define is_float_nan(num) ((num) < FLOAT_NAN + 0.000001) && ((num) > FLOAT_NAN - 0.000001)


#pragma pack(1)
//基本的TICK数据. (86B = 25 + 9 + 4 + 8 * 6)
struct BaseTick/* : CommDataHead.*/ 
{
	Symbol symbol;						
	SimpleDateTime date_time;			
	PriceType last_price;
	VolumeType volume;	//当天的总成交量.
	PriceType amount;		//当天的总成交额.
	//为了行情体系的简单，添加以下字段.
	PriceType pre_close;
	PriceType today_open;
	PriceType today_high;
	PriceType today_low;
};
//股票TICK数据. (152B = 86 + 8 * 2 + 10 + 4 * 10)
struct StockTick : public BaseTick 
{
	PriceType up_limit;
	PriceType drop_limit;
	PriceType ask_price[PRICE_LEVEL_NUM];
	PriceType bid_price[PRICE_LEVEL_NUM];
	VolumeType ask_volume[PRICE_LEVEL_NUM];
	VolumeType bid_volume[PRICE_LEVEL_NUM];
};

//期货TICK数据. (146B = 86 + 8 * 6 + 4 * 3)
struct OldFutureTick : public BaseTick 
{
	PriceType up_limit;			//涨停价.
	PriceType drop_limit;		//跌停价.
	double open_interest;			//持仓量.
	PriceType pre_settlement;	//昨结算价.
	VolumeType pre_open_interest; //昨持仓量.
	VolumeType bid_volume;	
	VolumeType ask_volume;
	PriceType bid_price;	
	PriceType ask_price;
};

struct FutureTick : public BaseTick 
{
	PriceType up_limit;			//涨停价.
	PriceType drop_limit;		//跌停价.
	VolumeType open_interest;	//持仓量.
	PriceType pre_settlement;	//昨结算价.
	VolumeType pre_open_interest; //昨持仓量.
	PriceType ask_price;
	PriceType bid_price;	
	VolumeType ask_volume;
	VolumeType bid_volume;	
	std::string Str() const {
		char tmp[256] = {0};
		sprintf(tmp, "%s,%s,%g,%d", symbol.Str().c_str(), date_time.Str().c_str(), last_price, volume);
		return tmp;
	}
};

//期权Tick数据.
//typedef FutureTick OptionTick;
struct OptionTick : public BaseTick 
{
	PriceType up_limit;			//涨停价.
	PriceType drop_limit;		//跌停价.
	VolumeType open_interest;	//持仓量.
	PriceType pre_settlement;	//昨结算价.
	VolumeType pre_open_interest; //昨持仓量.
	PriceType ask_price;
	PriceType bid_price;	
	VolumeType ask_volume;
	VolumeType bid_volume;	
	PriceType underling_price;//标的价格.
};

//指数TICK数据. (72B)
struct IndexTick : public BaseTick 
{
	int advance_num;	//上涨家数.
	int decline_num;	//下跌家数.
};

struct SpotTick : public BaseTick 
{
	PriceType up_limit;			//涨停价.
	PriceType drop_limit;		//跌停价.
	VolumeType open_interest;	//持仓量.
	PriceType pre_settlement;	//昨结算价.
	VolumeType pre_open_interest; //昨持仓量.
	PriceType ask_price[PRICE_LEVEL_NUM];
	PriceType bid_price[PRICE_LEVEL_NUM];
	VolumeType ask_volume[PRICE_LEVEL_NUM];
	VolumeType bid_volume[PRICE_LEVEL_NUM];
};

//K线信息.
struct KlineInfo 
{
	Symbol symbol;
	DimensionType dimension;
	unsigned short dimen_cnt;

	KlineInfo() : dimension(DIMENSION_MINUTE), dimen_cnt(1) {}
};

//K线数据.
struct Kline : public KlineInfo
{
	SimpleDateTime b_time;
	SimpleDateTime e_time;
	PriceType open;
	PriceType high;
	PriceType low;
	PriceType close;
	//PriceType volume; 
	VolumeType volume; // last tick volume - first tick volume
	VolumeType sum_volume; // last tick volue
	
	Kline() : open(), high(), low(), close(), volume(), b_time(), e_time() {}
	void clear() { open = 0; high = 0; low = 0; close = 0; volume = 0; }
};

struct KlineExt1 : public Kline
{
	double amount;    //成交额.
	PriceType pre_close; //昨收.
	PriceType up_limit;  //涨停价.
	PriceType drop_limit;//跌停价.

	KlineExt1() : Kline(), amount(), pre_close(), up_limit(), drop_limit() {}
	void clear() { Kline::clear(); amount = 0; pre_close = 0; up_limit = 0; drop_limit = 0; }
};

//期货、期权K线数据.
struct FutureKline : public KlineExt1
{
	VolumeType open_interest;
	PriceType pre_settle_price;
	VolumeType pre_open_interest;

	FutureKline() : KlineExt1(), open_interest(), pre_settle_price(), pre_open_interest() {}
	void clear() { KlineExt1::clear(); open_interest = 0; pre_settle_price = 0; pre_open_interest = 0; }
	void update(FutureTick *tick, bool first/*first tick in 1 min*/) {
		if (first) {
			symbol = tick->symbol;
			SimpleDateTime dt(time(NULL));
			dt.time.sec = dt.time.mil_sec = 0;
			b_time = dt;
			dt.time.AddMin(1);
			e_time = dt;
			open = tick->last_price;
			high = tick->last_price;
			low = tick->last_price;
			close = tick->last_price;
			volume = 0;
			sum_volume = tick->volume;
			amount = tick->amount;
			pre_close = tick->pre_close;
			up_limit = tick->up_limit;
			drop_limit = tick->drop_limit;
			open_interest = tick->open_interest;
			pre_settle_price = tick->pre_settlement;
			pre_open_interest = tick->pre_open_interest;
		} else {
			if (tick->last_price > high) high = tick->last_price;
			if (tick->last_price < low) low = tick->last_price;
			close = tick->last_price;
			volume += (tick->volume - sum_volume);
			sum_volume = tick->volume;
			amount = tick->amount;
			open_interest = tick->open_interest;
		}
	}
	std::string Str() const {
		char tmp[256] = {0};
		sprintf(tmp, "%s,%s %s,%g,%g,%g,%g,%d", symbol.Str().c_str(), b_time.Str().c_str(), e_time.Str().c_str(), 
			open, high, low, close, volume);
		return tmp;
	}
};


typedef Array<FutureTick> TickSeries;
typedef Array<FutureKline> KlineSeries;

//tick、kline union for strategy
struct Bars {
	FutureTick tick;
	KlineSeries klines;
};

//股票财务数据.
struct StockFinance
{
	Date report_date;
	double total_stock;	//总股本.
	double country_stock;	//国家股.
	double promoter_stock;	//发起人股
	double legal_stock;	//法人股
	double staff_stock;	//职工股.
	double h_stock;		//H股
	double b_stock;		//B股
	double a2_stock;	//A2转配股
	double tradable_stock;	//流通股
	double total_asset;	//总资产
	double tradable_asset;	//流动资产
	double fixed_asset;		//固定资产
	double virtual_asset;	//无形资产
	double long_invest;		//长期投资
	double float_debt;		//流动负债
	double long_debt;		//长期负债
	double captial_fund;	//资本公积金
	double stock_fund;		//每股公积金
	double holder_equity;	//股东权益(净资产)
	double main_income;		//主营收入
	double main_profit;		//主营利润
	double other_profit;	//其它利润
	double business_profit;	//营业利润
	double invest_income;	//投资收益
	double subsidy_income;	//补贴收入
	double out_business_income;	//营业外收益
	double pre_adjust_income;	//上年收益调整
	double total_profit;	//利润总额
	double after_tax_profit;	//税后利润
	double net_profit;		//净利润
	double non_assign_profit;	//未分配利润
	double stock_non_assign;	//每股未分配
	double stock_profit;		//每股收益
	double stock_net_asset;		//每股净资产
	double adjust_stock_net_asset;	//调整的每股净资产
	double equity_ratio;		//股东权益比
	double asset_profit_ratio;	//净资产收益率
};

#pragma pack()



#endif