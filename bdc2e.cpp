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

		Logger::info("bd2c 0.0.1");

		///////////////	body		

		// std::string title;
		// std::string body;
		{
			std::ifstream ifb;
			open(ifb, options.bodyTextFile);
			Logger::info() << "parse " << options.bodyTextFile;

			auto v = JsonIO::parse(ifb);
			if( !v.is_array() ) {
				throw std::invalid_argument("invalid data format");				
			}
			// auto array = array_cast(std::move(v));
			// title = JsonIO::readString(object, "title");
			// body = JsonIO::readString(object, "body_text_split");
			// if( body.empty() ) {
			// 	Logger::warn() << "empty body";
			// }
		}		
		
#if 0
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

		std::map<int, std::string> labels;
		{
			const auto& labelArray = datas->getLabels();

			for( auto& i : labelArray ) {

				if( !i.is_array() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto ary = array_cast(std::move(i));
				auto it = ary.begin();

				if( it == ary.end() || !it->is_string() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto label_id = boost::lexical_cast<int>(string_cast(std::move(*it)));

				++it;

				if( it == ary.end() || !it->is_string() ) {
					throw std::invalid_argument("invalid data format");
				}

				auto key_jp = string_cast(std::move(*it));

				++it;

				if( it == ary.end() ) {
					labels.insert( std::make_pair(label_id, key_jp) );
					continue;
				}

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
					word += strs->at(i).at(1);
				}

				if( label == "NONE" || label == "なし" ) {
					continue;
				}

				auto v = ujson::object { { std::move(label), std::move(word) } };
				array.push_back(v);
			}

			crf_estimate.push_back(std::move(array));
		}

		///////////////	output

		auto object = ujson::object {
			{ "title", std::move(datas->getTitle()) },
			{ "crf_estimate", std::move(crf_estimate) }
		};
		std::cout << to_string(object) << std::endl;
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
