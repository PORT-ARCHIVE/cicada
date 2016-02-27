#ifndef APPLICATION_REQUIREMENTS__H
#define APPLICATION_REQUIREMENTS__H

#include "SemiCrf.hpp"

namespace ApplicationRequirements {

	class Campany : public SemiCrf::Label_ {};
	class Location : public SemiCrf::Label_ {};

	class AppReqF0 : public SemiCrf::FeatureFunction_ {
	public:
		virtual void operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, SemiCrf::Data d) {};
	};
	class AppReqF1 : public SemiCrf::FeatureFunction_ {
	public:
		virtual void operator() (SemiCrf::Segment s0, SemiCrf::Segment s1, SemiCrf::Data d) {};
	};
}

#endif // APPLICATION_REQUIREMENTS__H
