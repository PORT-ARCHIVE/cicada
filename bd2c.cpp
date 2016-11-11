// © 2016 PORT INC.

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <deque>
#include <numeric>
#include <regex>
#include <boost/lexical_cast.hpp>
#include "mecab.h"
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
		, sentence_size(1024)
		, overlap_size(8)
		, normalize(true)
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
	int sentence_size;
	int overlap_size;
	bool normalize;
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
			} else if( arg == "--set-sentence-size" ) {
				sentence_size = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "--set-overlap-size" ) {
				overlap_size = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "--set-log-pattern" ) {
				logPattern = argv[++i];
			} else if( arg == "--disable-log-color" ) {
				logColor = false;
			} else if( arg == "--disable-normalize" ) {
				normalize = false;
			} else if( arg == "--log-level" ) {
				logLevel = boost::lexical_cast<int>(argv[++i]);
			} else {
				throw Error("unknown option specified");
			}
		}

	} catch(...) {
		throw Error("invalid option specified");
	}

	if( bodyTextFile.empty() ) {
		throw Error("no -b specified");
	}

	// if( w2vMatrixFile.empty() ) {
	// 	throw Error("no -m specified");
	// }

	if( labelTableFile.empty() ) {
		throw Error("no -l specified");
	}

	if( feature.empty() ) {
		throw Error("no -f specified");
	}
}

void replace_string(std::string& body, const std::string& from, const std::string& to)
{
	std::string::size_type pos = body.find(from);
	while(pos != std::string::npos){
		body.replace(pos, from.size(), to);
		pos = body.find(from, pos + to.size());
	}
}

// bodyを [max_splited_body_size]文字オーバーラップさせて [max_splited_body_size]byte 毎に分割する
void split_body(std::string& body, std::list<std::string>& bodies, int max_offset_size, int max_splited_body_size)
{
	const char* p = body.c_str();
	std::string splited_body;
	std::deque<int> offsets;

	setlocale(LC_CTYPE, "ja_JP.UTF-8");

	int i = 0;
	while( i < body.size() ) {

		int s = mblen(p, MB_CUR_MAX);

		offsets.push_back(s);
		if( max_offset_size < offsets.size() ) {
			offsets.pop_front();
		}

		int t = i;
		for( int j = 0; j < s; j++ ) {
			splited_body.push_back(body[t++]);
		}

		p += s;
		i += s;

		if( max_splited_body_size <= splited_body.size() ) {
			// std::cout << splited_body << std::endl;
			bodies.push_back(splited_body);
			splited_body.clear();
			int offset = std::accumulate(offsets.begin(), offsets.end(), 0);
			offsets.clear();
			p -= offset;
			i -= offset;
		}
	}

	if( !splited_body.empty() ) {
		bodies.push_back(splited_body);
	}
}

static std::map<std::string,std::string> zen2han_map =
{
	{"　"," "},
	{"！","!"},
	{"”","\""},
	{"＃","#"},
	{"＄","$"},
	{"％","%"},
	{"＆","&"},
	{"’","\""},
	{"（","("},
	{"）",")"},
	{"＊","*"},
	{"＋","+"},
	{"，",","},
	{"－","-"},
	{"．","."},
	{"／","/"},
	{"０","0"},
	{"１","1"},
	{"２","2"},
	{"３","3"},
	{"４","4"},
	{"５","5"},
	{"６","6"},
	{"７","7"},
	{"８","8"},
	{"９","9"},
	{"：",":"},
	{"；","},"},
	{"＜","<"},
	{"＝","="},
	{"＞",">"},
	{"？","?"},
	{"＠","@"},
	{"Ａ","A"},
	{"Ｂ","B"},
	{"Ｃ","C"},
	{"Ｄ","D"},
	{"Ｅ","E"},
	{"Ｆ","F"},
	{"Ｇ","G"},
	{"Ｈ","H"},
	{"Ｉ","I"},
	{"Ｊ","J"},
	{"Ｋ","K"},
	{"Ｌ","L"},
	{"Ｍ","M"},
	{"Ｎ","N"},
	{"Ｏ","O"},
	{"Ｐ","P"},
	{"Ｑ","Q"},
	{"Ｒ","R"},
	{"Ｓ","S"},
	{"Ｔ","T"},
	{"Ｕ","U"},
	{"Ｖ","V"},
	{"Ｗ","W"},
	{"Ｘ","X"},
	{"Ｙ","Y"},
	{"Ｚ","Z"},
	{"［","["},
	{"￥","\\"},
	{"］","]"},
	{"＾","^"},
	{"＿","_"},
	{"‘","`"},
	{"ａ","a"},
	{"ｂ","b"},
	{"ｃ","c"},
	{"ｄ","d"},
	{"ｅ","e"},
	{"ｆ","f"},
	{"ｇ","g"},
	{"ｈ","h"},
	{"ｉ","i"},
	{"ｊ","j"},
	{"ｋ","k"},
	{"ｌ","l"},
	{"ｍ","m"},
	{"ｎ","n"},
	{"ｏ","o"},
	{"ｐ","p"},
	{"ｑ","q"},
	{"ｒ","r"},
	{"ｓ","s"},
	{"ｔ","t"},
	{"ｕ","u"},
	{"ｖ","v"},
	{"ｗ","w"},
	{"ｘ","x"},
	{"ｙ","y"},
	{"ｚ","z"},
	{"｛","{"},
	{"｜","|"},
	{"｝","}"},
	{"〜","~"}
};

std::string zen2han(const std::string& body, bool flg)
{
	std::string result = body;
	if( flg ) {
		for( auto& kv : zen2han_map ) {
			std::string from = kv.first;
			std::string to = kv.second;
			replace_string(result, from, to);
		}
	}
	return std::move(result);
}

int main(int argc, char *argv[])
{
	int ret = 0x0;
	Options options;
	Logger::setName("bd2c");

	try {

		options.parse(argc, argv);

		Logger::setLevel(options.logLevel);

		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		Logger::info() << "bd2c 0.0.1";
		Logger::info() << "Copyright (C) 2016 PORT, Inc.";

		/////////////// mecab

		std::shared_ptr<MeCab::Tagger> tagger(MeCab::createTagger("-Owakati"));

		/////////////// labels

		std::vector<ujson::value> labelArray;
		{
			std::ifstream ifb;
			open(ifb, options.labelTableFile);
			Logger::info() << "parsing... " << options.labelTableFile;
			auto v = JsonIO::parse(ifb);
			auto object = object_cast(std::move(v));
			labelArray = JsonIO::readUAry(object, "labels");
		}

		/////////////// w2v

		W2V::Matrix matrix(new W2V::Matrix_());
		if( !options.w2vMatrixFile.empty() ) {

			matrix->read(options.w2vMatrixFile);

			const long long wth0 = 100000;
			if( matrix->getNumWords() < wth0 ) {
				std::stringstream ss;
				ss << "# of words in word2vec vector less than ";
				ss << wth0;
				Logger::out()->warn("{}: {}", options.w2vMatrixFile, ss.str());
			}

			const long long wth1 = 10000;
			if( matrix->getNumWords() < wth1 ) {
				std::stringstream ss;
				ss << options.w2vMatrixFile;
				ss << ": # of words in word2vec vector less than ";
				ss << wth1;
				throw Error(ss.str());
			}
		}

		/////////////// bodies

		std::ifstream ifb;
		open(ifb, options.bodyTextFile);
		Logger::info() << "parsing... " << options.bodyTextFile;
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
			std::string title;
			try {
				title = JsonIO::readString(object, "title");
				Logger::info() << "transforming... " << title;
			} catch(Error& e) {
				Logger::out()->warn("{}: {}", options.bodyTextFile, e.what());
			}
			auto body = JsonIO::readString(object, "body_text");
			if( body.empty() ) {
				Logger::out()->warn("{}: empty body", options.bodyTextFile);
			}

			///////////////	data

			ujson::array data;
			ujson::array lines;

			// body分割
			std::list<std::string> bodies;
			split_body(body, bodies, options.overlap_size, options.sentence_size);
			bodies.push_front(title);

			setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

			for( auto& sbd : bodies ) {

				Logger::out()->trace("sbd size = {}", sbd.size());
				Logger::out()->trace(sbd);

				std::string line(tagger->parse(sbd.c_str()));
				MultiByteTokenizer toknizer(line);
				toknizer.setSeparator(" ");
				toknizer.setSeparator("　");
				toknizer.setSeparator("\t");
				toknizer.setSeparator("\n");
				toknizer.setSeparator("\r");

				std::string tok = zen2han(toknizer.get(), options.normalize);
				std::string tok0 = tok;
#if 0
				std::regex pattern(R"(([0-9]+))");
				std::smatch results;
				if( std::regex_match( tok, results, pattern ) &&
					results.size() == 2 ) {
					tok0 = results[1];
					auto s = tok0.size();
					tok0 += "\\:";
					tok0 += boost::lexical_cast<std::string>(s);
					tok0 += "_digit";
				}
#endif
				while( !tok.empty() ) {

					ujson::array line;
					auto i = matrix->w2i(tok);
					std::string ID = boost::lexical_cast<std::string>(i);
					line.push_back(ID);
					line.push_back("*");
					line.push_back("*");
					line.push_back(tok0);
					lines.push_back(std::move(line));

					tok = zen2han(toknizer.get(), options.normalize);
					tok0 = tok;
#if 0
					//tok1 = tok;
					if( std::regex_match( tok, results, pattern ) &&
						results.size() == 2 ) {
						tok0 = results[1];
						auto s = tok0.size();
						tok0 += "\\:";
						tok0 += boost::lexical_cast<std::string>(s);
						tok0 += "_digit";
					}
#endif
				}

				data.push_back(std::move(lines));
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
				std::stringstream ss;
				ss << options.w2vMatrixFile	<< ": too large matrix";
				throw Error(ss.str());
			}
			int dim0 = size; // !!! long long -> int !!!
			int dim1 = labelArray.size();
			auto object = ujson::object {
				{ "feature", options.feature },
				{ "dimension", std::move(ujson::array{ dim0, dim1 }) },
				{ "labels", std::move(labelArray) },
				{ "pages", out_array }
			};
			std::cout << to_string(object) << std::endl;
		}
	
	} catch(Error& e) {

		Logger::out()->error("{}", e.what());
		ret = 0x1;

	} catch(std::exception& e) {

		Logger::out()->error("{}: {}", options.bodyTextFile, e.what());
		ret = 0x2;

	} catch(...) {

		Logger::out()->error("{}: unexpected exception", options.bodyTextFile);
		ret = 0x3;
	}

	if( !ret ) {
		Logger::info("OK");
	}

	exit(ret);
}
