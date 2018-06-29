#ifndef FUNC_H
#define FUNC_H

#include "libdata/Array.h"
#include "libdata/SimpleMath.h"



double Average(NumericSeries prices, int length);
double AverageFC(NumericSeries avgs, NumericSeries prices, int length);
//@param x_averages ：指数平均价序列
//@param price ：价格序列
//@param length ：周期
double XAverage(NumericSeries xAvgs, NumericSeries prices, int length);

double Summation(NumericSeries prices, int length);
double SummationFC(NumericSeries sums, NumericSeries prices, int length);

double Highest(NumericSeries prices, int length, int offset = 0);
double Lowest(NumericSeries prices, int length, int offset = 0);

// 均线下穿、上穿
bool DownCross(NumericSeries fastMovAvg, NumericSeries slowMovAvg);
bool UpCross(NumericSeries fastMovAvg, NumericSeries slowMovAvg);

// 相对强弱
double RelativeStrength(NumericSeries prices, int length);
double RelativeStrengthFC(NumericSeries rsis, NumericSeries prices, int length);

// 标准差
double StandardDev(NumericSeries avgs, NumericSeries prices, int length);

#endif