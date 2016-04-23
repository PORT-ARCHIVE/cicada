// © 2016 PORT INC.

#ifndef LOGGER__H
#define LOGGER__H

#if 0
#include <iostream>
#include <fstream>

class Logger {
public:
	static Logger& out(int level = 0);
	static void setLevel(int level = 0);
	static int getLevel();
	template < class T >
	friend std::ostream& operator<<(Logger& logger, T& in);
private:
	static Logger* getLogger();
	std::ofstream& getDevNull() { return dev_null; }
	void setLLevel(int arg) { llevel = arg; }
	void setGLevel(int arg) { glevel = arg; }
	int getLLevel() { return llevel; }
	int getGLevel() { return glevel; }
private:
	Logger()
		: dev_null("/dev/null")
		, llevel(0)
		, glevel(0)
		{};
	std::ofstream dev_null;
	int llevel;
	int glevel;
};

template < class T >
std::ostream& operator<<(Logger& logger, T& in)
{
	int local = logger.getLLevel();
	int global = logger.getGLevel();

	if( local <= global ) {
		if( local <= 0 ) {
			std::cout << in;
			return std::cout;
		} if( 1 <= local ) {
			std::cerr << in;
			return std::cerr;
		}
	}

	return logger.getDevNull();
}

#else

#include "spdlog/spdlog.h"

namespace Logger
{
	namespace spd = spdlog;

	decltype ( spd::stderr_logger_mt("console", true) ) out();

	void setLevel(int level);

	decltype(( out()->trace() )) trace();
	decltype(( out()->debug() )) debug();
	decltype(( out()->info() )) info();
	decltype(( out()->notice() )) notice();
	decltype(( out()->warn() )) warn();
	decltype(( out()->error() )) error();
	decltype(( out()->critical() )) critical();
	decltype(( out()->alert() )) alert();

	decltype(( out()->trace() )) trace(const char* msg);
	decltype(( out()->debug() )) debug(const char* msg);
	decltype(( out()->info() )) info(const char* msg);
	decltype(( out()->notice() )) notice(const char* msg);
	decltype(( out()->warn() )) warn(const char* msg);
	decltype(( out()->error() )) error(const char* msg);
	decltype(( out()->critical() )) critical(const char* msg);
	decltype(( out()->alert() )) alert(const char* msg);
}

#endif

#endif // LOGGER__H
