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

	try {

		Options options;
		options.parse(argc, argv);

		Logger::setLevel(options.logLevel);
		Logger::setName("bd2c");
		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		Logger::info("bd2c 0.0.1");

		///////////////	body

		std::string title;
		std::string body;
		{
			std::ifstream ifb;
			open(ifb, options.bodyTextFile);
			Logger::info() << "parse " << options.bodyTextFile;
			std::string jsonstr;
			jsonstr.assign((std::istreambuf_iterator<char>(ifb)), std::istreambuf_iterator<char>());
			ifb.close();

			auto v = ujson::parse(jsonstr);
			jsonstr.clear();
			if( !v.is_object() ) {
				throw std::invalid_argument("invalid JSON");
			}

			auto object = object_cast(std::move(v));
			title = JsonIO::readString(object, "title");
			body = JsonIO::readString(object, "body_text_split");
			if( body.empty() ) {
				Logger::warn() << "empty body";
			}
		}

		/////////////// labels

		std::vector<ujson::value> labelArray;
		{
			std::ifstream ifb;
			open(ifb, options.labelTableFile);
			Logger::info() << "parse " << options.labelTableFile;
			std::string jsonstr;
			jsonstr.assign((std::istreambuf_iterator<char>(ifb)), std::istreambuf_iterator<char>());
			ifb.close();

			auto v = ujson::parse(jsonstr);
			jsonstr.clear();
			if( !v.is_object() ) {
				throw std::invalid_argument("invalid JSON");
			}

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

		///////////////	data
		Logger::info("transform data...");

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.
		MultiByteTokenizer toknizer(body);
		toknizer.setSeparator(" ");
		toknizer.setSeparator("　");
		toknizer.setSeparator("\t");
		toknizer.setSeparator("\n"); // T.B.D.

		ujson::array data;
		ujson::array lines;
		std::string tok = toknizer.get();

		while( !tok.empty() ) {

			ujson::array line;
			auto i = matrix->w2i(tok);
			std::string ID = boost::lexical_cast<std::string>(i);
			line.push_back(ID);
			line.push_back("*");
			line.push_back("*");
			line.push_back(tok);
			lines.push_back(std::move(line));
			if( tok == "。" ){ // T.B.D.
				data.push_back(std::move(lines));
				lines.clear();
			}

			tok = toknizer.get();
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
				{ "title", title },
				{ "feature", options.feature },
				{ "dimension", std::move(ujson::array{ dim0, dim1 }) },
				{ "labels", labelArray },
				{ "data", data }
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
