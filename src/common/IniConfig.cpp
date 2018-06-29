#include "common/IniConfig.h"
#include <QtCore/QSettings>

using namespace std;

namespace zhongan {
namespace common {

IniConfig::IniConfig(const std::string& path)
{
	settings_ = new QSettings(path.c_str(), QSettings::IniFormat);
}

IniConfig::~IniConfig()
{
	delete settings_;
}

#define SET_VAL_IMP(type) \
void IniConfig::SetValue(const std::string& key, const type val) \
{ \
	settings_->setValue(key.c_str(), val); \
}

#define GET_VAL_IMP(type, qt_func) \
type IniConfig::type##Value(const std::string& key) \
{ \
	return settings_->value(key.c_str()).qt_func; \
}

SET_VAL_IMP(int)
SET_VAL_IMP(char)
SET_VAL_IMP(float)
SET_VAL_IMP(double)
SET_VAL_IMP(char*)
SET_VAL_IMP(bool)

void IniConfig::SetValue(const std::string& key, std::string& val)
{
	settings_->setValue(key.c_str(), val.c_str()); \
}

GET_VAL_IMP(int, toInt())
GET_VAL_IMP(char, toChar().toLatin1())
GET_VAL_IMP(float, toFloat())
GET_VAL_IMP(double, toDouble())
GET_VAL_IMP(string, toString().toStdString())
GET_VAL_IMP(bool, toBool())

}
}
