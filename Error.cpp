// © 2016 PORT INC.

#include "Error.hpp"
#include "Logger.hpp"

Error::Error(const std::string& arg)
	: msg(arg)
{
	Logger::out(2) << "Error()" << std::endl;
}

Error::~Error()
{
}

std::string Error::what()
{
	return std::move(msg);
}
