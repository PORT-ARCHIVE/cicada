
#include <iostream>
#include "SemiCrf.hpp"
#include "ApplicationRequirements.hpp"

int main(int argc, char *argv[])
{
	SemiCrf::FtrFnctnPrmtrs ffps;

	try { // 素性関数を準備する

		// T.B.D.
		typedef SemiCrf::FeatureFunction SFF;	
		SFF ff0(new ApplicationRequirements::AppReqF0());
		SFF ff1(new ApplicationRequirements::AppReqF1());

		SemiCrf::FtrFnctnPrmtr ffp0 = std::make_pair(0, ff0);
		SemiCrf::FtrFnctnPrmtr ffp1 = std::make_pair(0, ff1);

		ffps.push_back(ffp0);
		ffps.push_back(ffp1);
		// T.B.D.		

	} catch(...) {
		// T.B.D.
	}

	try { // 学習
	
		SemiCrf::Data trainingData(new SemiCrf::TrainingData());
		trainingData->read();
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->compute(trainingData, ffps);

		ffps.write();

	} catch(...) {
		// T.B.D.		
	}

	try { // 推論

		ffps.read();

		SemiCrf::Data inferenceData(new SemiCrf::InferenceData());
		inferenceData->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->compute(ffps, inferenceData);
		inferenceData->write();

	} catch(...) {
		// T.B.D.		
	}
		
	return (0);
}
