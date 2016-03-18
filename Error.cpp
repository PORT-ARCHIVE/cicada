// © 2016 PORT INC.

#include "Error.hpp"
#include "DebugOut.hpp"

Error::Error(const std::string& arg) :
	msg(arg)
{
	Debug::out() << "Error()" << std::endl;
}