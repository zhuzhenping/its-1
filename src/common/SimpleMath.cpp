
#include "common/SimpleMath.h"

#define FLOAT_PRECISION 0.001
#define DOUBLE_PRECISION 0.000001
#define PRICE_PRECISION  0.00001
#define VOLUME_PRECISION 0.1

namespace zhongan {
namespace common {

bool PriceGreatThan(double val1, double val2) {
	return val1 > val2 + PRICE_PRECISION;
}

bool PriceEqual(double val1, double val2) {
	return val1 > val2 - PRICE_PRECISION && val1 < val2 + PRICE_PRECISION;
}

bool PriceUnEqual(double val1, double val2) {
	return val1 < val2 - PRICE_PRECISION || val1 > val2 + PRICE_PRECISION;
}

bool DoubleUnEqual(double val1, double val2) {
	return val1 < val2 - DOUBLE_PRECISION || val1 > val2 + DOUBLE_PRECISION;
}

bool FloatUnEqual(float val1, float val2) {
	return val1 < val2 - FLOAT_PRECISION || val1 > val2 + FLOAT_PRECISION;
}

bool PriceInRange(double val, double val1, double val2) {
	return val > val1 - PRICE_PRECISION && val < val2 + PRICE_PRECISION;
}

template<class T> bool UnEqual(T t1, T t2) { return t1 != t2; }

template<> bool UnEqual(float t1, float t2) { return FloatUnEqual(t1, t2); }
template<> bool UnEqual(double t1, double t2) { return DoubleUnEqual(t1, t2); }

}
}