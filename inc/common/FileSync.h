#ifndef COMMON_FILE_SYNC_H_  
#define COMMON_FILE_SYNC_H_  

#include <string>
#include "common/Global.h"

class QFile;

namespace zhongan {
namespace common {

//由外部保证线程同步.
class COMMON_API FileSync
{
public:
	///@data_size：要存储的结构体大小.
	///@ori_capacity：初始的数量.
	FileSync(const std::string& path, unsigned int data_size, unsigned int reserve_size = 0, unsigned int ori_capacity = 256);
	FileSync(){}
	~FileSync();
	bool Init(std::string& err);

	template <typename T>
	bool Add(T* data, std::string& err);

	template <typename T>
	bool Update(unsigned int idx, T* data, std::string& err);

	bool Remove(unsigned int idx, std::string& err);

	void Clear();

	unsigned int GetDataNum() { return data_num_; }
	char* GetDataBuf() { return data_buf_; }

	unsigned int GetReserveSize() { return reserve_size_; }
	char* GetReserveBuf() { return buf_; }

private:
	bool Check(int size, std::string& err);

	char* GetTailAddr(std::string& err);

private:
	QFile* file_;
	unsigned int data_size_; 
	unsigned int data_num_;
	unsigned int data_capacity_;
	unsigned int reserve_size_;
	char* buf_;
	char* data_buf_;
};

template <typename T>
bool FileSync::Add(T* data, std::string& err)
{
	if (!Check(sizeof(T), err)) { return false; }

	char* add_buf = GetTailAddr(err);
	if (NULL == add_buf) { return false; }

	memcpy(add_buf, data, data_size_);
	data_num_++;
	memcpy(buf_ + reserve_size_, &data_num_, sizeof(unsigned int));
	return true;
}

template <typename T>
bool FileSync::Update(unsigned int idx, T* data, std::string& err)
{
	if (!Check(sizeof(T), err)) { return false; }

	if (idx >= data_num_)
	{
		err = "idx out of data num";
		return false;
	}

	memcpy(data_buf_ + idx * data_size_, data, data_size_);
	return true;
}

}
}
#endif

