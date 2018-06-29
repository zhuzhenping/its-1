#ifndef EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_
#define EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_

#include "common/Global.h"

namespace itstation {
namespace marketapi {

class TradeApi;

class MARKET_TRADE_API MarketTradeApiFactory
{
public:
	static const std::string kCtpFutureApi; // CTP
	static const std::string kJZStockApi; // 金证
	static const std::string kHSStockApi; // 恒生
	static const std::string kJSDStockApi; // 金仕达
	static const std::string kBackTestApi; // 回测

	static TradeApi* CreateMarketTradeApi(std::string kApi);
};

}
}

#endif	//EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_

