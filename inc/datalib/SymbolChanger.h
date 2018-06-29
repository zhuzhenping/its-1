#ifndef ITS_COMMON_SYMBOL_CHANGER_H_
#define ITS_COMMON_SYMBOL_CHANGER_H_

#include "common/Global.h"
#include "datalib/MarketDefine.h"

//namespace itstation {
//namespace common {

// 提取品种代码 "IF1511" => "IF" 
std::string DATALIB_API GetFutureProName(const char *instrument);
// ==> "CFFEX"等
std::string DATALIB_API ToExchangeStr(ExchangeIdType id);

// ==> 
std::string DATALIB_API PeriodName(DimensionType dim);

// 提取品种名字 "FUTURE" "STOCK" "INDEX"
std::string DATALIB_API ProductName(ProductIdType product);

void DATALIB_API CalcProductId(Symbol* sym);


#endif