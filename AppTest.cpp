// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "AppTest.hpp"
#include "Logger.hpp"
#include "Error.hpp"
#include "MultiByteTokenizer.hpp"
#include "W2V.hpp"

namespace App {

    typedef boost::numeric::ublas::vector<double> vector;

	SemiCrf::FeatureFunction createFeatureFunction(const std::string& feature, const std::string& w2vmat)
	{
		SemiCrf::FeatureFunction ff;

		if( feature == "DIGIT" || feature.empty() ) {

			SemiCrf::FeatureFunction digitFeature(new Digit());
			ff = digitFeature;

		} else if( feature == "JPN" ) {
			
			std::shared_ptr<Jpn> jpnFeature(new Jpn());
			W2V::Matrix m(new W2V::Matrix_());
			if( w2vmat.empty() ) {
				throw Error("no w2v matrix file specifed");
			}
			m->read(w2vmat); // T.B.D.
			jpnFeature->setMatrix(m);
			ff = jpnFeature;

		} else {

			throw Error("unsupported feature specifed");
		}

		return ff;
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

	///////////////

	Digit::Digit()
	{
		feature = "DIGIT";
		Logger::out(2) << "Digit()" << std::endl;
	}

	Digit::~Digit()
	{
		Logger::out(2) << "~Digit()" << std::endl;
	}

	void Digit::read()
	{
		Logger::out(2) << "Digit::read()" << std::endl;
	}

	void Digit::write()
	{
		Logger::out(2) << "Digit::write()" << std::endl;
	}

	double Digit::operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i)
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
				throw Error("Digit::operator(): invalid dimension specified");
			}

		} catch (...) {
			throw Error("Digit::operator(): unexpected exception");
		}

		return ret;
	}

	double Digit::wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		assert(0 < xDim);
		assert(0 < yDim);

		double v = 0.0;

		try {

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			int dim0 = yDim * xDim;
			int dim1 = yDim * ( xDim + yDim );

			vector fvec(dim1, 0.0);

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) { 

				std::string str = x->getStrs()->at(j+l).at(0);
				int xval = boost::lexical_cast<int>(str);
				fvec(yval*xDim+xval) += 1.0;
			}

            // y2y
			fvec(dim0+yval*yDim+yval) += 1.0;

			if( ydval != yval ) {
				fvec(dim0+ydval*yDim+yval) += 1.0;
			}

            // y2l
			// fvec(dim1+d) += 1.0;

			int k = 0;
			for( auto w : *ws ) {
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
		Logger::out(2) << "Jpn()" << std::endl;
	}

	Jpn::~Jpn()
	{
		Logger::out(2) << "~Jpn()" << std::endl;
	}

	void Jpn::read()
	{
		Logger::out(2) << "Jpn::read()" << std::endl;
	}

	void Jpn::write()
	{
		Logger::out(2) << "Jpn::write()" << std::endl;
	}

	double Jpn::operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i)
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
				throw Error("Jpn::operator(): invalid dimension specified");
			}

		} catch (...) {
			throw Error("Jpn::operator(): unexpected exception");
		}

		return ret;
	}

	double Jpn::wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		assert(0 < xDim);
		assert(0 < yDim);

		double v = 0.0;

		try {

			int dim0 = yDim * xDim;
			int dim1 = dim0 + yDim * yDim;
			int dim2 = dim2 + yDim * ws->getMaxLength();

			int yval = static_cast<int>(y);
			int ydval = static_cast<int>(yd);

			vector fvec(dim2);

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) { 

				std::string str = x->getStrs()->at(j+l).at(0);
				int xval = boost::lexical_cast<int>(str);
				const vector& wvec = w2vmat->i2v(xval);

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
			throw Error("Jpn::operator(): unexpected exception");
		}

		return v;
	}

    ///////////////

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
