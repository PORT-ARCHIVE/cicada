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

	Dictonary_::Dictonary_()
	{
		Logger::trace() << "Dictonary_()";
	}

	Dictonary_::~Dictonary_()
	{
		Logger::trace() << "~Dictonary_()";
	}

	void Dictonary_::read(const std::string& file)
	{
		Logger::trace() << "Dictonary_::read()";

		typedef boost::char_separator<char> char_separator;
		typedef boost::tokenizer<char_separator> tokenizer;

		std::ifstream ifs;
		open(ifs, file);
		Logger::out()->info( "parsing... {}", file );
		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

		std::string line;
		while( std::getline(ifs, line) ) {

			char_separator sep(",", "", boost::keep_empty_tokens);
			tokenizer tokens(line, sep);
			std::string word = *tokens.begin();
			if( word[0] == '#' ) {
				continue;
			}
			dic.insert(word);
			Logger::out()->trace( "{}", word );
		}

		Logger::out()->info( "# of words: {}", dic.size() );
	}

	bool Dictonary_::exist(const std::string& word)
	{
		bool ret = true;
		if( dic.find(word) == dic.end() ) {
			ret = false;
		}
		return ret;
	}

	JobDictonary_::JobDictonary_()
	{
		Logger::trace() << "JobDictonary_()";
	}

	JobDictonary_::~JobDictonary_()
	{
		Logger::trace() << "~JobDictonary_()";
	}

	void JobDictonary_::read(const std::string& file)
	{
		Logger::trace() << "Dictonary_::read()";

		typedef boost::char_separator<char> char_separator;
		typedef boost::tokenizer<char_separator> tokenizer;

		std::ifstream ifs;
		open(ifs, file);
		Logger::out()->info( "parsing... {}", file );
		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

		std::string line;
		while( std::getline(ifs, line) ) {

			char_separator sep(",","", boost::keep_empty_tokens);
			tokenizer tokens(line, sep);
			auto it = tokens.begin();
			std::string word = *it++;
			if( word[0] == '#' ) {
				continue;
			}
			std::string flg = *it++;
			dic.insert(std::make_pair(word,flg));
			Logger::out()->trace( "{}", word );
		}

		Logger::out()->info( "# of words: {}", dic.size() );
	}

	bool JobDictonary_::exist(const std::string& word, bool& is_person)
	{
		bool ret = true;
		is_person = false;

		auto it = dic.find(word);
		if( it == dic.end() ) {
			ret = false;
		} else if( it->second == "1" ) {
			is_person = true;
		}

		return ret;
	}

	///////////////

	decltype( std::make_shared<FeatureFunction>() )
	createFeatureFunction(const std::string& feature, const std::string& w2vmat, const std::string& areaDic, const std::string& jobDic)
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
				auto dic = std::make_shared<Dictonary_>();
				dic->read(areaDic);
				jpnff->setAreaDic(dic);
			}

			if( !jobDic.empty() ) {
				auto dic = std::make_shared<JobDictonary_>();
				dic->read(jobDic);
				jpnff->setJobDic(dic);
			}

			ff = jpnff;

		} else {

			throw Error("unsupported feature specifed");
		}

		return ff;
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
			for( const auto& w : ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Digit::wg: unexpected exception");
		}

		return v;
	}

	///////////////

	const int Jpn::FEATURE_DIM = 11;

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
		return (FEATURE_DIM + yDim)*yDim;
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

	std::vector<std::string> Jpn::open_brakets { "(","{","[","（","｛","「","【" };
	std::vector<std::string> Jpn::close_brakets { ")","}","]","）","｝","」","】" };
	std::vector<std::string> Jpn::delmiters { ",","、",":","：","/","／","・" };

	double Jpn::front_delimiter_feature(const std::vector<std::string>& words)
	{
		double ret = 0.0;

		if( words.empty() )
			return ret;

		const auto& w = words.back();
		for( const auto& d : delmiters	) {
			if( w == d ) {
				ret = 1.0;
				break;
			}
		}

		return ret;
	}

	double Jpn::back_delimiter_feature(const std::vector<std::string>& words)
	{
		double ret = 0.0;

		if( words.empty() )
			return ret;

		const auto& w = words.front();
		for( const auto& d : delmiters	) {
			if( w == d ) {
				ret = 1.0;
				break;
			}
		}

		return ret;
	}

	double Jpn::place_feature(const std::vector<std::string>& words)
	{
		static std::set<std::string> prefecture_divisions { "都","道","府","県","州","省","国","王国" };
		static std::set<std::string> sub_divisions { "市","区","町","村","郡","字","大字","小字" };
		static std::set<std::string> prefecture_names
		{
		  "北海","青森","岩手","宮城","秋田","山形","福島","茨城","栃木",
 		  "群馬","埼玉","千葉","東京都","神奈川","新潟","富山","石川","福井",
		  "山梨","長野","岐阜","静岡","愛知","三重","滋賀","京都","大阪","兵庫",
		  "奈良","和歌山","鳥取","島根","岡山","広島","山口","徳島","香川","愛媛","高知",
		  "福岡","佐賀","長崎","熊本","大分","宮崎","鹿児島","沖縄"
		};

		double feature = 0;
		int is_area = 0;
		int is_head_area = 0;
		int is_prefecture = 0;
		int is_head_prefecture = 0;
		int is_none_area_relate	= 0;
		int is_prefecture_name = 0;
		int is_station = 0;
		std::map<std::string, int> is_sub_division;
		std::string prefecture_name;

		int i = 0;
		int s = words.size();
		bool is_first = true;
		for( const auto& w : words ) {

			bool is_none_area_relate_check = true;

			// 都道府県州省のどれか
			if( prefecture_divisions.find(w) != prefecture_divisions.end() ) {
				is_prefecture++;
				if( is_first ) {
					is_head_prefecture = 1;
				}
				is_none_area_relate_check = false;
			}

			// 地名
			if( areadic->exist(w) ) {
				is_area++;
				if( is_first ) {
					is_head_area = 1;
				}
				if( prefecture_names.find(w) != prefecture_names.end() && prefecture_name != w ) {
					prefecture_name = w;
				 	is_prefecture_name++;
				}
				is_none_area_relate_check = false;
			}

			// 市町村郡(大/小)字のどれか
			if( sub_divisions.find(w) != sub_divisions.end() ) {
				is_sub_division[w]++;
				if( is_first ) {
					is_head_prefecture = 1;
				}
				is_none_area_relate_check = false;
			}

			if( w == "駅" && 1 < s && i == s-1 ) {
				is_station++;
			}

			// 地名関連語ではない
			if( is_none_area_relate_check ) {
				is_none_area_relate++;
			}

			is_first = false;
			i++;
		}

		feature += is_area;
		feature += is_prefecture;
		feature += is_station;
		for( const auto& p : is_sub_division ) {
			feature += p.second;
		}

		// 先頭が地名でない、地名関連語以外を含む
		if( !is_head_area || is_none_area_relate ) {

			feature = 0; // あり得ない

		} else if( 1 < is_prefecture || // 行政区分(都,道,府,県)を2以上含む
				   is_head_prefecture || // 先頭が行政区分
				   1 < is_prefecture_name ) { // 異なる都道府県名を含む

			feature *= 0.1; // 可能性は低い

		} else {

			for( const auto& p : is_sub_division ) {
				if( 1 < p.second ) { // 同じ行政区分(市町村郡字)を2以上含む
					feature *= 0.1; // 可能性は低い
					break;
				}
			}
		}

		return feature;
	}

	double Jpn::place_indicator_feature(const std::vector<std::string>& words)
	{
		static std::set<std::string> place_indicators
		{ "最寄駅", "最寄り駅", "アクセス", "所在地", "本社", "支社", "オフィス", "住所", "勤務地", "勤務先", "勤務場所", "就業先", "就業場所" };

		double feature = 0.0;

		for( const auto& w : words ) {

			if( place_indicators.find(w) != place_indicators.end() ) {
				feature += 1.0;
			}
		}

		return feature;
	}

	void Jpn::job_feature(const std::vector<std::string>& words, double& jfp, double& jfw)
	{
		jfp = 0.0;
		jfw = 0.0;

		for( const auto& w : words ) {

			// 職種
			bool is_person = false;
			if( jobdic.get() && jobdic->exist(w, is_person) ) {
				if( is_person ) {
					jfp++;
				} else {
					jfw++;
				}
			}
		}

		if( 1.0 < jfp ) {
			jfp *= 0.1;
		}
	}

	double Jpn::back_job_feature(const std::vector<std::string>& words)
	{
		double ret = 0.0;

		if( words.empty() )
			return ret;

		const auto& w = words.back();
		if( w == "求人" || w == "募集" || w == "募集要項" ) {
			ret = 1.0;
		}

		return ret;
	}

	double Jpn::job_indicator_feature(const std::vector<std::string>& words)
	{
		static std::set<std::string> job_indicators
		{ "募集", "仕事", "業務", "職務", "職種", "区分", "内容", "カテゴリ", "科目", "分類", "概要" };

		double feature = 0.0;

		for( const auto& w : words ) {

			if( job_indicators.find(w) != job_indicators.end() ) {
				feature += 1.0;
			}
		}

		return feature;
	}

	double Jpn::front_bracket_feature(const std::vector<std::string>& words)
	{
		double ret = 0.0;

		if( words.empty() )
			return ret;

		const auto& w = words.back();
		for( const auto& ob : open_brakets ) {
			if( w == ob ) {
				ret = 1.0;
				break;
			}
		}

		return ret;
	}

	double Jpn::back_bracket_feature(const std::vector<std::string>& words)
	{
		double ret = 0.0;

		if( words.empty() )
			return ret;

		const auto& w = words.front();
		for( const auto& cb : close_brakets ) {
			if( w == cb ) {
				ret = 1.0;
				break;
			}
		}

		return ret;
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
		assert(0 < yDim);

		const double eps = 1e-3;
		const int w = 10;

		double v = 0.0;
		int yval = static_cast<int>(y);
		int ydval = static_cast<int>(yd);
		int d = i - j + 1;
		uvector fvec(getDim(), 0.0);
		std::vector<std::string> words;
		std::vector<std::string> pre_words;
		std::vector<std::string> post_words;

		try {

			auto& xs = *x.getStrs();

			for( int i = 0; i < d; i++ ) {
				words.push_back(xs.at(j+i).back());
				// 学習、推論で words のカラムが違うが、どちらにしろ最後に入っている
			}

			for( int i = 0; i < w; i++ ) {
				int pos = j-w+i;
				if( -1 < pos ) {
					pre_words.push_back(xs.at(pos).back());
				}
			}

			for( int i = 0; i < w; i++ ) {
				int pos = j+d+i;
				if( pos < xs.size() ) {
					post_words.push_back(xs.at(pos).back());
				}
			}

			int fd = yval*FEATURE_DIM;
			double pf = place_feature(words);
			double pif0 = place_indicator_feature(pre_words);
			double pif1 = place_indicator_feature(words);
			double jfp = 0.0;
			double jfw = 0.0;
			job_feature(words, jfp, jfw);
			double bjf = back_job_feature(post_words);
			double jif0 = job_indicator_feature(pre_words);
			double jif1 = job_indicator_feature(words);
			double fbf = front_bracket_feature(pre_words);
			double bbf = back_bracket_feature(post_words);
			double fdf = front_delimiter_feature(pre_words);
			double bdf = back_delimiter_feature(post_words);

			fvec(fd++) = pif0;
			fvec(fd++) = pif1;
			fvec(fd++) = pf;
			fvec(fd++) = jif0;
			fvec(fd++) = jif1;
			fvec(fd++) = bjf;
			fvec(fd++) = jfp;
			fvec(fd++) = jfw;
			fvec(fd++) = fbf;
			fvec(fd++) = bbf;
			fvec(fd++) = fdf;
			fvec(fd++) = bdf;
#if 0
			for( const auto& s : words ) {
				std::cout << s;
			}
			std::cout << " ";
			for( int i = yval*FEATURE_DIM; i < yval*FEATURE_DIM+FEATURE_DIM; i++ ) {
				std::cout << fvec(i) << " ";
			}
			std::cout << std::endl;
#endif
		} catch (...) {
			throw Error("Jpn::wg: y2x: unexpected exception");
		}

		// y2y
		fvec(yDim*FEATURE_DIM+ydval*yDim+yval) = 1.0;

		// innner product
		try {

			int k = 0;
			for( const auto& w : ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Jpn::wg: innner product: unexpected exception");
		}

		return v;
	}
}
