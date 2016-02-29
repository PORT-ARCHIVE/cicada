
#include <limits>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"

namespace SemiCrf {

	void TrainingData::read() {
		std::cout << "TrainingData::read()" << std::endl;

		// add strings
		// T.B.D.
		strs->push_back("AAA");
		strs->push_back("BBB");
		strs->push_back("CCC");
		strs->push_back("DDD");

		// add feature functions
		// T.B.D.		
		Segment s0(new Segment_( 0, 1, AppReqs::Label::Campany ));
		segs->push_back(s0);

		// add feature functions
		// T.B.D.				
		Segment s1(new Segment_( 2, 3, AppReqs::Label::Location ));
		segs->push_back(s1);		
	}

	void TrainingData::write() const {
		std::cout << "TrainingData::write()" << std::endl;
		// nothing to do
	}

	void InferenceData::read() {
		std::cout << "InferenceData::read()" << std::endl;

		// T.B.D.
		strs->push_back("AAA");
		strs->push_back("BBB");
		strs->push_back("CCC");
		strs->push_back("DDD");
	}

	void InferenceData::write() const {
		std::cout << "InferenceData::write()" << std::endl;

		// iterate segments
		for(auto i : *segs){
			int start = i->getStart();
			int end = i->getEnd();
			AppReqs::Label l = i->getLabel();
			// T.B.D.			
		}

		// iterate strings
		for(auto i : *strs){
			std::string& s = i;
			// T.B.D.
		}
	}

	void FeatureFunctions_::read() {
		// T.B.D.		
		// for( auto i : *this ) {
		// 	FeatureFunction f = i.first;
		// 	f->read();
		// 	i.second = 0; 
		// }
	}

	void FeatureFunctions_::write() {
		for( auto f : *this ) {
			f->write();
		}		
	}	

	void Learner::compute() const {
		std::cout << "Learner::compute()" << std::endl;

		Segments segments = data->getSegments();
		Strs strs = data->getStrs();

		// iterate segments
		auto si = segments->begin();
		auto sj = segments->begin()++;		
		for( ; sj != segments->end(); si++, sj++ ){

			// iterate feature functions
			for(auto f : *ffs) {
				bool v = (*f)(*si, *sj, std::move(strs));
				// T.B.D.
			}
		}
	}

	void Inferer::compute() const {
		std::cout << "Inferer::compute()" << std::endl;

		int maxd = - 1;
		AppReqs::Label maxy;
		Segments maxsegs;
		int s = data->getStrs()->size();		
		double maxV = std::numeric_limits<double>::min();

		for( auto y : *labels ) {
			int d = -1;
			Segments segs;
			double v = V(s, y, segs, d);
			if( maxV < v ) {
				maxy = y;
				maxV = v;
				maxd = d;
				maxsegs = segs;
			}
		}

		Segment seg(new Segment_(s-maxd, s, maxy));
		maxsegs->push_back(seg);
		data->setSegments(maxsegs);
	}

	double Inferer::V(int i, AppReqs::Label y, Segments segs, int& maxd) const {

		maxd = -1;
		AppReqs::Label maxyd;
		double maxV = std::numeric_limits<double>::min();
		
		if( 0 < i ) {

			for( int d = 1; d <= maxLength; d++ ) {
				for( auto yd : *labels ) {
					double v = 0.0;

					int tmp = -1;
					v += V(i-d, yd, segs, tmp);

					auto w = weights->begin();
					for( auto f : *ffs ) {
						v += (*w) * (*f)(y, yd, data, i-d+1, i);
						w ++;
					}
					
					if( maxV < v ) {
						maxV = v;						
						maxd = d;
						maxyd = yd;
					}
				}
			}

			Segment seg(new Segment_(i-maxd+1, i, maxyd));
			segs->push_back(seg);

		} else if( i == 0 ) {
			maxV = 0.0;

		} else if( i < 0 ) {
			//  nothong to do;
		}

		return maxV;
	}
}
