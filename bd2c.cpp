// © 2016 PORT INC.

#include <iostream>
#include <boost/lexical_cast.hpp>
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "ujson.hpp"
#include "JsonIO.hpp"
#include "MultiByteTokenizer.hpp"

class Options {
public:
	Options()
		: bodyTextFile("")
		, w2vMatrixFile("")
		, logLevel(0)		  
		{};
	void parse(int argc, char *argv[]);
public:
	std::string bodyTextFile;
	std::string w2vMatrixFile;
	int logLevel;	
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

		std::ifstream ifb;
		open(ifb, options.bodyTextFile);

		std::string jsonstr;
		jsonstr.assign((std::istreambuf_iterator<char>(ifb)), std::istreambuf_iterator<char>());
		ifb.close();

		auto v = ujson::parse(jsonstr);
		if( !v.is_object() ) {
			throw std::invalid_argument("object expected for Datas_"); // T.B.D
		}

		JsonIO::Object object = object_cast(std::move(v));
		std::string bdtxt = JsonIO::readString(object, "body_text_split");

		Logger::out(1) << "" << bdtxt << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D
		MultiByteTokenizer tokenizer(bdtxt);
		tokenizer.setSeparator("\t");
		tokenizer.setSeparator(" ");
		tokenizer.setSeparator("　");

		std::string tok = tokenizer.get();
		Logger::out(1) << "" << tok << std::endl;
		while( tok == "。" ||
			   tok == "。" ||
			   tok == "\n" ) {
			tok = tokenizer.get();
			Logger::out(1) << "" << tok << std::endl;
		}

		std::vector<std::vector<std::string>> data;

		while( tok != "\0" ) {

			std::vector<std::string> sentence;
			while( tok != "。" &&
				   tok != "\n" &&
				   tok != "\0" ) {
				   // tok != "、" &&
				   // tok != ","  &&

				sentence.push_back(tok);
				tok = tokenizer.get();
				Logger::out(1) << "" << tok << std::endl;
			}

			data.push_back(std::move(sentence));
			tok = tokenizer.get();
			Logger::out(1) << "" << tok << std::endl;
		}

	} catch(Error& e) {

		std::cerr << "error: " << e.what() << std::endl;
		ret = 0x1;

	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	if( !ret ) {
		Logger::out(1) << "OK" << std::endl;
	}

	exit(ret);
}
