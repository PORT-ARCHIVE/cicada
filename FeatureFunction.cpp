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
		const int FEATURE_DIM = 6;

		try {

			for( int l = 0; l < d; l++ ) {
				int s = x.getStrs()->at(j+l).size();
				word.push_back(x.getStrs()->at(j+l).at(s-1));
				// 学習、推論で word のカラムが違うが、どちらにしろ最後に入っている
			}

			int fd = yval*FEATURE_DIM;

			{ // 勤務地指示子

				static std::string chi("地");
				static std::string kinmu("勤務");
				static std::string syozai("所在");

				if( word[0] == kinmu || word[0] == syozai ) {

					fvec(fd) += 1.0;

					if( 1 < d && word[1] == chi ) {

						fvec(fd) += 1.0;
					}
				}
			}

			fd++;

			{ // 勤務地

				static std::vector<std::string> key { "都", "道", "府","県","市","区","町","郡","字" };

				for( auto& w : word ) {

					if( isDelimiter(w) ) {
						fvec(fd) = 0.0;
						break;
					}

					if( areadic->exist(w) ) {
						fvec(fd) += 1.0;
						continue;
					}

					for( auto& k : key ) {
						if( w == k ) {
							fvec(fd) += 1.0;
							break;
						}
					}
				}
			}

			fd++;

			{ // 番地

				static std::vector<std::string> key { "丁目","番","地","号","ー","-","0","1","2","3","4","5","6","7","8","9" };

				fvec(fd) = d; // できるだけ長く

				for( auto& w : word ) {

					if( isDelimiter(w) ) {
						fvec(fd) = 0.0;
						break;
					}

					for( auto& k : key ) {
						if( w.find(k) == std::string::npos ) {
							fvec(fd) += 1.0;
							break;
						} else {
							fvec(fd) = 0.0;
							goto EXIT;
						}
					}
				}
			EXIT:;
			}

			fd++;

			{ // 施設名

				fvec(fd) = d; // できるだけ長く

				for( auto& w : word ) {

					// if( w.find("タワー") != std::string::npos ) {
					// 	fvec(3) += 1.0;
					// }

					if( isDelimiter(w) ) {
						fvec(fd) = 0.0;
						break;
					}
				}
			}

			fd++;

			{ // 階数

				static std::vector<std::string> key { "F","階","0","1","2","3","4","5","6","7","8","9" };

				for( auto& w : word ) {

					if( isDelimiter(w) ) {
						fvec(fd) = 0.0;
						break;
					}

					for( auto& k : key ) {
						if( w == k ) {
							fvec(fd) += 1.0;
							break;
						}
					}
				}
			}

			{ // デリミタ

				if( d == 1 && isDelimiter(word[0]) ) {
					fvec(fd) += 1.0;
				}
			}

			// else if( label_map.find(0) != label_map.end() && yval == label_map[0] ) { // 無し
			// 	fvec(6) += 1.0;
		    // }

		} catch (...) {
			throw Error("Jpn::wg: y2x: unexpected exception");
		}

		// y2y
		fvec(yDim*FEATURE_DIM+ydval*yDim+yval) = 1.0;

		// y2l
		// try {

		// 	auto m = x.getMean(static_cast<int>(y));
		// 	auto s = x.getVariance(static_cast<int>(y));
		// 	const double eps = 1.0e-5;
		// 	double f = 0.0;
		// 	if( eps < s ) {
		// 		auto dm = d - m;
		// 		f = dm*dm/(2.0*s);
		// 	} else {
		// 		f = 1.0;
		// 	}

		// 	fvec(xDim+yval) = f;

		// } catch (...) {
		// 	throw Error("Jpn::wg: y2l: unexpected exception");
		// }

		// innner product
		try {

			// if( Logger::getLevel() < 2 ) {

			// 	std::string segw {""};
			// 	for( auto& w : word ) {
			// 		segw += w;
			// 	}

			// 	Logger::out()->debug( "word: {}", segw );

			// 	std::stringstream ss;
			// 	ss << ydval << " " << yval << " : " ;
			// 	for( int i = 0; i < xDim; i++ ) {
			// 		ss << fvec(i) << " ";
			// 	}

			// 	Logger::out()->debug( "word: {}", ss.str() );
			// }

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
