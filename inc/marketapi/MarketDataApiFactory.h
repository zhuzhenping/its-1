#ifndef EYEGLE_MARKETAPI_MARKETDATAAPI_FACTORY_H_
#define EYEGLE_MARKETAPI_MARKETDATAAPI_FACTORY_H_

#include <string>
#include "common/Global.h"

namespace itstation {
namespace marketapi {

	class MarketDataApi;

class MARKET_DATA_API MarketDataApiFactory
{
public:
	static const std::string kCtpFutureDataType;
	static const std::string kCtpStockDataType;
	static const std::string kCtpOptionDataType;
	static const std::string kGtaStockDataType;//国泰君安证券

public:
	MarketDataApiFactory(void);
	~MarketDataApiFactory(void);

	static MarketDataApi* CreateMarketDataApi(std::string type);
};

}
}

#endif	//EYEGLE_MARKETAPI_MARKETDATAAPI_FACTORY_H_

