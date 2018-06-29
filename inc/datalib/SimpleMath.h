#ifndef SIMPLEMATH_H
#define SIMPLEMATH_H


bool PriceGreatThan(double val1, double val2);
bool PriceInRange(double val, double val1, double val2);
bool PriceEqual(double val1, double val2);
bool PriceUnEqual(double val1, double val2);
///成交额.
bool DoubleUnEqual(double val1, double val2);

bool FloatUnEqual(float val1, float val2);

template <typename T>
bool UnEqual(T t1, T t2);

template bool UnEqual(signed char, signed char);
template bool UnEqual(int, int);
template<> bool UnEqual(float, float); // 加<>表示需要重新实现.
template<> bool UnEqual(double, double);


#endif