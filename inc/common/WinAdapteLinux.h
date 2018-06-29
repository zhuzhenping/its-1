#ifndef WIN_ADAPATE_LINUX_H_  
#define WIN_ADAPATE_LINUX_H_  

#include <string>
#include "common/Global.h"
#include <stdlib.h>

#ifdef WIN32
long long COMMON_API atoll (const char *p);
#endif

std::string COMMON_API UTF8ToMB(const std::string& str);

std::string COMMON_API MBToUTF8(const std::string& str);

#endif

