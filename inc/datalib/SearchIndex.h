#ifndef ITSTATION_DATASERVER_SEARCH_INDEX_H_
#define ITSTATION_DATASERVER_SEARCH_INDEX_H_

#include "datalib/DataServerStruct.h"
#include "common/Global.h"



//索引
class DATALIB_API SearchIndex
{
public:
	

	static int IndexSize();
	static int Index(const Symbol& symbol);

private:
	SearchIndex(void) {}
	~SearchIndex(void) {}
};



#endif

