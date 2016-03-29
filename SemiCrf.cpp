// © 2016 PORT INC.

#include <cstdio>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <cassert>
#include <cmath>
#include <clocale>
#include <mecab.h>
#include "SemiCrf.hpp"
#include "AppTest.hpp"
#include "MultiByteTokenizer.hpp"
#include "Logger.hpp"
#include "Error.hpp"

namespace SemiCrf {

	std::string date() {
        time_t t = time(0);
        char *s,*p;
        p = s = asctime(localtime(&t));
        while(*s != '\0') { if (*s == '\n') {*s = '\0'; break;} else {s++;}}
        return std::move(std::string(p));
    }

	// ctr
	Labels createLabels()
	{
		return Labels( new Labels_() );
	}

	Segment createSegment(int start, int end, App::Label label)
	{
		return Segment( new Segment_(start, end, label) );
	}

	Segments createSegments()
	{
		return Segments( new Segments_() );
	}

	CheckTable createCheckTable(int capacity)
	{
		return CheckTable( new CheckTable_(capacity, CheckTuple()) );
	}

	// Labels
	Labels_::Labels_()
	{
		Logger::out(2) << "Labels_()" << std::endl;
	}

	Labels_::~Labels_()
	{
		Logger::out(2) << "~Labels_()" << std::endl;
	}

	// Data_ ctr
	Data_::Data_()
		: strs( new Strs_() )
		, segs( new Segments_() )
	{
		Logger::out(2) << "Data_()" << std::endl;
	}

	Data_::~Data_()
	{
		Logger::out(2) << "~Data_()" << std::endl;
	}

	void Data_::read()
	{
		Logger::out(2) << "Data_::read()" << std::endl;
	}

	void Data_::write(std::ostream& output) const
	{
		Logger::out(2) << "Data_::write()" << std::endl;

		for( auto s : *segs ) {

			int start = s->getStart();
			int end = s->getEnd();
			App::Label l = s->getLabel();

			for( int i = start; i <= end; i++ ) {

				output << strs->at(i).at(0);

				if( i == start ) {

					if( i == end ) {
						output << "\tS/E\t";
					} else {
						output << "\tS\t";
					}

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

		if( segs->empty() ) {
			for( auto str : *strs ) {
				for( auto s : str ) {
					output << s << "\t";
				}
				output << std::endl;
			}
		}
	}

	// Datas ctr
	Datas_::Datas_()
		: xDim(0)
		, yDim(0)
	{
		Logger::out(2) << "Datas_()" << std::endl;
	};

	Datas_::~Datas_()
	{
		Logger::out(2) << "~Datas_()" << std::endl;
	};

	void Datas_::write(std::ostream& output)  const {
		Logger::out(2) << "Datas_::write()" << std::endl;

		output << "# DIMENSION" << " " << xDim << " " << yDim << std::endl;
		for( auto d : *this ) {

			output << "# BEGIN" << std::endl;
		 	d->write(output);
			output << "# END" << std::endl;
		}
	}

	// TrainingDatas ctr
	TrainingDatas_::TrainingDatas_()
	{
		Logger::out(2) << "TrainingDatas_()" << std::endl;
	}

	TrainingDatas_::~TrainingDatas_()
	{
		Logger::out(2) << "~TrainingDatas_()" << std::endl;
	}

	void TrainingDatas_::read(std::istream& strm)
	{
		Logger::out(2) << "TrainingDatas_::read()" << std::endl;

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

			MultiByteTokenizer tokenizer(line);
			counter++;

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					data = Data( new Data_() );
					counter = -1;
					Logger::out(2) << "BEGIN : data was created." << std::endl;
				} else if( line == "# END" ) {
					push_back(data);
					Logger::out(2) << "END : data was pushed." << std::endl;
				} else {
					tokenizer.get(); // # を捨てる
					std::string tok = tokenizer.get();
					if( tok == "DIMENSION" ) {
						tok = tokenizer.get();
						if( !tok.empty() ) {
							xDim = boost::lexical_cast<int>(tok);
						}
						tok = tokenizer.get();
						if( !tok.empty() ) {
							yDim = boost::lexical_cast<int>(tok);
						}
						tok = tokenizer.get();
						if( !tok.empty() ) {
							// T.B.D.
						}
					}
				}
				continue;
			}

			std::string word = tokenizer.get();
			if( word.empty() ) {
				// T.B.D.
			} else {
				Logger::out(2) << word << std::endl;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string descriptor = tokenizer.get();
			if( descriptor.empty() ) {
				// T.B.D.
			} else {
				Logger::out(2) << descriptor << std::endl;
			}

			std::string label = tokenizer.get();
			if( label.empty() ) {
				// T.B.D.

			} else {
				Logger::out(2) << label << std::endl;


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
					// record (l - counter + 1)

				} else if( descriptor == "S/E" ) {

					seg	= createSegment(counter, counter, l);
					data->getSegments()->push_back(seg);

				} else {

					Logger::out(2) << "warning: unknown descriptor" << std::endl;
				}
			}

			std::string remains = tokenizer.get();
			if( !remains.empty() ) {
				// T.B.D.
			}
		}
	}

	// PredictionDatas ctr
	PredictionDatas_::PredictionDatas_()
	{
		Logger::out(2) << "PredictionDatas_()" << std::endl;
	}

	PredictionDatas_::~PredictionDatas_()
	{
		Logger::out(2) << "~PredictionDatas_()" << std::endl;
	}

	void PredictionDatas_::read(std::istream& strm)
	{
		Logger::out(2) << "PredictionDatas_::read()" << std::endl;
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
					Logger::out(2) << "BEGIN : data was created." << std::endl;

				} else if( line == "# END" ) {

					Tagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger("")); // T.B.D.
					const MeCab::Node* node = tagger->parseToNode(input.c_str());

					for( ; node ; node = node->next ) {

						std::string cppstr = node->feature;
						Logger::out(2) << cppstr << std::endl;
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
						Logger::out(2) << word << std::endl;

						MultiByteTokenizer tok(cppstr);
						std::string t = tok.get();

						while( !t.empty() ) {
							Logger::out(2) << t << std::endl;
							vs.push_back(t);
							t = tok.get();
						}

						data->getStrs()->push_back(vs);
					}

					push_back(data);
					Logger::out(2) << "END : data was pushed." << std::endl;
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
		Logger::out(2) << "Weights_()" << std::endl;
	}

	Weights_::~Weights_()
	{
		Logger::out(2) << "~Weights_()" << std::endl;
	}

	void Weights_::read(std::ifstream& ifs)
	{
		Logger::out(2) << "Weights_::read()" << std::endl;

		bool state = false;

		std::string line;
		while( std::getline(ifs, line) ) {

			if( line == "" ) {
				continue;
			}

			MultiByteTokenizer tokenizer(line);

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					state = true;
				} else if( line == "# END" ) {
					state = false;
					break;
				} else {
					tokenizer.get(); // # を捨てる
					std::string tok = tokenizer.get();
					if( tok == "DIMENSION" ) {
						tok = tokenizer.get();
						if( !tok.empty() ) {
							xDim = boost::lexical_cast<int>(tok);
						}
						tok = tokenizer.get();
						if( !tok.empty() ) {
							yDim = boost::lexical_cast<int>(tok);
						}
						tok = tokenizer.get();
						if( !tok.empty() ) {
							// T.B.D.
						}
					}
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

		int s = size();
		if( s != yDim*(yDim+xDim) ) {
			throw Error("dimension mismatch");
		}
	}

	void Weights_::write(std::ofstream& ofs)
	{
		Logger::out(2) << "Weights_::write()" << std::endl;

		ofs << "# Semi-CRF Weights" << std::endl;
		ofs << std::endl;
		ofs << "# DIMENSION" << " " << xDim << " " << yDim << std::endl;
		ofs << std::endl;
		ofs << "# BEGIN" << std::endl;

		for( auto w : *this ) {
			ofs << boost::format("%14.8e") % w << std::endl;
		}

		ofs << "# END" << std::endl;
	}

	FeatureFunction_::FeatureFunction_()
		: xDim(-1)
		, yDim(-1)
	{
		Logger::out(2) << "FeatureFunction_()" << std::endl;
	}

	FeatureFunction_::~FeatureFunction_()
	{
		Logger::out(2) << "~FeatureFunction_()" << std::endl;
	}

	//// Algorithm ////

	Algorithm_::Algorithm_(int arg)
		: labels( nullptr )
		, ff( nullptr )
		, weights( nullptr )
		, datas( nullptr )
		, maxLength(5)
		, maxIteration(1024)
		, e0(1.0e-5)
		, e1(1.0e-5)
		, flg(arg)
	{
		Logger::out(2) << "Algorithm_()" << std::endl;
	}

	Algorithm_::~Algorithm_()
	{
		Logger::out(2) << "~Algorithm_()" << std::endl;
	}

	void Algorithm_::setLabels(Labels arg)
	{
		labels = arg;
	}

	void Algorithm_::setMaxLength(int arg)
	{
		maxLength = arg;
	}

	void Algorithm_::setMaxIteration(int arg)
	{
		maxIteration = arg;
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
		dim = datas->getDim();
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

	void Algorithm_::setFlg(int arg)
	{
		flg = arg;
	}

	//// Learner ////

	Algorithm createLearner(int arg)
	{
		return std::shared_ptr<Learner>(new Learner(arg));
	}

	Learner::Learner(int arg)
		: Algorithm_(arg)
	{
		Logger::out(2) << "Learner()" << std::endl;
		Logger::out(1) << "Semi-CRF";
		if( flg & DISABLE_DATE_VERSION ) {
			Logger::out(1) << "" << std::endl;
		} else {
			Logger::out(1) << " 0.0.1" << std::endl;
			Logger::out(1) << "" << date() << std::endl;
		}
		if( !(flg & ENABLE_LIKELIHOOD_ONLY) ) {
			Logger::out(1) << "Learning ..." << std::endl;
		}
	}

	Learner::~Learner()
	{
		Logger::out(2) << "~Learner()" << std::endl;
		Logger::out(1) << "OK" << std::endl;
	}

	void Learner::preProcess(const std::string& wfile, const std::string& w0file)
	{
		SemiCrf::Weights weights = SemiCrf::createWeights(dim);
		setWeights(weights);

		if( !w0file.empty() ) {
			std::ifstream ifs; // 入力
			open(ifs, w0file);
			weights->resize(0);
			weights->read(ifs);
		}

		ff->setXDim(datas->getXDim());
		ff->setYDim(datas->getYDim());
	}

	void Learner::postProcess(const std::string& wfile)
	{
		if( !(flg & ENABLE_LIKELIHOOD_ONLY) ) {
			std::ofstream ofs; // 出力
			open(ofs, wfile);
			weights->setXDim(datas->getXDim());
			weights->setYDim(datas->getYDim());
			weights->write(ofs);
		}
	}

	void Learner::compute()
	{
		Logger::out(2) << "Learner::compute()" << std::endl;

		int itr = 0;
		double tdl0 = 0.0;
		double rerr = 1.0;
		bool isfirst = true;

		while( itr < maxIteration ) {

			double L = 0.0;
			std::vector<double> dL(dim, 0.0);

			for( auto data : *datas ) {

				current_data = data;

				double WG = 0.0;
				double Z = computeZ();
				auto Gs = computeG(WG);
				auto Gms = computeGm(Z);

				L += WG - log(Z);
				if( flg & ENABLE_LIKELIHOOD_ONLY ) {
					Logger::out(1) << boost::format("L= %+10.6e WG= %+10.6e logZ= %+10.6e") % L % WG % log(Z) << std::endl;
				}

				auto idL = dL.begin();
				for( int k = 0; k < dim; k++, idL++ ) {
					(*idL) += Gs[k] - Gms[k]; Logger::out(2) << "dL(" << k << ")=" << *idL << std::endl;
				}
			}

			assert( weights->size() == dL.size() );

			auto dLi = dL.begin();
			auto wi = weights->begin();
			double e = e0;
			if( !(flg & DISABLE_ADAGRAD) ) {
				e /= rerr;
			}
			for( int k = 0; k < dim; wi++, dLi++, k++ ) {
				(*wi) += e * (*dLi); Logger::out(2) << "W(" << k << ")=" << *wi << std::endl;
			}

			if( isConv(L, dL, tdl0, rerr, isfirst) ) {
				break;
			}

			itr++;
		}
	}

	bool Learner::isConv(double L, const std::vector<double>& dL, double& tdl0, double& rerr, bool& isfirst)
	{
		double tdl = 0.0;

		for( auto dl : dL ) {
			tdl += dl*dl;
		}

		tdl = sqrt(tdl);

		if( isfirst ) {
			tdl0 = tdl;
			isfirst = false;
			if( flg & ENABLE_LIKELIHOOD_ONLY ) {
				return true;
			}
			return false;
		}

		rerr = tdl/tdl0;
		Logger::out(1) << boost::format("L= %10.6e |dL|= %10.6e") % L % rerr << std::endl;
		return (rerr < e1);
	}

	std::vector<double> Learner::computeG(double& WG)
	{
		std::vector<double> Gs;
		auto segments = current_data->getSegments();
		assert( 0 < segments->size() );

		auto iw = weights->begin();
		for( int k = 0; k < dim; k++, iw++ ) {

			double G = 0.0;
			auto sj = segments->begin(); // !!!!
			auto si = segments->begin();

			for( ; si != segments->end(); si++ ){

				auto y = (*si)->getLabel();
				auto y1 = (*sj)->getLabel();
				int ti = (*si)->getStart();
				int ui = (*si)->getEnd();
				G += (*ff)(k, y, y1, current_data, ti, ui);
				sj = si;
			}

			Gs.push_back(G); Logger::out(2) << "G(" << k << ")=" << G << std::endl;
			double w = *iw;
			WG += w*G;
		}

		if( flg & ENABLE_LIKELIHOOD_ONLY ) {

			auto sj = segments->begin(); // !!!!
			auto si = segments->begin();
			double awg = 0.0;

			for( ; si != segments->end(); si++ ){

				double wg = 0.0;
				auto y = (*si)->getLabel();
				auto y1 = (*sj)->getLabel();
				int ti = (*si)->getStart();
				int ui = (*si)->getEnd();

				auto iw = weights->begin();
				for( int k = 0; k < dim; k++, iw++ ) {
					double g = (*ff)(k, y, y1, current_data, ti, ui);
					double w = *iw;
					wg += w*g;
				}

				sj = si;
				awg += wg;

				Logger::out(1) << "(" << ti << "," << ui << ")";
				Logger::out(1) << boost::format(" WG= %+10.6e AWG= %+10.6e") % wg % awg << std::endl;
			}
		}

		return(std::move(Gs));
	}

	double Learner::computeZ()
	{
		double Z = 0;

		int l = labels->size();
		int s = current_data->getStrs()->size();
		int capacity = l*s;
		current_actab = createCheckTable(capacity);

		for( auto y : *labels ) {
			Z += alpha(s-1, y);
		}

		Logger::out(2) << "Z=" << Z << std::endl;
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
			Logger::out(2) << "Gm(" << k << ")=" << Gmk << std::endl;
		}

		return(std::move(Gms));
	}

	double Learner::alpha(int i, App::Label y)
	{
		double v = 0;

		if( -1 < i ) {

			int idx = (i*labels->size()) + (static_cast<int>(y));
			auto& tp = current_actab->at(idx);

			if( std::get<0>(tp) ) {

				v = std::get<1>(tp);

			} else {

				for( int d = 1; d <= std::min(maxLength, i+1); d++ ) {
					for( auto yd : *labels ) {

						double alp = alpha(i-d, yd);
						double wg = computeWG(y, yd, i, d);
						v += alp*exp(wg);
					}
				}

				std::get<0>(tp) = true;
				std::get<1>(tp) = v;
			}

		} else if( i == -1 ) {

			v = 1.0;

		} else {
			assert( -2 < i ); // T.B.D.
		}

		Logger::out(3) << "alpha(i=" << i << ",y=" << int(y) << ")=" << v << std::endl;
		return v;
	}

	double Learner::eta(int i, App::Label y, int k)
	{
		double v = 0;

		if( -1 < i ) {

			int idx = (i*labels->size()) + (static_cast<int>(y));
			auto& tp = current_ectab->at(idx);

			if( std::get<0>(tp) ) {

				v = std::get<1>(tp);

			} else {

				for( int d = 1; d <= std::min(maxLength, i+1); d++ ) {
					for( auto yd : *labels ) {

						double cof = eta(i-d, yd, k) + alpha(i-d, yd) * (*ff)(k, y, yd, current_data, i-d+1, i);
						double wg = computeWG(y, yd, i, d);
						v += cof*exp(wg);
					}
				}

				std::get<0>(tp) = true;
				std::get<1>(tp) = v;
			}

		} else if( i == -1 ) {

			v = 0.0;

		} else {
			assert( -2 < i ); // T.B.D.
		}

		Logger::out(3) << "eta(i=" << i << ",y=" << int(y) << ",k=" << k << ")=" << v << std::endl;
		return v;
	}

	//// Predictor ////

	Algorithm createPredictor(int arg)
	{
		return std::shared_ptr<Predictor>(new Predictor(arg));
	}

	Predictor::Predictor(int arg)
		: Algorithm_(arg)
	{
		Logger::out(2) << "Predictor()" << std::endl;
		Logger::out(1) << "Semi-CRF";
		if( flg & DISABLE_DATE_VERSION ) {
			Logger::out(1) << "" << std::endl;
		} else {
			Logger::out(1) << " 0.0.1" << std::endl;
			Logger::out(1) << "" << date() << std::endl;
		}
		Logger::out(1) << "Prediction ..." << std::endl;
	}

	Predictor::~Predictor()
	{
		Logger::out(2) << "~Predictor()" << std::endl;
		Logger::out(1) << "OK" << std::endl;
	}

	void Predictor::preProcess(const std::string& wfile, const std::string& w0file)
	{
		SemiCrf::Weights weights = SemiCrf::createWeights();
		std::ifstream ifs; // 入力
		open(ifs, wfile);
		weights->read(ifs);
		setWeights(weights);
		ff->setXDim(weights->getXDim());
		ff->setYDim(weights->getYDim());
		datas->setXDim(weights->getXDim());
		datas->setYDim(weights->getYDim());
	}

	void Predictor::postProcess(const std::string& wfile)
	{
		datas->write(Logger::out(0) << "");
	}

	void Predictor::compute()
	{
		Logger::out(2) << "Predictor::compute()" << std::endl;

		for( auto data : *datas ) {

			current_data = data;

			int l = labels->size();
			int s = current_data->getStrs()->size();
			int capacity = l*s;

			current_vctab = createCheckTable(capacity);

			int maxd = - 1;
			App::Label maxy;
			double maxV = - std::numeric_limits<double>::max();

			for( auto y : *labels ) {

				int d = -1;
				double v = V(s-1, y, d);

				if( maxV < v ) {
					maxy = y;
					maxV = v;
					maxd = d;
				}
			}

			if( flg & ENABLE_LIKELIHOOD_ONLY ) {
				Logger::out(1) << boost::format("WG(maxV)= %10.6e") % maxV << std::endl;
			}
			assert( 0 < maxd );
			backtrack(maxy, maxd);
			printV();
		}
	}

	double Predictor::V(int i, App::Label y, int& maxd)
	{
		double maxV = - std::numeric_limits<double>::max();

		if( -1 < i ) {

			int idx = (i*labels->size()) + (static_cast<int>(y));
			auto& tp = current_vctab->at(idx);

			if( std::get<0>(tp) ) {

				maxd = std::get<2>(tp);
				maxV = std::get<1>(tp);

			} else {

				maxd = -1;
				App::Label maxyd;

				for( int d = 1; d <= std::min(maxLength, i+1); d++ ) {
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
				std::get<0>(tp) = true;
				std::get<1>(tp) = maxV;
				std::get<2>(tp) = maxd;
				std::get<3>(tp) = maxyd;
			}

		} else if( i == -1 ) {

			maxV = 0.0;

		} else {
			assert( -2 < i ); // T.B.D.
		}

		Logger::out(3) << "V(i=" << i << ", y=" << int(y) << ")=" << maxV << std::endl;
		return maxV;
	}

	void Predictor::backtrack(App::Label maxy, int maxd)
	{
		Logger::out(3) << "Predictor::backtrack()" << std::endl;

		int l = labels->size();
		int s = current_data->getStrs()->size();
		int i = s-1;
		int idx = i*l + (int)maxy;
		auto& tp0 = current_vctab->at(idx);
		maxd = std::get<2>(tp0);
		App::Label maxyd = std::get<3>(tp0);

		std::list<Segment> ls;

		while(1) {

			Segment seg = createSegment(i-maxd+1, i, maxy);
			ls.push_front(seg);

			i -= maxd;
			if( i < 0 ) {
				assert( i == -1 );
				break;
			}

			idx = i*l + (int)maxyd;
			auto& tp = current_vctab->at(idx);
			maxy = maxyd;
			maxd = std::get<2>(tp);
			maxyd = std::get<3>(tp);
		}

		for( auto s : ls ) {
			current_data->getSegments()->push_back(s);
		}
	}

	void Predictor::printV()
	{
		if( flg & ENABLE_LIKELIHOOD_ONLY ) {

			int l = labels->size();
			int s = current_data->getStrs()->size();
			for( auto y : *labels ) {
				for( int i = 0; i < s; i++ ) {

					int idx = i*l + (static_cast<int>(y));
					auto& tp = current_vctab->at(idx);
					double maxV = std::get<1>(tp);
					int maxd = std::get<2>(tp);
					App::Label maxyd = std::get<3>(tp);
					Logger::out(1) << boost::format("(%+10.6e %2d %2d)") % maxV % maxd % (int)maxyd << " ";
				}
				Logger::out(1) << "" << std::endl;
			}
		}
	}
}
