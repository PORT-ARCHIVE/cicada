
#include <limits>
#include <cassert>
#include <cmath>
#include <mecab.h>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"

namespace SemiCrf {

	// ctr
	Weights createWeights() { return Weights( new Weights_() ); }
	Labels	createLabels() { return Labels( new Labels_() ); }
	FeatureFunctions createFeatureFunctions() { return FeatureFunctions( new FeatureFunctions_() ); }
	Segment createSegment(int start, int end, AppReqs::Label label) { return Segment( new Segment_(start, end, label) ); }
	Segments createSegments() { return Segments( new Segments_() ); }
	CheckTable createCheckTable(int capacity) { return CheckTable( new CheckTable_(capacity, CheckTuple()) ); }

	void Data_::read() {
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

	void Data_::write() const {
		std::cout << "InferenceData::write()" << std::endl;

		// iterate segments
		for( auto i : *segs ){
			int start = i->getStart();
			int end = i->getEnd();
			AppReqs::Label l = i->getLabel();
			// T.B.D.			
		}

		// iterate strings
		for( auto i : *strs ){
			std::string& s = i;
			// T.B.D.
		}
	}

	std::string&& nfind(const std::string& str0, const std::string& str1, int n) {

		int match = 0;
		size_t pos0 = 0;
		do {
			pos0 = str0.find(str1, pos0);
			pos0++;
			match++;
		} while( match != n && pos0 != std::string::npos );

		size_t pos1 = str0.find(str1, pos0);
		std::string m1 = str0.substr(pos0, pos1-pos0);
		return std::move(m1);
	}

	void Datas_::read() {
		std::cout << "Datas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		const char input[] = u8"これが機械学習エンジニアの募集要項です。";
		Data data( new Data_() );
		std::string delimita(",");
		typedef std::shared_ptr<MeCab::Tagger> MeCabTagger;
		MeCabTagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger(""));
		const MeCab::Node* node = tagger->parseToNode(input);

		for( ; node ; node = node->next ) {

			std::string cppstr = node->feature;

			if( cppstr == "\n" ) {
				this->push_back(data);
				data = Data( new Data_() );
			}

			std::cout << cppstr << std::endl;
			std::string&& m = nfind(cppstr, delimita, 6);
			std::cout << m << std::endl;
			data->getStrs()->push_back(m);
		}
	}

	void Datas_::write() const {
		std::cout << "Datas_::write()" << std::endl;
		for( auto i : *this ) {
			i->write();
		}
	}

	Datas createDatas() { return Datas( new Datas_() ); }

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

	//// Algorithm ////

	double Algorithm_::computeWG(AppReqs::Label y, AppReqs::Label yd, int i, int d)
	{
		double v = 0.0;
		auto w = weights->begin();

		for( auto g : *ffs ) {
			v += (*w) * (*g)(y, yd, current_data, i-d+1, i);
			w++;
		}

		return v;
	}

	//// Learner ////

	Algorithm createLearner() { return std::shared_ptr<Learner>(new Learner()); }

	void Learner::compute() {

		std::cout << "Learner::compute()" << std::endl;
		assert( weights->size() == ffs->size() );
		int l = labels->size();
		std::vector<double> dW(datas->size());
		auto pdW = dW.begin();

		for( auto data : *datas ) {

			current_data = data;
			int s = current_data->getStrs()->size();
			int capacity = l*s;
			current_actab = createCheckTable(capacity);
			current_ectab = createCheckTable(capacity);

			double Z = computeZ();
			auto&& Gs = computeG();
			auto&& Gms = computeGm(Z);

			for( int k = 0; k < ffs->size(); k++ ) {
				(*pdW) += Gs[k] - Gms[k];
			}

			pdW++;
		}
	}

	std::vector<double>&& Learner::computeG() {

		std::vector<double> Gs;
		auto segments = current_data->getSegments();

		for( auto g : *ffs ) {

			double G = 0.0;
			auto sj = segments->begin();
			auto si = segments->begin(); si++;

			for( ; si != segments->end(); si++, sj++ ){

				auto y = (*si)->getLabel();
				auto y1 = (*sj)->getLabel();
				int ti = (*si)->getStart();
				int ui = (*si)->getEnd();
				G += (*g)(y, y1, current_data, ti, ui);
			}

			Gs.push_back(G);
		}

		return(std::move(Gs));
	}

	double Learner::computeZ() {

		double Z = 0;
		int size = current_data->getStrs()->size();

		for( auto y : *labels ) {
			Z += alpha(size-1, y);
		}

		return Z;
	}

	std::vector<double>&& Learner::computeGm(double Z) {

		std::vector<double> Gms;
		int size = current_data->getStrs()->size();
		double Gm = 0.0;

		for( int k = 0; k < ffs->size(); k++ ) {
			for( auto y : *labels ) {
				Gm += eta(size-1, y, k);
			}

			Gms.push_back(Gm/Z);
		}

		return(std::move(Gms));
	}

	double Learner::alpha(int i, AppReqs::Label y) {

		std::cout << "i=" << i << ", y=" << int(y) << std::endl;
		int idx = (i*labels->size()) + (static_cast<int>(y));

		auto& tp = current_actab->at(idx);
		if( std::get<0>(tp) ) {
			return std::get<1>(tp);
		}

		double v = 0;

		if( 1 < i ) {

			for( int d = 1; d <= std::min(maxLength, i); d++ ) {
				for( auto yd : *labels ) {

					double e0 = alpha(i-d, yd);
					double e1 = computeWG(y, yd, i, d);
					v += e0*exp(e1);
				}
			}

		} else if( i == 1 ) {

			for( auto yd : *labels ) {

				double e = computeWG(y, yd, 1, 1);
				v = exp(e);
			}

		} else {
			assert( 0 <= i );
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = v;

		return v;
	}

	double Learner::eta(int i, AppReqs::Label y, int k) {

		std::cout << "i=" << i << ", y=" << int(y) << std::endl;
		int idx = (i*labels->size()) + (static_cast<int>(y));

		auto& tp = current_ectab->at(idx);
		if( std::get<0>(tp) ) {
			return std::get<1>(tp);
		}

		double v = 0;

		if( 1 < i ) {

			for( int d = 1; d <= std::min(maxLength, i); d++ ) {
				for( auto yd : *labels ) {

					FeatureFunction_& gk = *(ffs->at(k));
					double e0 = eta(i-d, yd, k) + alpha(i-d, yd) * gk(y, yd, current_data, i-d+1, i);
					double e1 = computeWG(y, yd, i, d);
					v += e0*exp(e1);
				}
			}

		} else if( i == 1 ) {

			for( auto yd : *labels ) {

				FeatureFunction_& gk = *(ffs->at(k));
				double e0 = gk(y, yd, current_data, 1, 1);
				double e1 = computeWG(y, yd, 1, 1);
				v = e0*exp(e1);
			}

		} else {
			assert( 0 <= i );
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = v;

		return v;
	}

	//// Inferer ////

	Algorithm createInferer() { return std::shared_ptr<Inferer>(new Inferer()); }

	void Inferer::compute() {

		std::cout << "Inferer::compute()" << std::endl;
		assert( weights->size() == ffs->size() );

		for( auto data : *datas ) {

			current_data = data;

			int l = labels->size();
			int s = current_data->getStrs()->size();
			int capacity = l*s;

			current_vctab = createCheckTable(capacity);

			int maxd = - 1;
			AppReqs::Label maxy;
			Segments maxsegs = createSegments();
			double maxV = std::numeric_limits<double>::min();

			for( auto y : *labels ) {

				int d = -1;
				Segments segs = createSegments();
				double v = V(s-1, y, d);

				if( maxV < v ) {
					maxy = y;
					maxV = v;
					maxd = d;
					maxsegs = segs;
				}
			}

			assert( 0 < maxd );
			Segment seg = createSegment(s-maxd, s-1, maxy);
			maxsegs->push_back(seg);
			current_data->setSegments(maxsegs);
		}
	}

	double Inferer::V(int i, AppReqs::Label y, int& maxd) {

		std::cout << "i=" << i << ", y=" << int(y) << std::endl;
		int idx = (i*labels->size()) + (static_cast<int>(y));

		auto& tp = current_vctab->at(idx);
		if( std::get<0>(tp) ) {
			maxd = std::get<2>(tp);
			return std::get<1>(tp);
		}

		maxd = -1;
		AppReqs::Label maxyd;
		double maxV = std::numeric_limits<double>::min();
		
		if( 0 < i ) {

			for( int d = 1; d <= std::min(maxLength, i); d++ ) {
				for( auto yd : *labels ) {

					int tmp = -1;
					double v = V(i-d, yd, tmp);
					v += computeWG(y, yd, i, d);
					
					if( maxV < v ) {
						maxV = v;						
						maxd = d;
						maxyd = yd;
					}
				}
			}

			assert( 0 < maxd );
			Segment seg = createSegment(i-maxd+1, i, maxyd);
			segs->push_back(seg);

		} else if( i == 0 ) {
			maxV = 0.0;

		} else {
			assert( 0 <= i );
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = maxV;
		std::get<2>(tp) = maxd;

		return maxV;
	}
}
