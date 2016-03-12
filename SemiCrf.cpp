
#include <cstdio>
#include <limits>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <cmath>
#include <clocale>
#include <mecab.h>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"
#include "MultiByteTokenizer.hpp"
#include "DebugOut.hpp"

namespace SemiCrf {

	// ctr
	Labels	createLabels() { return Labels( new Labels_() ); }
	Segment createSegment(int start, int end, AppReqs::Label label) { return Segment( new Segment_(start, end, label) ); }
	Segments createSegments() { return Segments( new Segments_() ); }
	CheckTable createCheckTable(int capacity) { return CheckTable( new CheckTable_(capacity, CheckTuple()) ); }

	// Data_ ctr
	Data_::Data_()
		: strs( new Strs_() )
		, segs( new Segments_() )
	{
		Debug::out() << "Data_()" << std::endl;
	}

	Data_::~Data_() {
		Debug::out() << "~Data_()" << std::endl;
	}

	void Data_::read() {
		Debug::out() << "TrainingData::read()" << std::endl;

		// add strings
		// T.B.D.
		// strs->push_back("AAA");
		// strs->push_back("BBB");
		// strs->push_back("CCC");
		// strs->push_back("DDD");

		// add feature functions
		// T.B.D.
		Segment s0(new Segment_( 0, 1, AppReqs::Label::CAMPANY ));
		segs->push_back(s0);

		// add feature functions
		// T.B.D.
		Segment s1(new Segment_( 2, 3, AppReqs::Label::LOCATION ));
		segs->push_back(s1);
	}

	void Data_::write() const {
		Debug::out() << "InferenceData::write()" << std::endl;

		// iterate segments
		for( auto i : *segs ){
			int start = i->getStart();
			int end = i->getEnd();
			AppReqs::Label l = i->getLabel();
			// T.B.D.
		}

		// iterate strings
		for( auto i : *strs ){
			std::vector<std::string>& s = i;
			// T.B.D.
		}
	}

	// Datas ctr
	Datas_::Datas_()
	{
		Debug::out() << "Datas_()" << std::endl;
	};

	Datas_::~Datas_()
	{
		Debug::out() << "~Datas_()" << std::endl;
	};

	void Datas_::write() const {
		Debug::out() << "Datas_::write()" << std::endl;
		for( auto i : *this ) {
			i->write();
		}
	}

	// TrainingDatas ctr
	TrainingDatas_::TrainingDatas_()
	{
		Debug::out() << "TrainingDatas_()" << std::endl;
	}

	TrainingDatas_::~TrainingDatas_()
	{
		Debug::out() << "~TrainingDatas_()" << std::endl;
	}

	void TrainingDatas_::read(std::istream& strm)
	{
		Debug::out() << "TrainingDatas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D

		int counter = -1;
		int seg_start = -1;
		Segment seg;
		Data data;
		std::string line;

		while( std::getline(strm, line) ) {

			if( line == "" ) {
				continue;
			}

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					data = Data( new Data_() );
					counter = -1;
					Debug::out() << "BEGIN : data was created." << std::endl;
				} else if( line == "# END" ) {
					push_back(data);
					Debug::out() << "END : data was pushed." << std::endl;
				}
				continue;
			}

			MultiByteTokenizer tokenizer(line);
			counter++;

			std::string word = tokenizer.get();
			if( word.empty() ) {
				// T.B.D.
			} else {
				Debug::out() << word << std::endl;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string descriptor = tokenizer.get();
			if( descriptor.empty() ) {
				// T.B.D.
			} else {
				Debug::out() << descriptor << std::endl;
			}

			std::string label = tokenizer.get();
			if( label.empty() ) {
				// T.B.D.

			} else {
				Debug::out() << label << std::endl;

				AppReqs::Label l = AppReqs::string2Label(label);

				if( descriptor == "N" ) {

					seg = createSegment(counter, counter, l);
					data->getSegments()->push_back(seg);

				} else if( descriptor == "S" ) {

					seg_start = counter;

				} else if( descriptor == "M" ) {

					// nothing to do

				} else if( descriptor == "E" ) {

					seg	= createSegment(seg_start, counter, l);
					data->getSegments()->push_back(seg);

				} else {

					Debug::out() << "warning: unknown descriptor" << std::endl;
				}
			}

			std::string remains = tokenizer.get();
			while( !remains.empty() ) {
				// T.B.D.
			}
		}
	}

	// InferenceDatas ctr
	InferenceDatas_::InferenceDatas_()
	{
		Debug::out() << "InferenceDatas_()" << std::endl;
	}

	InferenceDatas_::~InferenceDatas_()
	{
		Debug::out() << "~InferenceDatas_()" << std::endl;
	}

	void InferenceDatas_::read(std::istream& strm)
	{
		Debug::out() << "InferenceDatas_::read()" << std::endl;
		typedef std::shared_ptr<MeCab::Tagger> Tagger;

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

		Data data;
		std::string input;
		std::string line;

		while( std::getline(strm, line) ) {

			if( line == "" ) {
				continue;
			}

			if( line[0] == '#' ) {

				if( line == "# BEGIN" ) {
					data = Data( new Data_() );
					input = "";
					Debug::out() << "BEGIN : data was created." << std::endl;

				} else if( line == "# END" ) {

					Tagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger("")); // T.B.D.
					const MeCab::Node* node = tagger->parseToNode(input.c_str());

					for( ; node ; node = node->next ) {

						std::string cppstr = node->feature;
						Debug::out() << cppstr << std::endl;
						if( cppstr.find("BOS") == 0 ) { // T.B.D.
							continue;
						}

						std::vector<std::string> vs;
						std::string word;

						auto c = node->surface;
						for( int i = 0; i < node->length; i++ ) {
							word.push_back(*c);
							c++;
						}

						vs.push_back(word);
						Debug::out() << word << std::endl;

						MultiByteTokenizer tok(cppstr);
						std::string t = tok.get();

						while( !t.empty() ) {
							Debug::out() << t << std::endl;
							vs.push_back(t);
							t = tok.get();
						}

						data->getStrs()->push_back(vs);
					}

					push_back(data);
					Debug::out() << "END : data was pushed." << std::endl;
				}

				continue;
			}

			input += line;
		}
	}

	// Weights ctr
	Weights createWeights()
	{
		return Weights( new Weights_() );
	}

	Weights_::Weights_()
	{
		Debug::out() << "Weights()" << std::endl;
	}

	Weights_::~Weights_()
	{
		Debug::out() << "~Weights()" << std::endl;
	}

	void Weights_::read()
	{
		Debug::out() << "Weights_::read()" << std::endl;
		push_back(1.0);
		push_back(2.0);
	}

	void Weights_::write()
	{
		Debug::out() << "Weights_::write()" << std::endl;
	}

	FeatureFunction_::FeatureFunction_()
	{
		Debug::out() << "FeatureFunction_()" << std::endl;
	}

	FeatureFunction_::~FeatureFunction_()
	{
		Debug::out() << "~FeatureFunction_()" << std::endl;
	}

	// FeatureFunctions ctr
	FeatureFunctions createFeatureFunctions()
	{
		return FeatureFunctions( new FeatureFunctions_() );
	}

	FeatureFunctions_::FeatureFunctions_()
	{
		Debug::out() << "FeatureFunctions_()" << std::endl;
	}

	FeatureFunctions_::~FeatureFunctions_()
	{
		Debug::out() << "~FeatureFunctions_()" << std::endl;
	}

	void FeatureFunctions_::read()
	{
		// T.B.D.		
	}

	void FeatureFunctions_::write()
	{
		for( auto f : *this ) {
			f->write();
		}		
	}

	//// Algorithm ////

	Algorithm_::Algorithm_()
		: labels( nullptr )
		, ffs( nullptr )
		, weights( nullptr )
		, datas( nullptr )
	{
		Debug::out() << "Algorithm_()" << std::endl;
	}

	Algorithm_::~Algorithm_()
	{
		Debug::out() << "~Algorithm_()" << std::endl;
	}

	void Algorithm_::setLabels(Labels arg)
	{
		labels = arg;
	}

	void Algorithm_::setMaxLength(int arg)
	{
		maxLength = arg;
	}

	void Algorithm_::setDatas(Datas arg)
	{
		datas = arg;
	}

	void Algorithm_::setFeatureFunctions(FeatureFunctions arg)
	{
		ffs = arg;
	}

	void Algorithm_::setWeights(Weights arg)
	{
		weights = arg;
	}

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

	Algorithm createLearner()
	{
		return std::shared_ptr<Learner>(new Learner());
	}

	Learner::Learner()
	{
		Debug::out() << "Learner()" << std::endl;
	}

	Learner::~Learner()
	{
		Debug::out() << "~Learner()" << std::endl;
	}

	Datas Learner::createDatas()
	{
		return Datas( new TrainingDatas_() );
	}

	void Learner::preProcess()
	{
		ffs->read();
		weights->read();
	}

	void Learner::postProcess()
	{
		ffs->write();
		weights->write();
	}

	void Learner::compute()
	{
		Debug::out() << "Learner::compute()" << std::endl;
		assert( weights->size() == ffs->size() );
		int l = labels->size();

		std::vector<double> dL(datas->size(), 1.0);

		while( !isConv(dL) ) {

			dL.clear();
			auto pdL = dL.begin();

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
					(*pdL) += Gs[k] - Gms[k];
				}

				pdL++;
			}

			assert( weights->size() == dL.size() );

			const double ep0 = 1.0e-5;
			auto wi = weights->begin();
			auto dLi = dL.begin();
			for( ; wi != weights->end(); wi++, dLi++ ) {
				(*wi) = (*wi) + ep0 * (*dLi); // !!!!
			}
		}
	}

	bool Learner::isConv(const std::vector<double>& dL)
	{
		const double ep1 = 1.0e-5;
		double l = 0.0;
		for( auto i : dL ) {
			l += i*i;
		}
		l = sqrt(l);

		return (l < ep1);
	}

	std::vector<double>&& Learner::computeG()
	{
		std::vector<double> Gs;
		auto segments = current_data->getSegments();
		assert( 0 < segments->size() );

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

	double Learner::computeZ()
	{
		double Z = 0;
		int size = current_data->getStrs()->size();

		for( auto y : *labels ) {
			Z += alpha(size-1, y);
		}

		return Z;
	}

	std::vector<double>&& Learner::computeGm(double Z)
	{
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

	double Learner::alpha(int i, AppReqs::Label y)
	{
		Debug::out() << "i=" << i << ", y=" << int(y) << std::endl;
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

	double Learner::eta(int i, AppReqs::Label y, int k)
	{
		Debug::out() << "i=" << i << ", y=" << int(y) << std::endl;
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

	Algorithm createInferer()
	{
		return std::shared_ptr<Inferer>(new Inferer());
	}

	Inferer::Inferer()
	{
		Debug::out() << "Inferer()" << std::endl;
	}

	Inferer::~Inferer() {
		Debug::out() << "~Inferer()" << std::endl;
	}

	Datas Inferer::createDatas()
	{
		return Datas( new InferenceDatas_() );
	}

	void Inferer::preProcess()
	{
		ffs->read();
		//weights->read(); T.B.D.
	}

	void Inferer::postProcess()
	{
		// datas->write(); T.B.D.
	}

	void Inferer::compute()
	{
		Debug::out() << "Inferer::compute()" << std::endl;
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

	double Inferer::V(int i, AppReqs::Label y, int& maxd)
	{
		Debug::out() << "i=" << i << ", y=" << int(y) << std::endl;
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
