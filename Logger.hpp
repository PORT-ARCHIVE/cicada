// © 2016 PORT INC.

#ifndef LOGGER__H
#define LOGGER__H

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

#endif // LOGGER__H
