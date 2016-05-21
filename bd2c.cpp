// © 2016 PORT INC.

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <boost/lexical_cast.hpp>
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "ujson.hpp"
#include "JsonIO.hpp"
#include "W2V.hpp"
#include "MultiByteTokenizer.hpp"

class Options {
public:
	Options()
		: bodyTextFile("")
		, w2vMatrixFile("")
		, labelTableFile("")
		, feature("JPN")
		, logLevel(2)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string bodyTextFile;
	std::string w2vMatrixFile;
	std::string labelTableFile;
	std::string feature;
	int logLevel;
	bool logColor;
	std::string logPattern;
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
			} else if( arg == "-l" ) {
				labelTableFile = argv[++i];
			} else if( arg == "-f" ) {
				feature = argv[++i];
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
	Logger::setName("bd2c");

	try {

		Options options;
		options.parse(argc, argv);

		Logger::setLevel(options.logLevel);

		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		Logger::info() << "bd2c 0.0.1";
		Logger::info() << "Copyright (C) 2016 PORT, Inc.";

		/////////////// labels

		std::vector<ujson::value> labelArray;
		{
			std::ifstream ifb;
			open(ifb, options.labelTableFile);
			Logger::info() << "parse " << options.labelTableFile;
			auto v = JsonIO::parse(ifb);
			auto object = object_cast(std::move(v));
			labelArray = JsonIO::readUAry(object, "labels");
		}

		/////////////// w2v

		W2V::Matrix matrix(new W2V::Matrix_());
		{
			if( options.w2vMatrixFile.empty() ) {
				throw Error("no w2v matrix file specifed");
			}
			Logger::info() << "parse " << options.w2vMatrixFile;
			matrix->read(options.w2vMatrixFile);
		}

		/////////////// bodies

		std::ifstream ifb;
		open(ifb, options.bodyTextFile);
		Logger::info() << "parse " << options.bodyTextFile;
		auto v = JsonIO::parse(ifb);

		if( !v.is_array() ) {
			std::stringstream ss;
			ss << options.bodyTextFile << ": ";
			ss << "top level is not an array";
			throw Error(ss.str());
		}

		ujson::array out_array;
		auto array = array_cast(std::move(v));

		for( auto& value : array ) {

			///////////////	body

			if( !value.is_object() ) {
				std::stringstream ss;
				ss << options.bodyTextFile << ": ";
				ss << "second level is not an object";
				throw Error(ss.str());
			}

			auto object = object_cast(std::move(value));
			auto title = JsonIO::readString(object, "title");
			auto body = JsonIO::readString(object, "body_text_split");
			if( body.empty() ) {
				Logger::warn() << title << ": empty body";
				continue;
			}

			///////////////	data

			Logger::info() << "transform " << title;

			setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.
			MultiByteTokenizer toknizer(body);
			toknizer.setSeparator(" ");
			toknizer.setSeparator("　");
			toknizer.setSeparator("\t");
			toknizer.setSeparator("\n"); // T.B.D.

			ujson::array data;
			ujson::array lines;
			std::string tok = toknizer.get();
			std::string tok0 = tok;
			std::string tok1 = tok;

			std::regex pattern("(diget_[0-9]+)\\\\\\:([0-9]+)");
			std::smatch results;
			if( std::regex_match( tok, results, pattern ) && results.size() == 3 ) {
				tok0 = results.position(1);
				tok1 = results.position(2);
			}

			while( !tok.empty() ) {

				ujson::array line;
				auto i = matrix->w2i(tok0);
				std::string ID = boost::lexical_cast<std::string>(i);
				line.push_back(ID);
				line.push_back("*");
				line.push_back("*");
				line.push_back(tok1);
				lines.push_back(std::move(line));
				if( tok == "。" ){ // T.B.D.
					data.push_back(std::move(lines));
					lines.clear();
				}

				tok = toknizer.get();
				tok0 = tok;
				tok1 = tok;
				if( std::regex_match( tok, results, pattern ) && results.size() == 3 ) {
					tok0 = results.position(1);
					tok1 = results.position(2);
				}
			}

			auto obj = ujson::object {
				{ "title", title },
				{ "data", std::move(data) }
			};

			out_array.push_back(std::move(obj));
		}

		///////////////	output
		{
			long long size = matrix->getSize();
			if( std::numeric_limits<int>::max() < size ) {
				throw Error("too large matrix");
			}
			int dim0 = size; // !!! long long -> int !!!
			int dim1 = labelArray.size();
			auto object = ujson::object {
				{ "feature", options.feature },
				{ "dimension", std::move(ujson::array{ dim0, dim1 }) },
				{ "labels", std::move(labelArray) },
				{ "data", out_array }
			};
			std::cout << to_string(object) << std::endl;
		}
	
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
