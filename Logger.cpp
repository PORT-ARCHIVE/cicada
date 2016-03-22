// © 2016 PORT INC.

#include <iostream>
#include <Logger.hpp>

static Logger* logger = nullptr;

Logger& Logger::out(int level)
{
	if( !logger ) {
		logger = new Logger();
	}

	logger->setLLevel(level);

	return *logger;
}

void Logger::setLevel(int level)
{
	if( !logger ) {
		logger = new Logger();
	}

	logger->setGLevel(level);
}

// void Logger::on()
// {
// 	out().flg = true;
// }

// void Logger::off()
// {
// 	out().flg = false;
// }
