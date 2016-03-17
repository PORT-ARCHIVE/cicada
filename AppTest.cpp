// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include "AppTest.hpp"
#include "DebugOut.hpp"
#include "Error.hpp"

namespace App {

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
			if( k < 100 ) {

				int col = k % 5;
				int row = k % 10;
				int xval = boost::lexical_cast<int>(x->getStrs()->at(0).at(j+col));

				if( col <= i-j && col == xval ) {

					if( ( yval == 0 &&  row < 5  )
					 || ( yval == 1 &&  5 <= row ) ) {

						ret = 1;
					}

				} else {

					ret = 0;
				}

			// y2y
			} else {

				int index = k - 100;
				int col = index % 5;
				int row = ( index - 5 ) / 5;

				if( yval == row && ydval == col ) {

					ret = 1;

				} else {

					ret = 0;
				}
			}

		} catch (...) {

			throw Error("Simple::operator(): unexpected exception");
		}

		return ret;
	}
}
