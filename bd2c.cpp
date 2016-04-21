// © 2016 PORT INC.

#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "ujson.hpp"
#include "W2V.hpp"

class Options {
public:
	Options()
		: bodyTextFile("")
		, w2vMatrixFile("")
		, logLevel(0)		  
		{};
	void parse(int argc, char *argv[]);
public:
	std::string bodyTextFile;
	std::string w2vMatrixFile;
	int logLevel;	
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-b" ) {
				bodyTextFile = argv[++i];
			} else if( arg == "-w" ) {
				w2vMatrixFile = argv[++i];
			} else if( arg == "--log-level" ) {
				logLevel = boost::lexical_cast<int>(argv[++i]);				
			} else {
				throw Error("unknown option specified");
			}
		}

	} catch(...) {
		throw Error("invalid option specified");
	}
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	try {

		Options options;
		options.parse(argc, argv);
		Logger::setLevel(options.logLevel);

		W2V::Matrix matrix(new W2V::Matrix_());
		if( options.w2vMatrixFile.empty() ) {
			throw Error("no w2v matrix file specifed");
		}
		matrix->read(options.w2vMatrixFile); // T.B.D.

		ujson::array data;		
		ujson::array lines;

		while( !std::cin.eof() ) {

			ujson::array line;
			std::string word;
			std::cin >> word;
			long long i = matrix->w2i(word);
			std::string ID = boost::lexical_cast<std::string>(i);
			line.push_back(ID);
			line.push_back("*");
			line.push_back("*");
			line.push_back(word);
			lines.push_back(std::move(line));
			if( word == "。" ){
				data.push_back(std::move(lines));
				lines.clear();
			}
		}

		auto object = ujson::object{ { "data", data } };
		Logger::out(0) << "" << to_string(object) << std::endl;
	
	} catch(Error& e) {

		std::cerr << "error: " << e.what() << std::endl;
		ret = 0x1;

	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	if( !ret ) {
		Logger::out(1) << "OK" << std::endl;
	}

	exit(ret);
}
