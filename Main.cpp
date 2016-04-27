// © 2016 PORT INC.

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
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
		, predictionDataFile("")
		, weightsFile("")
		, initWeightsFile("")
		, w2vMatrixFile("")
		, maxLength(0)
		, maxIteration(1024)
		, e0(1.0e-5)
		, e1(1.0e-5)
		, rp(1.0e-7)
		, logLevel(2)
		, flg(0)
		, method("bfgs")
		, cacheSize(0xff)
		, logColor(true)
		, logPattern("")
		{};
	void parse(int argc, char *argv[]);
public:
	std::string trainingDataFile;
	std::string predictionDataFile;
	std::string weightsFile;
	std::string initWeightsFile;
	std::string w2vMatrixFile;
	int maxLength;
	int maxIteration;
	double e0;
	double e1;
	double rp;
	int logLevel;
	int flg;
	std::string method;
	int cacheSize;
	bool logColor;
	std::string logPattern;
};

void Options::parse(int argc, char *argv[])
{
	try {

		for( int i = 1; i < argc; i++ ) {
			std::string arg = argv[i];
			if( arg == "-t" ) {
				trainingDataFile = argv[++i];
			} else if( arg == "-i" ) {
				predictionDataFile = argv[++i];
			} else if( arg == "-l" || arg == "--max-length") {
				maxLength = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-r" || arg == "--max-iteration") {
				maxIteration = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "-e0" ) {
				e0 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-e1" ) {
				e1 = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-g" || arg == "--regularization-parameter") {
				rp = boost::lexical_cast<double>(argv[++i]);
			} else if( arg == "-w" ) {
				weightsFile = argv[++i];
			} else if( arg == "-w0" ) {
				initWeightsFile = argv[++i];
			} else if( arg == "-m" || arg == "--w2v-matrix" ) {
				w2vMatrixFile = argv[++i];
			} else if( arg == "--set-optimizer" ) {
				method = argv[++i];
			} else if( arg == "--set-log-pattern" ) {
				logPattern = argv[++i];
			} else if( arg == "--cache-size" ) {
				cacheSize = boost::lexical_cast<int>(argv[++i]);
			} else if( arg == "--enable-likelihood-only" ) {
				flg |= SemiCrf::ENABLE_LIKELIHOOD_ONLY;
			} else if( arg == "--disable-adagrad" ) {
				flg |= SemiCrf::DISABLE_ADAGRAD;
			} else if( arg == "--disable-date-version" ) {
				flg |= SemiCrf::DISABLE_DATE_VERSION;
			} else if( arg == "--disable-regularization" ) {
				flg |= SemiCrf::DISABLE_REGULARIZATION;
			} else if( arg == "--disable-wg-cache" ) {
				flg |= SemiCrf::DISABLE_WG_CACHE;
			} else if( arg == "--disable-log-color" ) {
				logColor = false;
			} else if( arg == "--log-level" ) {
				logLevel = boost::lexical_cast<int>(argv[++i]);
			} else {
				throw Error("unknown option specified");
			}
		}

		if( trainingDataFile.empty() && predictionDataFile.empty() ) {
			throw Error("neither training data file nor inference data file specified");
		}

		if( !trainingDataFile.empty() && !predictionDataFile.empty() ) {
			throw Error("both training data file and inference data file specified");
		}

		if( weightsFile.empty() && (
				(!(flg & SemiCrf::ENABLE_LIKELIHOOD_ONLY) && !(trainingDataFile.empty())) ||
				(!(predictionDataFile.empty()))) ) {
			throw Error("no weights file specified");
		}

	} catch(Error& e) {

		throw e;

	} catch(...) {
		throw Error("invalid option specified");
	}
}

decltype( std::make_shared<SemiCrf::Algorithm>() ) createAlgorithm(const Options& options)
{
	std::string file;
	decltype( std::make_shared<SemiCrf::Algorithm>() ) alg;
	SemiCrf::Datas datas;

	if( !options.trainingDataFile.empty() ) {

		file = options.trainingDataFile;
		alg = SemiCrf::createLearner(options.flg);
		datas = SemiCrf::Datas( new SemiCrf::TrainingDatas_() );

	} else if( !options.predictionDataFile.empty() ) {

		file = options.predictionDataFile;
		alg = SemiCrf::createPredictor(options.flg);
		datas = SemiCrf::Datas( new SemiCrf::PredictionDatas_() );

	} else {
		throw Error("no input file specified");
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

	// datas->write(std::cerr);

	alg->setDatas(datas);
	if( 0 < options.maxLength ) {
		alg->setMaxLength(options.maxLength);
	} else {
		alg->setMaxLength(datas->getMaxLength());
	}
	alg->setMaxIteration(options.maxIteration);
	alg->setE0(options.e0);
	alg->setE1(options.e1);
	alg->setRp(options.rp);
	alg->setMethod(options.method);
	alg->setCacheSize(options.cacheSize);

	return alg;
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	try {

		Options options;
		options.parse(argc, argv);
		Logger::setLevel(options.logLevel);
		Logger::setName("cicada");
		Logger::setColor(options.logColor);
		if( !options.logPattern.empty() ) {
			Logger::setPattern(options.logPattern);
		}

		auto alg = createAlgorithm(options);
		alg->preProcess(options.weightsFile, options.initWeightsFile, options.w2vMatrixFile);
		alg->compute();
		alg->postProcess(options.weightsFile);

	} catch(Error& e) {

		Logger::error() << e.what();
		ret = 0x1;

	} catch(std::exception& e) {

		Logger::error() << e.what();
		ret = 0x2;

	} catch(...) {

		Logger::error() << "unexpected exception";
		ret = 0x3;
	}

	if( !ret ) {
		Logger::info() << "OK";
	}

	exit(ret);
}
