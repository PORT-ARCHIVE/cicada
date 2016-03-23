// © 2016 PORT INC.

#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
//#include <boost/random.hpp>
#include <random>
#include "Logger.hpp"
#include "Error.hpp"

using namespace boost;
using namespace std;

vector<vector<double>> y2x =
{
	{ 0.3125, 0.0937, 0.2188, 0.3125, 0.0625 },
	{ 0.2206, 0.4412, 0.1176, 0.1471, 0.0735 }
};

vector<vector<double>> a_y2x;

vector<vector<double>> y2y =
{
	{ 0.7999, 0.1999 },
	{ 0.0999, 0.8999 }
};

vector<vector<double>> a_y2y;

void preProcess()
{
	for( int i = 0; i < y2x.size(); i++ ) {
		a_y2x.push_back(vector<double>(y2x.at(i).size()));
	}

	for( int i = 0; i < y2x.size(); i++ ) {
		for( int j = 0; j < y2x.at(i).size(); j++ ) {
			if( 0 < j ) {
				a_y2x.at(i).at(j) = a_y2x.at(i).at(j-1) + y2x.at(i).at(j);
			} else {
				a_y2x.at(i).at(j) = y2x.at(i).at(j);
			}
		}
	}

	for( int i = 0; i < y2y.size(); i++ ) {
		a_y2y.push_back(vector<double>(y2y.at(i).size()));
	}

	for( int i = 0; i < y2y.size(); i++ ) {
		for( int j = 0; j < y2y.at(i).size(); j++ ) {
			if( 0 < j ) {
				a_y2y.at(i).at(j) = a_y2y.at(i).at(j-1) + y2y.at(i).at(j);
			} else {
				a_y2y.at(i).at(j) = y2y.at(i).at(j);
			}
		}
	}	
}

int f_y2x(int y, double r)
{
	int i;
	assert ( 0 < a_y2x.size() );
	for( i = 0; i < a_y2x.at(y).size(); i++ ) {
		if( i == 0 && 0                          <= r && r < a_y2x.at(y).at(i) ) {
			break;
		} else if(  0 < i && a_y2x.at(y).at(i-1) <= r && r < a_y2x.at(y).at(i) ) {
			break;
		}
	}
	if( !(i < a_y2x.at(y).size()) ){
		Logger::out(1) << "warrning: r=" << r << ", ay2x(" << y << ")=" << a_y2x.at(y).at(i-1) << endl;
		--i;
	}
	return i;
}

int f_y2y(int y, double r)
{
	int i;
	assert ( 0 < a_y2y.size() );
	for( i = 0; i < a_y2y.at(y).size(); i++ ) {
		if( i == 0 && 0                          <= r && r < a_y2y.at(y).at(i) ) {
			break;
		} else if(  0 < i && a_y2y.at(y).at(i-1) <= r && r < a_y2y.at(y).at(i) ) {
			break;
		}
	}
	if( !(i < a_y2y.size()) ) {
		Logger::out(1) << "warrning: r=" << r << ", ay2y(" << y << ")=" << a_y2y.at(y).at(i-1) << endl;
		--i;
	}
	return i;
}

class Options {
public:
	Options()
		: length(16)
		, iteration(16)
		, seed(42)
		, logLevel(0) {};
	void parse(int argc, char *argv[]);
public:
	int length;
	int iteration;
	int seed;
	int logLevel;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			string arg = argv[i];
			if( arg == "-l" || arg == "--length") {
				length = lexical_cast<int>(argv[++i]);
			} else if( arg == "-r" || arg == "--iteration") {
				iteration = lexical_cast<int>(argv[++i]);
			} else if( arg == "-s" ) {
				seed = lexical_cast<int>(argv[++i]);
			} else if( arg == "--log-level" ) {
				logLevel = lexical_cast<int>(argv[++i]);
			} else {
				stringstream ss;
				ss << "unknown option specified";
				throw Error(ss.str());
			}
		}

	} catch(Error& e) {
		throw e;
	} catch(...) {
		stringstream ss;
		ss << "invalid option specified";
		throw Error(ss.str());
	}
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	try {

		Options options;
		options.parse(argc, argv);
		Logger::setLevel(options.logLevel);

		if( options.length < 3 ) {
			stringstream ss;
			ss << "length must be greater than 2";
			throw Error(ss.str());
		}

		preProcess();
	
		mt19937 mt(options.seed);
		uniform_real_distribution<double> score(0.0,1.0);

		for( int j = 0; j < options.iteration; j++ ) {

			Logger::out(0) << "# BEGIN" << endl;

			vector<int> y;
			vector<int> x;
			int py = 0;
			for( int i = 0; i < options.length; i++ ) {
				int yi = f_y2y(py, score(mt)); y.push_back(py);
				int xi = f_y2x(py, score(mt)); x.push_back(xi);
				py = yi;
			}

			auto xi = x.begin();
			auto yi = y.begin();
			auto yj = y.begin(); yj += 1;
			auto yk = y.begin(); yk += 2;

			Logger::out(0) << *xi << "\t";

			if( *yi == *yj ) {
				Logger::out(0) << "S\t";
			} else {
				Logger::out(0) << "S/E\t";
			}

			Logger::out(0) << *yi << endl;

			++xi;
			for( ; yk != y.end(); ++xi, ++yi, ++yj, ++yk ) {
				Logger::out(0) << *xi;
				if( *yi != *yj && *yj == *yk ) {
					Logger::out(0) << "\tS\t";
				} else if( *yi == *yj && *yj == *yk ) {
					Logger::out(0) << "\tM\t";
				} else if( *yi != *yj && *yj != *yk ) {
					Logger::out(0) << "\tS/E\t";
				} else if( *yi == *yj && *yj != *yk ) {
					Logger::out(0) << "\tE\t";
				} else {
					assert(0);
				}
				Logger::out(0) << *yj << endl;
			}

			Logger::out(0) << *xi;

			if( *yi != *yj ) {
				Logger::out(0) << "\tS/E\t";
			} else if( *yi == *yj ) {
				Logger::out(0) << "\tE\t";
			} else {
				assert(0);
			}

			Logger::out(0) << *yj << endl;
			Logger::out(0) << "# END" << endl;
		}

	} catch(Error& e) {

		cerr << "error: " << e.what() << endl;
		ret = 0x1;
		
	} catch(...) {

		cerr << "error: unexpected exception" << endl;
		ret = 0x2;
	}

	exit(ret);
}
