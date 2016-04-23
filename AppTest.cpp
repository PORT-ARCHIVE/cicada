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
		Logger::debug() << "Digit()";
	}

	Digit::~Digit()
	{
		Logger::debug() << "~Digit()";
	}

	int Digit::getDim()
	{
		return yDim * ( xDim + yDim + 1 );
	}

	void Digit::read()
	{
		Logger::debug() << "Digit::read()";
	}

	void Digit::write()
	{
		Logger::debug() << "Digit::write()";
	}

	double Digit::wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i, SemiCrf::vector& gs)
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

			vector fvec(dim2, 0.0);

			// y2x
			int d = i - j + 1;
			for( int l = 0; l < d; l++ ) { 

				std::string str = x->getStrs()->at(j+l).at(0);
				int xval = boost::lexical_cast<int>(str);
				fvec(yval*xDim+xval) += 1.0;
			}

            // y2y
			fvec(dim0+ydval*yDim+yval) = 1.0;

            // y2l
			double m = x->getMean(static_cast<int>(y));
			double s = x->getVariance(static_cast<int>(y));
			const double eps = 1.0e-5;
			double f = 0.0;
			if( eps < s ) {
				double dm = d - m;
				f = dm*dm/(2.0*s);
			} else {
				f = 1.0;
			}

			fvec(dim1+yval) = f;

			int k = 0;
			for( auto w : *ws ) {
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
		Logger::debug() << "Jpn()";
	}

	Jpn::~Jpn()
	{
		Logger::debug() << "~Jpn()";
	}

	int Jpn::getDim()
	{
		return yDim * ( xDim + yDim + maxLength );
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
		Logger::debug() << "Jpn::read()";
	}

	void Jpn::write()
	{
		Logger::debug() << "Jpn::write()";
	}

	double Jpn::wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i, SemiCrf::vector& gs)
	{
		assert(0 < xDim);
		assert(0 < yDim);

		int l, k;
		double v = 0.0;
		int yval = static_cast<int>(y);
		int ydval = static_cast<int>(yd);
		int dim0 = yDim * xDim;
		int dim1 = yDim * ( xDim + yDim );
		int dim2 = yDim * ( xDim + yDim + maxLength );
		int d = i - j + 1;

		vector fvec(dim2, 0.0);

		try {

			// y2x
			for( l = 0; l < d; l++ ) {

				std::string str = x->getStrs()->at(j+l).at(0);
				long long xval = boost::lexical_cast<long long>(str);
				const vector& wvec = w2vmat->i2v(xval);

				for( k = 0; k < xDim; k++ ) {
					fvec(yval*xDim+k) += wvec(k);
				}
			}

            // y2y
			fvec(dim0+ydval*yDim+yval) = 1.0;

            // y2l
			fvec(dim1+yval*maxLength+(d-1)) = 1.0;

			k = 0;
			for( auto w : *ws ) {
				gs(k) = fvec(k);
				v += w*fvec(k++);
			}

		} catch (...) {
			throw Error("Jpn::wg: unexpected exception");
		}

		return v;
	}

    ///////////////

	void PridectionDigitDatas_::writeJson(std::ostream& output) const
	{
		assert(0);
	}

	void PridectionDigitDatas_::read(std::istream& strm)
	{
		int warningsLimit = 5;
		int numOfWarnings = 0;
		Logger::debug() << "PridectionDigitDatas_::read()";

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
					Logger::debug() << "BEGIN : data was created.";
				} else if( line == "# END" ) {
					push_back(data);
					Logger::debug() << "END : data was pushed.";
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
				Logger::debug() << word;
				std::vector<std::string> vs;
				vs.push_back(word);
				data->getStrs()->push_back(vs);
			}

			std::string remains = tokenizer.get();
			if( !remains.empty() ) {
				if( numOfWarnings < warningsLimit ) {
					Logger::info() << "warning: Extra data has been detected.";
					if( numOfWarnings == warningsLimit-1 ) {
						Logger::info() << "warning: repeated warnings suppressesed.";
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
