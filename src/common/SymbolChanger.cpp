
#include "common/SymbolChanger.h"


//namespace itstation {
//namespace common {

std::string GetFutureProName(const char *instrument) {
	char pro_str[8] = {0};
	sscanf(instrument, "%[^0-9]", pro_str);
	return pro_str;
}

std::string ToExchangeStr(ExchangeIdType id) {
	switch(id)
	{
	case EXCHANGE_CFFEX: return "CFFEX";
	case EXCHANGE_CZCE: return "CZCE";
	case EXCHANGE_DCE: return "DCE";
	case EXCHANGE_SHFE: return "SHFE";  //SHFE
	case EXCHANGE_INE: return "INE";
	case EXCHANGE_SSE: return "SSE";
	case EXCHANGE_SZE: return "SZE";
	default:
		return "";
	}
}


std::string PeriodName(DimensionType dimension) { 
	switch(dimension) {
	case DIMENSION_TICK:
	case DIMENSION_SECOND:
		return "Tick";
	case DIMENSION_MINUTE:
	case DIMENSION_HOUR:
		return "Minute";
	default:
		return "Day";
	}
}	

std::string ProductName(ProductIdType product) {
	switch (product) 
	{
	case PRODUCT_STOCK:
		return "STOCK";
	case PRODUCT_FUTURE:
		return "FUTURE";
	case PRODUCT_OPTION:
		return "OPTION";
	case PRODUCT_BOND:
		return "BOND";
	case PRODUCT_FUND:
		return "FUND";
	case PRODUCT_INDEX:
		return "INDEX";
	case PRODUCT_SPOT:
		return "SPOT";
	default:
		return "OTHER";
	}
}

void CalcProductId(Symbol* sym)
{
	std::string prefix = sym->instrument;
	std::string  prefix_1 = prefix.substr(0, 3);
	prefix = prefix.substr(0, 2);
	switch (sym->exchange)
	{
	case EXCHANGE_SSE:
		sym->product = prefix == "60" ? PRODUCT_STOCK : (
			prefix == "00" ? PRODUCT_INDEX : (
			prefix == "50" || prefix == "51" ? PRODUCT_FUND : 
			PRODUCT_OTHER));
		break;

	case EXCHANGE_SZE:
		sym->product = prefix == "00" || prefix == "30" ? PRODUCT_STOCK : (
			prefix > "14" && prefix < "19" ? PRODUCT_FUND : (
			prefix_1 == "399" ? PRODUCT_INDEX : 
			PRODUCT_OTHER));
		break;

	default:
		sym->product = PRODUCT_OTHER;
		break;
	}
}

