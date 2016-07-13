// © 2016 PORT INC.

#include "Logger.hpp"

namespace Logger
{
	namespace spd = spdlog;

	static decltype ( spd::stderr_logger_mt("", true) ) lg;
	static std::string name = "console";
	static bool flg = true;
	static int lvl;

	decltype ( spd::stderr_logger_mt("", true) ) out()
	{
		if( !lg.get() ) {
			size_t q_size = 4096; // queue size must be power of 2
			spd::set_async_mode(q_size);
			lg = spd::stderr_logger_mt(name, flg);
		}
		return lg;
	}

	void setLevel(int level)
	{
		spd::set_level((spd::level::level_enum)level);
		lvl = level;
	}

	int getLevel()
	{
		return lvl;
	}

	void setName(const std::string& arg)
	{
		name = arg;
	}

	void setColor(bool arg)
	{
		flg = arg;
	}

	void setPattern(const std::string& arg)
	{
		spd::set_pattern(arg);
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
