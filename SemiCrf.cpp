
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
		std::cout << "Data_()" << std::endl;
	}

	Data_::~Data_() {
		std::cout << "~Data_()" << std::endl;
	}

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

	// Datas ctr
	Datas_::Datas_()
	{
		std::cout << "Datas_()" << std::endl;
	};

	Datas_::~Datas_()
	{
		std::cout << "~Datas_()" << std::endl;
	};

	Datas createDatas() {
		return Datas( new Datas_() );
	}

	// str0をstr1をデリミタとして区切りn番目の区間を返す
	std::string nfind(const std::string& str0, const std::string& str1, int n) {

		int match = 0;
		size_t pos0 = 0;
		do {
			pos0 = str0.find(str1, pos0);
			pos0++;
			match++;
		} while( match != n && pos0 != std::string::npos );

		size_t pos1 = str0.find(str1, pos0);
		std::string m1 = str0.substr(pos0, pos1-pos0);
		return m1;
	}

	void Datas_::read(const char* input) {
		std::cout << "Datas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		Data data( new Data_() );
		std::string delimita(",");
		typedef std::shared_ptr<MeCab::Tagger> Tagger;
		Tagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger(""));
		const MeCab::Node* node = tagger->parseToNode(input);

		for( ; node ; node = node->next ) {

			std::string cppstr = node->feature;
			// std::cout << cppstr << std::endl;
			// std::string m = nfind(cppstr, delimita, 6);
			// std::cout << m << std::endl;
			// data->getStrs()->push_back(m);
			data->getStrs()->push_back(cppstr);
		}

		push_back(data);
	}

	void Datas_::write() const {
		std::cout << "Datas_::write()" << std::endl;
		for( auto i : *this ) {
			i->write();
		}
	}

	// TrainingDatas ctr
	TrainingDatas_::TrainingDatas_()
	{
		std::cout << "TrainingDatas_()" << std::endl;
	}

	TrainingDatas_::~TrainingDatas_()
	{
		std::cout << "~TrainingDatas_()" << std::endl;
	}

	Datas createTrainingDatas()
	{
		return Datas( new TrainingDatas_() );
	}

    // 文字列を置換する
	std::string Replace( std::string String1, std::string String2, std::string String3 ){
		std::string::size_type  Pos( String1.find( String2 ) );
		while( Pos != std::string::npos ) {
			String1.replace( Pos, String2.length(), String3 );
			Pos = String1.find( String2, Pos + String3.length() );
		}
		return String1;
	}

	class MultiByteIterator {
	public:
		MultiByteIterator(std::string arg);
		virtual ~MultiByteIterator();
		char operator * ();
		operator const char* ();
		MultiByteIterator& operator ++();
		void setBuf();

	private:
		char* buf;
		char* p;
	};

	MultiByteIterator::MultiByteIterator(std::string arg) {
		std::cout << "MultiByteIterator()" << std::endl;
		buf = new char[MB_CUR_MAX];
		p = const_cast<char*>(arg.c_str());
		setBuf();
	}

	MultiByteIterator::~MultiByteIterator() {
		std::cout << "~MultiByteIterator()" << std::endl;
		delete [] buf;
	}

	char MultiByteIterator::operator * () {
		return buf[0];
	}

	MultiByteIterator::operator const char* () {
		return buf;
	}

	MultiByteIterator& MultiByteIterator::operator ++() {
		setBuf();
		return (*this);
	}

	void MultiByteIterator::setBuf() {
		int i = 0;
		int s = mblen(p, MB_CUR_MAX);
		for( i = 0; i < s; i++ ) {
			buf[i] = *(p++);
		}
		buf[i] = '\0';
	}

	class MultiByteTokenizer {
	public:
		// MultiByteTokenizer(MultiByteIterator& arg);
		MultiByteTokenizer(std::string str);
		virtual ~MultiByteTokenizer();
		std::string get();

	private:
		//MultiByteIterator& itr;
		MultiByteIterator itr;
	};

	// MultiByteTokenizer::MultiByteTokenizer(MultiByteIterator& arg)
	// 	: itr(arg)
	// {
	// 	std::cout << "MultiByteTokenizer()" << std::endl;
	// }

	MultiByteTokenizer::MultiByteTokenizer(std::string str)
	 	: itr(str)
	{
	 	std::cout << "MultiByteTokenizer()" << std::endl;
	}

	MultiByteTokenizer::~MultiByteTokenizer()
	{
		std::cout << "~MultiByteTokenizer()" << std::endl;
	}

	std::string MultiByteTokenizer::get() {
		char c = *itr;
		int cmp = strcmp(itr, " ");
		while( strcmp(itr, " ") == 0 || strcmp(itr, "　") == 0 ) {
			++itr;
		}
		c = *itr;
		std::string token;
		while( strcmp(itr, " ") != 0 &&
			   strcmp(itr, "　") != 0 &&
			   strcmp(itr, "\0") != 0 ) {
			token.push_back(*itr);
			c = *itr;
			++itr;
			c = *itr;
		}
		c = *itr;
		return token;
	}

	void TrainingDatas_::read(const char* input) {
		std::cout << "Datas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8");

		std::ifstream ifs;
		ifs.open("test_data/test0.txt");

		std::string str;
		while(std::getline(ifs,str)) {
			if( str[0] == '#' ) {
				continue;
			}

			std::cout<< str << std::endl;

			// MultiByteIterator itr(str);
			// MultiByteTokenizer tokenizer(itr);
			MultiByteTokenizer tokenizer(str);
			std::string tok = tokenizer.get();
			std::cout << tok << std::endl;
			while( !tok.empty() ) {
				tok = tokenizer.get();
			 	std::cout << tok << std::endl;
			}

			// MultiByteIterator mi(str);
			// while( *mi ) {
			//  	std::cout << mi << std::endl;
			//  	++mi;
			// }
		}

		//push_back(data);
		ifs.close();
	}

	// InferenceDatas ctr
	InferenceDatas_::InferenceDatas_()
	{
		std::cout << "InferenceDatas_()" << std::endl;
	}

	InferenceDatas_::~InferenceDatas_()
	{
		std::cout << "~InferenceDatas_()" << std::endl;
	}

	Datas createInferenceDatas()
	{
		return Datas( new InferenceDatas_() );
	}

	void InferenceDatas_::read(const char* input) {
		std::cout << "Datas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		Data data( new Data_() );
		std::string delimita(",");
		typedef std::shared_ptr<MeCab::Tagger> Tagger;
		Tagger tagger = std::shared_ptr<MeCab::Tagger>(MeCab::createTagger(""));
		const MeCab::Node* node = tagger->parseToNode(input);

		for( ; node ; node = node->next ) {

			std::string cppstr = node->feature;
			// std::cout << cppstr << std::endl;
			// std::string m = nfind(cppstr, delimita, 6);
			// std::cout << m << std::endl;
			// data->getStrs()->push_back(m);
			data->getStrs()->push_back(cppstr);
		}

		push_back(data);
	}

	// Weights ctr
	Weights createWeights() { return Weights( new Weights_() ); }

	void Weights_::read() {
		std::cout << "Weights_::read()" << std::endl;
		push_back(1.0);
		push_back(2.0);
	}

	void Weights_::write() {
		std::cout << "Weights_::write()" << std::endl;
	}

	// FeatureFunctions ctr
	FeatureFunctions createFeatureFunctions() { return FeatureFunctions( new FeatureFunctions_() ); }	

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

	Algorithm_::Algorithm_()
		: labels( nullptr )
		, ffs( nullptr )
		, weights( nullptr )
		, datas( nullptr )
	{
		std::cout << "Algorithm_()" << std::endl;
	}

	Algorithm_::~Algorithm_()
	{
		std::cout << "~Algorithm_()" << std::endl;
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

	Algorithm createLearner() { return std::shared_ptr<Learner>(new Learner()); }

	void Learner::compute() {

		std::cout << "Learner::compute()" << std::endl;
		assert( weights->size() == ffs->size() );
		int l = labels->size();
		std::vector<double> dL(datas->size());
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
	}

	std::vector<double>&& Learner::computeG() {

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
