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
#include "JsonIO.hpp"
#include "Optimizer.hpp"
#include "ujson.hpp"

namespace App {

	// ラベル
	using Label = int;

	// 文字列→ラベル
	Label string2Label(const std::string& str);

	// ラベル→文字列
	std::string label2String(Label label);
}

namespace SemiCrf {

    using uvector = boost::numeric::ublas::vector<double>;

	enum {
		DISABLE_ADAGRAD          =  0x1
		, DISABLE_DATE_VERSION   = (0x1 << 1)
		, ENABLE_LIKELIHOOD_ONLY = (0x1 << 2)
		, DISABLE_REGULARIZATION = (0x1 << 3)
		, DISABLE_WG_CACHE       = (0x1 << 4)
	};

	// ラベル集合
	class Labels : public std::vector<App::Label> {
	public:

		Labels(int size = 0);
		virtual ~Labels();
	};

	decltype( std::make_shared<Labels>() ) createLabels(int size);

	// セグメント 
	class Segment {
	private:

		int start;
		int end;
		App::Label label;

	public:

		Segment(int start_, int end_, App::Label label_)
			: start(start_), end(end_), label(label_) {}
		virtual ~Segment(){}

		void setStart(int arg) { start = arg; }
		void setEnd(int arg) { end = arg; }
		void setLabel(App::Label arg) { label = arg; }
		int getStart() const { return start; }
		int getEnd() const { return end; }
		auto getLabel() -> decltype( label ) const { return label; } // const?
	};

	decltype(std::make_shared<Segment>()) createSegment(int start, int end, App::Label label);

	// セグメント集合
	class Segments : public std::vector<decltype(std::make_shared<Segment>())> {};
	decltype(std::make_shared<Segments>()) createSegments();

	// 文字列集合
	class Strs : public std::vector<std::vector<std::string>> {};

	// データ
	class Data {
	protected:

		decltype(std::make_shared<Strs>()) strs;
		decltype(std::make_shared<Segments>()) segs;
		std::map<int,int>* count;
		std::map<int,double>* mean;
		std::map<int,double>* variance;

	public:

		Data();
		virtual ~Data();

		virtual void writeJson(ujson::array& ary) const;

		auto getStrs() -> decltype(strs) { return strs; }
		auto getSegments() -> decltype(segs) { return segs; }
		auto getMean(int lb) -> decltype((*mean)[lb]) { return (*mean)[lb]; }
		auto getVariance(int lb) -> decltype((*variance)[lb]) { return (*variance)[lb]; }
		void setSegments(decltype(segs) arg) { segs = arg; }
		void setMeans(std::map<int,double>* arg) { mean = arg; }
		void setVariancies(std::map<int,double>* arg) { variance = arg; }
		void computeMeanLength
		( std::map<int,int>* count
		  , std::map<int,double>* mean
		  , std::map<int,double>* variance
			);
	};
	
	// データ集合
	class Datas : public std::vector<std::shared_ptr<Data>> {
	protected:

		int xDim;
		int yDim;
		int maxLength;
		std::string feature;
		std::string title;
		std::map<int,int> count;
		std::map<int,double> mean;
		std::map<int,double> variance;
		std::vector<ujson::value> labels;

	public:

		Datas();
		virtual ~Datas();

		virtual void read(std::istream& input) = 0;
		virtual void readJson(std::istream& input);
		virtual void write(std::ostream& output) const;
		virtual void writeJson(std::ostream& output) const;
		virtual void setXDim(int arg) { xDim = arg; }
		virtual void setYDim(int arg) { yDim = arg; }
		virtual int getXDim() { return xDim; }
		virtual int getYDim() { return yDim; }
		virtual int getMaxLength() { return maxLength; }

		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int,double>& arg);
		void setVariance(const std::map<int,double>& arg);
		auto getMean() -> decltype((mean)) { return mean; }
		auto getVariance() -> decltype((variance)) { return variance; }
		auto getFeature() -> decltype((feature)) { return feature; }

	protected:

		virtual void readJsonData(JsonIO::Object& object);
		virtual void readJsonDataCore(ujson::value& value, Data& data) = 0;
		virtual void computeMeanLength();
	};

	// 教師データ集合
	class TrainingDatas : public Datas {
	public:

		TrainingDatas();
		virtual ~TrainingDatas();

		virtual void read(std::istream& input);

	protected:

		virtual void readJsonDataCore(ujson::value& value, Data& data);
	};

	decltype( std::make_shared<Datas>() ) createTrainingDatas();

	// 推論データ集合
	class PredictionDatas : public Datas {
	public:

		PredictionDatas();
		virtual ~PredictionDatas();

		virtual void read(std::istream& input);

	protected:

		virtual void readJsonDataCore(ujson::value& value, Data& data);
	};

	decltype( std::make_shared<Datas>() ) createPredictionDatas();
  
	// 重みベクトル
	class Weights : public std::vector<double> {
	protected:

		int xDim;
		int yDim;
		int maxLength;
		std::string feature;
		std::map<int,double> mean;
		std::map<int,double> variance;

	public:

		Weights(int dim);
		virtual ~Weights();

		void read(std::istream& is);
		void readJson(std::istream& is);
		void write(std::ostream& os);
		void writeJson(std::ostream& os);
		void setXDim(int arg) { xDim = arg; }
		void setYDim(int arg) { yDim = arg; }
		void setMaxLength(int arg) { maxLength = arg; }
		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int,double>& arg) { mean = arg; }
		void setVariance(const std::map<int,double>& arg) { variance = arg; }
		int getXDim() { return xDim; }
		int getYDim() { return yDim; }
		int getMaxLength() { return maxLength; }
		auto getMean() -> decltype((mean)) { return mean; }
		auto getVariance() -> decltype((variance)) { return variance; }
		auto getFeature() -> decltype((feature)) { return feature; }
	};

	decltype(std::shared_ptr<Weights>()) createWeights(int dim = 0);

	// 素性関数
	class FeatureFunction {
	protected:

		int xDim;
		int yDim;
		int maxLength;
		std::string feature;

	public:

		FeatureFunction();
		virtual ~FeatureFunction();

		virtual void read() = 0;
		virtual void write() = 0;
		virtual void setXDim(int arg) { xDim = arg; }
		virtual int getDim() = 0;
		virtual double wg
		( Weights& w
		  , App::Label y
		  , App::Label yd
		  , Data& x
		  , int j
		  , int i
		  , uvector& gs
			) = 0;

		void setYDim(int arg) { yDim = arg; }
		void setMaxLength(int arg) { maxLength = arg; }
		int getMaxLength() { return maxLength; }
		auto getFeature() -> decltype((feature)) { return feature; }
	};

	decltype( std::make_shared<FeatureFunction>() ) createFeatureFunction();

	// チェックテーブル
	using CheckTuple = std::tuple<bool,double,int,App::Label>;
	using CheckTable_ = std::vector<CheckTuple>;
	using CheckTable = std::shared_ptr<CheckTable_>;
	CheckTable createCheckTable(int capacity);

	// vector用チェックテーブル
	using SVector = std::shared_ptr<uvector>;
	using CheckPair = std::pair<bool,SVector>;
	using CheckVTable_ = std::vector<CheckPair>;
	using CheckVTable = std::shared_ptr<CheckVTable_>;
	CheckVTable createCheckVTable(int capacity);

	// WGキャッシュ
	using CacheTuple = std::tuple<int,double,SVector>;
	using CacheTable_ = std::vector<CacheTuple>;
	using CacheTable = std::shared_ptr<CacheTable_>;
	CacheTable createCacheTable(int capacity);

	// 抽象アルゴリズム
	class Algorithm {
	protected:

		int dim;
		int y2xDim;
		int y2yDim;
		int maxLength; // 最大セグメント長
		int maxIteration;
		double e0;
		double e1;
		double rp;
		decltype( std::make_shared<Labels>() ) labels;
		decltype( std::shared_ptr<Weights>() ) weights;
		decltype( std::make_shared<FeatureFunction>() ) ff;
		decltype( std::make_shared<Datas>() ) datas;
		decltype( std::make_shared<Data>() ) current_data;
		CheckTable current_vctab;
		CheckTable current_actab;
		CheckTable current_ectab;
		CheckVTable current_ecvtab;
		CacheTable current_wgtab;
		int flg;
		std::string method;
		uvector gs; // 作業領域
		int cacheSize;
		int hit;
		int miss;

	public:

		Algorithm(int arg);
		virtual ~Algorithm();

		virtual void setLabels(decltype(labels) arg);
		virtual void setMaxLength(int arg);
		virtual void setMaxIteration(int arg);
		virtual void setE0(double arg);
		virtual void setE1(double arg);
		virtual void setRp(double arg);
		virtual void setMethod(const std::string& arg);
		virtual void setDatas(decltype(datas) arg);
		virtual void setFeatureFunction(decltype(ff) arg);
		virtual void setWeights(decltype(weights) arg);
		virtual void setDimension(int arg);
		virtual void compute() = 0;
		virtual void preProcess
		( const std::string& wfile
		  , const std::string& w0file
		  , const std::string& w2vfile
			) = 0;
		virtual void postProcess(const std::string& wfile) = 0;

		void setFlg(int flg);
		void setCacheSize(int size) { cacheSize = size; }

	protected:

		double computeWG
		( App::Label y
		  , App::Label yd
		  , int i
		  , int d
		  , uvector& gs
			);
		// void clearWGCache();
	};

	// 学習器
	class Learner : public Algorithm {
	public:

		friend class Likelihood;

		Learner(int arg);
		virtual ~Learner();

		virtual void compute();
		virtual void preProcess
		( const std::string& wfile
		  , const std::string& w0file
		  , const std::string& w2vfile
			);
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

	decltype( std::make_shared<Algorithm>() ) createLearner(int arg);

	// 尤度関数
	class Likelihood : public Optimizer::ObjectiveFunction_ {
	public:

		Likelihood(Learner* arg);
		virtual ~Likelihood();

		virtual double value(uvector& x);
		virtual double savedValue();
		virtual uvector grad(uvector& x);
		virtual void preProcess(uvector& x);
		virtual void beginLoopProcess(uvector& x);
		virtual void afterUpdateXProcess(uvector& x);
		virtual void endLoopProcess(uvector& x);
		virtual void postProcess(uvector& x);

	private:

		Learner* learner;
		double L;
	};

	Optimizer::ObjectiveFunction createLikelihood(Learner* learner);

	// 推論器
	class Predictor : public Algorithm {
	public:

		Predictor(int arg);
		virtual ~Predictor();

		virtual void compute();
		virtual void preProcess
		( const std::string& wfile
		  , const std::string& w0file
		  , const std::string& w2vfile
			);
		virtual void postProcess(const std::string& wfile);

	private:

		double V(int i, App::Label y, int& maxd);
		void backtrack(App::Label maxy, int maxd);
		void printV();
	};

	decltype( std::make_shared<Algorithm>() ) createPredictor(int arg);
}

#endif // SEMI_CRF__H
