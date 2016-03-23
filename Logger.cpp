// © 2016 PORT INC.

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
