// © 2016 PORT INC.

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
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
		: training_data_file("")
		, inference_data_file("")
		, format("digit")
		, weights_file("weights.txt")
		, maxLength(5)
		, maxIteration(1024)
		, e0(1.0e-5)
		, e1(1.0e-5)
		, debug_level(0) {};
	void parse(int argc, char *argv[]);
public:
	std::string training_data_file;
	std::string inference_data_file;
	std::string format;
	std::string weights_file;
	int maxLength;
	int maxIteration;
	double e0;
	double e1;
	int debug_level;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-t" ) {
				training_data_file = argv[++i];
			} else if( arg == "-i" ) {
				inference_data_file = argv[++i];
			} else if( arg == "-f" || arg == "--input-file-format" ) {
				format = argv[++i];
			} else if( arg == "-l" || arg == "--max-length") {
				maxLength = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-r" || arg == "--max-iteration") {
				maxIteration = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-e0" ) {
				e0 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-e1" ) {
				e1 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-w" ) {
				weights_file = argv[++i];
			} else if( arg == "--debug-level" ) {
				debug_level = boost::lexical_cast<int>(argv[++i]);
			} else {
				std::stringstream ss;
				ss << "error: unknown option specified";
				throw Error(ss.str());
			}
		}

	} catch(Error& e) {

		throw e;

	} catch(...) {

		std::stringstream ss;
		ss << "error: invalid option specified";
		throw Error(ss.str());
	}
}

int main(int argc, char *argv[])
{
	preProcess();
	
	const int N = 100;
	int yi = 0; // T.B.D

	minstd_rand gen( 42 );
	uniform_real<> dst( 0, 1 );
	variate_generator<minstd_rand&, uniform_real<> > rand( gen, dst );

	std::cout << yi << "\t" << f_y2x(yi, rand()) << std::endl;
	
	for( int i = 1; i < N; i++ ) {
		
		yi = f_y2y(yi, rand());
		std::cout << yi << "\t" << f_y2x(yi, rand()) << std::endl;
	}

	return 0;
}
