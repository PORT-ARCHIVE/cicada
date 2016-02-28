
#include <iostream>
#include "SemiCrf.hpp"
#include "ApplicationRequirements.hpp"

int main(int argc, char *argv[])
{
	SemiCrf::Ffps ffps;

	try { // prepare feature functions

		// T.B.D.
		typedef SemiCrf::FeatureFunction SFF;	
		SFF ff0(new ApplicationRequirements::AppReqF0());
		SFF ff1(new ApplicationRequirements::AppReqF1());

		SemiCrf::Ffp ffp0 = std::make_pair(ff0, 0);
		SemiCrf::Ffp ffp1 = std::make_pair(ff1, 0);

		ffps.push_back(ffp0);
		ffps.push_back(ffp1);

	} catch(...) {
	}

	try { // Traing	
	
		SemiCrf::Data trainingData(new SemiCrf::TrainingData());
		trainingData->read();
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->compute(trainingData, ffps);

	} catch(...) {
	}

	try { // Inference

		SemiCrf::Data inferenceData(new SemiCrf::InferenceData());
		inferenceData->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->compute(ffps, inferenceData);
		inferenceData->write();

	} catch(...) {
	}
		
	return (0);
}
