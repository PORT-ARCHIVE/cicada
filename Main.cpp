
#include <iostream>
#include <sstream>
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
		, debug(false) {};
public:
	std::string training_data_file;
	std::string inference_data_file;
	bool debug;
};

int main(int argc, char *argv[])
{
	int ret = 0x0;

	Options options;
	for( int i = 0; i < argc; i++ ) {
		std::string arg = argv[i];
		if( arg == "-t" ) {
			options.training_data_file = argv[++i];
		} else if( arg == "-i" ) {
			options.inference_data_file = argv[++i];
		} else if( arg == "--debug" ) {
			options.debug = true;
		}
	}

	if( options.debug ) {
		Debug::on();
	} else {
		Debug::off();
	}

	Debug::out() << "##### Start Semi-CRF ####" << std::endl;

	int maxLength = 5;
	SemiCrf::Weights weights = SemiCrf::createWeights();
	SemiCrf::FeatureFunctions ffs = SemiCrf::createFeatureFunctions();
	SemiCrf::Labels labels = SemiCrf::createLabels();

	try { // 素性関数,Labelを準備する

		// T.B.D.
		typedef SemiCrf::FeatureFunction SFF;	
		SFF ff0(new AppReqs::AppReqF0());
		SFF ff1(new AppReqs::AppReqF1());

		ffs->push_back(ff0);
		ffs->push_back(ff1);

		labels->push_back(AppReqs::Label::NONE);
		labels->push_back(AppReqs::Label::CAMPANY);
		labels->push_back(AppReqs::Label::LOCATION);
		// T.B.D.	

	} catch(...) {
		// T.B.D.
	}

	try { // 学習

		if( !options.training_data_file.empty() ) {

			ffs->read();
			weights->read();

			std::ifstream ifs;
			ifs.open( options.training_data_file.c_str() );
			if( ifs.fail() ) {
				std::stringstream ss;
				ss << "error: connot open such file: " << options.training_data_file;
				throw error(ss.str());
			}

			SemiCrf::Datas trainingDatas = SemiCrf::createTrainingDatas();
			trainingDatas->read(ifs);

			SemiCrf::Algorithm learner = SemiCrf::createLearner();
			learner->setLabels(labels);
			learner->setMaxLength(maxLength);
			learner->setDatas(trainingDatas);
			learner->setFeatureFunctions(ffs);
			learner->setWeights(weights);
			// learner->compute();

			ffs->write();
			weights->write();
		}

	} catch(error& e) {
		std::cerr << e.what() << std::endl;
		ret = 0x1;
	} catch(...) {
		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x2;
	}

	try { // 推論

		if( !options.inference_data_file.empty() ) {

			ffs->read();
			//weights->read(); T.B.D.

			std::ifstream ifs;
			ifs.open( options.inference_data_file.c_str() );
			if( ifs.fail() ) {
				std::stringstream ss;
				ss << "error: connot open such file: " << options.inference_data_file;
				throw error(ss.str());
			}

			SemiCrf::Datas inferenceDatas = SemiCrf::createInferenceDatas();
			inferenceDatas->read(ifs);

			SemiCrf::Algorithm inferer = SemiCrf::createInferer();
			inferer->setLabels(labels);
			inferer->setMaxLength(maxLength);
			inferer->setDatas(inferenceDatas);
			inferer->setFeatureFunctions(ffs);
			inferer->setWeights(weights);
			// inferer->compute();

			//inferenceData->write(); T.B.D.
		}

	} catch(error& e) {
		std::cerr << e.what() << std::endl;
		ret = 0x3;
	} catch(...) {
		std::cerr << "error: unexpected exception" << std::endl;
		ret = 0x4;
	}

	exit(ret);
}
