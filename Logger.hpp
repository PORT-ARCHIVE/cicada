// © 2016 PORT INC.

#ifndef LOGGER_OUT__H
#define LOGGER_OUT__H

#include <iostream>
#include <fstream>

class Logger {
public:
	static Logger& out(int level = 0);
	static void setLevel(int level = 0);
	std::ofstream& getDevNull() { return dev_null; }
	void setLLevel(int arg) { llevel = arg; }
	void setGLevel(int arg) { glevel = arg; }
	int getLLevel() { return llevel; }
	int getGLevel() { return glevel; }
private:
	Logger() : dev_null("/dev/null") {};
	std::ofstream dev_null;
	int llevel;
	int glevel;
};

template< class T >
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

#endif // LOGGER_OUT__H
