
#include "SemiCrf.hpp"
#include "ApplicationRequirements.hpp"

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
		Label l0( new ApplicationRequirements::Campany() );
		Segment s0(new Segment_( 0, 1, l0 ));
		segs.push_back(s0);

		// add feature functions
		// T.B.D.				
		Label l1( new ApplicationRequirements::Location() );
		Segment s1(new Segment_( 2, 3, l1 ));
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
			Label l = i->getLabel();
			// T.B.D.			
		}

		// iterate strings
		for(auto i : strs){
			std::string& s = i;
			// T.B.D.
		}
	}

	void Learner::compute(const Data data, Ffps ffps){
		std::cout << "Learner::compute()" << std::endl;

		std::vector<Segment> segments = data->getSegments();
		std::vector<std::string> strs = data->getStrs();

		// iterate segments
		auto si = segments.begin();
		auto sj = si + 1;
		for( ; sj != segments.end(); si++, sj++ ){

			// iterate feature functions
			for(auto fp : ffps) {
				FeatureFunction f = fp.first;
				double p = fp.second;
				bool v = (*f)(*si, *sj, std::move(strs));
				// T.B.D.
			}
		}
	}

	void Inferer::compute(const Ffps ffps, Data data){
		std::cout << "Inferer::compute()" << std::endl;

		std::vector<Segment> segments = data->getSegments();
		std::vector<std::string> strs = data->getStrs();

		// iterate segments
		auto si = segments.begin();
		auto sj = si + 1;
		for( ; sj != segments.end(); si++, sj++ ){

			// iterate feature functions
			for(auto fp : ffps) {
				FeatureFunction f = fp.first;
				double p = fp.second;
				bool v = (*f)(*si, *sj, std::move(strs));
				// T.B.D.
			}
		}
	}
}
