#ifndef ITSTATION_DATASERVER_ARRAY_H_
#define ITSTATION_DATASERVER_ARRAY_H_

#include "dataserver/DataServerStruct.h"
#include "common/SpinLock.h"
#include <assert.h>
#include <exception>
#include <stdexcept>
#include <new>
#include <memory>

namespace itstation {
namespace strategy { class TechnicalIndicator; }
namespace common {

//20: || 改成 &&
#define DEF_EQUAL_FUNC(ope) \
	Array<T>& operator ope(Array<T>& arr) \
	{ \
		if (data_field_->data != NULL && data_field_->cur_pos >= 0) { (*this)[0] ope arr[0]; } \
		else{ \
			data_field_ = arr.data_field_; \
			if (data_field_->data != NULL) { \
				for (int i=0; i <= data_field_->cur_pos; ++i){ \
					T val = T(); \
					val ope data_field_->data[i]; \
					data_field_->data[i] = val; \
				} \
			} \
		} \
		return *this; \
	}

#define DEF_EQUAL_T_FUNC(ope) \
	Array<T>& operator ope(T val) \
	{ \
		(*this)[0] ope val;  \
		return *this; \
	}

#define DEF_OPERATOR_FUNC(ope) T operator ope(Array<T>& arr) { return (*this)[0] ope arr[0]; }

template <typename T>
class Array
{
	enum { MAX_LEN = 1000000 };
	struct ArrayDataField
	{
		int cur_pos;
		int capcity;
		bool zero_idx_flag;
		int zero_idx;
		T* data;

		ArrayDataField() : cur_pos(-1), capcity(0), zero_idx_flag(false), zero_idx(-1), data(NULL) {}
		~ArrayDataField() { delete data; }
	};
public:
	static T ZERO_;

	Array(int default_capcity = 0) : data_field_(new ArrayDataField())
	{
		// 默认容量
		data_field_->capcity = default_capcity;   
		if (default_capcity > 0)
		{
			data_field_->data = new T[default_capcity];
			memset(data_field_->data, 0, default_capcity * sizeof(T));
		}
	}

	Array(const Array<T> &other)
	{
		data_field_ = other.data_field_;	
	}

	operator T() { return operator[](0); }

	int Size() const { return (data_field_->zero_idx_flag ? data_field_->zero_idx : data_field_->cur_pos) + 1; }
	int Capacity() { return data_field_->capcity; }

	void Resize(int s) 
	{
		if (s < 0) { throw std::invalid_argument("链表长度不能小于0"); }
		if (s > MAX_LEN) { throw std::out_of_range("超过允许最大长度1000000"); }
		if (data_field_->capcity < s) { Stretch(s); }
		data_field_->cur_pos = s - 1;
	}

	void Clear() { data_field_->cur_pos = -1; }

	T* Data(int i = 0) 
	{ 
		if (data_field_->data == NULL || i >= data_field_->capcity) { return NULL; }
		return data_field_->data + i; 
	}

	T& operator[](int idx) 
	{
		int pos = (data_field_->zero_idx_flag ? data_field_->zero_idx : data_field_->cur_pos) - idx;
		if (data_field_->data == NULL || pos < 0) { ZERO_ = T(); return ZERO_; } //保证每次返回都是0
		return data_field_->data[pos];
	}
	
	int Pos(int idx)
	{
		int pos = (data_field_->zero_idx_flag ? data_field_->zero_idx : data_field_->cur_pos) - idx;
		if (data_field_->data == NULL || pos < 0) { return -1; }
		return pos;
	}

	void Update(const T& val)
	{
		if(data_field_->data == NULL || data_field_->cur_pos < 0) { return; }
		data_field_->data[data_field_->cur_pos] = val;
	}
	void Append(const T& val)
	{
		if (data_field_->cur_pos >= data_field_->capcity - 1) { Stretch(data_field_->cur_pos + 2); }
		data_field_->data[++data_field_->cur_pos] = val;
	}

	void SetZeroIdxFlag(bool flag) { data_field_->zero_idx_flag = flag; }
	void SetZeroIdx(int idx) { data_field_->zero_idx = idx; }

	DEF_EQUAL_FUNC(=)
	DEF_EQUAL_FUNC(+=)
	DEF_EQUAL_FUNC(-=)
	DEF_EQUAL_FUNC(*=)
	DEF_EQUAL_FUNC(/=)

	DEF_EQUAL_T_FUNC(=)
	DEF_EQUAL_T_FUNC(+=)
	DEF_EQUAL_T_FUNC(-=)
	DEF_EQUAL_T_FUNC(*=)
	DEF_EQUAL_T_FUNC(/=)

	DEF_OPERATOR_FUNC(+)
	DEF_OPERATOR_FUNC(-)
	DEF_OPERATOR_FUNC(*)
	DEF_OPERATOR_FUNC(/)

private:
	void Stretch(int s)
	{
		if (s <= data_field_->capcity) { return; }
		if (data_field_->data == NULL)
		{
			data_field_->capcity = s;
			data_field_->data = new T[data_field_->capcity];
			memset(data_field_->data, 0, data_field_->capcity * sizeof(T));
			return;
		}

		while (data_field_->capcity < s) { data_field_->capcity *= 2; }
		try {
			T* new_data = new T[data_field_->capcity];
			memset(new_data, 0, data_field_->capcity * sizeof(T));
			memcpy(new_data, data_field_->data, sizeof(T) * Size());
			T* tmp = data_field_->data;
			data_field_->data = new_data;
			delete [] tmp;
		}
		catch (...) {
			throw std::out_of_range("分配内存失败.");
		}
	}

private:
	std::tr1::shared_ptr<ArrayDataField>  data_field_;
};

template <typename T> T Array<T>::ZERO_ = T();

typedef Array<double> NumericSeries;
typedef Array<DateTime> DateTimeSeries;

}
}
#endif
