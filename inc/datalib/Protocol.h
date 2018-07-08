#ifndef ITSTATION_DATASERVER_PROCTROL_H_
#define ITSTATION_DATASERVER_PROCTROL_H_

#include "datalib/DataServerStruct.h"

//namespace itstation {

#define REQ_HIS_DATA '0'
#define REQ_HIS_DATA2 '1' //采用最新的K线压缩算法.
#define REQ_RUN_DATA '2' // request tick and kline
#define REQ_LOGIN '3'
#define REQ_RUN_KLINE '4'

#define RSP_LOGIN 'a'
#define RSP_HIS_DATA 'b'
#define RSP_HIS_DATA2 'c' //采用最新的K线压缩算法.
#define RSP_STRATEGY_DEBUG 'd'
#define STRATEGY_PRINT_MSG 'e'
#define STRATEGY_EXIT_MSG 'f'
// RSP DATA
#define RSP_RUN_TICK 'g'
#define RSP_RUN_KLINE 'h'
#define RSP_HIS_TICK 'i'
#define RSP_HIS_KLINE 'j'

#define FUTURE_INFO_PACKAGE 'A'
#define STOCK_INFO_PACKAGE 'B'
#define INDEX_INFO_PACKAGE 'C'
#define FUND_INFO_PACKAGE 'D'
#define SPOT_INFO_PACKAGE 'E'
#define OPTION_INFO_PACKAGE 'F'
typedef char ClientRequestType;

#define COUNT_MOD_ALL '0'	//所有K线,只支持日线及以上级别.
#define COUNT_MOD_IN_NUMBER '1'	//按K线数量计数。一般用于实盘，直接向服务端请求K线，与缓存模式无关，与tick无关.
#define COUNT_MOD_TIME_RANGE '2'	//按时间范围计数.
#define COUNT_MOD_NONE '3'		//只订阅TICK，不请求K线.
typedef char KlineCountMode;	//K线计数方式.

#define HIS_STOCK_TICK		'1'
#define HIS_FUTURE_TICK		'2'
#define HIS_INDEX_TICK		'3'
#define HIS_FUND_TICK		'4'
#define HIS_SPOT_TICK		'5'
#define HIS_KLINE_EXT_1		'6'
#define HIS_KLINE_EXT_2		'7'
#define HIS_OPTION_TICK		'9' //后面加的.
typedef char HisDataType;

#define SYMBOL_FOR_ALL Symbol(PRODUCT_STOCK, EXCHANGE_SSE, "999999")
#define NAN_SYMBOL Symbol(PRODUCT_OTHER, EXCHANGE_OTHER, "")

#pragma pack(1)
struct ProtocolHead 
{
	ClientRequestType type;
	ProtocolHead (ClientRequestType t = '0') : type(t) {}
};

struct HisDataParam {
	Symbol symbol;
	DimensionType dim;
	short dim_count;
	KlineCountMode count_mode;
	//表示K线根数或起止日期(例：2014101620150305).
	//请求历史数据时，明确写结束日期为今日，服务端不发送今日未结束的K线.
	//请求实时数据时，把结束日期填为0，则收到当前交易日未结束的K线.
	long long mode_data;	

	HisDataParam() : symbol(Symbol()), dim(DIMENSION_MINUTE)
		, dim_count(1), count_mode(COUNT_MOD_TIME_RANGE), mode_data(0)
	{

	}

	bool operator==(const HisDataParam& other) const {
		return symbol == other.symbol && dim == other.dim && dim_count == other.dim_count
			&& count_mode == other.count_mode && mode_data == other.mode_data;
	}
	bool operator!=(const HisDataParam& other) const {
		return !(*this == other);
	}
	bool operator<(const HisDataParam& other) const {
		return symbol < other.symbol || (symbol == other.symbol && dim < other.dim) ||
			(symbol == other.symbol && dim == other.dim && dim_count < other.dim_count) || 
			(symbol == other.symbol && dim == other.dim && dim_count == other.dim_count && count_mode < other.count_mode) || 
			(symbol == other.symbol && dim == other.dim && dim_count == other.dim_count && count_mode == other.count_mode && mode_data < other.mode_data);
	}
};


struct RunDataReq : public ProtocolHead {
	bool is_sub;	//true:订阅  false:取消订阅.
	int num;
	Symbol symbols[0];
	RunDataReq() : ProtocolHead(REQ_RUN_DATA), num(0), is_sub(true) {}
};

struct HisDataReq : public ProtocolHead {
	int num;
	Symbol symbols[0];
	HisDataReq() : ProtocolHead(REQ_HIS_DATA), num(0) {}
};

struct RunTickRsp : public ProtocolHead {
	FutureTick tick;
	RunTickRsp() : ProtocolHead(RSP_RUN_TICK){}
};
struct RunKlineRsp : public ProtocolHead {
	FutureKline kline;
	RunKlineRsp() : ProtocolHead(RSP_RUN_KLINE){}
};
struct HisTickRsp : public ProtocolHead {
	int num;
	FutureTick ticks[0];
	HisTickRsp() : ProtocolHead(RSP_HIS_TICK), num(0) {}
};

struct HisKlineRsp : public ProtocolHead {
	int num;
	FutureKline klines[0];
	HisKlineRsp() : ProtocolHead(RSP_HIS_KLINE), num(0) {}
};






struct RunKlineRequt : public ProtocolHead {
	int num;
	bool is_sub;	//true:订阅  false:取消订阅.
	KlineInfo kline_infos[0];
	RunKlineRequt() : ProtocolHead(REQ_RUN_KLINE), num(0), is_sub(true) {}
};


/*
struct HisDataRequt : public ProtocolHead, public HisDataParam {
	int req_id;		//客户端请求id.
	bool cache_mode; //是否使用缓存模式.
	HisDataRequt() : ProtocolHead(REQ_HIS_DATA), HisDataParam(), cache_mode(false) {}
};*/

struct HisDataResponse : public ProtocolHead {
	int req_id;		//客户端请求id.
	bool cache_mode;  //是否使用缓存模式.
	int actual_end_date;//实际结束日期.
	bool is_last;
	HisDataType data_type;
	int trading_day; //tick分包发送时，表示数据的交易日.
	int data_len; //长度为0表示请求的交易日没有数据.
	char data[0];
	HisDataResponse() : ProtocolHead(RSP_HIS_DATA) {}
};

struct LoginRequest : public ProtocolHead {
	char user_id[32];
	PwdType pwd;
	LoginRequest() : ProtocolHead(REQ_LOGIN) {}
};

struct LoginResponse : public ProtocolHead {
	bool succeed;
	char msg[32];
	LoginResponse() : ProtocolHead(RSP_LOGIN), succeed(true) { memset(msg, 0, 32); }
};

struct FutureInfoPackage : public ProtocolHead {
	char data[0];
	FutureInfoPackage() : ProtocolHead(FUTURE_INFO_PACKAGE) {}
};

struct StockInfoPackage : public ProtocolHead {
	char data[0];
	StockInfoPackage() : ProtocolHead(STOCK_INFO_PACKAGE) {}
};

struct IndexInfoPackage : public ProtocolHead {
	char data[0];
	IndexInfoPackage() : ProtocolHead(INDEX_INFO_PACKAGE) {}
};

struct FundInfoPackage : public ProtocolHead {
	char data[0];
	FundInfoPackage() : ProtocolHead(FUND_INFO_PACKAGE) {}
};

struct SpotInfoPackage : public ProtocolHead {
	char data[0];
	SpotInfoPackage() : ProtocolHead(SPOT_INFO_PACKAGE) {}
};

struct OptionInfoPackage : public ProtocolHead {
	char data[0];
	OptionInfoPackage() : ProtocolHead(OPTION_INFO_PACKAGE) {}
};



struct StrategyDebugResponse : public ProtocolHead {
	int unit_id; //策略id，启动成功后返回.
	bool secced; //是否成功.
	char error_msg[128]; //错误信息.
	StrategyDebugResponse() : ProtocolHead(RSP_STRATEGY_DEBUG) {}
};

struct StrategyExitMsg : public ProtocolHead {
	int unit_id; //策略id.
	StrategyExitMsg() : ProtocolHead(STRATEGY_EXIT_MSG) {}
};

struct StrategyPrintMsg : public ProtocolHead {
	int level;
	char msg[128];
	StrategyPrintMsg() : ProtocolHead(STRATEGY_PRINT_MSG) {}
};

#pragma pack()


#endif