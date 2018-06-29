#include "common/FileSync.h"
#include <QtCore/QFile>
#include <assert.h>
#include "common/AppLog.h"

using namespace std;

namespace zhongan {
namespace common {

FileSync::FileSync(const std::string& path, unsigned int data_size, unsigned int reserve_size, unsigned int ori_capacity)
	: data_size_(data_size), data_capacity_(ori_capacity), data_buf_(NULL), buf_(NULL), data_num_(0), reserve_size_(reserve_size)
{
	assert(data_size > 0 && ori_capacity > 0);
	file_ = new QFile(path.c_str());
}

FileSync::~FileSync()
{
	delete file_;
}

bool FileSync::Init(std::string& err)
{
	if (buf_ != NULL) { return true; } //已经初始化过

	if (!file_->open(QIODevice::ReadWrite))
	{
		err = "file open error";
		return false;
	}

	int file_size = file_->size();
	int file_capacity = 0;
	//第一次创建
	if (file_size <= reserve_size_ + sizeof(unsigned int))
	{
		data_num_ = 0;
		if (!file_->resize(reserve_size_ + sizeof(unsigned int) + data_capacity_ * data_size_))
		{
			err = "file resize error";
			return false;
		}
	}
	else
	{
		if ((file_size - reserve_size_  - sizeof(unsigned int)) % data_size_ != 0)
		{
			err = "file size error";
			return false;
		}

		file_capacity = (file_size - reserve_size_ - sizeof(unsigned int)) / data_size_;
		if (file_capacity < data_capacity_)
		{
			if (!file_->resize(reserve_size_ + sizeof(unsigned int) + data_capacity_ * data_size_))
			{
				err = "file resize error";
				return false;
			}
		}
		else
		{
			data_capacity_ = file_capacity;
		}
	}

	buf_ = (char*)file_->map(0, reserve_size_ + sizeof(unsigned int) + data_capacity_ * data_size_);
	if (buf_ == NULL)
	{
		err = "file map error";
		return false;
	}
	data_buf_ = buf_ + reserve_size_ + sizeof(unsigned int);

	if (file_capacity == 0)
	{
		memset(buf_, 0, reserve_size_ + sizeof(unsigned int));
	}
	else
	{
		memcpy(&data_num_, buf_ + reserve_size_, sizeof(unsigned int));
		if (data_num_ > file_capacity)
		{
			err = "data num error";
			buf_ = NULL;
			data_buf_ = NULL;
			return false;
		}
	}

	return true;
}

bool FileSync::Check(int size, std::string& err)
{
	if (buf_ == NULL)
	{
		err = "has not been init";
		return false;
	}

	if (size != data_size_)
	{
		err = "data type error";
		return false;
	}

	return true;
}

char* FileSync::GetTailAddr(std::string& err)
{
	if (data_num_ == data_capacity_)
	{
		data_capacity_ *= 2;
		if (data_capacity_ <= data_num_)
		{
			char err_tmp[32] = {0};
			sprintf(err_tmp, "exced max len: %ud", data_num_);
			err = err_tmp;
			data_capacity_ = data_num_;
			return NULL;
		}

		if (!file_->resize(reserve_size_ + sizeof(unsigned int) + data_capacity_ * data_size_))
		{
			char err_tmp[64] = {0};
			sprintf(err_tmp, "file resize error, data_capacity_ =  %ud", data_capacity_);
			err = err_tmp;
			data_capacity_ = data_num_;
			return NULL;
		}

		file_->unmap((uchar*)buf_);
		buf_ = (char*)file_->map(0, reserve_size_ + sizeof(unsigned int) + data_capacity_ * data_size_);
		if (buf_ == NULL)
		{
			err = "file map error";
			data_buf_ = NULL;
			return NULL;
		}
		data_buf_ = buf_ + reserve_size_ + sizeof(unsigned int);
	}

	return data_buf_ + data_size_ * data_num_;
}

bool FileSync::Remove(unsigned int idx, std::string& err)
{
	if (buf_ == NULL)
	{
		err = "has not been init";
		return false;
	}

	if (idx >= data_num_)
	{
		err = "idx out of data num";
		return false;
	}

	int copy_len = (data_num_ - idx - 1) * data_size_;
	if (copy_len > 0)
	{
		memcpy(data_buf_ + idx * data_size_, data_buf_ + (idx + 1) * data_size_, copy_len);
	}

	data_num_--;
	memcpy(buf_ + reserve_size_, &data_num_, sizeof(unsigned int));
	return true;
}

void FileSync::Clear()
{
	if (buf_ != NULL)
	{
		data_num_ = 0;
		memcpy(buf_ + reserve_size_, &data_num_, sizeof(unsigned int));
	}
}

}
}
