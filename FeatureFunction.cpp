// © 2016 PORT INC.

#include <cstdlib>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "FeatureFunction.hpp"
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
			std::string word = *tokens.begin();
			dic.insert(word);
			Logger::out()->trace( "{}", word );
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

			// auto m = std::make_shared<W2V::Matrix_>();
			// if( w2vmat.empty() ) {
			// 	throw Error("no w2v matrix file specifed");
			// }
			// m->read(w2vmat);
			// jpnff->setMatrix(m);

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

	const int Jpn::FEATURE_DIM = 2;

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
		return (6 + yDim)*yDim;
	}

	void Jpn::setXDim(int arg)
	{
		// if( w2vmat->getSize() != arg ) {
		// 	throw Error("dimension mismatch");
		// }
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

	bool Jpn::isDelimiter(const std::string& word)
	{
		static std::vector<std::string> delmiters { ",","、",":","：","/","／" };
		for( auto d : delmiters	) {
			if( word.find(d) != std::string::npos ) {
				return true;
			}
		}
		return false;
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
		int d = i - j + 1;
		uvector fvec(getDim(), 0.0);
		std::vector<std::string> word;

		try {

			for( int l = 0; l < d; l++ ) {
				int s = x.getStrs()->at(j+l).size();
				word.push_back(x.getStrs()->at(j+l).at(s-1));
				// 学習、推論で word のカラムが違うが、どちらにしろ最後に入っている
			}

			static std::vector<std::string> prefectures { "都", "道", "府","県" };
			static std::string city("市");
			static std::string ward("区");
			static std::string town("町");
			static std::string village("村");
			static std::string county("郡");
			static std::string sub_ward("字");

			int fd = yval*FEATURE_DIM;
			int feature = 0;
			int is_delimiter = 0;
			int is_area = 0;
			int is_head_area = 0;
			int is_prefecture = 0;
			int is_head_prefecture = 0;
			int is_city = 0;
			int is_ward = 0;
			int is_town = 0;
			int is_village = 0;
			int is_county = 0;
			int is_sub_ward = 0;

			bool is_first = true;
			for( auto& w : word ) {

				if( isDelimiter(w) ) {
					is_delimiter++;
				}

				if( areadic->exist(w) ) {
					is_area++;
					if( is_first ) {
						is_head_area = 1;
					}
				}

				for( auto& p : prefectures ) {
					if( w == p ) {
						is_prefecture++;
						if( is_first ) {
							is_head_prefecture = 1;
						}
					}
				}

				if( w == city ) {
					is_city++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				if( w == ward ) {
					is_ward++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				if( w == town ) {
					is_town++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				if( w == village ) {
					is_village++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				if( w == county ) {
					is_county++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				if( w == sub_ward ) {
					is_sub_ward++;
					if( is_first ) {
						is_head_prefecture = 1;
					}
				}

				is_first = false;
			}

			feature += is_area;
			feature += is_prefecture;
			feature += is_city;
			feature += is_ward;
			feature += is_town;
			feature += is_village;
			feature += is_county;
			feature += is_sub_ward;

			// デリミタを含む、先頭は地名でない　
			if( 0 < is_delimiter || (!is_head_area ) ) {

				feature = 0; // あり得ない

			} else if( 1 < is_prefecture || // 都,道,府,県を2以上含む
				1 < is_city || // 市を2以上含む
				1 < is_town || // 町を2以上含む
				1 < is_village || // 村を2以上含む
				1 < is_county || // 郡を2以上含む
				1 < is_sub_ward || // 区を2以上含む
				is_head_prefecture ) { // 先頭が行政区分

				feature *= 0.1; // 可能性は低い
			}

			if( 0 < feature ) { // "地名"

				fvec(fd) = feature;
			}

			fd++;

			if( v == 0 ) { // "なし"

				fvec(fd) = 1;
			}

		} catch (...) {
			throw Error("Jpn::wg: y2x: unexpected exception");
		}

		// y2y
		fvec(yDim*FEATURE_DIM+ydval*yDim+yval) = 1.0;

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
