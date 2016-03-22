// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include "MultiByteTokenizer.hpp"

namespace App {

	Simple::Simple()
	{
		Logger::out(2) << "Simple()" << std::endl;
	}

	Simple::~Simple()
	{
		Logger::out(2) << "~Simple()" << std::endl;
	}

	void Simple::read()
	{
		Logger::out(2) << "Simple::read()" << std::endl;
	}

	void Simple::write()
	{
		Logger::out(2) << "Simple::write()" << std::endl;
	}

	Label string2Label(const std::string& str) {
		if( str == "0" ) {
			return Label::ZERO;
		} else if( str == "1" ) {
			return Label::ONE;
		} else {
			throw Error("warning: unknown label");
		}
	}

	std::string label2String(Label label) {
		if( label == Label::ZERO ) {
			return std::move(std::string("0"));
		} else if( label == Label::ONE ) {
			return std::move(std::string("1"));
		} else {
			throw Error("warning: unknown label");
		}
	}

	SemiCrf::FeatureFunction createFeatureFunction()
	{
		SemiCrf::FeatureFunction ff(new Simple());
		return ff;
	}

	int getFeatureDimention()
	{
		// return (35);
		return (8);
	}

	SemiCrf::Labels createLabels()
	{
		SemiCrf::Labels labels = SemiCrf::createLabels();
		labels->push_back(Label::ZERO);
		labels->push_back(Label::ONE);
		return labels;
	}

	double Simple::operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		int ret = 0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			// y2x
			if( k < 4 ) {

				int col = k % 2;
				int row = k < 2 ? 0 : 1;
				int d = i - j + 1;

				for( int l = 0; l < d; l++ ) {

					std::string str = x->getStrs()->at(j+l).at(0);
					int xval = boost::lexical_cast<int>(str);

					if( yval == row && xval == col ) {
						ret += 1;
					}
				}

			// y2y
			} else if( 4 <= k && k < 8 ) {

				int index = k - 4;
				int col = index % 4;
				int row = ( index - col ) / 2;

				if( ydval == row && yval == col ) {
					ret = 1;
				}

			} else {
				assert( k < 8 );
			}
#if 0
			// y2x
			if( k < 10 ) {

				int col = k % 5;
				int row = k < 5 ? 0 : 1;
				int d = i - j + 1;
				// T.B.D. should warn if d < 1 || maxlength < d

				for( int l = 0; l < d; l++ ) {

					std::string str = x->getStrs()->at(j+l).at(0);
					int xval = boost::lexical_cast<int>(str);
					if( yval == row && xval == col && col < d ) {
						ret += 1;
					}
				}

			// y2y
			} else {

				int index = k - 10;
				int col = index % 5;
				int row = ( index - col ) / 5;
				if( yval == row && ydval == col ) {
					ret = 1;
				}
			}
#endif

		} catch (...) {

			throw Error("Simple::operator(): unexpected exception");
		}

		return ret;
	}

	void PridectionDigitDatas_::read(std::istream& strm)
	{
		Logger::out(2) << "PridectionDigitDatas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

		SemiCrf::Data data;
		std::string line;

		while( std::getline(strm, line) ) {

			if( line == "" ) {
				continue;
			}

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					data = SemiCrf::Data( new SemiCrf::Data_() );
					Logger::out(2) << "BEGIN : data was created." << std::endl;
				} else if( line == "# END" ) {
					push_back(data);
					Logger::out(2) << "END : data was pushed." << std::endl;
				}
				continue;
			}

			MultiByteTokenizer tokenizer(line);

			std::string word = tokenizer.get();
			if( word.empty() ) {
				// T.B.D.
			} else {
				Logger::out(2) << word << std::endl;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string remains = tokenizer.get();
			if( !remains.empty() ) {
				// T.B.D.
			}
		}
	}
}
