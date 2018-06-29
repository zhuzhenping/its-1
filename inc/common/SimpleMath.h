#ifndef _COMMON_MATH_H_
#define _COMMON_MATH_H_

#include "common/Global.h"

namespace zhongan {
namespace common {

bool COMMON_API PriceGreatThan(double val1, double val2);
bool COMMON_API PriceInRange(double val, double val1, double val2);
bool COMMON_API PriceEqual(double val1, double val2);
bool COMMON_API PriceUnEqual(double val1, double val2);
///成交额.
bool COMMON_API DoubleUnEqual(double val1, double val2);

bool COMMON_API FloatUnEqual(float val1, float val2);

template <typename T>
bool UnEqual(T t1, T t2);

template bool COMMON_API UnEqual(signed char, signed char);
template bool COMMON_API UnEqual(int, int);
template<> bool COMMON_API UnEqual(float, float); // 加<>表示需要重新实现.
template<> bool COMMON_API UnEqual(double, double);

}
}

#endif