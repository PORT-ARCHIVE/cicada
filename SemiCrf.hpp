// © 2016 PORT INC.

#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <boost/numeric/ublas/vector.hpp>
#include "Error.hpp"
#include "FileIO.hpp"
#include "Optimizer.hpp"
#include "ujson.hpp"

namespace App {

	// ラベル
	typedef int Label;

	// 文字列→ラベル
	Label string2Label(const std::string& str);

	// ラベル→文字列
	std::string label2String(Label label);
}

namespace SemiCrf {

    typedef boost::numeric::ublas::vector<double> vector;

	enum {
		DISABLE_ADAGRAD          =  0x1
		, DISABLE_DATE_VERSION   = (0x1 << 1)
		, ENABLE_LIKELIHOOD_ONLY = (0x1 << 2)
		, DISABLE_REGULARIZATION = (0x1 << 3)
		, DISABLE_WG_CACHE       = (0x1 << 4)
	};

	// ラベル集合
	class Labels_ : public std::vector<App::Label> {
	public:
		Labels_(int size = 0);
		virtual ~Labels_();
	};
	typedef std::shared_ptr<Labels_> Labels;
	Labels	createLabels(int size);

	// セグメント 
	class Segment_ {
	public:
		Segment_(int start_, int end_, App::Label label_)
			: start(start_), end(end_), label(label_) {}

		int getStart() const { return start; }
		void setStart(int arg) { start = arg; }

		int getEnd() const { return end; }
		void setEnd(int arg) { end = arg; }

		App::Label getLabel() const { return label; } // const?
		void setLabel(App::Label arg) { label = arg; }
		
	private:
		int start;
		int end;
		App::Label label;
	};

	typedef std::shared_ptr<Segment_> Segment;
	Segment createSegment(int start, int end, App::Label label);

	// セグメント集合
	class Segments_ : public std::vector<Segment> {};
	typedef std::shared_ptr<Segments_> Segments;
	Segments createSegments();

	// 文字列集合
	class Strs_ : public std::vector<std::vector<std::string>> {};
	typedef std::shared_ptr<Strs_> Strs;

	// データ
	class Data_ {
	public:
		Data_();
		virtual ~Data_();
		virtual void writeJson(ujson::array& ary) const;
		Strs getStrs() { return strs; }
		Segments getSegments() { return segs; }
		void setSegments(Segments arg) { segs = arg; }
		void computeMeanLength( std::map<int, int>* count, std::map<int ,double>* mean, std::map<int ,double>* variance );
		double getMean(int lb) { return (*mean)[lb]; }
		double getVariance(int lb) { return (*variance)[lb]; }
		void setMeans(std::map<int ,double>* arg) { mean = arg; }
		void setVariancies(std::map<int ,double>* arg) { variance = arg; }
	protected:
		Strs strs;
		Segments segs;
		std::map<int, int>* count;
		std::map<int ,double>* mean;
		std::map<int ,double>* variance;
	};
	
	typedef std::shared_ptr<Data_> Data;

	// データ集合
	class Datas_ : public std::vector<Data> {
	public:
		Datas_();
		virtual ~Datas_();
		virtual void read(std::istream& input) = 0;
		virtual void readJson(std::istream& input);
		virtual void write(std::ostream& output) const;
		virtual void writeJson(std::ostream& output) const;
		virtual int getXDim() { return xDim; }
		virtual int getYDim() { return yDim; }
		virtual void setXDim(int arg) { xDim = arg; }
		virtual void setYDim(int arg) { yDim = arg; }
		virtual int getMaxLength() { return maxLength; }
		const std::string& getFeature() { return feature; }
		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int ,double>& arg) {
			mean = arg;
			for( auto& d : *this ) {
				d->setMeans(&mean);
			}
		}
		void setVariance(const std::map<int ,double>& arg) {
			variance = arg;
			for( auto& d : *this ) {
				d->setVariancies(&variance);
			}
		}
		const std::map<int ,double>& getMean() { return mean; }
		const std::map<int ,double>& getVariance() { return variance; }
	protected:
		virtual void readJsonData(std::vector<std::pair<std::string, ujson::value>>& object) = 0;
		virtual void computeMeanLength();
	protected:
		int xDim;
		int yDim;
		int maxLength;
		std::string feature;
		std::string title;
		std::map<int, int> count;
		std::map<int ,double> mean;
		std::map<int ,double> variance;
		std::vector<ujson::value> labels;
	};

	typedef std::shared_ptr<Datas_> Datas;

	// 教師データ集合
	class TrainingDatas_ : public Datas_ {
	public:
		TrainingDatas_();
		virtual ~TrainingDatas_();
		virtual void read(std::istream& input);
	protected:
		virtual void readJsonData(std::vector<std::pair<std::string, ujson::value>>& object);
	};

	Datas createTrainingDatas();

	// 推論データ集合
	class PredictionDatas_ : public Datas_ {
	public:
		PredictionDatas_();
		virtual ~PredictionDatas_();
		virtual void read(std::istream& input);
	protected:
		virtual void readJsonData(std::vector<std::pair<std::string, ujson::value>>& object);
	};

	Datas createPredictionDatas();
  
	// 重みベクトル
	class Weights_ : public std::vector<double> {
	public:
		Weights_(int dim);
		virtual ~Weights_();
		void read(std::ifstream& ifs);
		void readJson(std::ifstream& ifs);
		void write(std::ostream& ofs);
		void writeJson(std::ostream& ofs);
		void setXDim(int arg) { xDim = arg; }
		void setYDim(int arg) { yDim = arg; }
		int getXDim() { return xDim; }
		int getYDim() { return yDim; }
		int getMaxLength() { return maxLength; }
		void setMaxLength(int arg) { maxLength = arg; }
		const std::string& getFeature() { return feature; }
		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int ,double>& arg) { mean = arg; }
		void setVariance(const std::map<int ,double>& arg) { variance = arg; }
		const std::map<int ,double>& getMean() { return mean; }
		const std::map<int ,double>& getVariance() { return variance; }
	protected:
		int xDim;
		int yDim;
		int maxLength;
		std::string feature;
		std::map<int ,double> mean;
		std::map<int ,double> variance;
	};

	typedef std::shared_ptr<Weights_> Weights;
	Weights createWeights(int dim = 0);

	// 素性関数
	class FeatureFunction_ {
	public:
		FeatureFunction_();
		virtual ~FeatureFunction_();
		virtual double wg(Weights w, App::Label y, App::Label yd, Data x, int j, int i, vector& gs) = 0;
		virtual void read() = 0;
		virtual void write() = 0;
		virtual void setXDim(int arg) { xDim = arg; }
		void setYDim(int arg) { yDim = arg; }
		virtual int getDim() = 0;
		int getMaxLength() { return maxLength; }
		void setMaxLength(int arg) { maxLength = arg; }
		const std::string& getFeature() { return feature; };
	protected:
		int xDim;
		int yDim;
		int maxLength;
		std::string feature;
	};

	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;

	FeatureFunction createFeatureFunction();

	// チェックテーブル
	typedef std::tuple<bool,double,int,App::Label> CheckTuple;
	typedef std::vector<CheckTuple> CheckTable_;
	typedef std::shared_ptr<CheckTable_> CheckTable;
	CheckTable createCheckTable(int capacity);

	// vector用チェックテーブル
	typedef std::shared_ptr<vector> SVector;
	typedef std::pair<bool,SVector> CheckPair;
	typedef std::vector<CheckPair> CheckVTable_;
	typedef std::shared_ptr<CheckVTable_> CheckVTable;
	CheckVTable createCheckVTable(int capacity);

	// WGキャッシュ
	typedef std::tuple<int,double,SVector> CacheTuple;
	typedef std::vector<CacheTuple> CacheTable_;
	typedef std::shared_ptr<CacheTable_> CacheTable;
	CacheTable createCacheTable(int capacity);

	// 抽象アルゴリズム
	class Algorithm_ {
	public:
		Algorithm_(int arg);
		virtual ~Algorithm_();
		virtual void setLabels(Labels arg);
		virtual void setMaxLength(int arg);
		virtual void setMaxIteration(int arg);
		virtual void setE0(double arg);
		virtual void setE1(double arg);
		virtual void setRp(double arg);
		virtual void setMethod(std::string arg);
		virtual void setDatas(Datas arg);
		virtual void setFeatureFunction(FeatureFunction arg);
		virtual void setWeights(Weights arg);
		virtual void setDimension(int arg);
		virtual void compute() = 0;
		virtual void preProcess(const std::string& wfile, const std::string& w0file, const std::string& w2vfile) = 0;
		virtual void postProcess(const std::string& wfile) = 0;
		void setFlg(int flg);
		void setCacheSize(int size) { cacheSize = size; }

	protected:
		double computeWG(App::Label y, App::Label yd, int i, int d);
		double computeWG(App::Label y, App::Label yd, int i, int d, vector& gs);
		void clearWGCache();

		int dim;
		int y2xDim;
		int y2yDim;
		Labels labels;
		FeatureFunction ff;
		Weights weights;
		Datas datas;
		int maxLength; // 最大セグメント長
		int maxIteration;
		double e0;
		double e1;
		double rp;
		Data current_data;
		CheckTable current_vctab;
		CheckTable current_actab;
		CheckTable current_ectab;
		CheckVTable current_ecvtab;
		CacheTable current_wgtab;
		int flg;
		std::string method;
		vector gs; // 作業領域
		int cacheSize;
	};

	typedef std::shared_ptr<Algorithm_> Algorithm;

	// 学習器
	class Learner_ : public Algorithm_ {
	public:
		friend class Likelihood_;
		Learner_(int arg);
		virtual ~Learner_();
		virtual void compute();
		virtual void preProcess(const std::string& wfile, const std::string& w0file, const std::string& w2vfile);
		virtual void postProcess(const std::string& wfile);

	private:
		void computeGrad(double& L, std::vector<double>& dL, bool grad=true);
		double computeZ();
		std::vector<double> computeG(double& WG);
		std::vector<double> computeGm(double Z);
		double alpha(int i, App::Label y);
		double eta(int i, App::Label y, int k);
		SVector eta(int i, App::Label y);
	};

	typedef std::shared_ptr<Learner_> Learner;

	Algorithm createLearner(int arg);

	class Likelihood_ : public Optimizer::ObjectiveFunction_ {
	public:
		Likelihood_(Learner_* arg)
			: learner(arg)
			, L(0.0)
			{};
		virtual ~Likelihood_(){};
		virtual double value(Optimizer::vector& x);
		virtual double savedValue();
		virtual Optimizer::vector grad(Optimizer::vector& x);
		virtual void preProcess(Optimizer::vector& x);
		virtual void beginLoopProcess(Optimizer::vector& x);
		virtual void afterUpdateXProcess(Optimizer::vector& x);
		virtual void endLoopProcess(Optimizer::vector& x);
		virtual void postProcess(Optimizer::vector& x);
	private:
		Learner_* learner;
		double L;
	};

	typedef std::shared_ptr<Likelihood_> Likelihood;

	Optimizer::ObjectiveFunction createLikelihood(Learner_* learner);

	// 推論器
	class Predictor_ : public Algorithm_ {
	public:
		Predictor_(int arg);
		virtual ~Predictor_();
		virtual void compute();
		virtual void preProcess(const std::string& wfile, const std::string& w0file, const std::string& w2vfile);
		virtual void postProcess(const std::string& wfile);
		
	private:
		double V(int i, App::Label y, int& maxd);
		void backtrack(App::Label maxy, int maxd);
		void printV();
	};

	typedef std::shared_ptr<Predictor_> Predictor;

	Algorithm createPredictor(int arg);
}

#endif // SEMI_CRF__H
