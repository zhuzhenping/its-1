#ifndef ITS_COMMON_EXCEPTION_ERR_H_
#define ITS_COMMON_EXCEPTION_ERR_H_

#include <string>
#include <sstream>
#include <exception>
#include "common/Global.h"

namespace itstation {

#define ThrowStringError(msg) throw ExceptionErr(msg).SourceInfo(__FILE__, __LINE__)
#define ThrowError(msg) do { std::stringstream ss; ss << msg; ThrowStringError(ss.str()); } while (0);

class COMMON_API ExceptionErr : public std::exception
{
public:
	ExceptionErr() : msg_(""), file_(""), line_(0), desc_("") {};
	ExceptionErr(const std::string& msg) : msg_(msg), file_(""), line_(0), desc_("") {};
	virtual ~ExceptionErr() throw() {}

	std::string Msg() const { return msg_; }

	ExceptionErr& SourceInfo(const char *file, int line);
	virtual const char * what() const throw();

private:
	std::string msg_;
	std::string file_;
	int line_;
	mutable std::string desc_;
};

}


#endif