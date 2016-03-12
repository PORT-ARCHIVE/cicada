
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"
#include "DebugOut.hpp"

class error {
public:
	error(std::string arg) : msg(arg) {};
	const std::string& what() { return msg; }
private:
	std::string msg;
};

class Options {
public:
	Options()
		: training_data_file("")
		, inference_data_file("")
		, maxLength(5)
		, debug(false) {};
	void parse(int argc, char *argv[]);
public:
	std::string training_data_file;
	std::string inference_data_file;
	int maxLength;
	bool debug;
};

void Options::parse(int argc, char *argv[])
{
	for( int i = 0; i < argc; i++ ) {
		std::string arg = argv[i];
		if( arg == "-t" ) {
			training_data_file = argv[++i];
		} else if( arg == "-i" ) {
			inference_data_file = argv[++i];
		} else if( arg == "-l" ) {
			maxLength = boost::lexical_cast<int>(argv[++i]);
		} else if( arg == "--debug" ) {
			debug = true;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0x0;

	Options options;
	options.parse(argc, argv);

	if( options.debug ) {
		Debug::on();
	} else {
		Debug::off();
	}

	Debug::out() << "##### Start Semi-CRF ####" << std::endl;

	try {

		SemiCrf::Weights weights = SemiCrf::createWeights();
		SemiCrf::FeatureFunctions ffs = AppReqs::createFeatureFunctions();
		SemiCrf::Labels labels = AppReqs::createLabels();

		std::string file;
		SemiCrf::Algorithm algorithm;
		if( !options.training_data_file.empty() ) {

			file = options.training_data_file;
			algorithm = SemiCrf::createLearner();

		} else if( !options.inference_data_file.empty() ) {

			file = options.inference_data_file;
			algorithm = SemiCrf::createInferer();
		}

		algorithm->setFeatureFunctions(ffs);
		algorithm->setWeights(weights);
		algorithm->preProcess();

		std::ifstream ifs;
		ifs.open( file.c_str() );
		if( ifs.fail() ) {
			std::stringstream ss;
			ss << "error: connot open such file: " << file;
			throw error(ss.str());
		}

		SemiCrf::Datas datas = algorithm->createDatas();
		datas->read(ifs);
		algorithm->setLabels(labels);
		algorithm->setMaxLength(options.maxLength);
		algorithm->setDatas(datas);
		// algorithm->compute();

		algorithm->postProcess();

	} catch(error& e) {
		std::cerr << e.what() << std::endl;
		ret = 0x1;
	} catch(...) {
		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	exit(ret);
}
