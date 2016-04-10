// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include "MultiByteTokenizer.hpp"

namespace App {

    typedef boost::numeric::ublas::vector<double> vector;

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

	Label string2Label(const std::string& str)
	{
		int val;
		try {
			val = boost::lexical_cast<int>(str);
		} catch(...) {
			throw Error("unknown label");
		}
		return ( static_cast<Label>(val) );
	}

	std::string label2String(Label label) {
		std::string str;
		try {
			str = boost::lexical_cast<std::string>(label);
		} catch(...) {
			throw Error("unknown label");
		}
		return ( std::move(str) );
	}

	SemiCrf::FeatureFunction createFeatureFunction()
	{
		SemiCrf::FeatureFunction ff(new Simple());
		return ff;
	}

	double Simple::operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		assert(0 < xDim);
		assert(0 < yDim);

		int ret = 0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			int dim0 = yDim * xDim;
			int dim1 = yDim * ( xDim + yDim );

			// y2x
			if( k < dim0 ) {

				int col = k % xDim;
				int row = k / xDim;
				int d = i - j + 1;

				for( int l = 0; l < d; l++ ) {

					std::string str = x->getStrs()->at(j+l).at(0);
					int xval = boost::lexical_cast<int>(str);

					if( yval == row && xval == col ) {
						ret += 1;
					}
				}

			// y2y
			} else if( dim0 <= k && k < dim1 ) {

				int index = k - dim0;
				int col = index % yDim;
				int row = index / yDim;

				if( row == col && row == yval ) {

					ret = 1; // Sum of this value equals to the number of segments whose label is yval.

				} else if( ydval == row && yval == col ) {

					ret = 1; // Sum of this value equals to the number of transitions from ydval to yval
				}

			} else {
				throw Error("Simple::operator(): invalid dimension specified");
			}

		} catch (...) {
			throw Error("Simple::operator(): unexpected exception");
		}

		return ret;
	}

	double Simple::wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		assert(0 < xDim);
		assert(0 < yDim);

		double v = 0.0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			int dim0 = yDim * xDim;
			int dim1 = dim0 + yDim * yDim;
			//int dim2 = dim2 + yDim * maxLength;
			int dim2;

			vector fvec(dim2);

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) {

				std::string str = x->getStrs()->at(j+l).at(0);
				int xval = boost::lexical_cast<int>(str);
				//vector wvec = w2v.i2v(xval);
				vector wvec;

				for( int k = 0; k < dim0; k++ ) {
					fvec(k) += wvec(k);
				}
			}

            // y2y
			fvec(dim0+ydval*yDim+yval) += 1;

            // y2l
			fvec(dim1+d) += 1;

			int k = 0;
			for( auto w : *ws ) {
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Simple::operator(): unexpected exception");
		}

		return v;
	}

	void PridectionDigitDatas_::read(std::istream& strm)
	{
		int warningsLimit = 5;
		int numOfWarnings = 0;
		Logger::out(2) << "PridectionDigitDatas_::read()" << std::endl;

		setlocale(LC_CTYPE, "ja_JP.UTF-8"); // T.B.D.

		SemiCrf::Data data;
		std::string line;

		while( std::getline(strm, line) ) {

			if( line == "" ) {
				continue;
			}

			MultiByteTokenizer tokenizer(line);

			if( line[0] == '#' ) {
				if( line == "# BEGIN" ) {
					data = SemiCrf::Data( new SemiCrf::Data_() );
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
				throw Error("invalid format"); // T.B.D.
			} else {
				Logger::out(2) << word << std::endl;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string remains = tokenizer.get();
			if( !remains.empty() ) {
				if( numOfWarnings < warningsLimit ) {
					Logger::out(1) << "warning: Extra data has been detected." << std::endl;
					if( numOfWarnings == warningsLimit-1 ) {
						Logger::out(1) << "warning: repeated warnings suppressesed." << std::endl;
					}
				}
				++numOfWarnings;
			}
		}

		if( empty() ) {
			throw Error("empty inference data file"); // T.B.D.
		}
	}
}
