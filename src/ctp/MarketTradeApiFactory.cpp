#include "marketapi/MarketTradeApiFactory.h"
#include "marketapi/CtpFutureTradeApi.h"

//namespace itstation {
namespace marketapi {

const std::string MarketTradeApiFactory::kCtpFutureApi = "CTP_FUTURE";
const std::string MarketTradeApiFactory::kJZStockApi = "JZ_STOCK";
const std::string MarketTradeApiFactory::kHSStockApi = "HS_STOCK";
const std::string MarketTradeApiFactory::kJSDStockApi = "JSD_STOCK";
const std::string MarketTradeApiFactory::kBackTestApi = "BACK_TEST";

TradeApi* MarketTradeApiFactory::CreateMarketTradeApi(std::string kApi) {
	if (kCtpFutureApi == kApi) {
		return new CtpFutureTradeApi();
	}
	else if (kJZStockApi == kApi) {
		return NULL;
	}
	else if (kHSStockApi == kApi) {
		return NULL;
	}
	else if (kJSDStockApi == kApi) {
		return NULL;
	}
	else if (kBackTestApi == kApi) {
		return NULL;
	}
	else {
		return nullptr;
	}
}

}
}