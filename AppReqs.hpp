#ifndef APP_REQS__H
#define APP_REQS__H

#include "SemiCrf.hpp"
#include "DebugOut.hpp"

namespace AppReqs {

	enum class Label : int {
		NONE = 0,
		CAMPANY,
		OCCUPATION,
		SALARY,
		LOCATION
	};

	class AppReqF0 : public SemiCrf::FeatureFunction_ {
	public:

		AppReqF0(){ Debug::out() << "AppReqF0()" << std::endl; }
		virtual ~AppReqF0(){ Debug::out() << "~AppReqF1()" << std::endl; }

		// virtual double operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, SemiCrf::Strs strs) {
		// 	std::cout << "AppReqF0::operator()" << std::endl;
			
		// 	// T.B.D.
		// 	int start0 = s0->getStart();
		// 	int end0 = s0->getEnd();
		// 	AppReqs::Label l0 = s0->getLabel();

		// 	// T.B.D.
		// 	int start1 = s1->getStart();
		// 	int end1 = s1->getEnd();
		// 	AppReqs::Label l1 = s1->getLabel();

		// 	// T.B.D.					
		// 	for(auto s : *strs) {
		// 		std::cout << s << std::endl;
		// 		// T.B.D.		
		// 	}
			
		// 	return true;
		// };

		virtual void read() {
			Debug::out() << "AppReqF0::read()" << std::endl;
		};

		virtual void write() {
			Debug::out() << "AppReqF0()::write()" << std::endl;
		};

		virtual double operator() (AppReqs::Label y, AppReqs::Label yd, SemiCrf::Data x, int j, int i) {
			return (0.0);
		}
	};
	
	class AppReqF1 : public SemiCrf::FeatureFunction_ {
	public:

		AppReqF1(){ Debug::out() << "AppReqF1()" << std::endl; }
		virtual ~AppReqF1(){ Debug::out() << "~AppReqF1()" << std::endl; }

		// virtual double operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, SemiCrf::Strs strs) {
		// 	std::cout << "AppReqF1::operator()" << std::endl;

		// 	// T.B.D.
		// 	return true;
		// };

		virtual void read() {
			Debug::out() << "AppReqF1::read()" << std::endl;
		};

		virtual void write() {
			Debug::out() << "AppReqF0::write()" << std::endl;
		};

		virtual double operator() (AppReqs::Label y, AppReqs::Label yd, SemiCrf::Data x, int j, int i) {
			return (0.0);
		}
	};
}

#endif // APP_REQS__H
