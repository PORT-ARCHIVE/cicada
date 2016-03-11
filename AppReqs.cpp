
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

}
