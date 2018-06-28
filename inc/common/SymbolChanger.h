#ifndef ITS_COMMON_SYMBOL_CHANGER_H_
#define ITS_COMMON_SYMBOL_CHANGER_H_

#include "common/Global.h"
#include "marketapi/MarketDefine.h"



// 提取品种代码 "IF1511" => "IF" 
std::string COMMON_API GetFutureProName(const char *instrument);
// ==> "CFFEX"等
std::string COMMON_API ToExchangeStr(ExchangeIdType id);

// ==> 
std::string COMMON_API PeriodName(DimensionType dim);

// 提取品种名字 "FUTURE" "STOCK" "INDEX"
std::string COMMON_API ProductName(ProductIdType product);

void COMMON_API CalcProductId(Symbol* sym);



#endif