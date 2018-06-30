#ifndef SIMPLEMATH_H
#define SIMPLEMATH_H

#include "common/Global.h"

bool DATALIB_API PriceGreatThan(double val1, double val2);
bool DATALIB_API  PriceInRange(double val, double val1, double val2);
bool DATALIB_API  PriceEqual(double val1, double val2);
bool DATALIB_API  PriceUnEqual(double val1, double val2);
///成交额.
bool DATALIB_API  DoubleUnEqual(double val1, double val2);

bool DATALIB_API  FloatUnEqual(float val1, float val2);

/*
template <typename T>
bool UnEqual(T t1, T t2);

template bool UnEqual(signed char, signed char);
template bool UnEqual(int, int);
template<> bool UnEqual(float, float); // 加<>表示需要重新实现.
template<> bool UnEqual(double, double);
*/

/*

template<class T> bool UnEqual(T t1, T t2) { return t1 != t2; }

template<> bool UnEqual(float t1, float t2) { return FloatUnEqual(t1, t2); }
template<> bool UnEqual(double t1, double t2) { return DoubleUnEqual(t1, t2); }

*/


#endif