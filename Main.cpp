
#include <iostream>
#include "SemiCrf.hpp"

int main(int argc, char *argv[])
{
	SemiCrf::Ffps ffps;

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



