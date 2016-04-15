// © 2016 PORT INC.

#ifndef APP_TEST__H
#define APP_TEST__H

#include <string>
#include "SemiCrf.hpp"
#include "W2V.hpp"

namespace App {

	const int ZERO = 0;

	SemiCrf::FeatureFunction createFeatureFunction(const std::string& feature, const std::string& w2vmat);

	class Digit : public SemiCrf::FeatureFunction_ {
	public:
		Digit();
		virtual ~Digit();
		virtual int getDim();
		virtual void read();
		virtual void write();
		virtual double wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i, SemiCrf::vector& gs);
	};

	class Jpn : public SemiCrf::FeatureFunction_ {
	public:
		Jpn();
		virtual ~Jpn();
		virtual int getDim();
		virtual void setXDim(int arg);
		virtual void read();
		virtual void write();
		virtual double wg(SemiCrf::Weights ws, Label y, Label yd, SemiCrf::Data x, int j, int i, SemiCrf::vector& gs);
		void setMatrix(W2V::Matrix m) { w2vmat = m; }
	private:
		W2V::Matrix w2vmat;
	};

	// 推論用数字データ集合
	class PridectionDigitDatas_ : public SemiCrf::Datas_ {
	public:
		PridectionDigitDatas_(){};
		virtual ~PridectionDigitDatas_(){};
		virtual void read(std::istream& input);
		virtual void writeJson(std::ostream& output) const;
	protected:
		virtual void readJsonData(std::vector<std::pair<std::string, ujson::value>>& object) {};
	};

}

#endif // APP_TEST__H
