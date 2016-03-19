// © 2016 PORT INC.

#ifndef DEBUG_OUT__H
#define DEBUG_OUT__H

#include <iostream>
#include <fstream>

class Debug {
public:
	static Debug& out(int level = 0);
	static void setLevel(int level = 0);
	std::ofstream& getDevNull() { return dev_null; }
	void setLLevel(int arg) { llevel = arg; }
	void setGLevel(int arg) { glevel = arg; }
	int getLLevel() { return llevel; }
	int getGLevel() { return glevel; }
private:
	Debug() : dev_null("/dev/null") {};
	std::ofstream dev_null;
	int llevel;
	int glevel;
};

template< class T >
std::ostream& operator<<(Debug& debug, T& in)
{
	int local = debug.getLLevel();
	int global = debug.getGLevel();
	if( local <= global ) {
		if( local <= 0 ) {
			std::cout << in;
			return std::cout;
		} if( 1 <= local ) {
			std::cerr << in;
			return std::cerr;
		}
	} else {
		return debug.getDevNull();
	}
}

#endif // DEBUG_OUT__H
