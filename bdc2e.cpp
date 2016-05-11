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

		ujson::value body;
		{
			std::ifstream ifs;
			open(ifs, options.bodyTextFile);
			Logger::info() << "parse " << options.bodyTextFile;

			body = JsonIO::parse(ifs);
			if( !body.is_array() ) {
				throw std::invalid_argument("invalid data format");				
			}
		}

		///////////////	accumulated prediction result

		ujson::value acc_prediction_result;
		{
			std::ifstream ifs;
			open(ifs, options.accPredictionResultFile);
			Logger::info() << "parse " << options.accPredictionResultFile;

			acc_prediction_result = JsonIO::parse(ifs);
			if( !acc_prediction_result.is_array() ) {
				throw std::invalid_argument("invalid data format");				
			}			
		}

		auto body_array = array_cast(std::move(body));
		auto ib = body_array.begin();

		auto acc_array = array_cast(std::move(acc_prediction_result));
		auto ia = acc_array.begin();

		ujson::array c;

		for( ; ib != body_array.end(); ib++, ia++ ) {

			if( !ib->is_object() ) {
				throw std::invalid_argument("invalid data format");				
			}

			if( !ia->is_object() ) {
				throw std::invalid_argument("invalid data format");				
			}

			auto ob = object_cast( std::move(*ib) );
			auto oa = object_cast( std::move(*ia) );

			auto itb = find(ob, "crf_estimate");
			if( itb == ob.end() || !itb->second.is_string() ) {
				throw std::invalid_argument("invalid data format");
			}

			auto ita = find(oa, "crf_estimate");
			if( ita == oa.end() || !itb->second.is_string() ) {
				throw std::invalid_argument("invalid data format");
			}

			*itb = *ita;
			c.push_back(ob);
		}

		std::cout << to_string(c) << std::endl;
		
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
