#include "common/ExceptionErr.h"

namespace zhongan {

ExceptionErr& ExceptionErr::SourceInfo(const char *file, int line) 
{
	file_ = file;
	std::string::size_type pos = std::string::npos;
	if ((pos = file_.rfind('/')) != std::string::npos)
	{
		file_ = file_.substr(pos + 1);
	}
	if ((pos = file_.rfind('\\')) != std::string::npos)
	{
		file_ = file_.substr(pos + 1);
	}
	line_ = line;
	return *this;
}

const char * ExceptionErr::what() const throw()
{
	if (desc_ == "")
	{
		std::stringstream ss;
		ss << msg_;
		if (file_ != "") { ss << " (" << file_ << ":" << line_ << ")"; }
		desc_ = ss.str();
	}
	return desc_.c_str();
}

}