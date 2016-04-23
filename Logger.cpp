// © 2016 PORT INC.

#if 0

#include <iostream>
#include <Logger.hpp>

static Logger* logger = nullptr;

Logger* Logger::getLogger()
{
	if( !logger ) {
		logger = new Logger();
	}

	return logger;
}

Logger& Logger::out(int level)
{
	Logger* l = getLogger();
	l->setLLevel(level);
	return *l;
}

void Logger::setLevel(int level)
{
	Logger* l = getLogger();
	l->setGLevel(level);
}

int Logger::getLevel()
{
	Logger* l = getLogger();
	return (l->getGLevel());
}

#else

#include <Logger.hpp>

namespace Logger
{
	namespace spd = spdlog;

	static decltype ( spd::stderr_logger_mt("console", true) ) lg;

	decltype ( spd::stderr_logger_mt("console", true) ) out()
	{
		if( !lg.get() ) {
			size_t q_size = 4096; // queue size must be power of 2
			spd::set_async_mode(q_size);
			lg = spd::stderr_logger_mt("console", true);
		}
		return lg;
	}

	void setLevel(int level)
	{
		spd::set_level((spd::level::level_enum)level);
	}

	decltype(( out()->trace() )) trace()
	{
		return out()->trace();
	}

	decltype(( out()->debug() )) debug()
	{
		return out()->debug();
	}

	decltype(( out()->info() )) info()
	{
		return out()->info();
	}

	decltype(( out()->notice() )) notice()
	{
		return out()->notice();
	}

	decltype(( out()->warn() )) warn()
	{
		return out()->warn();
	}

	decltype(( out()->error() )) error()
	{
		return out()->error();
	}

	decltype(( out()->critical() )) critical()
	{
		return out()->critical();
	}

	decltype(( out()->trace() )) trace(const char* msg)
	{
		return out()->trace(msg);
	}

	decltype(( out()->debug() )) debug(const char* msg)
	{
		return out()->debug(msg);
	}

	decltype(( out()->info() )) info(const char* msg)
	{
		return out()->info(msg);
	}

	decltype(( out()->notice() )) notice(const char* msg)
	{
		return out()->notice(msg);
	}

	decltype(( out()->warn() )) warn(const char* msg)
	{
		return out()->warn(msg);
	}

	decltype(( out()->error() )) error(const char* msg)
	{
		return out()->error(msg);
	}

	decltype(( out()->critical() )) critical(const char* msg)
	{
		return out()->critical(msg);
	}
}

#endif
