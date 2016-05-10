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

		auto datas = SemiCrf::createTrainingDatas();
		{
			std::ifstream ifs;
			open(ifs, options.predictionResultFile);
			Logger::info() << "parse " << options.predictionResultFile;

			datas->readJson(ifs);
			if( datas->empty() ) {
				Logger::warn() << "empty prediction";
			}
		}

		/////////////// labels

		std::vector<ujson::value> labelArray;
		std::map<int, std::string> labels;
		{
			std::ifstream ifb;
			open(ifb, options.labelTableFile);
			Logger::info() << "parse " << options.labelTableFile;

			auto v = JsonIO::parse(ifb);
			auto object = object_cast(std::move(v));
			labelArray = JsonIO::readUAry(object, "labels");

			for( auto& i : labelArray ) {

				if( !i.is_array() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto ary = array_cast(std::move(i));
				auto it = ary.begin();

				if( !it->is_string() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto label_id = boost::lexical_cast<int>(string_cast(std::move(*it)));

				++it;

				if( !it->is_string() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto key_jp = string_cast(std::move(*it));

				++it;

				if( !it->is_string() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto key_en = string_cast(std::move(*it));

				labels.insert( std::make_pair(label_id, key_en) );
			}
		}


		///////////////	data
		Logger::info("transform data...");

		ujson::array crf_estimate;

		for( auto& data : *datas ) {

			ujson::array array;
			auto strs = data->getStrs();

			for( auto& seg : *data->getSegments() ) {

				auto s = seg->getStart();
				auto e = seg->getEnd();
				auto label_id = seg->getLabel();
				auto label = labels[label_id];

				std::string word;
				for( int i = s; i <= e; i++ ) {
					word += strs->at(i).at(0);
				}

				array.push_back(label);
				array.push_back(word);
			}

			crf_estimate.push_back(std::move(array));
		}

		///////////////	output

		auto object = ujson::object {
			{ "title", std::move(datas->getTitle()) },
			{ "crf_estimate", std::move(crf_estimate) }
		};
		std::cout << to_string(object) << std::endl;

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
