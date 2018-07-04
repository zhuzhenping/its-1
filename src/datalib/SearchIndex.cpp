#include "datalib/SearchIndex.h"
#include <common/AppLog.h>



int SearchIndex::IndexSize()
{
	return 947700;
}

int SearchIndex::Index(const Symbol& symbol)
{
	
	int date;
	char product[8] = {0};
	sscanf(symbol.instrument, "%[^0-9]%d", product, &date);

	int code_num = 0;
	for (int i=0; i < 8 && product[i] != 0; ++i)
	{
		code_num *= 26;
		code_num += (product[i] - 64) % 32;
	}

	code_num = code_num *13 + date % 100;
	code_num = code_num * 100 + date / 100;
	return code_num < 947700 ? code_num : -1;

}

