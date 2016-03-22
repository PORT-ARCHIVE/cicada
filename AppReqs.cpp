// © 2016 PORT INC.

#include <boost/lexical_cast.hpp>
#include "AppReqs.hpp"
#include "DebugOut.hpp"

namespace App {

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
			Logger::out() << "warning: unknown label" << std::endl; // T.B.D.
			return Label::NONE;
		}
	}

	std::string label2String(Label label) {
		return std::move( std::string("NONE") );
	}

	SemiCrf::FeatureFunction createFeatureFunction()
	{
		SemiCrf::FeatureFunction ff0(new AppReqF0());
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

}
