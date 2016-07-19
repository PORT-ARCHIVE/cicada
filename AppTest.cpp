// © 2016 PORT INC.

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include "MultiByteTokenizer.hpp"
#include "W2V.hpp"

namespace App {

	AreaDic_::AreaDic_()
	{
		Logger::trace() << "AreaDic_()";
	}

	AreaDic_::~AreaDic_()
	{
		Logger::trace() << "~AreaDic_()";
	}

	void AreaDic_::read(std::string file)
	{
		Logger::trace() << "AreaDic_::read()";

		typedef boost::char_separator<char> char_separator;
		typedef boost::tokenizer<char_separator> tokenizer;

		std::ifstream ifs;
		open(ifs, file);
		Logger::out()->info( "read {}", file );

		std::string line;
		while( std::getline(ifs, line) ) {

			//std::cout << line << std::endl;

			char_separator sep(",", "", boost::keep_empty_tokens);
			tokenizer tokens(line, sep);

			tokenizer::iterator tok_iter = tokens.begin();

			// 8カラムまでとばす
			for(int i = 0; i < 7; ++i) {
				tok_iter++;
			}

			if( dic.find(*tok_iter) == dic.end() ){
				dic.insert(*tok_iter);
				Logger::out()->trace( "area word: {}", *tok_iter );
				//std::cout << *tok_iter << std::endl;
			}

			// 10カラムまでとばす
			tok_iter++;
			tok_iter++;

			if( dic.find(*tok_iter) == dic.end() ){
				dic.insert(*tok_iter);
				Logger::out()->trace( "area word: {}", *tok_iter );
				//std::cout << *tok_iter << std::endl;
			}

			// 12カラムまでとばす
		    tok_iter++;
			tok_iter++;

			if( *tok_iter == "" ) {
				continue;
			}

			Logger::out()->trace( "area word: {}", *tok_iter );

			if( dic.find(*tok_iter) == dic.end() ){
				dic.insert(*tok_iter);
				Logger::out()->trace( "area word: {}", *tok_iter );
				//std::cout << *tok_iter << std::endl;
			}
		}

		Logger::out()->info( "the number of words: {}", dic.size() );
	}

	///////////////

	decltype( std::make_shared<FeatureFunction>() )
	createFeatureFunction(const std::string& feature, const std::string& w2vmat, const std::string& areaDic)
	{
		decltype( std::make_shared<FeatureFunction>() ) ff;

		if( feature == "DIGIT" || feature.empty() ) {

			ff = std::make_shared<Digit>();

		} else if( feature == "JPN" ) {
			
			auto jpnff = std::make_shared<Jpn>();

			auto m = std::make_shared<W2V::Matrix_>();
			if( w2vmat.empty() ) {
				throw Error("no w2v matrix file specifed");
			}
			m->read(w2vmat);
			jpnff->setMatrix(m);

			auto dic = std::make_shared<AreaDic_>();
			if( areaDic.empty() ) {
				throw Error("no area dictionary file specifed");
			}
			dic->read(areaDic);
			jpnff->setAreaDic(dic);

			ff = jpnff;

		} else {

			throw Error("unsupported feature specifed");
		}

		return ff;
	}

	bool AreaDic_::exist(std::string word)
	{
		bool ret = true;
		if( dic.find(word) == dic.end() ) {
			ret = false;
		}
		return ret;
	}

	///////////////

	Digit::Digit()
	{
		feature = "DIGIT";
		Logger::trace() << "Digit()";
	}

	Digit::~Digit()
	{
		Logger::trace() << "~Digit()";
	}

	int Digit::getDim()
	{
		return yDim * ( xDim + yDim + 1 );
	}

	void Digit::read()
	{
		Logger::trace() << "Digit::read()";
	}

	void Digit::write()
	{
		Logger::trace() << "Digit::write()";
	}

	double Digit::wg (
		Weights& ws,
		Label y,
		Label yd,
		Data& x,
		int j,
		int i,
		uvector& gs )
	{
		assert(0 < xDim);
		assert(0 < yDim);

		double v = 0.0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			int dim0 = yDim * xDim;
			int dim1 = yDim * ( xDim + yDim );
			int dim2 = yDim * ( xDim + yDim + 1 );

			uvector fvec(dim2, 0.0);

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) {

				const auto& str = x.getStrs()->at(j+l).at(0);
				int xval = boost::lexical_cast<int>(str);
				fvec(yval*xDim+xval) += 1.0;
			}

            // y2y
			fvec(dim0+ydval*yDim+yval) = 1.0;

            // y2l
			auto m = x.getMean(static_cast<int>(y));
			auto s = x.getVariance(static_cast<int>(y));
			const double eps = 1.0e-5;
			double f = 0.0;
			if( eps < s ) {
				auto dm = d - m;
				f = dm*dm/(2.0*s);
			} else {
				f = 1.0;
			}

			fvec(dim1+yval) = f;

			int k = 0;
			for( auto w : ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Digit::wg: unexpected exception");
		}

		return v;
	}

	///////////////

	Jpn::Jpn()
	{
		feature = "JPN";
		Logger::trace() << "Jpn()";
	}

	Jpn::~Jpn()
	{
		Logger::trace() << "~Jpn()";
	}

	int Jpn::getDim()
	{
		return yDim * ( xDim + yDim + 4 );
	}

	void Jpn::setXDim(int arg)
	{
		if( w2vmat->getSize() != arg ) {
			throw Error("dimension mismatch");
		}
		xDim = arg;
	}

	void Jpn::read()
	{
		Logger::trace() << "Jpn::read()";
	}

	void Jpn::write()
	{
		Logger::trace() << "Jpn::write()";
	}

	double Jpn::wg (
		Weights& ws,
		Label y,
		Label yd,
		Data& x,
		int j,
		int i,
		uvector& gs )
	{
		assert(0 < xDim);
		assert(0 < yDim);

		double v = 0.0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			int dim0 = yDim * xDim;
			int dim1 = yDim * ( xDim + yDim );
			int dim2 = yDim * ( xDim + yDim + 4 );

			uvector fvec(dim2, 0.0);

			int is_area = 0;
			int is_area_indicator = 0;
			int is_address = 0;

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) {

				const auto& str = x.getStrs()->at(j+l).at(0);
				long long xval = boost::lexical_cast<long long>(str);
				int s = x.getStrs()->at(j+l).size();
				std::string word = x.getStrs()->at(j+l).at(s-1); // 学習、推論で word のカラムが違うが、どちらにしろ最後に入っている

				// 地域
				if( areadic->exist(word) ) {
					is_area = 1;
				}

				// 数字
				if( word.find("digit") != std::string::npos ||
					word.find("-") != std::string::npos ||
					word.find("ー") != std::string::npos ) {
					is_address = 1;
				}

				// 地域指示子
				if( ( word.find("勤務") != std::string::npos && word.find("地") != std::string::npos ) ||
					( word.find("勤務") != std::string::npos && word.find("先") != std::string::npos ) ||
					word.find("アクセス") != std::string::npos ) {
					is_area_indicator = 1;
				}


				if( xval == -1 ) {
					if( unknown_words.find(word) == unknown_words.end() ) {
						Logger::out()->warn( "unknown word: {}", word );
						unknown_words.insert(word);
					}
					continue;
				}
				const auto& wvec = w2vmat->i2v(xval);
				for( int k = 0; k < xDim; k++ ) {
					fvec(yval*xDim+k) += wvec(k);
				}
			}

            // y2y
			fvec(dim0+ydval*yDim+yval) = 1.0;

			// y2l
			auto m = x.getMean(static_cast<int>(y));
			auto s = x.getVariance(static_cast<int>(y));
			const double eps = 1.0e-5;
			double f = 0.0;
			if( eps < s ) {
				auto dm = d - m;
				f = dm*dm/(2.0*s);
			} else {
				f = 1.0;
			}

			fvec(dim1+yval) = f;

			if( yval == 3 ) { // 勤務地 T.B.D.
				fvec(dim1+yval+1) = i;
			}

			if( yval == 18 ) { // 番地 T.B.D.
				fvec(dim1+yval+2) = is_address;
			}

			if( yval == 19 ) { // 勤務地指示子 T.B.D.
				fvec(dim1+yval+3) = is_area_indicator;
			}

			int k = 0;
			for( auto w : ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Jpn::wg: unexpected exception");
		}

		return v;
	}
}
