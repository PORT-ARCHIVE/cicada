// © 2016 PORT INC.

#ifndef APP_TEST__H
#define APP_TEST__H

#include <set>
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
	createFeatureFunction(const std::string& feature, const std::string& w2vmat, const std::string& areaDic);

	class AreaDic_ {
	public:
		AreaDic_();
		virtual ~AreaDic_();
		virtual void read(std::string file);
		bool exist(std::string word);
	private:
		std::string removePrefecture(std::string area, bool& is_remove);
		std::set<std::string> dic;
	};

	typedef std::shared_ptr<AreaDic_> AreaDic;

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
		void setAreaDic(AreaDic dic) { areadic = dic; }
	private:
		bool isDelimiter(const std::string& word);
		double place_feature(const std::vector<std::string>& word);
		double place_indicator_feature(const std::vector<std::string>& word);
		double job_feature(const std::vector<std::string>& word);
		double job_indicator_feature(const std::vector<std::string>& word);
	private:
		W2V::Matrix w2vmat;
		AreaDic	areadic;
		std::set<std::string> unknown_words;
		const static int FEATURE_DIM;
	};
}

#endif // APP_TEST__H
