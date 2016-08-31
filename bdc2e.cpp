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
		: predictionResultFile("")
		, bodyTextFile("")
		, logLevel(2)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string predictionResultFile;
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
				predictionResultFile = argv[++i];
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
			if( !body.is_array() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": top level is not an array";
				throw Error(ss.str());
			}
		}

		///////////////	prediction result

		ujson::value prediction_result;
		{
			std::ifstream ifs;
			open(ifs, options.predictionResultFile);
			Logger::info() << "parse " << options.predictionResultFile;
			try {
				prediction_result = JsonIO::parse(ifs);
			} catch(...) {
				std::stringstream ss;
				ss << options.predictionResultFile << ": cannot parse";
				throw Error(ss.str());
			}
			if( !prediction_result.is_array() ) {
				std::stringstream ss;
				ss << options.predictionResultFile << ": top level is not an array";
				throw Error(ss.str());
			}			
		}

		///////////////	transform

		Logger::info("transform data...");

		auto ob = array_cast(std::move(body));
		auto oa = array_cast(std::move(prediction_result));

		if( ob.size() != oa.size() ) {
			std::stringstream ss;
			ss << "size of data are different";
			throw Error(ss.str());
		}

		std::vector<ujson::value> ary;

		auto ib = ob.begin();
		auto ia = oa.begin();
		for( int count = 0; ib != ob.end(); ++count, ++ib, ++ia ) {

			if( !ib->is_object() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": " << count <<  "th element not an object";
				throw Error(ss.str());
			}
			auto body_object = object_cast(std::move(*ib));

			if( !ia->is_object() ) {
				std::stringstream ss;
				ss << options.predictionResultFile << ": " << count <<  "th element not an object";
				throw Error(ss.str());
			}
			auto prediction_object = object_cast(std::move(*ia));


			// check title
			{
				bool flg0 = true;
				auto t0 = find(body_object, "title");
				if( t0 == body_object.end() || !t0->second.is_string() ) {
					std::stringstream ss;
					ss << options.bodyTextFile << ": no title found in " << count << " th element";
					Logger::out()->warn("{}", ss.str());
					flg0 = false;
				}

				bool flg1 = true;
				auto t1 = find(prediction_object, "title");
				if( t1 == prediction_object.end() || !t1->second.is_string() ) {
					std::stringstream ss;
					ss << options.predictionResultFile << ": no title found in " << count << " th element";
					Logger::out()->warn("{}", ss.str());
					flg1 = false;
				}

				if( flg0 && flg1 ) {

					auto tmp = t0->second; // copy to keep the original
					auto s0 = string_cast(std::move(tmp));
					auto s1 = string_cast(std::move(t1->second));

					if( s0 != s1 ) {
						std::stringstream ss;
						ss << "titles of objects are different in " << count << " th element";
						throw Error(ss.str());
					}
				}
			}

			// replace crf_estimate
			{
				auto itb = find(body_object, "crf_estimate");
				if( itb == body_object.end() ) {
					std::stringstream ss;
					ss << options.bodyTextFile << ": no crf_estimate found " << count << " th element";
					throw Error(ss.str());
				}

				auto ita = find(prediction_object, "crf_estimate");
				if( ita == prediction_object.end() ) {
					std::stringstream ss;
					ss << options.predictionResultFile << ": no crf_estimate found " << count << " th element";
					throw Error(ss.str());
				}

				*itb = *ita; // replace
			}

#if 0 // 分かち書き本文も残す
			// remove body_split_text
			{
				auto itb = find(body_object, "body_split_text");
				if( itb == body_object.end() || !itb->second.is_string() ) {
					Logger::warn() << options.bodyTextFile << ": no body_split_text found " << count << " th element";
				}
				itb->second = std::string("");
			}
#endif
			ary.push_back(body_object);
		}

		std::cout << to_string(ary) << std::endl;

	} catch(Error& e) {

		Logger::error() << e.what();
		ret = 0x1;

	} catch(std::bad_alloc) {

		Logger::error() << "memory exhausted";
		ret = 0x2;

	} catch(...) {

		Logger::error() << "unexpected exception";
		ret = 0x3;
	}

	if( !ret ) {
		Logger::info("OK");
	}

	exit(ret);
}
