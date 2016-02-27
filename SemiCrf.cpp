
#include "SemiCrf.hpp"

namespace SemiCrf {

	void Learner::compute(const Data data, Ffps ffps){
		std::cout << "Learner::compute()" << std::endl;
		for(auto i : ffps) {
			FeatureFunction f = i.first;			
			double p = i.second;
			// T.B.D.
		}
	};

	void Inferer::compute(const Ffps ffps, Data data){
		std::cout << "Inferer::compute()" << std::endl;
		for(auto i : ffps) {
			FeatureFunction f = i.first;			
			double p = i.second;
			// T.B.D.			
		}		
	};	
	
}
