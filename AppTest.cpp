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
			throw Error("unknown label");
		}
	}

	SemiCrf::FeatureFunction createFeatureFunction()
	{
		SemiCrf::FeatureFunction ff(new Simple());
		return ff;
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

				if( ydval == row && yval == col ) {
					ret = 1;
				}

			} else {
				throw Error("Simple::operator(): invalid dimension specified");
			}

		} catch (...) {
			throw Error("Simple::operator(): unexpected exception");
		}

		return ret;
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

		if( data->getStrs()->empty() ) {
			throw Error("empty input file"); // T.B.D.
		}
	}
}
