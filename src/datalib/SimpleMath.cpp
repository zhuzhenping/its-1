
#include "datalib/SimpleMath.h"
#include <stdarg.h>

#define FLOAT_PRECISION 0.001
#define DOUBLE_PRECISION 0.000001
#define PRICE_PRECISION  0.00001
#define VOLUME_PRECISION 0.1

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

double DoubleMax(int n, ...) {
	va_list arg;
	va_start(arg, n);
	double max = 0.;
	for (int i = 0; i < n; ++i) {
		int val = va_arg(arg, double);
		if (PriceGreatThan(val, max)) max = val;
	}
	va_end(arg);
	return max;
}

double DoubleMin(int n, ...) {
	va_list arg;
	va_start(arg, n);
	double min = 99999999.;
	for (int i = 0; i < n; ++i) {
		int val = va_arg(arg, double);
		if (PriceGreatThan(min, val)) min = val;
	}
	va_end(arg);
	return min;
}