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
	Labels createLabels(int size =0)
	{
		return Labels( new Labels_(size) );
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
	Labels_::Labels_(int size)
	{
		Logger::out(2) << "Labels_()" << std::endl;
		for( int i = 0; i < size; i++ ) {
			push_back(i);
		}
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

				output << App::label2String(l) << std::endl;
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
		, maxLength(-std::numeric_limits<int>::max())
		, feature("")
	{
		Logger::out(2) << "Datas_()" << std::endl;
	};

	Datas_::~Datas_()
	{
		Logger::out(2) << "~Datas_()" << std::endl;
	};

	void Datas_::write(std::ostream& output)  const {
		Logger::out(2) << "Datas_::write()" << std::endl;
		if( !feature.empty() ) {
			output << "# FEATURE" << " " << feature << std::endl;
		}
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
					} else if( tok == "FEATURE" ) {
						tok = tokenizer.get();
						if( !tok.empty() ) {
							feature = tok;
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
					int length = counter - seg_start + 1;
					if( maxLength < length ) {
						maxLength = length;
					}

				} else if( descriptor == "S/E" ) {

					seg	= createSegment(counter, counter, l);
					data->getSegments()->push_back(seg);
					if( maxLength < 1 ) {
						maxLength = 1;
					}

				} else {

					Logger::out(1) << "warning: unknown descriptor" << std::endl;
				}
			}

			std::string remains = tokenizer.get();
			if( !remains.empty() ) {
				// T.B.D.
			}
		}

		if( empty() ) {
			throw Error("empty training data file"); // T.B.D.
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

		if( empty() ) {
			throw Error("empty inference data file"); // T.B.D.
		}
	}

	// Weights ctr
	Weights createWeights(int dim)
	{
		return Weights( new Weights_(dim) );
	}

	Weights_::Weights_(int dim)
		: std::vector<double>(dim)
		, xDim(-1)
		, yDim(-1)
		, maxLength(-1)
		, feature("")
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
					} else if( tok == "MAXLENGTH" ) {
						tok = tokenizer.get();
						if( !tok.empty() ) {
							maxLength = boost::lexical_cast<int>(tok);
						} else {
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
			throw Error("dimension mismatch"); // T.B.D.
		}

		if( empty() ) {
			throw Error("empty weights file"); // T.B.D.
		}
	}

	void Weights_::write(std::ofstream& ofs)
	{
		Logger::out(2) << "Weights_::write()" << std::endl;

		ofs << "# Semi-CRF Weights" << std::endl;
		if( !feature.empty() ) {
			ofs << std::endl;
			ofs << "# FEATURE" << " " << feature << std::endl;
		}
		ofs << std::endl;
		ofs << "# DIMENSION" << " " << xDim << " " << yDim << std::endl;
		ofs << std::endl;
		ofs << "# MAXLENGTH" << " " << maxLength << std::endl;
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
		, method("bfgs")
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

	void Algorithm_::setMethod(std::string arg)
	{
		method = arg;
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

	static int debug = 0;

	double Algorithm_::computeWG(App::Label y, App::Label yd, int i, int d, vector& gs)
	{
		double v = ff->wg(weights, y, yd, current_data, i-d+1, i, gs);
		return v;
	}

	void Algorithm_::setFlg(int arg)
	{
		flg = arg;
	}

	//// Learner ////

	Algorithm createLearner(int arg)
	{
		return Learner(new Learner_(arg));
	}

	Learner_::Learner_(int arg)
		: Algorithm_(arg)
	{
		Logger::out(2) << "Learner_()" << std::endl;
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

	Learner_::~Learner_()
	{
		Logger::out(2) << "~Learner_()" << std::endl;
		Logger::out(1) << "OK" << std::endl;
	}

	void Learner_::preProcess(const std::string& wfile, const std::string& w0file, const std::string& w2vfile)
	{
		int xdim = datas->getXDim();
		int ydim = datas->getYDim();
		const std::string& feature = datas->getFeature();
		ff = App::createFeatureFunction(feature, w2vfile);
		ff->setXDim(xdim);
		ff->setYDim(ydim);
		ff->setMaxLength(maxLength);
		dim = ff->getDim();

		SemiCrf::Weights weights = SemiCrf::createWeights(dim);
		setWeights(weights);

		if( !w0file.empty() ) {
			std::ifstream ifs; // 入力
			open(ifs, w0file);
			weights->resize(0);
			weights->read(ifs);

		}

		weights->setFeature(feature);
		Labels labels = createLabels(ydim);
		setLabels(labels);
	}

	void Learner_::postProcess(const std::string& wfile)
	{
		if( !(flg & ENABLE_LIKELIHOOD_ONLY) ) {
			std::ofstream ofs; // 出力
			open(ofs, wfile);
			weights->setXDim(datas->getXDim());
			weights->setYDim(datas->getYDim());
			weights->setMaxLength(maxLength);
			weights->write(ofs);
		}
	}

	void Learner_::compute()
	{
		Logger::out(2) << "Learner_::compute()" << std::endl;

		if( !(flg & ENABLE_LIKELIHOOD_ONLY) ){

			Optimizer::ObjectiveFunction ofunc = createLikelihood(this);
			Optimizer::UnconstrainedNLP optimizer;

			if( method == "bfgs" ) {
				optimizer = createBfgs(dim, ofunc);
			} else if( method == "steepest_decent" ) {
				optimizer = createSteepestDescent(dim, ofunc);
			}

			int f = 0x0;
			if( !(DISABLE_ADAGRAD & flg) ){
				f |= Optimizer::ENABLE_ADAGRAD;
			}

			optimizer->setFlg(f);
			optimizer->setE0(e0);
			optimizer->setRe(e1);
			optimizer->setAe(e1);
			optimizer->setMaxIteration(maxIteration);
			optimizer->optimize();

		} else {

			double L = 0.0;
			std::vector<double> dL(dim, 0.0);
			computeGrad(L, dL);
		}
	}

	void Learner_::computeGrad(double& L, std::vector<double>& dL, bool grad)
	{
		for( auto data : *datas ) {

			current_data = data;

			double WG = 0.0;
			double Z = computeZ();
			auto Gs = computeG(WG);

			L += WG - log(Z);
			if( flg & ENABLE_LIKELIHOOD_ONLY ) {
				Logger::out(1) << boost::format("L= %+10.6e WG= %+10.6e logZ= %+10.6e") % L % WG % log(Z) << std::endl;
			}

			if( grad ) {

				auto Gms = computeGm(Z);
				auto idL = dL.begin();
				for( int k = 0; k < dim; k++, idL++ ) {
					(*idL) += Gs[k] - Gms[k]; Logger::out(2) << "dL(" << k << ")=" << *idL << std::endl;
				}
			}
		}
	}

	std::vector<double> Learner_::computeG(double& WG)
	{
		std::vector<double> Gs;
		auto segments = current_data->getSegments();
		assert( 0 < segments->size() );

		Gs.resize(dim);
		auto si = segments->begin();
		auto y1 = App::ZERO;
		for( ; si != segments->end(); si++ ){

			auto y = (*si)->getLabel();
			int ti = (*si)->getStart();
			int ui = (*si)->getEnd();
			vector gs(dim);
			WG += computeWG(y, y1, ui, ui-ti+1, gs);
			int k = 0;
			for( auto& g : Gs ) {
				g += gs(k++);
			}
			y1 = y;
		}

		int k = 0;
		for( auto g : Gs ) {
			Logger::out(2) << "G(" << k++ << ")=" << g << std::endl;
		}

		if( flg & ENABLE_LIKELIHOOD_ONLY ) {

			auto si = segments->begin();
			double awg = 0.0;

			auto y1 = App::ZERO;
			for( ; si != segments->end(); si++ ){

				double wg = 0.0;
				auto y = (*si)->getLabel();
				int ti = (*si)->getStart();
				int ui = (*si)->getEnd();
				vector gs(dim);
				wg = computeWG(y, y1, ui, ui-ti+1, gs);

				y1 = y;
				awg += wg;

				Logger::out(1) << boost::format( "(%3d,%3d)" ) % ti % ui;
				Logger::out(1) << boost::format(" WG= %+10.6e AWG= %+10.6e") % wg % awg << std::endl;
			}
		}

		return(std::move(Gs));
	}

	double Learner_::computeZ()
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

	std::vector<double> Learner_::computeGm(double Z)
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

	double Learner_::alpha(int i, App::Label y)
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

						if( i == 0 && yd != App::ZERO ) {
							continue;
						}

						double alp = alpha(i-d, yd);
						vector gs(dim);
						double wg = computeWG(y, yd, i, d, gs);
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

	double Learner_::eta(int i, App::Label y, int k)
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

						if( i == 0 && yd != App::ZERO ) {
							continue;
						}

						vector gs(dim);
						double wg = computeWG(y, yd, i, d, gs);
						double cof = eta(i-d, yd, k) + alpha(i-d, yd) * gs(k);
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

	//// Likilihood class ////

	Optimizer::ObjectiveFunction createLikelihood(Learner_* learner)
	{
	 	return Optimizer::ObjectiveFunction(new Likelihood_(learner));
	}

	double Likelihood_::value(Optimizer::vector& x)
	{
		int i = 0;
		for( auto& w : *(learner->weights) ) {
			w = x[i++];
		}

		L = 0.0;
		std::vector<double> dL(learner->dim);
		learner->computeGrad(L, dL, false);

		return (-L); //
	}

	double Likelihood_::savedValue()
	{
		return (-L); //
	}

	Optimizer::vector Likelihood_::grad(Optimizer::vector& x)
	{
		int i;

		i = 0;
		for( auto& w : *(learner->weights) ) {
			w = x[i++];
		}

		L = 0.0;
		std::vector<double> dL(learner->dim);
		learner->computeGrad(L, dL, true);

		Optimizer::vector g(learner->dim);

		i = 0;
		for( auto& idL : dL ) {
			g(i++) = (-idL); //
		}

		return std::move(g);
	}

	void Likelihood_::preProcess(Optimizer::vector& x)
	{
		int i = 0;
		for( auto& w : *(learner->weights) ) {
			x[i++] = w;
		}
	}

	void Likelihood_::beginLoopProcess(Optimizer::vector& x)
	{
	}

	void Likelihood_::afterUpdateXProcess(Optimizer::vector& x)
	{
	}

	void Likelihood_::endLoopProcess(Optimizer::vector& x)
	{
	}

	void Likelihood_::postProcess(Optimizer::vector& x)
	{
		int i = 0;
		for( auto& w : *(learner->weights) ) {
			w = x[i++];
		}
	}

	//// Predictor ////

	Algorithm createPredictor(int arg)
	{
		return Predictor(new Predictor_(arg));
	}

	Predictor_::Predictor_(int arg)
		: Algorithm_(arg)
	{
		Logger::out(2) << "Predictor_()" << std::endl;
		Logger::out(1) << "Semi-CRF";
		if( flg & DISABLE_DATE_VERSION ) {
			Logger::out(1) << "" << std::endl;
		} else {
			Logger::out(1) << " 0.0.1" << std::endl;
			Logger::out(1) << "" << date() << std::endl;
		}
		Logger::out(1) << "Prediction ..." << std::endl;
	}

	Predictor_::~Predictor_()
	{
		Logger::out(2) << "~Predictor_()" << std::endl;
		Logger::out(1) << "OK" << std::endl;
	}

	void Predictor_::preProcess(const std::string& wfile, const std::string& w0file, const std::string& w2vfile)
	{
		SemiCrf::Weights weights = SemiCrf::createWeights();
		std::ifstream ifs; // 入力
		open(ifs, wfile);
		weights->read(ifs);
		setWeights(weights);
		if( maxLength < 1 ) {
			int ml = weights->getMaxLength();
			if( 0 < ml ) {
				setMaxLength(ml);
			} else {
				throw Error("could not determine maxLength");
			}
		}

		int xdim = weights->getXDim();
		int ydim = weights->getYDim();
		const std::string& feature = weights->getFeature();
		ff = App::createFeatureFunction(feature, w2vfile);
		ff->setXDim(xdim);
		ff->setYDim(ydim);
		ff->setMaxLength(maxLength);
		dim = ff->getDim();
		if( weights->size() != dim ) {
			throw Error("warning: feature mismatch");
		}
		datas->setXDim(xdim);
		datas->setYDim(ydim);
		if( datas->getFeature() != feature ) {
			Logger::out(1) << "warning: feature mismatch" << std::endl;
		}
		datas->setFeature(feature);
		Labels labels = createLabels(ydim);
		setLabels(labels);
	}

	void Predictor_::postProcess(const std::string& wfile)
	{
		datas->write(Logger::out(0) << "");
	}

	void Predictor_::compute()
	{
		Logger::out(2) << "Predictor_::compute()" << std::endl;

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

	double Predictor_::V(int i, App::Label y, int& maxd)
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

						if( i == 0 && yd != App::ZERO ) {
							continue;
						}

						int tmp = -1;
						double v = V(i-d, yd, tmp);
						vector gs(dim);
						v += computeWG(y, yd, i, d, gs);
					
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

	void Predictor_::backtrack(App::Label maxy, int maxd)
	{
		Logger::out(3) << "Predictor_::backtrack()" << std::endl;

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

	void Predictor_::printV()
	{
		if( flg & ENABLE_LIKELIHOOD_ONLY ) {

			int l = labels->size();
			int s = current_data->getStrs()->size();
			for( int i = 0; i < s; i++ ) {
				for( auto y : *labels ) {

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
