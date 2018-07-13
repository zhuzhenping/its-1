#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "datalib/MarketDefine.h"
#include "datalib/DataServerStruct.h"
#include "common/Global.h"
#include "common/SpinLock.h"
#include "common/XmlConfig.h"
#include "common/SimpleDateTime.h"



struct TradeSectionTime 
{
	SimpleTime begin;
	SimpleTime end;

	//bool IsLess(const SimpleTime& time)
	//{
	//	if (begin.hour > 20) { return time.hour >= 18 && time < begin; }
	//	else { return time < begin; }
	//}
	//bool IsGreater(const SimpleTime& time)
	//{
	//	if (begin.hour > 20) 
	//	{ 
	//		return time >=end && time.hour < 18;
	//	}
	//	else { return time >= end; }
	//}

	static bool GreaterWithoutMilsec(const SimpleTime& time1, SimpleTime& time2)
	{
		return time1.hour > time2.hour || (time1.hour == time2.hour && time1.minute > time2.minute)
			|| (time1.hour == time2.hour && time1.minute == time2.minute && time1.sec > time2.sec);
	}

	bool IsIn(const SimpleTime& time)
	{
		if (begin < end) { return time >= begin && !GreaterWithoutMilsec(time, end); }
		else { return time >= begin || !GreaterWithoutMilsec(time, end); }
	}

	bool IsStrictIn(const SimpleTime& time)
	{
		if (begin < end) { return time >= begin && time < end; }
		else { return time >= begin || time < end; }
	}

	static SimpleTime StrToTime(const std::string& str)
	{
		int hour, minute;
		sscanf(str.c_str(), "%d:%02d", &hour, &minute);
		return SimpleTime(hour, minute);
	}

	TradeSectionTime(SimpleTime& b, SimpleTime& e) : begin(b), end(e) {}
	TradeSectionTime() {}
};

class DATALIB_API SymbolInfoSet
{
public:
	static SymbolInfoSet* Instance();
	void Denit();

	const std::vector<Symbol>& FutureSymbols() { return future_symbols_; }
	const std::vector<Symbol>& NightFutureSymbols() { return night_future_symbols_; }
	const std::vector<Symbol>& StockASymbols() { return stock_a_symbols_; }
	const std::vector<Symbol>& StockFundSymbols() { return stock_fund_symbols_; }
	const std::vector<Symbol>& StockIndexSymbols() { return stock_index_symbols_; }
	const std::vector<Symbol>& SpotSymbols() { return spot_symbols_; }
	const std::vector<Symbol>& OptionSymbols() { return option_symbols_; }
	std::map<std::string, std::string>& SymbolNames() { return symbol_names_; }

	double GetPriceTick(const Symbol& sym);
	int GetVolMulti(const Symbol& sym);

	std::string GetProductName(const char* inst);
	ExchangeIdType GetFutureExchange(const char* inst);

	void InsertStock(SymbolEx& sym);
	bool ExistStock(const Symbol& sym);

	//主力合约
	bool IsZhuli(const char* inst);
	Symbol GetZhuli(const char* inst);
	void GetZhuli(std::set<Symbol>& syms); //获取所有主力合约(原始合约，如IF1606)
	const std::vector<Symbol>& FutureZhuliSymbols() { return future_zhuli_symbols_; } //主力合约名(IF888)

	//未考虑期权
	static ProductIdType GetProduct(const char* inst, ExchangeIdType exch);

	static void GetTradingTime(const Symbol& sym, std::vector<TradeSectionTime>& times);

	~SymbolInfoSet(void);
private:
	SymbolInfoSet(void);
	bool Init(std::string& err);

	bool IsNightTrading(Symbol& sym, XmlConfig* settings);
	void InseerFutureInfo(Symbol& sym, InstrumentInfo& info, bool zhuli_info = true);

	bool GetInfo(const Symbol& sym, InstrumentInfo& info);
	bool GetInfo(const Symbol& sym, OptionInfo& info);

	static ProductIdType GetShProduct(const char* inst);
	static ProductIdType GetSzProduct(const char* inst);

	static std::string ExchangeName(ExchangeIdType exchange);
	static void InsertSectionTime(XmlNode& node, std::vector<TradeSectionTime>& times);

private:
	static SymbolInfoSet* inst_;

	std::vector<Symbol> future_symbols_;
	std::vector<Symbol> future_zhuli_symbols_;
	std::vector<Symbol> night_future_symbols_;
	std::vector<Symbol> stock_a_symbols_;
	std::vector<Symbol> stock_fund_symbols_;
	std::vector<Symbol> stock_index_symbols_;
	std::vector<Symbol> spot_symbols_;
	std::vector<Symbol> option_symbols_;
	std::map<std::string, std::string> symbol_names_;
	std::map<std::string, std::string> product_names_;
	std::map<std::string, InstrumentInfo> future_info_;
	std::map<std::string, InstrumentInfo> spot_info_;
	std::map<std::string, OptionInfo> option_info_;
	std::map<std::string, Symbol> zhuli_ori_symbols_;

	bool is_init_;
	SpinLock mutex_;
	static XmlConfig *trading_time_cfg_;
};

