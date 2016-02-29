#ifndef APPLICATION_REQUIREMENTS__H
#define APPLICATION_REQUIREMENTS__H

#include "SemiCrf.hpp"

namespace ApplicationRequirements {

	class Campany : public SemiCrf::Label_{};
	class Location : public SemiCrf::Label_{};

	class AppReqF0 : public SemiCrf::FeatureFunction_ {
	public:

		AppReqF0(){ std::cout << "AppReqF0()" << std::endl; }
		virtual ~AppReqF0(){ std::cout << "~AppReqF1()" << std::endl; }

		virtual bool operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, std::vector<std::string>&& strs) {
			std::cout << "AppReqF0::operator()" << std::endl;
			
			// T.B.D.
			int start0 = s0->getStart();
			int end0 = s0->getEnd();
			SemiCrf::Label l0 = s0->getLabel();

			// T.B.D.			
			int start1 = s1->getStart();
			int end1 = s1->getEnd();
			SemiCrf::Label l1 = s1->getLabel();

			// T.B.D.						
			for(auto s : strs) {
				std::cout << s << std::endl;
				// T.B.D.							
			}
			
			return true;
		};

		virtual void read() {
			std::cout << "AppReqF0::read()" << std::endl;
		};

		virtual void write() {
			std::cout << "AppReqF0()::write()" << std::endl;			
		};
	};
	
	class AppReqF1 : public SemiCrf::FeatureFunction_ {
	public:

		AppReqF1(){ std::cout << "AppReqF1()" << std::endl; }
		virtual ~AppReqF1(){ std::cout << "~AppReqF1()" << std::endl; }

		virtual bool operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, std::vector<std::string>&& strs) {
			std::cout << "AppReqF1::operator()" << std::endl;

			// T.B.D.										
			return true;
		};

		virtual void read() {
			std::cout << "AppReqF1::read()" << std::endl;			
		};

		virtual void write() {
			std::cout << "AppReqF0::write()" << std::endl;			
		};		
	};
}

#endif // APPLICATION_REQUIREMENTS__H
