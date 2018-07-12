#ifndef CTP_MARGIN_COMMISION_H_
#define CTP_MARGIN_COMMISION_H_

#include <map>
#include <string>
#include "ctp/MarketTradeApiFactory.h"
#include "ctp/MarketTradeApi.h"


class SymbolInfoSet;

class CTPMarginCommision : public SecurityInfoSpi
{
protected:
	CTPMarginCommision();
	virtual ~CTPMarginCommision();


	void QryMargin(const std::string& symbol);
	void QryCommision(const std::string& symbol);

	
	PriceType CalcMargin(const Symbol &, OrderDirection, PriceType open_price, VolumeType open_volume);
	PriceType CalcCommision(const Symbol &, OpenCloseFlag, PriceType open_price, VolumeType open_volume);

	/*void Init();
	void Denit();*/

private:
	virtual void OnInstrumentInfo(const BaseInstrumentInfo& info, bool is_last) {}
	virtual void OnMarginInfo(const MarginInfo& info, bool is_last);
	virtual void OnCommisionInfo(const CommisionInfo& info, bool is_last);

protected:
	TradeApi* api_;
	SymbolInfoSet* info_set_;

	struct LocalMargin {
		bool need_qry;
		MarginInfo margin;
		LocalMargin() : need_qry(true) {}
	};
	std::map<std::string, LocalMargin> margins_;

	struct LocalCommision {
		bool need_qry;
		CommisionInfo commision;
		LocalCommision() : need_qry(true) {}
	};
	std::map<std::string, LocalCommision> commisions_;

};


#endif