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

		Simple();
		virtual ~Simple();
		virtual void read();
		virtual void write();
		virtual double operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i);
	};

	SemiCrf::FeatureFunction createFeatureFunction();
	int getFeatureDimention();

	SemiCrf::Labels createLabels();

	// 推論用数字データ集合
	class PridectionDigitDatas_ : public SemiCrf::Datas_ {
	public:
		PridectionDigitDatas_(){};
		virtual ~PridectionDigitDatas_(){};
		virtual void read(std::istream& input);
	};

}

#endif // APP_TEST__H
