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

	decltype( std::make_shared<FeatureFunction>() )
	createFeatureFunction(const std::string& feature, const std::string& w2vmat)
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
			ff = jpnff;

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

	std::string label2String(Label label)
	{
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

	double Digit::wg
	( Weights ws
	  , Label y
	  , Label yd
	  , Data& x
	  , int j
	  , int i
	  , uvector& gs
		)
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

				auto str = x.getStrs()->at(j+l).at(0);
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

	double Jpn::wg
	( Weights ws
	  , Label y
	  , Label yd
	  , Data& x
	  , int j
	  , int i
	  , uvector& gs
		)
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

		uvector fvec(dim2, 0.0);

		try {

			// y2x
			for( l = 0; l < d; l++ ) {

				auto str = x.getStrs()->at(j+l).at(0);
				long long xval = boost::lexical_cast<long long>(str);
				const auto& wvec = w2vmat->i2v(xval);

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
}
