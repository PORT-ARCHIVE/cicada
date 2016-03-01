
#include <limits>
#include <assert.h>
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

	void Weights_::read() {
		std::cout << "Weights_::read()" << std::endl;
		push_back(1.0);
		push_back(2.0);
	}

	void Weights_::write() {
		std::cout << "Weights_::write()" << std::endl;
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

	void Learner::compute() {
		std::cout << "Learner::compute()" << std::endl;

		Segments segments = data->getSegments();
		Strs strs = data->getStrs();

		// iterate segments
		auto si = segments->begin();
		auto sj = segments->begin()++;		
		for( ; sj != segments->end(); si++, sj++ ){

			// iterate feature functions
			for(auto f : *ffs) {
				bool v = (*f)(*si, *sj, strs);
				// T.B.D.
			}
		}
	}

	void Inferer::compute() {
		std::cout << "Inferer::compute()" << std::endl;

		int capacity = (labels->size())*(data->getStrs()->size());
		std::vector<std::tuple<bool,double,int>> checkV(capacity, std::tuple<bool,double,int>());

		int maxd = - 1;
		Segments maxsegs( new Segments_() );
		AppReqs::Label maxy;
		int s = data->getStrs()->size();
		double maxV = std::numeric_limits<double>::min();

		for( auto y : *labels ) {
			int d = -1;
			Segments segs( new Segments_() );
			double v = V(s, y, segs, d, checkV);
			if( maxV < v ) {
				maxy = y;
				maxV = v;
				maxd = d;
				maxsegs = segs;
			}
		}

		assert( 0 < maxd );
		Segment seg(new Segment_(s-maxd+1, s, maxy));
		maxsegs->push_back(seg);
		data->setSegments(maxsegs);
	}

	double Inferer::V(int i, AppReqs::Label y, Segments segs, int& maxd, std::vector<std::tuple<bool,double,int>>& checkV) {

		int ind = (i*labels->size()) + (static_cast<int>(y));
		auto& tp = checkV[ind];
		if( std::get<0>(tp) ) {
			maxd = std::get<2>(tp);
			return std::get<1>(tp);
		}

		maxd = -1;
		AppReqs::Label maxyd;
		double maxV = std::numeric_limits<double>::min();
		
		if( 0 < i ) {

			for( int d = 1; d <= maxLength; d++ ) {
				for( auto yd : *labels ) {

					int tmp = -1;
					double v = V(i-d, yd, segs, tmp, checkV);

					auto w = weights->begin();
					assert( weights->size() == ffs->size() );
					for( auto f : *ffs ) {
						v += (*w) * (*f)(y, yd, data, i-d+1, i);
						w++;
					}
					
					if( maxV < v ) {
						maxV = v;						
						maxd = d;
						maxyd = yd;
					}
				}
			}

			assert( 0 < maxd );
			Segment seg(new Segment_(i-maxd+1, i, maxyd));
			segs->push_back(seg);

		} else if( i == 0 ) {
			maxV = 0.0;

		} else if( i < 0 ) {
			// nothong to do
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = maxV;
		std::get<2>(tp) = maxd;
		return maxV;
	}
}
