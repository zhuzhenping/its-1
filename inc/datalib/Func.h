#ifndef FUNC_H
#define FUNC_H

#include "datalib/Array.h"
#include "datalib/SimpleMath.h"



double DATALIB_API Average(NumericSeries prices, int length);
double DATALIB_API AverageFC(NumericSeries avgs, NumericSeries prices, int length);
//@param x_averages ：指数平均价序列
//@param price ：价格序列
//@param length ：周期
double DATALIB_API XAverage(NumericSeries xAvgs, NumericSeries prices, int length);

double DATALIB_API Summation(NumericSeries prices, int length);
double DATALIB_API SummationFC(NumericSeries sums, NumericSeries prices, int length);

double DATALIB_API Highest(NumericSeries prices, int length, int offset = 0);
double DATALIB_API Lowest(NumericSeries prices, int length, int offset = 0);

// 均线下穿、上穿
bool DATALIB_API DownCross(NumericSeries fastMovAvg, NumericSeries slowMovAvg);
bool DATALIB_API UpCross(NumericSeries fastMovAvg, NumericSeries slowMovAvg);

// 相对强弱
double DATALIB_API RelativeStrength(NumericSeries prices, int length);
double DATALIB_API RelativeStrengthFC(NumericSeries rsis, NumericSeries prices, int length);

// 标准差
double DATALIB_API StandardDev(NumericSeries avgs, NumericSeries prices, int length);

#endif