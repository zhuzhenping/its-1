#include "datalib/SymbolInfoSet.h"
//#include "dataserver/iKlineMerger.h"
#include <iostream>
#include <fstream>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include "common/Directory.h"



SymbolInfoSet* SymbolInfoSet::inst_ = NULL;

SymbolInfoSet::SymbolInfoSet(void) : is_init_(false)
{
}

SymbolInfoSet::~SymbolInfoSet(void)
{
}

bool SymbolInfoSet::IsNightTrading(Symbol& sym, XmlConfig* settings)
{
	char sym_code[6] = {0};
	sscanf(sym.instrument, "%[^0-9]", sym_code);

	std::string exchange_name = ExchangeName(sym.exchange);
	std::string key = std::string("TradingTimes/") + exchange_name + "/Nights";
	XmlNodeVec night_sections = settings->FindChileren(key);
	for (int i=0; i < night_sections.size(); ++i)
	{
		QString insts = night_sections[i].GetValue("InstrumentID").c_str();
		QStringList inst_list = insts.split(',');
		if (inst_list.contains(sym_code))
		{
			return true;
		}
	}

	return false;
}

//bool SymbolInfoSet::IsNightTrading(Symbol& sym, QSettings* settings)
//{
//	char sym_code[6] = {0};
//	sscanf(sym.instrument, "%[^0-9]", sym_code);
//
//	QStringList night_excs = settings->value("TradingTime/NithtTrading").toStringList();
//	std::string exchange_name = iKlineMerger::ExchangeName(sym.exchange);
//	QString exc_str = QObject::tr("%1.NIGHT1").arg(exchange_name.c_str());
//	if (night_excs.contains(exc_str))
//	{
//		QStringList insts = settings->value(exc_str + "/InstrumentID").toStringList();
//		if (insts.contains(sym_code))
//		{
//			return true;
//		}
//		else 
//		{
//			exc_str = QObject::tr("%1.NIGHT2").arg(exchange_name.c_str());
//			if (night_excs.contains(exc_str))
//			{
//				insts = settings->value(exc_str + "/InstrumentID").toStringList();
//				if (insts.contains(sym_code))
//				{
//					return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}

bool SymbolInfoSet::Init(std::string& err)
{
	if (is_init_) { return true; }

	Locker locker(&mutex_);
	if (is_init_) { return true; }

	char* its_home = getenv("ITS_HOME");
	if(NULL == its_home) 
	{ 
		err = "the env_var ITS_HOME is not set";
		return false;
	}
	//DBTableFactory* factory = DBTableFactory::GetInstance();
	//if (!factory->Init(its_home))
	//{
	//	err = "DBTableFactory初始化失败";
	//	return false;
	//}

	std::string trading_conf_path = std::string(its_home) + "/config/TradingTime.xml";
	if (!IsDirExist(trading_conf_path.c_str()))
	{
		err = trading_conf_path + " is not exist";
		return false;
	}
	XmlConfig settings(trading_conf_path);
	if (!settings.Load())
	{
		err = settings.GetLastError();
		return false;
	}

	std::string path = std::string(its_home) + "/data/FutureTable.txt";
	if (!IsDirExist(path))
	{
		err = path + " is not exist";
		return false;
	}
	std::ifstream future_file(path.c_str());
	if (!future_file.is_open())
	{
		err = "open file error";
		return false;
	}
	while (!future_file.eof() ) 
	{
		char buf[128] = {0};
		future_file.getline(buf,128);
		InstrumentInfo info;
		info.FromStr(buf);
		if (strlen(info.symbol.instrument) > 0 && strlen(info.symbol.name) > 0)
		{
			Symbol sym(info.symbol.product, info.symbol.exchange, info.symbol.instrument);
			future_symbols_.push_back(sym);
			if (IsNightTrading(sym, &settings))
			{
				night_future_symbols_.push_back(sym);

				//static std::string pre_procuct = "";
				//char sym_code[6] = {0};
				//sscanf(info.symbol.instrument, "%[^0-9]", sym_code);
				//if (pre_procuct != sym_code)
				//{
				//	cout << iKlineMerger::ExchangeName(info.symbol.exchange) << "  " << sym_code << endl;
				//	pre_procuct = sym_code;
				//}
			}
			char name_key[16] = {0};
			sprintf(name_key, "%c%c_%s", info.symbol.product, info.symbol.exchange, info.symbol.instrument);
			symbol_names_[name_key] = info.symbol.name;
			InseerFutureInfo(sym, info);
		}
	}
	future_file.close();

	path = std::string(its_home) + "/data/SpotTable.txt";
	if (IsDirExist(path))
	{
		std::ifstream spot_file(path.c_str());
		if (!spot_file.is_open())
		{
			err = "open file error";
			return false;
		}
		while (!spot_file.eof() ) 
		{
			char buf[128] = {0};
			spot_file.getline(buf,128);
			InstrumentInfo info;
			info.symbol.product = PRODUCT_SPOT;
			info.FromStr(buf);
			if (strlen(info.symbol.instrument) > 0 && strlen(info.symbol.name) > 0)
			{
				Symbol sym(PRODUCT_SPOT, info.symbol.exchange, info.symbol.instrument);
				spot_symbols_.push_back(sym);
				char name_key[16] = {0};
				sprintf(name_key, "%c%c_%s", PRODUCT_SPOT, info.symbol.exchange, info.symbol.instrument);
				symbol_names_[name_key] = info.symbol.name;
				spot_info_[info.symbol.instrument] = info;
			}
		}
		spot_file.close();
	}

	path = std::string(its_home) + "/data/OptionTable.txt";
	if (IsDirExist(path))
	{
		std::ifstream option_file(path.c_str());
		if (!option_file.is_open())
		{
			err = "open file error";
			return false;
		}
		while (!option_file.eof() ) 
		{
			char buf[128] = {0};
			option_file.getline(buf,128);
			OptionInfo info;
			info.symbol.product = PRODUCT_OPTION;
			info.FromStr(buf);
			if (strlen(info.symbol.instrument) > 0 && strlen(info.symbol.name) > 0)
			{
				Symbol sym(PRODUCT_OPTION, info.symbol.exchange, info.symbol.instrument);
				option_symbols_.push_back(sym);
				char name_key[16] = {0};
				sprintf(name_key, "%c%c_%s", PRODUCT_OPTION, info.symbol.exchange, info.symbol.instrument);
				//symbol_names_[name_key] = info.symbol.name;
				symbol_names_[name_key] = GetProductName(info.symbol.instrument) + "期权";

				char code_str[6] = {0};
				sscanf(info.symbol.instrument, "%[^0-9]", code_str);
				if (option_info_.find(code_str) == option_info_.end())
				{
					option_info_[code_str] = info;
				}
			}
		}
		option_file.close();
	}
	
	//老合约只加入信息查询，不加入合约列表
	path = std::string(its_home) + "/data/OldFuture.txt";
	if (IsDirExist(path))
	{
		std::ifstream future_file2(path.c_str());
		if (future_file2.is_open())
		{
			while (!future_file2.eof() ) 
			{
				char buf[128] = {0};
				future_file2.getline(buf,128);
				InstrumentInfo info;
				info.FromStr(buf);
				if (strlen(info.symbol.instrument) > 0 && strlen(info.symbol.name) > 0)
				{
					Symbol sym(info.symbol.product, info.symbol.exchange, info.symbol.instrument);
					InseerFutureInfo(sym, info, false);
				}
			}
			future_file2.close();
		}
	}
	

	path = std::string(its_home) + "/data/StockTable.txt";
	if (IsDirExist(path))
	{
		std::ifstream stock_file(path.c_str());
		if (!stock_file.is_open())
		{
			err = "open file error";
			return false;
		}
		while (!stock_file.eof() ) 
		{
			char buf[128] = {0};
			stock_file.getline(buf,128);
			SymbolEx sym;
			sym.FromStr(buf);
			sym.product = PRODUCT_STOCK;
			if (strlen(sym.instrument) > 0 && strlen(sym.name) > 0)
			{
				char name_key[16] = {0};
				sprintf(name_key, "%c%c_%s", sym.product, sym.exchange, sym.instrument);
				symbol_names_[name_key] = sym.name;

				Symbol symbol;
				memcpy(&symbol, &sym, sizeof(Symbol));
				stock_a_symbols_.push_back(symbol);
			}
		}
		stock_file.close();
	}

	path = std::string(its_home) + "/data/FundTable.txt";
	if (IsDirExist(path))
	{
		std::ifstream fund_file(path.c_str());
		if (!fund_file.is_open())
		{
			err = "open file error";
			return false;
		}
		while (!fund_file.eof() ) 
		{
			char buf[128] = {0};
			fund_file.getline(buf,128);
			SymbolEx sym;
			sym.FromStr(buf);
			sym.product = PRODUCT_FUND;
			if (strlen(sym.instrument) > 0 && strlen(sym.name) > 0)
			{
				char name_key[16] = {0};
				sprintf(name_key, "%c%c_%s", sym.product, sym.exchange, sym.instrument);
				symbol_names_[name_key] = sym.name;

				Symbol symbol;
				memcpy(&symbol, &sym, sizeof(Symbol));
				stock_fund_symbols_.push_back(symbol);
			}
		}
		fund_file.close();
	}

	path = std::string(its_home) + "/data/IndexTable.txt";
	if (IsDirExist(path))
	{
		std::ifstream index_file(path.c_str());
		if (!index_file.is_open())
		{
			err = "open file error";
			return false;
		}
		while (!index_file.eof() ) 
		{
			char buf[128] = {0};
			index_file.getline(buf,128);
			SymbolEx sym;
			sym.FromStr(buf);
			sym.product = PRODUCT_INDEX;
			if (strlen(sym.instrument) > 0 && strlen(sym.name) > 0)
			{
				char name_key[16] = {0};
				sprintf(name_key, "%c%c_%s", sym.product, sym.exchange, sym.instrument);
				symbol_names_[name_key] = sym.name;

				Symbol symbol;
				memcpy(&symbol, &sym, sizeof(Symbol));
				stock_index_symbols_.push_back(symbol);
			}
		}
		index_file.close();
	}
	
	is_init_ = true;
	return true;
}

void SymbolInfoSet::InseerFutureInfo(Symbol& sym, InstrumentInfo& info, bool zhuli_info)
{
	char code_str[6] = {0};
	sscanf(sym.instrument, "%[^0-9]", code_str);

	if (future_info_.find(code_str) == future_info_.end())
	{
		future_info_[code_str] = info;
	}

	if (zhuli_info && zhuli_ori_symbols_.find(code_str) == zhuli_ori_symbols_.end())
	{
		zhuli_ori_symbols_[code_str] = sym;

		Symbol zhuli_sym = sym;
		sprintf(zhuli_sym.instrument, "%s888", code_str);
		future_zhuli_symbols_.push_back(zhuli_sym);

		char name_key[16] = {0};
		sprintf(name_key, "%c%c_%s", zhuli_sym.product, zhuli_sym.exchange, (zhuli_sym.instrument));
		char chi_name[16] = {0};
		for (int i = strlen(info.symbol.name) - 1; i >= 0; --i)
		{
			if (info.symbol.name[i] < '0' || info.symbol.name[i] > '9') 
			{ 
				memcpy(chi_name, info.symbol.name, i + 1);
				product_names_[code_str] = chi_name;
				std::string zhuli_name = std::string(chi_name) + "主力";
				symbol_names_[name_key] = zhuli_name;
				break;
			}
		}
	}
}

std::string SymbolInfoSet::GetProductName(const char* inst)
{
	char code_str[16] = {0};
	sscanf(inst, "%[^0-9]", code_str);
	std::map<std::string, std::string>::iterator it = product_names_.find(code_str);
	return it != product_names_.end() ? it->second : "";
}

double SymbolInfoSet::GetPriceTick(const Symbol& sym)
{
	switch(sym.product)
	{
	case PRODUCT_INDEX:
	case PRODUCT_STOCK:
		return 0.01;

	case PRODUCT_FUND:
		return 0.001;

	case PRODUCT_BOND:
		return 0.0001;

	case PRODUCT_FUTURE:
		{
			InstrumentInfo info;
			if (GetInfo(sym, info))
			{
				return info.price_tick;
			}
			else
			{
				//assert(false);
				break;
			}
		}

	case PRODUCT_SPOT:
		{
			std::map<std::string, InstrumentInfo>::iterator it = spot_info_.find(sym.instrument);
			if (it != spot_info_.end())
			{
				return it->second.price_tick;
			}
			else
			{
				//assert(false);
				break;
			}
		}

	case PRODUCT_OPTION:
		{
			OptionInfo info;
			if (GetInfo(sym, info))
			{
				return info.price_tick;
			}
			else
			{
				//assert(false);
				break;
			}
		}
		
	default: return 0;
	}
}

int SymbolInfoSet::GetVolMulti(const Symbol& sym)
{
	switch(sym.product)
	{
	case PRODUCT_FUTURE:
		{
			InstrumentInfo info;
			if (GetInfo(sym, info))
			{
				return info.vol_multi;
			}
			else
			{
				return 0;
			}
		}

	case PRODUCT_SPOT:
		{
			std::map<std::string, InstrumentInfo>::iterator it = spot_info_.find(sym.instrument);
			if (it != spot_info_.end())
			{
				return it->second.vol_multi;
			}
			else
			{
				return 0;
			}
		}

	case PRODUCT_OPTION:
		{
			OptionInfo info;
			if (GetInfo(sym, info))
			{
				return info.vol_multi;
			}
			else
			{
				return 0;
			}
		}

	default:
		return 1;
	}
}

bool SymbolInfoSet::GetInfo(const Symbol& sym, InstrumentInfo& info)
{
	Locker locker(&mutex_);
	if (sym.product == PRODUCT_FUTURE) 
	{
		char exc_str[6] = {0};
		sscanf(sym.instrument, "%[^0-9]", exc_str);
		std::map<std::string, InstrumentInfo>::iterator iter = future_info_.find(exc_str);
		if (iter == future_info_.end())
		{
			return false;
		}
		info = iter->second;
		return true;
	}
	else if (sym.product == PRODUCT_SPOT)
	{
		std::map<std::string, InstrumentInfo>::iterator it = spot_info_.find(sym.instrument);
		if (it == spot_info_.end())
		{
			return false;
		}
		info = it->second;
		return true;
	}
	else
	{
		return false;
	}
}

bool SymbolInfoSet::GetInfo(const Symbol& sym, OptionInfo& info)
{
	Locker locker(&mutex_);
	char exc_str[6] = {0};
	sscanf(sym.instrument, "%[^0-9]", exc_str);
	std::map<std::string, OptionInfo>::iterator iter = option_info_.find(exc_str);
	if (iter == option_info_.end())
	{
		return false;
	}
	info = iter->second;
	return true;
}

ExchangeIdType SymbolInfoSet::GetFutureExchange(const char* inst)
{
	Locker locker(&mutex_);
	char exc_str[16] = {0};
	sscanf(inst, "%[^0-9]", exc_str);
	std::map<std::string, InstrumentInfo>::iterator iter = future_info_.find(exc_str);
	if (iter == future_info_.end())
	{
		return EXCHANGE_OTHER;
	}
	return iter->second.symbol.exchange;
}

bool SymbolInfoSet::IsZhuli(const char* inst)
{
	Locker locker(&mutex_);
	char exc_str[16] = {0};
	sscanf(inst, "%[^0-9]", exc_str);
	std::map<std::string, Symbol>::iterator iter = zhuli_ori_symbols_.find(exc_str);
	if (iter == zhuli_ori_symbols_.end())
	{
		return false;
	}

	return inst == iter->second.instrument;
}

Symbol SymbolInfoSet::GetZhuli(const char* inst)
{
	Locker locker(&mutex_);
	char exc_str[16] = {0};
	sscanf(inst, "%[^0-9]", exc_str);
	std::map<std::string, Symbol>::iterator iter = zhuli_ori_symbols_.find(exc_str);
	if (iter == zhuli_ori_symbols_.end())
	{
		return Symbol();
	}

	return iter->second;
}

void SymbolInfoSet::GetZhuli(std::set<Symbol>& syms)
{
	Locker locker(&mutex_);
	for (std::map<std::string, Symbol>::iterator it = zhuli_ori_symbols_.begin(); it != zhuli_ori_symbols_.end(); ++it)
	{
		syms.insert(it->second);
	}
}

ProductIdType SymbolInfoSet::GetShProduct(const char* inst)
{
	char head2[8] = {0};
	char head3[8] = {0};
	memcpy(head2, inst, 2);
	memcpy(head3, inst, 3);

	if (strcmp(head3, "000") == 0) { return PRODUCT_INDEX; }

	//else if (head2 == "01" || head2 == "02" ||  //国债
	//	head2 == "11" || //可转债
	//	head2 == "12" || //公司债、企业债
	//	head2 == "13") //地方债
	//{
	//	return PRODUCT_BOND;
	//}

	else if (strcmp(head2, "50") == 0 ||  //封闭式基金 502为分级
		strcmp(head3, "519") == 0) // 开放式基金
	{
		return PRODUCT_FUND;
	}
	else if (strcmp(head3, "510") >= 0 && strcmp(head3, "518") <= 0)
	{
		// 代码最后一个字符为0：ETF基金交易代码
		// 代码最后一个字符为1：ETF申赎代码
		// 代码最后一个字符为2：ETF申赎资金代码
		// 代码最后一个字符为5：ETF跨市资金代码
		if (inst[5] == '0') { return PRODUCT_FUND; } //ETF
	}
	else if (strcmp(head2, "60") == 0) { return PRODUCT_STOCK; }  

	else { return PRODUCT_OTHER; }
}

ProductIdType SymbolInfoSet::GetSzProduct(const char* inst)
{
	char head2[8] = {0};
	char head3[8] = {0};
	memcpy(head2, inst, 2);
	memcpy(head3, inst, 3);

	if (strcmp(head3, "399") == 0) { return PRODUCT_INDEX; }

	if (strcmp(head2, "00") == 0 || strcmp(head2, "30") == 0) { return PRODUCT_STOCK; }  

	//else if (head2 == "10" || //国债
	//	head2 == "11" || //企业债
	//	head2 == "12") //可转债
	//{
	//	return PRODUCT_BOND;
	//}

	else if (strcmp(head3, "150") == 0 || strcmp(head2, "16") == 0 || strcmp(head3, "184") == 0 ||
		strcmp(head3, "159") == 0) //ETF
	{
		return PRODUCT_FUND;
	}

	else { return PRODUCT_OTHER; }
}

ProductIdType SymbolInfoSet::GetProduct(const char* inst, ExchangeIdType exch)
{
	switch (exch)
	{
	case EXCHANGE_SSE: return GetShProduct(inst);
	case EXCHANGE_SZE: return GetSzProduct(inst);
	default: return PRODUCT_OTHER;
	}
}



void SymbolInfoSet::Deinit()
{
	Locker locker(&mutex_);
	future_symbols_.clear();
	future_zhuli_symbols_.clear();
	night_future_symbols_.clear();
	stock_a_symbols_.clear();
	stock_fund_symbols_.clear();
	stock_index_symbols_.clear();
	spot_symbols_.clear();
	option_symbols_.clear();
	symbol_names_.clear();
	product_names_.clear();
	future_info_.clear();
	spot_info_.clear();
	option_info_.clear();
	zhuli_ori_symbols_.clear();

	is_init_ = false;
}

SymbolInfoSet* SymbolInfoSet::GetInstance()
{
	if (NULL == inst_) {
		inst_ = new SymbolInfoSet();
	}

	return inst_;
}

void SymbolInfoSet::InsertStock(SymbolEx& sym)
{
	if (!is_init_) { return; }

	Locker locker(&mutex_);
	char name_key[16] = {0};
	sprintf(name_key, "%c%c_%s", sym.product, sym.exchange, sym.instrument);
	if (symbol_names_.find(name_key) != symbol_names_.end())
	{
		return;
	}

	std::string path = getenv("ITS_HOME");
	Symbol symbol;
	memcpy(&symbol, &sym, sizeof(Symbol));
	if (symbol.product == PRODUCT_STOCK)
	{
		path += "/data/StockTable.txt";
		stock_a_symbols_.push_back(symbol);
	} 
	else if (symbol.product == PRODUCT_FUND)
	{
		path += "/data/FundTable.txt";
		stock_fund_symbols_.push_back(symbol);
	} 
	else if (symbol.product == PRODUCT_INDEX)
	{
		path += "/data/IndexTable.txt";
		stock_index_symbols_.push_back(symbol);
	} 
	else
	{
		return;
	}

	symbol_names_[name_key] = sym.name;

	if (!IsDirExist(path))
	{
		return;
	}
	std::ofstream file(path.c_str(), std::ios::app);
	if (!file.is_open())
	{
		return;
	}
	std::string str = sym.ToStr();
	file << sym.ToStr() << std::endl;
	file.close();
}

bool SymbolInfoSet::ExistStock(const Symbol& sym)
{
	Locker locker(&mutex_);
	char name_key[16] = {0};
	sprintf(name_key, "%c%c_%s", sym.product, sym.exchange, sym.instrument);
	return symbol_names_.find(name_key) != symbol_names_.end();
}

std::string SymbolInfoSet::ExchangeName(ExchangeIdType exchange)
{
	switch(exchange)
	{
	case EXCHANGE_SSE:
		return "SSE";
	case EXCHANGE_SZE:
		return "SZE";
	case EXCHANGE_CFFEX:
		return "CFFEX";
	case EXCHANGE_DCE:
		return "DCE";
	case EXCHANGE_CZCE:
		return "CZCE";
	case EXCHANGE_SHFE:
		return "SHFE";
	case EXCHANGE_SGE:
		return "SGE";
	case EXCHANGE_HK:
		return "HK";
	default:
		return "OTHER";
	}
}

void SymbolInfoSet::InsertSectionTime(XmlNode& node, std::vector<TradeSectionTime>& times)
{
	TradeSectionTime trade_time;
	std::string begin_str = node.GetValue("Begin");
	std::string end_str = node.GetValue("End");
	assert(begin_str != "" && end_str != "");
	trade_time.begin = TradeSectionTime::StrToTime(begin_str);
	trade_time.end = TradeSectionTime::StrToTime(end_str);
	times.push_back(trade_time);
}

void SymbolInfoSet::GetTradingTime(const Symbol& sym, XmlConfig* conf, std::vector<TradeSectionTime>& times)
{
	char product[16] = {0};
	sscanf(sym.instrument, "%[^0-9]", product);

	std::string key = std::string("TradingTimes/") + ExchangeName(sym.exchange);
	std::string night_key = key + "/Nights";
	XmlNodeVec node_vec = conf->FindChileren(night_key);
	for (int i=0; i < node_vec.size(); ++i)
	{
		QString insts_str = node_vec[i].GetValue("InstrumentID").c_str();
		//QStringList insts_list = insts_str.split(',');
		//if (insts_list.contains(product))
		if (insts_str == "" || insts_str.split(',').contains(product))
		{
			XmlNodeVec night_sections = node_vec[i].FindChileren();
			for (int j=0; j < night_sections.size(); ++j)
			{
				InsertSectionTime(night_sections[j], times);
			}
			break;
		}
	}

	XmlNodeVec day_nodes = conf->FindChileren(key, "Day");
	for (int i=0; i < day_nodes.size(); ++i)
	{
		QString insts_str = day_nodes[i].GetValue("InstrumentID").c_str();
		if (insts_str == "" || insts_str.split(',').contains(product))
		{
			XmlNodeVec day_sections = day_nodes[i].FindChileren();
			for (int j=0; j < day_sections.size(); ++j)
			{
				InsertSectionTime(day_sections[j], times);
			}
			break;
		}
	}

	//std::string day_key = key + "/Day";
	//XmlNodeVec day_sections = conf->FindChileren(day_key);
	//assert(day_sections.size() > 0);
	//for (int j=0; j < day_sections.size(); ++j)
	//{
	//	InsertSectionTime(day_sections[j], times);
	//}
}

