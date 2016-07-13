// © 2016 PORT INC.

#ifndef LOGGER__H
#define LOGGER__H

#include "spdlog/spdlog.h"

namespace Logger
{
	namespace spd = spdlog;

	decltype ( spd::stderr_logger_mt("", true) ) out();

	void setLevel(int level);
	void setName(const std::string& name);
	void setColor(bool flg);
	void setPattern(const std::string& pattern);
	int getLevel();

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

#endif // LOGGER__H
