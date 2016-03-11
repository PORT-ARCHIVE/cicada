#ifndef DEBUG_OUT__H
#define DEBUG_OUT__H

#include <fstream>

class Debug {
public:
	static Debug& out();
	static void on();
	static void off();
	bool getFlg() { return flg; }
	std::ofstream& getDevNull() { return dev_null; }
private:
	Debug() : flg(false), dev_null("/dev/null") {};
	bool flg;
	std::ofstream dev_null;
};

template< class T >
std::ostream& operator<<(Debug& debug, T& in)
{
	if( debug.getFlg() ) {
		std::cout << in;
		return std::cout;
	} else {
		return debug.getDevNull();
	}
}

#endif // DEBUG_OUT__H
