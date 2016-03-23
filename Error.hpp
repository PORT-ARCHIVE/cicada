// © 2016 PORT INC.

#ifndef ERROR__H
#define ERROR__H

#include <string>
#include "Logger.hpp"

class Error {
public:
	Error(const std::string& arg);
	virtual ~Error();
	std::string what();
private:
	std::string msg;
};

#endif // ERROR__H
