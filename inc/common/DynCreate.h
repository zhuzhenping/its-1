#ifndef COMMON_NIN_CONFIG_H_  
#define COMMON_NIN_CONFIG_H_  

#include <string>
#include <map>
#include "common/Global.h"

class QSettings;

namespace zhongan {

#define DYN_CREATE(class_name) (class_name*)DynCreate::New(#class_name)

#define DECL_DYN_CREATE(class_name) \
public:\
	static void* NewObject() { return new class_name(); } \
	static StaticInit static_init_obj_;

#define IMP_DYN_CREATE(class_name) \
	StaticInit class_name::static_init_obj_(#class_name, &class_name::NewObject);

typedef void* (*pfNewObject)();

struct COMMON_API StaticInit
{
	StaticInit(const std::string& name, pfNewObject func);
};

class COMMON_API DynCreate
{
public:
	static void* New(const std::string& name);
private:
	friend struct StaticInit;
	static void Push(const std::string& name, pfNewObject func);
private:
	static std::map<std::string, pfNewObject> new_funcs_;
};

}
#endif

