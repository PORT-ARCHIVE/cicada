
#include <iostream>
#include "SemiCrf.hpp"
#include "ApplicationRequirements.hpp"

int main(int argc, char *argv[])
{
	typedef SemiCrf::FeatureFunction SFF;
	
	SemiCrf::Ffps ffps;

	SFF ff0(new ApplicationRequirements::AppReqF0());
	SFF ff1(new ApplicationRequirements::AppReqF1());

	SemiCrf::Ffp ffp0 = std::make_pair(ff0, 0);
	SemiCrf::Ffp ffp1 = std::make_pair(ff1, 0);

	ffps.push_back(ffp0);
	ffps.push_back(ffp1);

	{ // Traing
		SemiCrf::Data trainingData(new SemiCrf::TrainingData());
		trainingData->read();
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->compute(trainingData, ffps);
	}

	{ // Inference
		SemiCrf::Data inferenceData(new SemiCrf::InferenceData());
		inferenceData->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->compute(ffps, inferenceData);
		inferenceData->write();
	}
		
	return (0);
}



