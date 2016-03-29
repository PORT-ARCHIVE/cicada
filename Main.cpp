// © 2016 PORT INC.

#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "SemiCrf.hpp"
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include "FileIO.hpp"

class Options {
public:
	Options()
		: trainingDataFile("")
		, inferenceDataFile("")
		, format("digit")
		, weightsFile("")
		, initWeightsFile("")
		, maxLength(5)
		, maxIteration(1024)
		, e0(1.0e-5)
		, e1(1.0e-5)
		, logLevel(0)
		, flg(0)
		{};
	void parse(int argc, char *argv[]);
public:
	std::string trainingDataFile;
	std::string inferenceDataFile;
	std::string format;
	std::string weightsFile;
	std::string initWeightsFile;
	int maxLength;
	int maxIteration;
	double e0;
	double e1;
	int logLevel;
	int flg;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-t" ) {
				trainingDataFile = argv[++i];
			} else if( arg == "-i" ) {
				inferenceDataFile = argv[++i];
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
				weightsFile = argv[++i];
			} else if( arg == "-w0" ) {
				initWeightsFile = argv[++i];
			} else if( arg == "--enable-likelihood-only" ) {
				flg |= SemiCrf::ENABLE_LIKELIHOOD_ONLY;
			} else if( arg == "--disable-adagrad" ) {
				flg |= SemiCrf::DISABLE_ADAGRAD;
			} else if( arg == "--disable-date-version" ) {
				flg |= SemiCrf::DISABLE_DATE_VERSION;
			} else if( arg == "--log-level" ) {
				logLevel = boost::lexical_cast<int>(argv[++i]);
			} else {
				throw Error("unknown option specified");
			}
		}

		if( weightsFile.empty() ) {
			throw Error("no weights file specified");
		}

		if( trainingDataFile.empty() && inferenceDataFile.empty() ) {
			throw Error("neither training data file nor inference data file specified");
		}

		if( !trainingDataFile.empty() && !inferenceDataFile.empty() ) {
			throw Error("both training data file and inference data file specified");
		}

	} catch(Error& e) {

		throw e;

	} catch(...) {
		throw Error("invalid option specified");
	}
}

SemiCrf::Algorithm createAlgorithm(const Options& options)
{
	std::string file;
	SemiCrf::Algorithm alg;
	SemiCrf::Datas datas;

	if( !options.trainingDataFile.empty() ) {

		file = options.trainingDataFile;
		alg = SemiCrf::createLearner(options.flg);
		datas = SemiCrf::Datas( new SemiCrf::TrainingDatas_() );

	} else if( !options.inferenceDataFile.empty() ) {

		file = options.inferenceDataFile;
		alg = SemiCrf::createPredictor(options.flg);

		if( options.format == "digit" ) {

			datas = SemiCrf::Datas( new App::PridectionDigitDatas_() );

		} else if( options.format == "jpn" ) {

			datas = SemiCrf::Datas( new SemiCrf::PredictionDatas_() );

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

	std::ifstream ifs;
	open(ifs, file);

	try {
		datas->read(ifs);
	} catch(Error& e) {
		std::stringstream ss;
		ss << "failed to read " << file << ": " << e.what();
		throw Error(ss.str());
	}

	datas->write(Logger::out(2) << "");

	alg->setDatas(datas);
	alg->setMaxLength(options.maxLength);
	alg->setMaxIteration(options.maxIteration);
	alg->setE0(options.e0);
	alg->setE1(options.e1);

	return alg;
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	try {

		Options options;
		options.parse(argc, argv);
		Logger::setLevel(options.logLevel);

		SemiCrf::Algorithm alg = createAlgorithm(options);
		SemiCrf::Labels labels = App::createLabels();
		SemiCrf::FeatureFunction ff = App::createFeatureFunction();

		alg->setLabels(labels);
		alg->setFeatureFunction(ff);

		alg->preProcess(options.weightsFile, options.initWeightsFile);
		alg->compute();
		alg->postProcess(options.weightsFile);

	} catch(Error& e) {

		std::cerr << "error: " << e.what() << std::endl;
		ret = 0x1;

	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	exit(ret);
}
