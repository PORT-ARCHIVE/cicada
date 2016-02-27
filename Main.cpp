
#include <iostream>
#include "SemiCrf.hpp"

int main(int argc, char *argv[])
{
	SemiCrf::Ffps ffps;

	{ // Traing
		SemiCrf::Data trainingData(new SemiCrf::TrainingData());
		trainingData->read();
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->computeFfps(ffps, trainingData);
	}

	{ // Inference
		SemiCrf::Data inferenceData(new SemiCrf::InferenceData());
		inferenceData->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->computeData(ffps, inferenceData);
		inferenceData->write();
	}
		
	return (0);
}


