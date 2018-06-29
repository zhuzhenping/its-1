#include "common/SimpleDateTime.h"
#include <sstream>
#include <QtCore/QDateTime>

namespace zhongan {
namespace common {

std::string SimpleDate::Str() const
{
	char str[16] = {0};
	sprintf(str, "%04d-%02d-%02d", year, month, day);
	return str;
}

void SimpleDate::AddDays(char n)
{
	QDate next(year, month, day);
	next = next.addDays(n);
	year = next.year();
	month = next.month();
	day = next.day();
}

int SimpleDate::WeekDay() const
{
	QDate now(year, month, day);
	return now.dayOfWeek();
}
//
////////////////////////////////////////////////////////////////////////////////////

std::string SimpleTime::Str() const
{
	char str[16] = {0};
	sprintf(str, "%02d:%02d:%02d", hour, minute, sec);
	return str;
}

////////////////////////////////////////////////////////////////////////////////////

std::string SimpleDateTime::Str() const 
{
	std::stringstream ss;
	ss << date.year << "-" << int(date.month) << "-" << int(date.day)
		<<" " << int(time.hour) <<":" << int(time.minute) << ":" <<int(time.sec) << "." <<int(time.mil_sec);
	return ss.str();
}

}
}