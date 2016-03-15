// Copyright:: Copyright 2016 PORT INC.
// Author:: Jiro Iwai

#ifndef ERROR__H
#define ERROR__H

#include <string>

class Error {
public:
	Error(const std::string& arg) : msg(arg) {}
	const std::string& what() { return msg; }
private:
	std::string msg;
};

#endif // ERROR__H
