#include "account/CTPMarginCommision.h"
#include <fstream>
#include <QtCore/QDate>
#include "common/Directory.h"
#include "datalib/SymbolInfoSet.h"

using namespace std;

//namespace itstation {

#define DateFormat "yyyy-MM-dd"

CTPMarginCommision::CTPMarginCommision() : api_(NULL), margin_fp_(NULL), commision_fp_(NULL) {
	api_ = MarketTradeApiFactory::CreateMarketTradeApi(MarketTradeApiFactory::kCtpFutureApi);
	Init();
}

CTPMarginCommision::~CTPMarginCommision() {
	Denit();
	delete api_; api_ = NULL;
}

void CTPMarginCommision::QryMargin(const std::string& symbol) {
	if (margins_[symbol].need_qry) {
		std::string err;
		api_->QryMargin(symbol, err);
		margins_[symbol].need_qry = false;
	}
}

void CTPMarginCommision::QryCommision(const std::string& symbol) {
	if (commisions_[symbol].need_qry) {
		std::string err;
		api_->QryCommision(symbol, err);
		commisions_[symbol].need_qry = false;
	}
}

PriceType CTPMarginCommision::CalcMargin(const Symbol &symbol, OrderDirection direction, PriceType open_price, VolumeType open_volume) {
	if (direction == LONG_DIRECTION)
	{
		return margins_[symbol.instrument].margin.LongMarginRatioByMoney * info_set_->GetVolMulti(symbol) * open_price * open_volume
			+ margins_[symbol.instrument].margin.LongMarginRatioByVolume * info_set_->GetVolMulti(symbol) * open_volume;
	}
	else
	{
		return margins_[symbol.instrument].margin.ShortMarginRatioByMoney * info_set_->GetVolMulti(symbol) * open_price * open_volume
			+ margins_[symbol.instrument].margin.ShortMarginRatioByVolume * info_set_->GetVolMulti(symbol) * open_volume;
	}
}

PriceType CTPMarginCommision::CalcCommision(const Symbol &symbol, OpenCloseFlag open_close_flag, PriceType open_price, VolumeType open_volume) {
	if (OPEN_ORDER == open_close_flag) {
		return open_price * open_volume * commisions_[symbol.instrument].commision.OpenRatioByMoney
			+ open_volume * commisions_[symbol.instrument].commision.OpenRatioByVolume;
	}
	else if (CLOSE_TODAY_ORDER == open_close_flag) {
		return open_price * open_volume * commisions_[symbol.instrument].commision.CloseTodayRatioByMoney
			+ open_volume * commisions_[symbol.instrument].commision.CloseTodayRatioByVolume;
	}
	else {
		return open_price * open_volume * commisions_[symbol.instrument].commision.CloseRatioByMoney
			+ open_volume * commisions_[symbol.instrument].commision.CloseRatioByVolume;
	}
}

void CTPMarginCommision::Init() {
	api_->SetSecurityInfoSpi(this);

	info_set_ = SymbolInfoSet::GetInstance();
	std::string err;
	if (!info_set_->Init(err)) { assert(false); return; }
	for (std::vector<Symbol>::const_iterator iter = info_set_->FutureSymbols().begin(); iter != info_set_->FutureSymbols().end(); ++iter) {
		margins_[iter->instrument] = LocalMargin();
		commisions_[iter->instrument] = LocalCommision();
	}

	std::string its_home = getenv("ITS_HOME");

	std::string margin_path = its_home + "/data/FutureMargin.txt";	
	if (IsDirExist(margin_path)) {
		ifstream margin_file(margin_path.c_str());
		while (!margin_file.eof()) {
			char buf[128] = {0};
			margin_file.getline(buf, 128);
			LocalMargin local_margin;
			std::string date = local_margin.margin.FromStr(buf);
			if (QDate::currentDate() == QDate::fromString(date.c_str(), DateFormat)) local_margin.need_qry = false;
			margins_[local_margin.margin.instrument] = local_margin;
		}
		margin_file.close();		
	}
	margin_fp_ = fopen(margin_path.c_str(), "w+");

	std::string commison_path = its_home + "/data/FutureCommision.txt";
	if (IsDirExist(commison_path)) {
		ifstream commision_file(commison_path.c_str());
		while (!commision_file.eof()) {
			char buf[128] = {0};
			commision_file.getline(buf, 128);
			LocalCommision local_commision;
			std::string date = local_commision.commision.FromStr(buf);
			if (QDate::currentDate() == QDate::fromString(date.c_str(), DateFormat)) local_commision.need_qry = false;
			commisions_[local_commision.commision.instrument] = local_commision;
		}
		commision_file.close();
	}
	commision_fp_ = fopen(commison_path.c_str(), "w+");
}

void CTPMarginCommision::Denit() {
	QString date = QDate::currentDate().toString(DateFormat);
	date += ",";
	for (std::map<std::string, LocalMargin>::const_iterator iter = margins_.begin(); iter != margins_.end(); ++iter) {
		if (!strcmp(iter->second.margin.instrument, "")) continue;
		fwrite(date.toLocal8Bit().constData(), strlen(date.toLocal8Bit().constData()), 1,margin_fp_);
		fwrite(iter->second.margin.ToStr().c_str(), iter->second.margin.ToStr().size(), 1, margin_fp_);
		fwrite("\n", 1, 1, margin_fp_);
	}

	for (std::map<std::string, LocalCommision>::const_iterator iter = commisions_.begin(); iter != commisions_.end(); ++iter) {
		if (!strcmp(iter->second.commision.instrument, "")) continue;
		fwrite(date.toLocal8Bit().constData(), strlen(date.toLocal8Bit().constData()), 1, commision_fp_);
		fwrite(iter->second.commision.ToStr().c_str(), iter->second.commision.ToStr().size(), 1, commision_fp_);
		fwrite("\n", 1, 1, commision_fp_);
	}

	if (margin_fp_) fclose(margin_fp_);
	if (commision_fp_) fclose(commision_fp_);
}

void CTPMarginCommision::OnMarginInfo(const MarginInfo& info, bool is_last) {	
	margins_[info.instrument].margin = info;
}

void CTPMarginCommision::OnCommisionInfo(const CommisionInfo& info, bool is_last) {	
	commisions_[info.instrument].commision = info;
}

