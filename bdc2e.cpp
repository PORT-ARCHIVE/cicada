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
#include "SemiCrfData.hpp"


class Options {
public:
	Options()
		: predictionResultFile("")
		// : bodyTextFile("")
		// , w2vMatrixFile("")
		, labelTableFile("")
		// , feature("JPN")
		, logLevel(2)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string predictionResultFile;
	// std::string bodyTextFile;
	// std::string w2vMatrixFile;
	std::string labelTableFile;
	// std::string feature;
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
			// if( arg == "-b" ) {
			// 	bodyTextFile = argv[++i];
			// } else if( arg == "-w" ) {
			// 	w2vMatrixFile = argv[++i];
			} else if( arg == "-l" ) {
				labelTableFile = argv[++i];
			// } else if( arg == "-f" ) {
			// 	feature = argv[++i];
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

		Logger::info("bd2c 0.0.1");

		///////////////	prediction result

		auto data = SemiCrf::createTrainingDatas();

		std::string title;
		std::string prediction;
		{
			std::ifstream ifb;
			open(ifb, options.predictionResultFile);
			Logger::info() << "parse " << options.predictionResultFile;

			auto v = JsonIO::parse(ifb);
			auto object = object_cast(std::move(v));
			title = JsonIO::readString(object, "title");

			prediction = JsonIO::readString(object, "data");
			if( prediction.empty() ) {
				Logger::warn() << "empty prediction";
			}
		}

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


		///////////////	data
		Logger::info("transform data...");
#if 0
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
				{ "labels", std::move(labelArray) },
				{ "data", std::move(data) }
			};
			std::cout << to_string(object) << std::endl;
		}
#endif
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
