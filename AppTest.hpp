// © 2016 PORT INC.

#ifndef APP_TEST__H
#define APP_TEST__H

#include "SemiCrf.hpp"
#include "W2V.hpp"

namespace App {

	const int ZERO = 0;

	class Simple : public SemiCrf::FeatureFunction_ {
	public:
		Simple();
		virtual ~Simple();
		virtual void read();
		virtual void write();
		virtual double operator() (int k, Label y, Label yd, SemiCrf::Data x, int j, int i);
		virtual double wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i);
	private:
		W2V::Matrix w2vmat;
	};

	SemiCrf::FeatureFunction createFeatureFunction();

	// 推論用数字データ集合
	class PridectionDigitDatas_ : public SemiCrf::Datas_ {
	public:
		PridectionDigitDatas_(){};
		virtual ~PridectionDigitDatas_(){};
		virtual void read(std::istream& input);
	};

}

#endif // APP_TEST__H
