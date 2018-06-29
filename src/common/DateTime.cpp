#include "common/DateTime.h"
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include "common/XmlConfig.h"
#include <QtCore/QFile>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace zhongan {
namespace common {

Date::Date(char* date_str) 
{
	if (strlen(date_str) != 8) {
		year = 1900;
		month = 1;
		day = 1;
		return;
	}

	sscanf(date_str, "%4d%2d%2d", &year, &month, &day);
}

bool Date::operator==(const Date& date) const
{
	if(year == date.year && month == date.month && day == date.day) { return true; }

	return false;
}

bool Date::operator>(const Date& date) const
{
	if(year > date.year || (year == date.year && month > date.month) || (year == date.year && month == date.month && day > date.day)) {
		return true;
	}

	return false;
}

bool Date::operator<(const Date& date) const
{
	if(year < date.year || (year == date.year && month < date.month) || (year == date.year && month == date.month && day < date.day)) {
		return true;
	}

	return false;
}

bool Date::operator<=(const Date& date) const
{
	return !(*this > date);
}

bool Date::operator>=(const Date& date) const
{
	return !(*this < date);
}

bool Date::operator!=(const Date& date) const
{
	return !(*this == date);
}

std::string Date::Str() const 
{
	std::stringstream ss;
	ss<<year<<"-"
		<<std::setfill('0')<<std::setw(2)<<month<<"-"
		<<std::setfill('0')<<std::setw(2)<<day;
	return ss.str();
}

std::string Date::FolderStr() const {
	std::stringstream ss;
	ss<<year
		<<std::setfill('0')<<std::setw(2)<<month
		<<std::setfill('0')<<std::setw(2)<<day;
	return ss.str();
}

bool Date::IsValid() const
{
	QDate now(year, month, day);
	return now.isValid();
}

bool Date::IsHoliday(XmlConfig* config) const
{
	XmlNodeVec node_vec;
	if (config != NULL)
	{
		node_vec = config->FindChileren("Holidays");
	}
	else
	{
		std::string zhongan_home = getenv("ZHONGAN_HOME");
		std::string conf_path = zhongan_home + "\\config\\TradingTime.xml";
		if (!QFile::exists(conf_path.c_str()))
		{
			return false;
		}

		XmlConfig config2(conf_path);
		if (!config2.Load())
		{
			return false;
		}
		node_vec = config2.FindChileren("Holidays");
	}
	
	QStringList holidays;
	for (int i=0; i < node_vec.size(); ++i)
	{
		if (atoi(node_vec[i].GetValue("Year").c_str()) == year)
		{
			holidays = QString(node_vec[i].GetValue("Days").c_str()).split(',');
			break;
		}
	}

	QDate now(year, month, day);
	QString day_str = now.toString("MMdd");
	return holidays.contains(day_str);
}

bool Date::IsTradingDay(XmlConfig* config) const
{
	return !IsHoliday(config) && WeekDay() < 6;
}

int Date::WeekDay() const
{
	QDate now(year, month, day);
	return now.dayOfWeek();
}

void Date::operator--()
{
	QDate pre(year, month, day);
	pre = pre.addDays(-1);
	year = pre.year();
	month = pre.month();
	day = pre.day();
}

void Date::operator++()
{
	QDate next(year, month, day);
	next = next.addDays(1);
	year = next.year();
	month = next.month();
	day = next.day();
}

Date Date::PreTradingDay(XmlConfig* config) const
{ 
	Date pre(year,month,day);
	do{
		--pre;
	}
	while (!pre.IsTradingDay(config));
	return pre;
}

Date Date::NextTradingDay(XmlConfig* config) const
{
	Date next(year,month,day);
	do{
		++next;
	}
	while (!next.IsTradingDay(config));
	return next;
}

void Date::AddDays(int d)
{
	QDate next(year, month, day);
	next = next.addDays(d);
	year = next.year();
	month = next.month();
	day = next.day();
}

int Date::operator-(const Date& date)
{
	QDate qdate_1(year, month, day);
	QDate qdate_2(date.year, date.month, date.day);
	return qdate_2.daysTo(qdate_1);
}
////////////////////////////////////////////////////////////////////////////////////

Time::Time(char* time_str, int mil) {
	milsec = mil;
	if (strlen(time_str) != 8) {
		hour = 0;
		minute = 0;
		sec = 0;
		return;
	}

	sscanf(time_str, "%2d:%2d:%2d", &hour, &minute, &sec);
}

Time& Time::operator=(const Time& t) {
	if (this != &t) {
		hour = t.hour;
		minute = t.minute;
		sec = t.sec;
		milsec = t.milsec;
	}

	return *this;
}

bool Time::operator==(const Time& t) const
{
	if(hour == t.hour && minute == t.minute && sec == t.sec && milsec == t.milsec) { return true; }

	return false;
}

bool Time::operator>(const Time& t) const
{
	if(hour > t.hour || (hour == t.hour && minute > t.minute) || (hour == t.hour && minute == t.minute && sec > t.sec)
		|| (hour == t.hour && minute == t.minute && sec == t.sec && milsec > t.milsec)) {
			return true;
	}

	return false;
}

bool Time::operator<(const Time& t) const
{
	if(hour < t.hour || (hour == t.hour && minute < t.minute) || (hour == t.hour && minute == t.minute && sec < t.sec)
		|| (hour == t.hour && minute == t.minute && sec == t.sec && milsec < t.milsec)) {
			return true;
	}

	return false;
}

bool Time::operator<=(const Time& t) const
{
	return !(*this > t);
}

bool Time::operator>=(const Time& t) const
{
	return !(*this < t);
}

bool Time::operator!=(const Time& t) const
{
	return !(*this == t);
}

//Time operator+(const Time& t1, const Time& t2) {
//	long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
//	long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
//	long ret_milsecs = t1_milsecs + t2_milsecs;
//	int ret_hour = ret_milsecs / (60 * 60 * 1000);
//	int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
//	int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
//	int ret_milsec = ret_milsecs % 1000;
//
//	return Time(ret_hour, ret_min, ret_sec, ret_milsec);
//}
//
//Time operator-(const Time& t1, const Time& t2) {
//	long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
//	long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
//	long ret_milsecs = t1_milsecs - t2_milsecs;
//	int ret_hour = ret_milsecs / (60 * 60 * 1000);
//	int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
//	int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
//	int ret_milsec = ret_milsecs % 1000;
//
//	return Time(ret_hour, ret_min, ret_sec, ret_milsec);
//}

int Time::AddMilSec(int ms)
{
	milsec += ms;
	if (milsec >= 1000)
	{
		int s = milsec / 1000;
		milsec %= 1000;
		return AddSec(s);
	} 
	else if (milsec < 0)
	{
		int s = milsec / 1000 - 1;
		do { milsec += 1000; } while (milsec < 0);
		milsec %= 1000;
		return AddSec(s);
	}
	return 0;
}

int Time::AddSec(int s)
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

int Time::AddMin(int min) 
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

std::string Time::Str() const 
{
	std::stringstream ss;
	ss<<std::setfill('0')<<std::setw(2)<<hour<<":"
		<<std::setfill('0')<<std::setw(2)<<minute<<":"
		<<std::setfill('0')<<std::setw(2)<<sec<<"."
		<<std::setfill('0')<<std::setw(3)<<std::setiosflags(std::ios::right)<<milsec;
	return ss.str();
}

double Time::operator-(const Time& time)
{
	double self_secs = (hour * 60 + minute) * 60 + sec + milsec / 1000.0;
	double secs = (time.hour * 60 + time.minute) * 60 + time.sec + time.milsec / 1000.0;
	return self_secs - secs;
}
////////////////////////////////////////////////////////////////////////////////////

DateTime::DateTime(void* p) 
{
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	date = Date(sys.wYear,sys.wMonth, sys.wDay);
	time = Time(sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
	struct timeval t_time;
	gettimeofday(&t_time, NULL);
	time_t t_date;
	::time(&t_date);
	tm* local_t = localtime(&t_date);
	date = Date(local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday);
	time = Time(local_t->tm_hour, local_t->tm_min, local_t->tm_sec, t_time.tv_usec/1000);
#endif
}

DateTime& DateTime::operator=(const DateTime& date_time) {
	if (this != &date_time) {
		date = date_time.date;
		time = date_time.time;
	}

	return *this;
}

bool DateTime::operator==(const DateTime& date_time) const
{
	if(date == date_time.date && time == date_time.time) { return true; }

	return false;
}

bool DateTime::operator>(const DateTime& date_time) const
{
	if(date > date_time.date || (date == date_time.date && time > date_time.time)) { return true; }

	return false;
}

bool DateTime::operator<(const DateTime& date_time) const
{
	if(date < date_time.date || (date == date_time.date && time < date_time.time)) { return true; }

	return false;
}

bool DateTime::operator!=(const DateTime& date_time) const 
{
	return !(*this == date_time);
}

bool DateTime::operator>=(const DateTime& date_time) const 
{
	return !(*this < date_time);
}

bool DateTime::operator<=(const DateTime& date_time) const 
{
	return !(*this > date_time);
}

std::string DateTime::Str() const 
{
	std::stringstream ss;
	ss<<date.Str()<<" "<<time.Str();
	return ss.str();
}

//DateTime operator+(const DateTime& t1, const Time& t2) 
//{
//	Time t3 = t1.time + t2;
//	return DateTime(t1.date, t3);
//}
//
//DateTime operator+(const Time& t1, const DateTime& t2) 
//{
//	return t2 + t1;
//}
//
//DateTime operator-(const DateTime& t1, const Time& t2) 
//{
//	Time t3 = t1.time - t2;
//	return DateTime(t1.date, t3);
//}

bool DateTime::IsTradingDay(XmlConfig* config) { return date.IsTradingDay(config); }

DateTime& DateTime::AddMilSec(int ms)
{
	int plus_day = time.AddMilSec(ms);
	if (plus_day != 0)
	{
		date.AddDays(plus_day);
	}

	return *this;
}

DateTime& DateTime::AddSec(int s)
{
	int plus_day = time.AddSec(s);
	if (plus_day != 0)
	{
		date.AddDays(plus_day);
	}

	return *this;
}

DateTime& DateTime::AddMin(int min)
{
	int plus_day = time.AddMin(min);
	if (plus_day != 0)
	{
		date.AddDays(plus_day);
	}

	return *this;
}

double DateTime::operator-(const DateTime& date_time)
{
	return (date - date_time.date) * 86400.0 + (time - date_time.time);
}

Date DateTime::CurrentTradingDay(DateTime time)
{
	if (time.time.hour >= 3 && time.time.hour < 18)
	{
		return time.date.IsTradingDay() ? time.date : time.date.NextTradingDay();
	}
	else if (time.time.hour < 3)
	{
		time.date.AddDays(-1);
	}

	return time.date.NextTradingDay();
}

}
}