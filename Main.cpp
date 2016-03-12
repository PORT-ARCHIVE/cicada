
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"
#include "DebugOut.hpp"
#include "Error.hpp"

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

	try {

		Options options;
		options.parse(argc, argv);

		if( options.debug ) {
			Debug::on();
		} else {
			Debug::off();
		}

		std::string file;
		SemiCrf::Algorithm algorithm;
		if( !options.training_data_file.empty() ) {

			file = options.training_data_file;
			algorithm = SemiCrf::createLearner();

		} else if( !options.inference_data_file.empty() ) {

			file = options.inference_data_file;
			algorithm = SemiCrf::createInferer();

		} else {

			std::stringstream ss;
			ss << "error: no input file specified";
			throw Error(ss.str());
		}

		SemiCrf::FeatureFunctions ffs = AppReqs::createFeatureFunctions();
		algorithm->setFeatureFunctions(ffs);

		SemiCrf::Labels labels = AppReqs::createLabels();
		algorithm->setLabels(labels);

		SemiCrf::Weights weights = SemiCrf::createWeights();
		algorithm->setWeights(weights);

		algorithm->preProcess();

		std::ifstream ifs;
		ifs.open( file.c_str() );
		if( ifs.fail() ) {

			std::stringstream ss;
			ss << "error: connot open such file: " << file;
			throw Error(ss.str());
		}

		SemiCrf::Datas datas = algorithm->createDatas();
		datas->read(ifs);
		algorithm->setDatas(datas);

		algorithm->setMaxLength(options.maxLength);
		// algorithm->compute();

		algorithm->postProcess();

	} catch(Error& e) {

		std::cerr << e.what() << std::endl;
		ret = 0x1;

	} catch(...) {

		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	exit(ret);
}
