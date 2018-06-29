#ifndef _COMMON_DATE_TIME_H_
#define _COMMON_DATE_TIME_H_

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "common/Global.h"

namespace zhongan {
namespace common {
class XmlConfig;

#define COMMON_DATA_END zhongan::common::Date()

#pragma pack(1)
struct COMMON_API Date 
{
	int year;
	int month;
	int day;

	Date(int y = 1900, int m = 1, int d = 1) : year(y), month(m), day(d) {}
	Date(char* date_str);

	bool operator==(const Date& date) const;
	bool operator>(const Date& date) const;
	bool operator<(const Date& date) const;
	bool operator<=(const Date& date) const;
	bool operator>=(const Date& date) const;
	bool operator!=(const Date& date) const;
	void operator--(); // 昨天
	void operator++(); // 明天

	std::string Str() const;
	std::string FolderStr() const;

	bool IsValid() const;
	bool IsHoliday(XmlConfig* config = NULL) const;
	bool IsTradingDay(XmlConfig* config = NULL) const;
	Date PreTradingDay(XmlConfig* config = NULL) const; //上一交易日
	Date NextTradingDay(XmlConfig* config = NULL) const;
	int WeekDay() const; //星期日为7

	void AddDays(int day);
	int operator-(const Date& date);
};

struct COMMON_API Time 
{
	int hour;
	int minute;
	int sec;
	int milsec;

	Time(int h = 0, int m = 0, int s = 0, int ms = 0) : hour(h), minute(m), sec(s), milsec(ms) {}
	Time(char* time_str, int mil);

	Time& operator=(const Time& t);
	bool operator==(const Time& t) const;
	bool operator>(const Time& t) const;
	bool operator<(const Time& t) const;
	bool operator<=(const Time& t) const;
	bool operator>=(const Time& t) const;
	bool operator!=(const Time& t) const;

	std::string Str() const;

	int AddMilSec(int ms);
	int AddSec(int s);
	int AddMin(int min);
	double operator-(const Time& time);
};

struct COMMON_API DateTime 
{
	Date date;
	Time time;

	DateTime() : date(), time() {}
	DateTime(void* p);  //获取当前时间
	DateTime(const Date& d, const Time& t) : date(d), time(t) {}

	DateTime(const DateTime& date_time) :date(date_time.date), time(date_time.time) {}

	DateTime& operator=(const DateTime& date_time);

	bool operator==(const DateTime& date_time) const;
	bool operator>(const DateTime& date_time) const;
	bool operator<(const DateTime& date_time) const;
	bool operator!=(const DateTime& date_time) const;
	bool operator>=(const DateTime& date_time) const;
	bool operator<=(const DateTime& date_time) const;

	std::string Str() const;
	bool IsTradingDay(XmlConfig* config = NULL);

	DateTime& AddMilSec(int ms);
	DateTime& AddSec(int s);
	DateTime& AddMin(int min);
	double operator-(const DateTime& date_time);

	DateTime& operator+=(const DateTime& date_time) { return *this; }
	DateTime& operator-=(const DateTime& date_time) { return *this; }
	DateTime& operator*=(const DateTime& date_time) { return *this; }
	DateTime& operator/=(const DateTime& date_time) { return *this; }
	DateTime& operator+(const DateTime& date_time) { return *this; }
	//DateTime& operator-(const DateTime& date_time) { return *this; }
	DateTime& operator*(const DateTime& date_time) { return *this; }
	DateTime& operator/(const DateTime& date_time) { return *this; }

	//返回当前时间所在的交易日
	static Date CurrentTradingDay(DateTime time = DateTime(NULL));
};

//COMMON_API DateTime operator+(const DateTime& t1, const Time& t2);
//COMMON_API DateTime operator+(const Time& t1, const DateTime& t2);
//COMMON_API DateTime operator-(const DateTime& t1, const Time& t2);

#pragma pack()

}
}
#endif	//_COMMON_DATE_TIME_H_