// © 2016 PORT INC.

#ifndef APP_TEST__H
#define APP_TEST__H

#include <string>
#include <set>
#include "SemiCrf.hpp"
#include "W2V.hpp"

namespace App {

	using uvector = SemiCrf::uvector;
	using Weights = SemiCrf::Weights;
	using Data = SemiCrf::Data;
	using FeatureFunction = SemiCrf::FeatureFunction;

	const int ZERO = 0;

	decltype( std::make_shared<FeatureFunction>() )
	createFeatureFunction(const std::string& feature, const std::string& w2vmat);

	class Digit : public FeatureFunction {
	public:
		Digit();
		virtual ~Digit();
		virtual int getDim();
		virtual void read();
		virtual void write();
		virtual double wg(Weights& ws, Label y, Label yd, Data& x, int j, int i, uvector& gs);
	};

	class Jpn : public FeatureFunction {
	public:
		Jpn();
		virtual ~Jpn();
		virtual int getDim();
		virtual void setXDim(int arg);
		virtual void read();
		virtual void write();
		virtual double wg(Weights& ws, Label y, Label yd, Data& x, int j, int i, uvector& gs);
		void setMatrix(W2V::Matrix m) { w2vmat = m; }
	private:
		W2V::Matrix w2vmat;
		std::set<std::string> unknown_words;
	};
}

#endif // APP_TEST__H
