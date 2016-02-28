
#include "SemiCrf.hpp"

namespace SemiCrf {

	void TrainingData::read() {
		std::cout << "TrainingData::read()" << std::endl;
	}

	void TrainingData::write() {
		std::cout << "TrainingData::write()" << std::endl;
	}

	void InferenceData::read() {
		std::cout << "InferenceData::read()" << std::endl;
	}

	void InferenceData::write() {
		std::cout << "InferenceData::write()" << std::endl;
	}		

	void Learner::compute(const Data data, Ffps ffps){
		std::cout << "Learner::compute()" << std::endl;
		for(auto i : ffps) {
			FeatureFunction f = i.first;
			double p = i.second;
			// T.B.D.
		}
	}

	void Inferer::compute(const Ffps ffps, Data data){
		std::cout << "Inferer::compute()" << std::endl;
		for(auto i : ffps) {
			FeatureFunction f = i.first;
			double p = i.second;
			// T.B.D. 
		}
	}
}
