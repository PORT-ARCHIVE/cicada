// © 2016 PORT INC.

#include <iostream>
#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include "Logger.hpp"
#include "Error.hpp"

using namespace boost;

const double y2x[2][5] =
{
	{ 0.3125, 0.0937, 0.2188, 0.3125, 0.0625 },
	{ 0.2206, 0.4412, 0.1176, 0.1471, 0.0735 }
};

double a_y2x[2][4] =
{
	{ 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0 }
};

const double y2y[2][2] =
{
	{ 0.4, 0.05 },
	{ 0.1, 0.45 }
};

double a_y2y[2][1] =
{
	{0.0},
	{0.0}
};

void preProcess()
{
	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < 4; j++ ) {
			if( 0 < j ) {
				a_y2x[i][j] = a_y2x[i][j-1] + y2x[i][j];
			} else {
				a_y2x[i][j] = y2x[i][j];
			}
		}
	}

	for( int i = 0; i < 2; i++ ) {
		for( int j = 0; j < 2; j++ ) {
			if( 0 < j ) {
				a_y2y[i][j] = a_y2y[i][j-1] + y2y[i][j];
			} else {
				a_y2y[i][j] = y2y[i][j];
			}
		}
	}	
}

int f_y2x(int y, double r)
{
	int i;
	for( i = 0; i < 4; i++ ) {
		if(                  0             <= r && r < a_y2x[y][i] ) {
			break;
		} else if(  0 < i && a_y2x[y][i-1] <= r && r < a_y2x[y][i] ) {
			break;
		}
	}
	return i;
}

int f_y2y(int y, double r)
{
	int i;
	for( i = 0; i < 1; i++ ) {
		if(                  0             <= r && r < a_y2y[y][i] ) {
			break;
		} else if(  0 < i && a_y2y[y][i-1] <= r && r < a_y2y[y][i] ) {
			break;
		}
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
			std::string arg = argv[i];
			if( arg == "-l" || arg == "--length") {
				length = lexical_cast<int>(argv[++i]);
			} else if( arg == "-r" || arg == "--iteration") {
				iteration = lexical_cast<int>(argv[++i]);
			} else if( arg == "-s" ) {
				seed = lexical_cast<int>(argv[++i]);
			} else if( arg == "--log-level" ) {
				logLevel = lexical_cast<int>(argv[++i]);
			} else {
				std::stringstream ss;
				ss << "unknown option specified";
				throw Error(ss.str());
			}
		}

	} catch(Error& e) {
		throw e;
	} catch(...) {
		std::stringstream ss;
		ss << "invalid option specified";
		throw Error(ss.str());
	}
}

int main(int argc, char *argv[])
{
	int ret = 0x0;
	
	Options options;
	options.parse(argc, argv);
	Logger::setLevel(options.logLevel);

	if( options.length < 3 ) {
		std::stringstream ss;
		ss << "length must be greater than 2";
		throw Error(ss.str());
	}

	try {

		preProcess();
	
		minstd_rand gen( options.seed );
		uniform_real<> dst( 0, 1 );
		variate_generator<minstd_rand&, uniform_real<> > rand( gen, dst );

		for( int j = 0; j < options.iteration; j++ ) {

			Logger::out(0) << "# BEGIN" << std::endl;

			std::vector<int> y;
			std::vector<int> x;
			for( int i = 0; i < options.length; i++ ) {
				int yi = f_y2y(yi, rand()); y.push_back(yi);
				int xi = f_y2x(yi, rand());	x.push_back(xi);
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

			Logger::out(0) << *yi << std::endl;

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
				Logger::out(0) << *yj << std::endl;
			}

			Logger::out(0) << *xi;

			if( *yi != *yj ) {
				Logger::out(0) << "\tS/E\t";
			} else if( *yi == *yj ) {
				Logger::out(0) << "\tE\t";
			} else {
				assert(0);
			}

			Logger::out(0) << *yj << std::endl;
			Logger::out(0) << "# END" << std::endl;
		}

	} catch(Error& e) {

		std::cerr << "error: " << e.what() << std::endl;
		ret = 0x1;
		
	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	exit(ret);
}
