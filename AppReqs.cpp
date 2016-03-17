// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include "AppReqs.hpp"
#include "DebugOut.hpp"

namespace AppReqs {

	Label string2Label(const std::string& str) {
		if( str == "NONE" || str == "-" || str.empty() ) {
			return Label::NONE;
		} else if( str == "COMPANY" ) {
			return Label::CAMPANY;
		} else if( str == "OCCUPATION" ) {
			return Label::OCCUPATION;
		} else if( str == "SALARY" ) {
			return Label::SALARY;
		} else if( str == "LOCATION" ) {
			return Label::LOCATION;
		} else {
			Debug::out() << "warning: unknown label" << std::endl; // T.B.D.
			return Label::NONE;
		}
	}

	std::string label2String(Label label) {
		return std::move( std::string("NONE") );
	}

	SemiCrf::FeatureFunction createFeatureFunction()
	{
		SemiCrf::FeatureFunction ff0(new Simple());
		return ff0;
	}

	SemiCrf::Labels createLabels()
	{
		SemiCrf::Labels labels = SemiCrf::createLabels();

		labels->push_back(Label::NONE);
		labels->push_back(Label::CAMPANY);
		labels->push_back(Label::LOCATION);

		return labels;
	}

	double Simple::operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i)
	{
		int ret = 0;
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

		return ret;
	}
}
