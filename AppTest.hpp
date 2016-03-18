// © 2016 PORT INC.

#ifndef APP_TEST__H
#define APP_TEST__H

#include "SemiCrf.hpp"
#include "DebugOut.hpp"

namespace App {

	enum class Label : int {
		ZERO = 0,
		ONE = 1
	};

	class Simple : public SemiCrf::FeatureFunction_ {
	public:

		Simple(){ Debug::out() << "Simple()" << std::endl; };
		virtual ~Simple() { Debug::out() << "~Simple()" << std::endl; };

		virtual void read() {
			Debug::out() << "Simple::read()" << std::endl;
		};

		virtual void write() {
			Debug::out() << "Simple::write()" << std::endl;
		};

		virtual double operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i);
	};

	SemiCrf::FeatureFunction createFeatureFunction();
	int getFeatureDimention();

	SemiCrf::Labels createLabels();
}

#endif // APP_TEST__H