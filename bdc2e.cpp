// © 2016 PORT INC.

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "ujson.hpp"
#include "JsonIO.hpp"

class Options {
public:
	Options()
		: accPredictionResultFile("")
		, bodyTextFile("")
		, logLevel(2)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string accPredictionResultFile;
	std::string bodyTextFile;
	int logLevel;
	bool logColor;
	std::string logPattern;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-c" ) {
				accPredictionResultFile = argv[++i];
			} else if( arg == "-b" ) {
				bodyTextFile = argv[++i];				
			} else if( arg == "--set-log-pattern" ) {
				logPattern = argv[++i];
			} else if( arg == "--disable-log-color" ) {
				logColor = false;
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
	Logger::setName("bdc2e");

	try {

		Options options;
		options.parse(argc, argv);

		Logger::setLevel(options.logLevel);

		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		Logger::info() << "bdc2e 0.0.1";
		Logger::info() << "Copyright (C) 2016 PORT, Inc.";

		///////////////	body		

		ujson::value body;
		{
			std::ifstream ifs;
			open(ifs, options.bodyTextFile);
			Logger::info() << "parse " << options.bodyTextFile;
			try {
				body = JsonIO::parse(ifs);
			} catch(...) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": cannot parse";
				throw Error(ss.str());
			}
			if( !body.is_object() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": top level is not an object";
				throw Error(ss.str());
			}
		}

		///////////////	accumulated prediction result

		ujson::value acc_prediction_result;
		{
			std::ifstream ifs;
			open(ifs, options.accPredictionResultFile);
			Logger::info() << "parse " << options.accPredictionResultFile;
			try {
				acc_prediction_result = JsonIO::parse(ifs);
			} catch(...) {
				std::stringstream ss;
				ss << options.accPredictionResultFile << ": cannot parse";
				throw Error(ss.str());
			}
			if( !acc_prediction_result.is_object() ) {
				std::stringstream ss;
				ss << options.accPredictionResultFile << ": top level is not an object";
				throw Error(ss.str());
			}			
		}

		///////////////	transform

		Logger::info("transform data...");

		auto ob = object_cast(std::move(body));
		auto oa = object_cast(std::move(acc_prediction_result));


		// check title
		{
			auto t0 = find(ob, "title");
			if( t0 == ob.end() || !t0->second.is_string() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": no title found";
				throw Error(ss.str());
			}

			auto t = t0->second; // copy to keep the original
			auto s0 = string_cast(std::move(t));

			auto t1 = find(oa, "title");
			if( t1 == ob.end() || !t1->second.is_string() ) {
				std::stringstream ss;
				ss << options.accPredictionResultFile << ": no title found";
				throw Error(ss.str());
			}

			auto s1 = string_cast(std::move(t1->second));

			if( s0 != s1 ) {
				std::stringstream ss;
				ss << "titles of objects are different";
				throw Error(ss.str());
			}
		}

		// replace crf_estimate
		{
			auto itb = find(ob, "crf_estimate");
			if( itb == ob.end() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": no crf_estimate found";
				throw Error(ss.str());
			}

			auto ita = find(oa, "crf_estimate");
			if( ita == oa.end() ) {
				std::stringstream ss;
				ss << options.accPredictionResultFile << ": no crf_estimate found";
				throw Error(ss.str());
			}

			*itb = *ita; // replace
		}

		// remove body_split_text
		{
			auto itb = find(ob, "body_split_text");
			if( itb == ob.end() || !itb->second.is_string() ) {
				Logger::warn() << options.bodyTextFile << ": no body_split_text found";
			}

			itb->second = std::string("");
		}

		std::cout << to_string(ob) << std::endl;

	} catch(Error& e) {

		Logger::error() << e.what();
		ret = 0x1;

	} catch(...) {

		Logger::error() << "unexpected exception";
		ret = 0x2;
	}

	if( !ret ) {
		Logger::info("OK");
	}

	exit(ret);
}
