// © 2016 PORT INC.

#include <cstdio>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <cmath>
#include <clocale>
#include <mecab.h>
#include "SemiCrf.hpp"
//#include "AppReqs.hpp"
#include "AppTest.hpp"
#include "MultiByteTokenizer.hpp"
#include "DebugOut.hpp"
#include "Error.hpp"

namespace SemiCrf {

	// ctr
	Labels	createLabels() { return Labels( new Labels_() ); }
	Segment createSegment(int start, int end, App::Label label) { return Segment( new Segment_(start, end, label) ); }
	Segments createSegments() { return Segments( new Segments_() ); }
	CheckTable createCheckTable(int capacity) { return CheckTable( new CheckTable_(capacity, CheckTuple()) ); }

	// Labels
	Labels_::Labels_()
	{
		Debug::out(2) << "Labels_()" << std::endl;
	}

	Labels_::~Labels_()
	{
		Debug::out(2) << "Labels_()" << std::endl;
	}

	// Data_ ctr
	Data_::Data_()
		: strs( new Strs_() )
		, segs( new Segments_() )
	{
		Debug::out(2) << "Data_()" << std::endl;
	}

	Data_::~Data_()
	{
		Debug::out(2) << "~Data_()" << std::endl;
	}

	void Data_::read()
	{
		Debug::out(2) << "Data_::read()" << std::endl;
	}

	void Data_::write(std::ostream& output) const
	{
		Debug::out(2) << "Data_::write()" << std::endl;

		for( auto s : *segs ){

			int start = s->getStart();
			int end = s->getEnd();
			App::Label l = s->getLabel();

			for( int i = start; i <= end; i++ ) {

				output << strs->at(i).at(0);

				if( i == start ) {

					output << "\tS\t";

				} else if( start < i && i < end ) {

					output << "\tM\t";

				} else if( i == end ) {

					output << "\tE\t";

				} else {
					// T.B.D.
				}

				output << label2String(l) << std::endl;
			}
		}
	}

	// Datas ctr
	Datas_::Datas_()
	{
		Debug::out(2) << "Datas_()" << std::endl;
	};

	Datas_::~Datas_()
	{
		Debug::out(2) << "~Datas_()" << std::endl;
	};

	void Datas_::write(std::ostream& output)  const {
		Debug::out(2) << "Datas_::write()" << std::endl;

		for( auto d : *this ) {

			output << "# BEGIN" << std::endl;
		 	d->write(output);
			output << "# END" << std::endl;
			output << std::endl;
		}
	}

	// TrainingDatas ctr
	TrainingDatas_::TrainingDatas_()
	{
		Debug::out(2) << "TrainingDatas_()" << std::endl;
	}

	TrainingDatas_::~TrainingDatas_()
	{
		Debug::out(2) << "~TrainingDatas_()" << std::endl;
	}

	void TrainingDatas_::read(std::istream& strm)
	{
		Debug::out(2) << "TrainingDatas_::read()" << std::endl;

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
					Debug::out(2) << "BEGIN : data was created." << std::endl;
				} else if( line == "# END" ) {
					push_back(data);
					Debug::out(2) << "END : data was pushed." << std::endl;
				}
				continue;
			}

			MultiByteTokenizer tokenizer(line);
			counter++;

			std::string word = tokenizer.get();
			if( word.empty() ) {
				// T.B.D.
			} else {
				Debug::out(2) << word << std::endl;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string descriptor = tokenizer.get();
			if( descriptor.empty() ) {
				// T.B.D.
			} else {
				Debug::out(2) << descriptor << std::endl;
			}

			std::string label = tokenizer.get();
			if( label.empty() ) {
				// T.B.D.

			} else {
				Debug::out(2) << label << std::endl;

				App::Label l = App::string2Label(label);

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

					Debug::out(2) << "warning: unknown descriptor" << std::endl;
				}
			}

			std::string remains = tokenizer.get();
			while( !remains.empty() ) {
				// T.B.D.
			}
		}
	}

	// PridectionDatas ctr
	PridectionDatas_::PridectionDatas_()
	{
		Debug::out(2) << "PridectionDatas_()" << std::endl;
	}

	PridectionDatas_::~PridectionDatas_()
	{
		Debug::out(2) << "~PridectionDatas_()" << std::endl;
	}

	void PridectionDatas_::read(std::istream& strm)
	{
		Debug::out(2) << "PridectionDatas_::read()" << std::endl;
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
					Debug::out(2) << "BEGIN : data was created." << std::endl;

				} else if( line == "# END" ) {

					Tagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger("")); // T.B.D.
					const MeCab::Node* node = tagger->parseToNode(input.c_str());

					for( ; node ; node = node->next ) {

						std::string cppstr = node->feature;
						Debug::out(2) << cppstr << std::endl;
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
						Debug::out(2) << word << std::endl;

						MultiByteTokenizer tok(cppstr);
						std::string t = tok.get();

						while( !t.empty() ) {
							Debug::out(2) << t << std::endl;
							vs.push_back(t);
							t = tok.get();
						}

						data->getStrs()->push_back(vs);
					}

					push_back(data);
					Debug::out(2) << "END : data was pushed." << std::endl;
				}

				continue;
			}

			input += line;
		}
	}

	// Weights ctr
	Weights createWeights(int dim)
	{
		return Weights( new Weights_(dim) );
	}

	Weights_::Weights_(int dim)
		: std::vector<double>(dim)
	{
		Debug::out(2) << "Weights_()" << std::endl;
	}

	Weights_::~Weights_()
	{
		Debug::out(2) << "~Weights_()" << std::endl;
	}

	void Weights_::read(std::ifstream& ifs)
	{
		Debug::out(2) << "Weights_::read()" << std::endl;

		bool state = false;

		std::string line;
		while( std::getline(ifs, line) ) {

			if( line == "" ) {
				continue;
			}

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					state = true;
				} else if( line == "# END" ) {
					state = false;
					break;
				}
				continue;
			}

			if( state ) {

				MultiByteTokenizer tokenizer(line);
				std::string weight = tokenizer.get();
				double wv  = boost::lexical_cast<double>(weight);
				push_back(wv);

				if( !tokenizer.get().empty() ) {
					// T.B.D
				}
			}
		}
	}

	void Weights_::write(std::ofstream& ofs)
	{
		Debug::out(2) << "Weights_::write()" << std::endl;

		ofs << "# Semi-CRF Weights" << std::endl;
		ofs << std::endl;
		ofs << "# BEGIN" << std::endl;

		for( auto w : *this ) {
			ofs << w << std::endl;
		}

		ofs << "# END" << std::endl;
	}

	FeatureFunction_::FeatureFunction_()
	{
		Debug::out(2) << "FeatureFunction_()" << std::endl;
	}

	FeatureFunction_::~FeatureFunction_()
	{
		Debug::out(2) << "~FeatureFunction_()" << std::endl;
	}

	//// Algorithm ////

	Algorithm_::Algorithm_()
		: labels( nullptr )
		, ff( nullptr )
		, weights( nullptr )
		, datas( nullptr )
		, maxLength(5)
		, e0(1.0e-5)
		, e1(1.0e-5)
	{
		Debug::out(2) << "Algorithm_()" << std::endl;
	}

	Algorithm_::~Algorithm_()
	{
		Debug::out(2) << "~Algorithm_()" << std::endl;
	}

	void Algorithm_::setLabels(Labels arg)
	{
		labels = arg;
	}

	void Algorithm_::setMaxLength(int arg)
	{
		maxLength = arg;
	}

	void Algorithm_::setE0(double arg)
	{
		e0 = arg;
	}

	void Algorithm_::setE1(double arg)
	{
		e1 = arg;
	}

	void Algorithm_::setDatas(Datas arg)
	{
		datas = arg;
	}

	void Algorithm_::setFeatureFunction(FeatureFunction arg)
	{
		ff = arg;
	}

	void Algorithm_::setWeights(Weights arg)
	{
		weights = arg;
	}

	void Algorithm_::setDimension(int arg)
	{
		dim = arg;
	}

	double Algorithm_::computeWG(App::Label y, App::Label yd, int i, int d)
	{
		double v = 0.0;

		int k = 0;
		for( auto w : *weights ) {
			v += w * (*ff)(k, y, yd, current_data, i-d+1, i);
			k++;
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
		Debug::out(2) << "Learner()" << std::endl;
	}

	Learner::~Learner()
	{
		Debug::out(2) << "~Learner()" << std::endl;
	}

	Datas Learner::createDatas()
	{
		return Datas( new TrainingDatas_() );
	}

	void Learner::preProcess(const std::string& wfile)
	{
		SemiCrf::Weights weights = SemiCrf::createWeights(dim);
		setWeights(weights);
	}

	void Learner::postProcess(const std::string& wfile)
	{
		std::ofstream ofs; // 出力
		open(ofs, wfile);
		weights->write(ofs);
	}

	void Learner::compute()
	{
		Debug::out(2) << "Learner::compute()" << std::endl;
		int l = labels->size();

		while(1) {

			double L = 0.0;
			std::vector<double> dL(dim, 0.0);

			for( auto data : *datas ) {

				current_data = data;
				int s = current_data->getStrs()->size();
				int capacity = l*s;
				current_actab = createCheckTable(capacity);

				double WG = 0.0;
				double Z = computeZ(); Debug::out(2) << "Z=" << Z << std::endl;
				auto Gs = computeG(WG);
				auto Gms = computeGm(Z);

				L += WG - log(Z);

				auto idL = dL.begin();
				for( int k = 0; k < dim; k++, idL++ ) {
					(*idL) += Gs[k] - Gms[k]; Debug::out(2) << "dL(" << k << ")=" << *idL << std::endl;
				}
			}

			assert( weights->size() == dL.size() );

			auto dLi = dL.begin();
			auto wi = weights->begin();
			for( int k = 0; k < dim; wi++, dLi++, k++ ) {
				(*wi) += e0 * (*dLi); Debug::out(2) << "W(" << k << ")=" << *wi << std::endl;
			}

			if( isConv(L, dL) ) {
				break;
			}
		}
	}

	bool Learner::isConv(double L, const std::vector<double>& dL)
	{
		double tdl = 0.0;

		for( auto dl : dL ) {
			tdl += dl*dl;
		}

		tdl = sqrt(tdl); Debug::out(1) << "L=" << L << " |dl|=" << tdl << std::endl;
		return (tdl < e1);
	}

	std::vector<double> Learner::computeG(double& WG)
	{
		std::vector<double> Gs;
		auto segments = current_data->getSegments();
		assert( 0 < segments->size() );

		auto iw = weights->begin();
		for( int k = 0; k < dim; k++, iw++ ) {

			double G = 0.0;
			auto sj = segments->begin();
			auto si = segments->begin(); si++;

			for( ; si != segments->end(); si++, sj++ ){ // 最初のセグメントが抜けている

				auto y = (*si)->getLabel();
				auto y1 = (*sj)->getLabel();
				int ti = (*si)->getStart();
				int ui = (*si)->getEnd();
				G += (*ff)(k, y, y1, current_data, ti, ui);
			}

			Gs.push_back(G); Debug::out(2) << "G(" << k << ")=" << G << std::endl;
			WG += (*iw)*G;
		}

		return(std::move(Gs));
	}

	double Learner::computeZ()
	{
		double Z = 0;
		int s = current_data->getStrs()->size();

		for( auto y : *labels ) {
			Z += alpha(s-1, y);
		}

		return Z;
	}

	std::vector<double> Learner::computeGm(double Z)
	{
		std::vector<double> Gms;

		int l = labels->size();
		int s = current_data->getStrs()->size();
		int capacity = l*s;

		for( int k = 0; k < dim; k++ ) {

			double Gmk = 0.0;
			current_ectab = createCheckTable(capacity);

			for( auto y : *labels ) {
				Gmk += eta(s-1, y, k);
			}

			Gmk /= Z;
			Gms.push_back(Gmk);
			Debug::out(2) << "Gm(" << k << ")=" << Gmk << std::endl;
		}

		return(std::move(Gms));
	}

	double Learner::alpha(int i, App::Label y)
	{
		Debug::out(3) << "alpha(i=" << i << ", y=" << int(y) << ")" << std::endl;

		int idx = (i*labels->size()) + (static_cast<int>(y));
		auto& tp = current_actab->at(idx);
		if( std::get<0>(tp) ) {
			return std::get<1>(tp);
		}

		double v = 0;

		if( 0 < i ) {

			for( int d = 1; d <= std::min(maxLength, i); d++ ) {
				for( auto yd : *labels ) {

					double e0 = alpha(i-d, yd);
					double e1 = computeWG(y, yd, i, d);
					v += e0*exp(e1);
				}
			}

		} else if( i == 0 ) {

			for( auto yd : *labels ) {

				double e = computeWG(y, yd, 0, 1); // ydの位置は-1 
				v += exp(e);
			}

		} else {
			assert( 0 <= i );
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = v;

		return v;
	}

	double Learner::eta(int i, App::Label y, int k)
	{
		Debug::out(3) << "eta(i=" << i << ", y=" << int(y) << ")" << std::endl;

		int idx = (i*labels->size()) + (static_cast<int>(y));
		auto& tp = current_ectab->at(idx);
		if( std::get<0>(tp) ) {
			return std::get<1>(tp);
		}

		double v = 0;

		if( 0 < i ) {

			for( int d = 1; d <= std::min(maxLength, i); d++ ) {
				for( auto yd : *labels ) {

					double e0 = eta(i-d, yd, k) + alpha(i-d, yd) * (*ff)(k, y, yd, current_data, i-d+1, i);
					double e1 = computeWG(y, yd, i, d);
					v += e0*exp(e1);
				}
			}

		} else if( i == 0 ) {

			for( auto yd : *labels ) {

				double e0 = (*ff)(k, y, yd, current_data, 1, 0); // ydの位置は-1
				double e1 = computeWG(y, yd, 0, 1); // ydの位置は-1
				v += e0*exp(e1);
			}

		} else {
			assert( 0 <= i );
		}

		std::get<0>(tp) = true;
		std::get<1>(tp) = v;

		return v;
	}

	//// Pridector ////

	Algorithm createPridector()
	{
		return std::shared_ptr<Pridector>(new Pridector());
	}

	Pridector::Pridector()
	{
		Debug::out(2) << "Pridector()" << std::endl;
	}

	Pridector::~Pridector() {
		Debug::out(2) << "~Pridector()" << std::endl;
	}

	Datas Pridector::createDatas()
	{
		return Datas( new PridectionDatas_() );
	}

	void Pridector::preProcess(const std::string& wfile)
	{
		SemiCrf::Weights weights = SemiCrf::createWeights(dim);
		std::ifstream ifs; // 入力
		open(ifs, wfile);
		weights->read(ifs);
		setWeights(weights);
	}

	void Pridector::postProcess(const std::string& wfile)
	{
		std::ofstream ofs; // 出力
		open(ofs, wfile);
		datas->write(ofs);
	}

	void Pridector::compute()
	{
		Debug::out(2) << "Pridector::compute()" << std::endl;

		for( auto data : *datas ) {

			current_data = data;

			int l = labels->size();
			int s = current_data->getStrs()->size();
			int capacity = l*s;

			current_vctab = createCheckTable(capacity);

			int maxd = - 1;
			App::Label maxy;
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

	double Pridector::V(int i, App::Label y, int& maxd)
	{
		Debug::out(3) << "V(i=" << i << ", y=" << int(y) << ")" << std::endl;

		int idx = (i*labels->size()) + (static_cast<int>(y));
		auto& tp = current_vctab->at(idx);
		if( std::get<0>(tp) ) {
			maxd = std::get<2>(tp);
			return std::get<1>(tp);
		}

		maxd = -1;
		App::Label maxyd;
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
