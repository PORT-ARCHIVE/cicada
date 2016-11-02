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
	createFeatureFunction(const std::string& feature, const std::string& w2vmat, const std::string& areaDic, const std::string& jobDic);

	class Dictonary_ {
	public:
		Dictonary_();
		virtual ~Dictonary_();
		virtual void read(const std::string& file);
		bool exist(const std::string& word);
	private:
		std::set<std::string> dic;
	};

	class JobDictonary_ : public Dictonary_ {
	public:
		JobDictonary_();
		virtual ~JobDictonary_();
		virtual void read(const std::string& file);
		bool exist(const std::string& word, bool& is_person);
	private:
		std::map<std::string,std::string> dic;
	};

	typedef std::shared_ptr<Dictonary_> Dictonary;
	typedef std::shared_ptr<JobDictonary_> JobDictonary;

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
		void setAreaDic(Dictonary dic) { areadic = dic; }
		void setJobDic(JobDictonary dic) { jobdic = dic; }
	private:
		double front_delimiter_feature(const std::vector<std::string>& words);
		double back_delimiter_feature(const std::vector<std::string>& words);
		double front_bracket_feature(const std::vector<std::string>& words);
		double back_bracket_feature(const std::vector<std::string>& words);
		double place_feature(const std::vector<std::string>& words);
		double place_indicator_feature(const std::vector<std::string>& words);
		void job_feature(const std::vector<std::string>& words, double& jfp, double& jfw);
		double back_job_feature(const std::vector<std::string>& words);
		double job_indicator_feature(const std::vector<std::string>& words);
	private:
		W2V::Matrix w2vmat;
		Dictonary areadic;
		JobDictonary jobdic;
		std::set<std::string> unknown_words;
		const static int FEATURE_DIM;
		static std::vector<std::string> open_brakets;
		static std::vector<std::string> close_brakets;
		static std::vector<std::string> delmiters;
	};
}

#endif // APP_TEST__H
