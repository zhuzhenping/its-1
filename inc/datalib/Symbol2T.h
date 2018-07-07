#ifndef ITSTATION_DATASERVER_SYMBOL_CONTAINER_H_
#define ITSTATION_DATASERVER_SYMBOL_CONTAINER_H_

#include <map>
#include "datalib/DataServerStruct.h"
#include "datalib/SearchIndex.h"
#include "datalib/SymbolInfoSet.h"
#include <fstream>


template<class T>
class Symbol2T
{
public:
/*
#ifndef EachHandle
	typedef  void (*EachHandle)(T* data, void* user_data);
#endif
*/

	Symbol2T() 
	{
		size_ = SearchIndex::IndexSize();
		data_ = new T*[size_];
		memset(data_, 0, sizeof(T*) * size_);
	}

	~Symbol2T(void)
	{
		delete [] data_;
	}

	int Size()
	{
		return size_;
	}

	T** Data(const Symbol& sym)
	{
		int idx = SearchIndex::Index(sym);
		if (idx < 0 || idx >= size_)
		{
			return NULL;
		}
		return &data_[idx];
	}

	T* Data(int i)
	{
		return data_[i];
	}

	/*void ForEach(EachHandle hdl, void* user_data = NULL)
	{
		for (int i=0; i<size_; ++i)
		{
			if (NULL == data_[i]) { continue; }
			hdl(data_[i], user_data);			
		}
	}*/

	void Clear()
	{
		for (int i=0; i<size_; ++i)
		{
			if (NULL == data_[i]) { continue; }
			delete data_[i];
			data_[i] = NULL;
		}
	}

private:
	T** data_;
	int size_;
};


#endif

