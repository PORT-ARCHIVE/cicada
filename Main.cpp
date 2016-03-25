// © 2016 PORT INC.

#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "SemiCrf.hpp"
//#include "AppReqs.hpp"
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"

class Options {
public:
	Options()
		: training_data_file("")
		, inference_data_file("")
		, format("digit")
		, weights_file("weights.txt")
		, maxLength(5)
		, maxIteration(1024)
		, e0(1.0e-5)
		, e1(1.0e-5)
		, debug_level(0)
		, flg(0)
		{};
	void parse(int argc, char *argv[]);
public:
	std::string training_data_file;
	std::string inference_data_file;
	std::string format;
	std::string weights_file;
	int maxLength;
	int maxIteration;
	double e0;
	double e1;
	int debug_level;
	int flg;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-t" ) {
				training_data_file = argv[++i];
			} else if( arg == "-i" ) {
				inference_data_file = argv[++i];
			} else if( arg == "-f" || arg == "--input-file-format" ) {
				format = argv[++i];
			} else if( arg == "-l" || arg == "--max-length") {
				maxLength = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-r" || arg == "--max-iteration") {
				maxIteration = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-e0" ) {
				e0 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-e1" ) {
				e1 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-w" ) {
				weights_file = argv[++i];
			} else if( arg == "--disable-adagrad" ) {
				flg |= SemiCrf::DISABLE_ADAGRAD;
			} else if( arg == "--debug-level" ) {
				debug_level = boost::lexical_cast<int>(argv[++i]);
			} else {
				std::stringstream ss;
				ss << "unknown option specified";
				throw Error(ss.str());
			}
		}

	} catch(Error& e) {

		throw e;

	} catch(...) {

		std::stringstream ss;
		ss << "error: invalid option specified";
		throw Error(ss.str());
	}
}

void createAlgorithm(const Options& options, SemiCrf::Algorithm& algorithm, std::string& file)
{
	if( !options.training_data_file.empty() ) {

		file = options.training_data_file;
		algorithm = SemiCrf::createLearner();

	} else if( !options.inference_data_file.empty() ) {

		file = options.inference_data_file;
		algorithm = SemiCrf::createPridector();

	} else {
		std::stringstream ss;
		ss << "no input file specified";
		throw Error(ss.str());
	}
}

SemiCrf::Datas createDatas(const Options& options)
{
	if( !options.training_data_file.empty() ) {

		return SemiCrf::Datas( new SemiCrf::TrainingDatas_() );

	} else if( !options.inference_data_file.empty() ) {

		if( options.format == "digit" ) {

			return SemiCrf::Datas( new App::PridectionDigitDatas_() );

		} else if( options.format == "jpn" ) {

			return SemiCrf::Datas( new SemiCrf::PridectionDatas_() );

		} else {
			std::stringstream ss;
			ss << "unsupported format specified";
			throw Error(ss.str());
		}
	} else {
		std::stringstream ss;
		ss << "no input file specified";
		throw Error(ss.str());
	}
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	try {

		Options options;
		options.parse(argc, argv);
		Logger::setLevel(options.debug_level);

		Logger::out(1) << "#### Semi-CRF Start ####" << std::endl;

		std::string file;
		SemiCrf::Algorithm algorithm;
		createAlgorithm(options, algorithm, file);
		algorithm->setFlg(options.flg);

		std::ifstream ifs;
		SemiCrf::open(ifs, file);
		SemiCrf::Datas datas = createDatas(options);
		try {
			datas->read(ifs);
		} catch(Error& e) {
			std::stringstream ss;
			ss << "failed to read " << file << ": " << e.what();
			throw Error(ss.str());
		}
		datas->write(Logger::out(2) << "");
		algorithm->setDatas(datas);
		SemiCrf::Labels labels = App::createLabels();
		SemiCrf::FeatureFunction ff = App::createFeatureFunction();
		algorithm->setLabels(labels);
		algorithm->setFeatureFunction(ff);
		algorithm->preProcess(options.weights_file);
		algorithm->setMaxLength(options.maxLength);
		algorithm->setMaxIteration(options.maxIteration);
		algorithm->setE0(options.e0);
		algorithm->setE1(options.e1);
		algorithm->compute();
		algorithm->postProcess(options.weights_file);

		Logger::out(1) << "#### Semi-CRF nomarly end ####" << std::endl;

	} catch(Error& e) {

		std::cerr << "error: " << e.what() << std::endl;
		ret = 0x1;

	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	exit(ret);
}
