// © 2016 PORT INC.

#include <cstdlib>
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

	std::string AreaDic_::removePrefecture(std::string area, bool& is_remove) {
		is_remove = false;
		area = area.substr(1, area.size()-2); // 先頭、末尾の"を削除
		int l = area.size();
		//const char* dist[] = { "都","県","府","道","市","区","町","村","郡","字" };
		const char* dist[] = { "都","県","府","道","市" };
		int size = sizeof(dist)/sizeof(char*);

		for( int i = 0; i < size; ++i ) {
			int s = mblen(dist[i], MB_CUR_MAX);
			std::string distcpp(dist[i]);
			if( area.find(distcpp, l-s) != std::string::npos ) {
				area = area.substr(0, l-s); // 末尾の行政区分を削除
				is_remove = true;
				break;
			}
		}

		return std::move(area);
	}

	void AreaDic_::read(std::string file)
	{
		Logger::trace() << "AreaDic_::read()";

		typedef boost::char_separator<char> char_separator;
		typedef boost::tokenizer<char_separator> tokenizer;

		std::ifstream ifs;
		open(ifs, file);
		Logger::out()->info( "read {}", file );
		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

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

			bool flg0 = false;
			std::string word0 = removePrefecture(*tok_iter, flg0);

			if( dic.find(word0) == dic.end() && flg0 ){
				dic.insert(word0);
				Logger::out()->trace( "area word: {}", word0 );
				//std::cout << *tok_iter << std::endl;
			}

			// 10カラムまでとばす
			tok_iter++;
			tok_iter++;

			bool flg1 = false;
			std::string word1 = removePrefecture(*tok_iter, flg1);

			if( dic.find(word1) == dic.end() && flg1 ){
				dic.insert(word1);
				Logger::out()->trace( "area word: {}", word1 );
				//std::cout << *tok_iter << std::endl;
			}

			// 以降のデータは使わない(暫定)
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

			if( !areaDic.empty() ) {
				auto dic = std::make_shared<AreaDic_>();
				dic->read(areaDic);
				jpnff->setAreaDic(dic);
			}

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
		int dim = yDim * ( xDim + yDim ) + yDim;
		if( considerArea ) {
			dim += yDim * 3;
		}
		return dim;
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

		int yval = static_cast<int>(y);
		int ydval = static_cast<int>(yd);

		int dim0 = 0;
		if( !considerArea ) {

			dim0 = yDim * xDim;         // y2x

		} else {

			dim0 = yDim * ( xDim + 3 ); // y2x
		}

		int dim1 = dim0 + yDim * yDim;  // y2x + y2y
		int dim2 = dim1 + yDim;         // y2x + y2y + l

		uvector fvec(dim2, 0.0);

		int is_area = 0;
		int is_address = 0;
		int is_area_indicator = 0;

		int d = i - j + 1;

		// y2x
		try {

			for( int l = 0; l < d; l++ ) {

				const auto& str = x.getStrs()->at(j+l).at(0);
				long long xval = boost::lexical_cast<long long>(str);
				int s = x.getStrs()->at(j+l).size();
				std::string word = x.getStrs()->at(j+l).at(s-1); // 学習、推論で word のカラムが違うが、どちらにしろ最後に入っている
				//std::cout << word << std::endl;

				if( considerArea ) {

					// 地域
					if( areadic->exist(word) ||
						word.find("都") != std::string::npos ||
						word.find("県") != std::string::npos ||
						word.find("府") != std::string::npos ||
						word.find("道") != std::string::npos ||
						word.find("市") != std::string::npos ||
						word.find("区") != std::string::npos ||
						word.find("町") != std::string::npos ||
						word.find("村") != std::string::npos ||
						word.find("郡") != std::string::npos ||
						word.find("字") != std::string::npos ) {
						is_area = 1;
					}

					// 数字
					if( word.find("digit") != std::string::npos ||
						word.find("丁") != std::string::npos ||
						word.find("目") != std::string::npos ||
						word.find("番") != std::string::npos ||
						word.find("地") != std::string::npos ||
						word.find("号") != std::string::npos ||
						word.find("-") != std::string::npos ||
						word.find("ー") != std::string::npos ) {
						is_address = 1;
					}

					// 地域指示子
					if( word.find("勤務") != std::string::npos ||
						word.find("地") != std::string::npos ||
						word.find("先") != std::string::npos ||
						word.find("アクセス") != std::string::npos ) {
						is_area_indicator = 1;
					}
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

			// 平均化
			for( int k = 0; k < xDim; k++ ) {
				fvec(yval*xDim+k) /= d;
			}

			// area
			if( considerArea ) {

				try {

					int base = yval*xDim+xDim;

					if( yval == label_map[3] ) { // 勤務地 T.B.D.
						fvec(base) = is_area;
					}

					if( yval == label_map[18] ) { // 番地 T.B.D.
						fvec(base+1) = is_address;
					}

					if( yval == label_map[19] ) { // 勤務地指示子 T.B.D.
						fvec(base+2) = is_area_indicator;
					}

				} catch (...) {
					throw Error("Jpn::wg: area: unexpected exception");
				}
			}

		} catch (...) {
			throw Error("Jpn::wg: y2x: unexpected exception");
		}

		// y2y
		try {

			fvec(dim0+ydval*yDim+yval) = 1.0;

		} catch (...) {
			throw Error("Jpn::wg: y2y: unexpected exception");
		}

		// y2l
		try {

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

		} catch (...) {
			throw Error("Jpn::wg: y2l: unexpected exception");
		}

		// innner product
		try {

			int k = 0;
			for( auto w : ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Jpn::wg: innner product: unexpected exception");
		}

		return v;
	}
}
