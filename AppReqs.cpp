
#include "AppReqs.hpp"
#include "DebugOut.hpp"

namespace AppReqs {

	Label string2Label(std::string str) {
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

	SemiCrf::FeatureFunctions createFeatureFunctions()
	{
		SemiCrf::FeatureFunctions ffs = SemiCrf::createFeatureFunctions();
		SemiCrf::FeatureFunction ff0(new AppReqs::AppReqF0());
		SemiCrf::FeatureFunction ff1(new AppReqs::AppReqF1());

		ffs->push_back(ff0);
		ffs->push_back(ff1);

		return ffs;
	}

	SemiCrf::Labels createLabels()
	{
		SemiCrf::Labels labels = SemiCrf::createLabels();

		labels->push_back(AppReqs::Label::NONE);
		labels->push_back(AppReqs::Label::CAMPANY);
		labels->push_back(AppReqs::Label::LOCATION);

		return labels;
	}
}
