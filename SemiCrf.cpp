
#include "SemiCrf.hpp"
#include "AppReqs.hpp"

namespace SemiCrf {

	void TrainingData::read() {
		std::cout << "TrainingData::read()" << std::endl;

		// add strings
		// T.B.D.
		strs.push_back("AAA");
		strs.push_back("BBB");
		strs.push_back("CCC");
		strs.push_back("DDD");

		// add feature functions
		// T.B.D.		
		Segment s0(new Segment_( 0, 1, AppReqs::Label::Campany ));
		segs.push_back(s0);

		// add feature functions
		// T.B.D.				
		Segment s1(new Segment_( 2, 3, AppReqs::Label::Location ));
		segs.push_back(s1);		
	}

	void TrainingData::write() const {
		std::cout << "TrainingData::write()" << std::endl;
		// nothing to do
	}

	void InferenceData::read() {
		std::cout << "InferenceData::read()" << std::endl;

		// T.B.D.
		strs.push_back("AAA");
		strs.push_back("BBB");
		strs.push_back("CCC");
		strs.push_back("DDD");
	}

	void InferenceData::write() const {
		std::cout << "InferenceData::write()" << std::endl;

		// iterate segments
		for(auto i : segs){
			int start = i->getStart();
			int end = i->getEnd();
			AppReqs::Label l = i->getLabel();
			// T.B.D.			
		}

		// iterate strings
		for(auto i : strs){
			std::string& s = i;
			// T.B.D.
		}
	}

	void FtrFnctnPrmtrs::read() {
		// T.B.D.		
		// for( auto i : *this ) {
		// 	FeatureFunction f = i.first;
		// 	f->read();
		// 	i.second = 0; 
		// }
	}

	void FtrFnctnPrmtrs::write() {
		for( auto i : *this ) {
			FeatureFunction f = i.second;
			f->write();
			// i.second >> (stream)
		}		
	}	

	void Learner::compute(const Data data, FtrFnctnPrmtrs ffps) const {
		std::cout << "Learner::compute()" << std::endl;

		std::vector<Segment> segments = data->getSegments();
		std::vector<std::string> strs = data->getStrs();

		// iterate segments
		auto si = segments.begin();
		auto sj = segments.begin()++;		
		for( ; sj != segments.end(); si++, sj++ ){

			// iterate feature functions
			for(auto fp : ffps) {
				double p = fp.first;				
				FeatureFunction f = fp.second;
				bool v = (*f)(*si, *sj, std::move(strs));
				// T.B.D.
			}
		}
	}

	void Inferer::compute(const FtrFnctnPrmtrs ffps, Data data) const {
		std::cout << "Inferer::compute()" << std::endl;

		std::vector<std::string> strs = data->getStrs();
		std::vector<Segment> segments = data->getSegments();

		// iterate segments
		auto si = segments.begin();
		auto sj = segments.begin()++;
		for( ; sj != segments.end(); si++, sj++ ){

			// iterate feature functions
			for(auto fp : ffps) {
				double p = fp.first;				
				FeatureFunction f = fp.second;
				bool v = (*f)(*si, *sj, std::move(strs));
				// T.B.D.
			}
		}
	}

	double Inferer::V(int i, AppReqs::Label y) const {
		return (0.0);
	}
}
