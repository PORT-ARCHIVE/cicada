// © 2016 PORT INC.

#ifndef APP_REQS__H
#define APP_REQS__H

#include "SemiCrf.hpp"
#include "Logger.hpp"

namespace App {

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

		virtual double operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i) {
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
			Debug::out() << "AppReqF1::write()" << std::endl;
		};

		virtual double operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i) {
			return (0.0);
		};
	};

	SemiCrf::FeatureFunction createFeatureFunction();

	SemiCrf::Labels createLabels();
}

#endif // APP_REQS__H
