// © 2016 PORT INC.

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <set>
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "ujson.hpp"
#include "JsonIO.hpp"

class Options {
public:
	Options()
		: file_name_0("")
		, file_name_1("")
		, item_name("")
		, logLevel(3)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string file_name_0;
	std::string file_name_1;
	std::string item_name;
	int logLevel;
	bool logColor;
	std::string logPattern;
};

void Options::parse(int argc, char *argv[])
{
	try {

		file_name_0 = argv[1];
		file_name_1 = argv[2];
		item_name   = argv[3];

		for( int i = 4; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "--set-log-pattern" ) {
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
	Logger::setName("jcmp");

	try {

		Options options;
		options.parse(argc, argv);

		Logger::setLevel(options.logLevel);

		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		Logger::info() << "jcmp 0.0.1";
		Logger::info() << "Copyright (C) 2016 PORT, Inc.";

		///////////////	body_0

		ujson::value body_0;
		{
			std::ifstream ifs;
			open(ifs, options.file_name_0);
			Logger::info() << "parsing... " << options.file_name_0;
			try {
				body_0 = JsonIO::parse(ifs);
			} catch(...) {
				std::stringstream ss;
				ss << options.file_name_0 << ": cannot parse";
				throw Error(ss.str());
			}
			if( !body_0.is_array() ) {
				std::stringstream ss;
				ss << options.file_name_0 << ": top level is not an array";
				throw Error(ss.str());
			}
		}

		///////////////	body_1

		ujson::value body_1;
		{
			std::ifstream ifs;
			open(ifs, options.file_name_1);
			Logger::info() << "parsing... " << options.file_name_1;
			try {
				body_1 = JsonIO::parse(ifs);
			} catch(...) {
				std::stringstream ss;
				ss << options.file_name_1 << ": cannot parse";
				throw Error(ss.str());
			}
			if( !body_1.is_array() ) {
				std::stringstream ss;
				ss << options.file_name_1 << ": top level not an array.";
				throw Error(ss.str());
			}			
		}

		///////////////	comppare

		Logger::info("comparing...");

		auto o0 = array_cast(std::move(body_0));
		auto o1 = array_cast(std::move(body_1));

		if( o0.size() != o1.size() ) {
			std::stringstream ss;
			ss << "size of data are different";
			throw Error(ss.str());
		}

		/////////////////////////////

		int c0 = 0, c1 = 0, c2 = 0, c3 = 0;

		std::vector<ujson::value> ary;

		auto i0 = o0.begin();
		auto i1 = o1.begin();
		for( int count = 0; i0 != o0.end(); ++count, ++i0, ++i1 ) {

			if( !i0->is_object() ) {
				std::stringstream ss;
				ss << options.file_name_0 << ": " << count <<  "th element not an object.";
				throw Error(ss.str());
			}
			auto object_0 = object_cast(std::move(*i0));

			if( !i1->is_object() ) {
				std::stringstream ss;
				ss << options.file_name_1 << ": " << count <<  "th element not an object.";
				throw Error(ss.str());
			}
			auto object_1 = object_cast(std::move(*i1));


			// check title
			{
				bool flg0 = true;
				auto t0 = find(object_0, options.item_name.c_str());
				if( t0 == object_0.end() || !t0->second.is_array() ) {
					std::stringstream ss;
					ss << options.file_name_0 << ": no title found in " << count << " th element";
					Logger::out()->warn("{}", ss.str());
					flg0 = false;
				}

				bool flg1 = true;
				auto t1 = find(object_1, options.item_name.c_str());
				if( t1 == object_1.end() || !t1->second.is_array() ) {
					std::stringstream ss;
					ss << options.file_name_1 << ": no title found in " << count << " th element";
					Logger::out()->warn("{}", ss.str());
					flg1 = false;
				}

				if( flg0 && flg1 ) {

					std::set<std::string> cset0;

					auto a0 = array_cast(std::move(t0->second));
					auto ai0 = a0.begin();
					for( ; ai0 != a0.end(); ai0++ ) {

						if( !ai0->is_string() ) {
							std::stringstream ss;
							ss << options.file_name_0 << "hoge"; // error
							throw Error(ss.str());
						}

						cset0.insert( string_cast(*ai0) );
					}


					std::set<std::string> cset1;

					auto a1 = array_cast(std::move(t1->second));
					auto ai1 = a1.begin();
					for( ; ai1 != a1.end(); ai1++ ) {

						if( !ai1->is_string() ) {
							std::stringstream ss;
							ss << options.file_name_0 << "fuga"; // error
							throw Error(ss.str());
						}

						cset1.insert( string_cast(*ai1) );
					}
					
					for( auto& s : cset0 ) {

						if( cset1.find(s) == cset1.end() ) {
							c0++;
							std::cout << s << " is missing in " << count << "th element." << std::endl;
						} else {
							c1++;
						}
					}

					for( auto& s : cset1 ) {

						if( cset0.find(s) == cset0.end() ) {
							c2++;
							std::cout << s << " is extra in " << count << "th element." << std::endl;
						} else {
							c3++;
						}
					}					
				}
			}
		}

		///// 結果表示

		double v0 = static_cast<double>(c1)/static_cast<double>(c0+c1)*1e2;
		double v1 = static_cast<double>(c3)/static_cast<double>(c2+c3)*1e2;
		std::cout << "item: " << options.item_name << std::endl;
		std::cout << boost::format("accuracy: %4.1f %%, recall: %4.1f %%") % v0 % v1 << std::endl;

	} catch(Error& e) {

		Logger::out()->error("{}", e.what());
		ret = 0x1;

	} catch(std::exception& e) {

		Logger::out()->error("{}", e.what());
		ret = 0x2;

	} catch(...) {

		Logger::out()->error("unexpected exception");
		ret = 0x3;
	}

	if( !ret ) {
		Logger::info("OK");
	}

	exit(ret);
}
