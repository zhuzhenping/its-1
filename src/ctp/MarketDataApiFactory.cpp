#include "ctp/MarketDataApiFactory.h"

#include "ctp/CtpFutureMarketDataApi.h"
//#ifdef WIN32
//#include "ctp/GtaStockMarketDataApi.h"
//#endif

////namespace itstation {
////namespace itstation {

const std::string MarketDataApiFactory::kCtpFutureDataType = "CTP_FUTURE";
const std::string MarketDataApiFactory::kCtpStockDataType = "CTP_STOCK";
const std::string MarketDataApiFactory::kCtpOptionDataType = "CTP_OPTION";
const std::string MarketDataApiFactory::kGtaStockDataType = "GTA_STOCK";

MarketDataApiFactory::MarketDataApiFactory(void)
{
}

MarketDataApiFactory::~MarketDataApiFactory(void) {
}

MarketDataApi* MarketDataApiFactory::CreateMarketDataApi(std::string type) {
	if (kCtpFutureDataType == type) {
		MarketDataApi* api = new CtpFutureMarketDataApi();
		return api;
	}
//#ifdef WIN32
#if 0
	else if (kGtaStockDataType == type) {
		MarketDataApi* api = new GtaStockMarketDataApi;
		return api;
	}
#endif
	else {
		return NULL;
	}
}

