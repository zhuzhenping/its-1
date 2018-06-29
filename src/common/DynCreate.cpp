#include "common/DynCreate.h"

namespace zhongan {

std::map<std::string, pfNewObject> DynCreate::new_funcs_;

StaticInit::StaticInit(const std::string& name, pfNewObject func)
{
	DynCreate::Push(name, func);
}

void* DynCreate::New(const std::string& name)
{
	std::map<std::string, pfNewObject>::iterator it = new_funcs_.find(name);
	if (it == new_funcs_.end()) { return NULL; }
	return (*(it->second))();
}

void DynCreate::Push(const std::string& name, pfNewObject func)
{
	new_funcs_[name] = func;
}

}
