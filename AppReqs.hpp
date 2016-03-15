// Copyright:: Copyright 2016 PORT INC.
// Author:: Jiro Iwai

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

		AppReqF0(){ Debug::out() << "AppReqF0()" << std::endl; };
		virtual ~AppReqF0(){ Debug::out() << "~AppReqF1()" << std::endl; };

		virtual void read() {
			Debug::out() << "AppReqF0::read()" << std::endl;
		};

		virtual void write() {
			Debug::out() << "AppReqF0()::write()" << std::endl;
		};

		virtual double operator() (AppReqs::Label y, AppReqs::Label yd, SemiCrf::Data x, int j, int i) {
			return (0.0);
		};
	};
	
	class AppReqF1 : public SemiCrf::FeatureFunction_ {
	public:

		AppReqF1(){ Debug::out() << "AppReqF1()" << std::endl; };
		virtual ~AppReqF1() { Debug::out() << "~AppReqF1()" << std::endl; };

		virtual void read() {
			Debug::out() << "AppReqF1::read()" << std::endl;
		};

		virtual void write() {
			Debug::out() << "AppReqF0::write()" << std::endl;
		};

		virtual double operator() (AppReqs::Label y, AppReqs::Label yd, SemiCrf::Data x, int j, int i) {
			return (0.0);
		};
	};

	SemiCrf::FeatureFunctions createFeatureFunctions();

	SemiCrf::Labels createLabels();
}

#endif // APP_REQS__H
