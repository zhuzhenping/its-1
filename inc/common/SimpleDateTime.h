#ifndef _COMMON_SIMPLE_DATE_TIME_H_
#define _COMMON_SIMPLE_DATE_TIME_H_

#include <stdio.h>
#include "common/Global.h"
#ifdef WIN32
#include <Windows.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

namespace zhongan {
namespace common {
#pragma pack(1)
struct COMMON_API SimpleDate 
{
	short year;
	char month;
	char day;
	explicit SimpleDate(short y=0, char m=0, char d=0) : year(y), month(m), day(d) {}

	bool operator==(const SimpleDate& other) const { return year == other.year && month == other.month && day == other.day; }
	bool operator!=(const SimpleDate& other) const { return !(*this == other); }
	bool operator<(const SimpleDate& other) const {
		return year < other.year || (year == other.year && month < other.month) || 
			(year == other.year && month == other.month && day < other.day);
	}
	bool operator>(const SimpleDate& other) const {
		return year > other.year || (year == other.year && month > other.month) || 
			(year == other.year && month == other.month && day > other.day);
	}
	bool operator<=(const SimpleDate& other) const { return !(*this > other); }
	bool operator>=(const SimpleDate& other) const { return !(*this < other); }

	std::string Str() const;
	void AddDays(char n);
	int WeekDay() const; //星期日为7.
};
//
struct COMMON_API SimpleTime 
{
	char hour;
	char minute;
	char sec;
	short mil_sec;
	explicit SimpleTime(char h=0, char M=0, char s=0, short ms = 0) : hour(h), minute(M), sec(s), mil_sec(ms) {};

	bool operator==(const SimpleTime& other) const {return hour == other.hour && minute == other.minute && sec == other.sec && mil_sec == other.mil_sec;}
	bool operator!=(const SimpleTime& other) const { return !(*this == other); }
	bool operator<(const SimpleTime& other) const {
		return hour < other.hour || (hour == other.hour && minute < other.minute) || 
			(hour == other.hour && minute == other.minute && sec < other.sec) ||
			(hour == other.hour && minute == other.minute && sec == other.sec && mil_sec < other.mil_sec);
	}
	bool operator>(const SimpleTime& other) const {
		return hour > other.hour || (hour == other.hour && minute > other.minute) || 
			(hour == other.hour && minute == other.minute && sec > other.sec) ||
			(hour == other.hour && minute == other.minute && sec == other.sec && mil_sec > other.mil_sec);
	}
	bool operator<=(const SimpleTime& other) const { return !(*this > other); }
	bool operator>=(const SimpleTime& other) const { return !(*this < other); }

	std::string Str() const;
	int AddMin(char min)
	{
		minute += min;
		if (minute >= 60)
		{
			hour += minute / 60;
			minute %= 60;
			if (hour >= 24)
			{
				int day = hour / 24;
				hour %= 24;
				return day;
			}
		}
		else if (minute < 0)
		{
			hour += minute / 60 - 1;
			do { minute += 60; } while (minute < 0);
			minute %= 60;
			if (hour < 0)
			{
				int day = hour / 24 - 1;
				hour %= 24;
				return day;
			}
		}
		return 0;
	}

	int AddSec(int s)
	{
		sec += s;
		if (sec >= 60)
		{
			int min = sec / 60;
			sec %= 60;
			return AddMin(min);
		}
		else if (sec < 0)
		{
			int min = sec / 60 - 1;
			do { sec += 60; } while (sec < 0);
			sec %= 60;
			return AddMin(min);
		}
		return 0;
	}
};
//时间. (9B)
struct COMMON_API SimpleDateTime {
	SimpleDate date;
	SimpleTime time;
	SimpleDateTime(SimpleDate d = SimpleDate(), SimpleTime t = SimpleTime()) : date(d), time(t) {};

	explicit SimpleDateTime(time_t t)
	{
		struct tm *tp = localtime(&t);
		date.year = tp->tm_year + 1900;
		date.month = tp->tm_mon + 1;
		date.day = tp->tm_mday;
		time.hour = tp->tm_hour;
		time.minute = tp->tm_min;
		time.sec = tp->tm_sec;
		time.mil_sec = 0;
	}

	bool IsValid() const
	{
		return date.year > 1900 && date.year < 2500 && date.month > 0 && date.month < 13 && date.day > 0 && date.day < 32
			&& time.hour > -1 && time.hour < 24 && time.minute > -1 && time.minute < 60 &&  time.sec > -1 && time.sec < 60;
	}

	bool operator==(const SimpleDateTime& other) const { return date == other.date && time == other.time; }
	bool operator!=(const SimpleDateTime& other) const { return !(*this == other); }
	bool operator<(const SimpleDateTime& other) const { return date < other.date || (date == other.date && time < other.time); }
	bool operator>(const SimpleDateTime& other) const { return date > other.date || (date == other.date && time > other.time); }
	bool operator<=(const SimpleDateTime& other) const { return !(*this > other); }
	bool operator>=(const SimpleDateTime& other) const { return !(*this < other); }

	void FromStr(const char *str)
	{
		int year, month, day, hour, minute, second;
		sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
		date.year = (short)year;
		date.month = (char)month;
		date.day = (char)day;
		time.hour = (char)hour;
		time.minute = (char)minute;
		time.sec = (char)second;
		time.mil_sec = 0;
	}

	std::string Str() const;
};
#pragma pack()

}
}
#endif	//_COMMON_DATE_TIME_H_