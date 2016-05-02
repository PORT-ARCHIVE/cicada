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
	using Label = App::Label;

	enum {
		DISABLE_ADAGRAD          =  0x1
		, DISABLE_DATE_VERSION   = (0x1 << 1)
		, ENABLE_LIKELIHOOD_ONLY = (0x1 << 2)
		, DISABLE_REGULARIZATION = (0x1 << 3)
		, DISABLE_WG_CACHE       = (0x1 << 4)
	};

	// ラベル集合
	class Labels : public std::vector<Label> {
	public:

		Labels(int size = 0);
		virtual ~Labels();
	};

	decltype( std::make_shared<Labels>() ) createLabels(int size);

	// セグメント 
	class Segment {
	private:

		int start{-1};
		int end{-1};
		Label label;

	public:

		Segment(
			decltype(start) start_,
			decltype(end) end_,
			decltype(label) label_ )
			: start(start_), end(end_), label(label_) {}
		virtual ~Segment(){}

		void setStart(decltype(start) arg) { start = arg; }
		void setEnd(decltype(end) arg) { end = arg; }
		void setLabel(decltype(label) arg) { label = arg; }
		decltype(start) getStart() const { return start; }
		decltype(end) getEnd() const { return end; }
		decltype(label) getLabel() const { return label; }
	};

	decltype(std::make_shared<Segment>()) createSegment(int start, int end, Label label);

	// セグメント集合
	class Segments : public std::vector<decltype(std::make_shared<Segment>())> {};
	decltype(std::make_shared<Segments>()) createSegments();

	// 文字列集合
	class Strs : public std::vector<std::vector<std::string>> {};

	// データ
	class Data {
	protected:

		decltype(std::make_shared<Strs>()) strs{ std::make_shared<Strs>() };
		decltype(std::make_shared<Segments>()) segs{ std::make_shared<Segments>() };
		std::map<int,int>* count{ nullptr };
		std::map<int,double>* mean{ nullptr };
		std::map<int,double>* variance{ nullptr };

	public:

		Data();
		virtual ~Data();

		virtual void writeJson(ujson::array& ary) const;

		decltype(strs) getStrs() const { return strs; }
		decltype(segs) getSegments() const { return segs; }
		decltype((*mean)[0]) getMean(int lb) { return (*mean)[lb]; }
		decltype((*variance)[0]) getVariance(int lb) { return (*variance)[lb]; }
		void setSegments(decltype(segs) arg) { segs = arg; }
		void setMeans(decltype(mean) arg) { mean = arg; }
		void setVariancies(decltype(variance) arg) { variance = arg; }
		void computeMeanLength (
			std::map<int,int>* count,
			std::map<int,double>* mean,
			std::map<int,double>* variance );
	};
	
	// データ集合
	class Datas : public std::vector<std::shared_ptr<Data>> {
	protected:

		int xDim{-1};
		int yDim{-1};
		int maxLength{-std::numeric_limits<int>::max()};
		std::string feature{""};
		std::string title{""};
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
		virtual void setXDim(decltype(xDim) arg) { xDim = arg; }
		virtual void setYDim(decltype(yDim) arg) { yDim = arg; }
		virtual decltype(xDim) getXDim() const { return xDim; }
		virtual decltype(yDim) getYDim() const { return yDim; }
		virtual decltype(maxLength) getMaxLength() const { return maxLength; }

		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int,double>& arg);
		void setVariance(const std::map<int,double>& arg);
		std::add_const<decltype((mean))>::type getMean() { return mean; }
		std::add_const<decltype((variance))>::type getVariance() { return variance; }
		std::add_const<decltype((feature))>::type getFeature() { return feature; }

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

		int xDim{-1};
		int yDim{-1};
		int maxLength{-1};
		std::string feature{""};
		std::map<int,double> mean;
		std::map<int,double> variance;

	public:

		Weights(int dim);
		virtual ~Weights();

		void read(std::istream& is);
		void readJson(std::istream& is);
		void write(std::ostream& os);
		void writeJson(std::ostream& os);
		void setXDim(decltype(xDim) arg) { xDim = arg; }
		void setYDim(decltype(yDim) arg) { yDim = arg; }
		void setMaxLength(decltype(maxLength) arg) { maxLength = arg; }
		void setFeature(const std::string& arg) { feature = arg; }
		void setMean(const std::map<int,double>& arg) { mean = arg; }
		void setVariance(const std::map<int,double>& arg) { variance = arg; }
		decltype(xDim) getXDim() const { return xDim; }
		decltype(yDim) getYDim() const { return yDim; }
		decltype(maxLength) getMaxLength() const { return maxLength; }
		std::add_const<decltype((mean))>::type getMean() { return mean; }
		std::add_const<decltype((variance))>::type getVariance() { return variance; }
		std::add_const<decltype((feature))>::type getFeature() { return feature; }
	};

	decltype(std::shared_ptr<Weights>()) createWeights(int dim = 0);

	// 素性関数
	class FeatureFunction {
	protected:

		int xDim{-1};
		int yDim{-1};
		int maxLength{-1};
		std::string feature{""};

	public:

		FeatureFunction();
		virtual ~FeatureFunction();

		virtual void read() = 0;
		virtual void write() = 0;
		virtual int getDim() = 0;
		virtual void setXDim(decltype(xDim) arg) { xDim = arg; }
		virtual void setYDim(decltype(yDim) arg) { yDim = arg; }
		virtual double wg (
			Weights& w,
			Label y,
			Label yd,
			Data& x,
			int j,
			int i,
			uvector& gs	) = 0;

		void setMaxLength(decltype(maxLength) arg) { maxLength = arg; }
		decltype(maxLength) getMaxLength() const { return maxLength; }
		std::add_const<decltype((feature))>::type getFeature() { return feature; }
	};

	decltype( std::make_shared<FeatureFunction>() ) createFeatureFunction();

	// チェックテーブル
	using CheckTuple = std::tuple<bool,double,int,Label>;
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

		int flg;
		int dim{-1};
		int y2xDim{-1};
		int y2yDim{-1};
		int maxLength{5}; // 最大セグメント長
		int maxIteration{1024};
		double e0{1.0e-5};
		double e1{1.0e-5};
		double rp{1.0e-7};
		decltype( std::make_shared<Labels>() ) labels{nullptr};
		decltype( std::shared_ptr<Weights>() ) weights{nullptr};
		decltype( std::make_shared<FeatureFunction>() ) ff{nullptr};
		decltype( std::make_shared<Datas>() ) datas{nullptr};
		decltype( std::make_shared<Data>() ) current_data{nullptr};
		CheckTable current_vctab{nullptr};
		CheckTable current_actab{nullptr};
		CheckTable current_ectab{nullptr};
		CheckVTable current_ecvtab{nullptr};
		CacheTable current_wgtab{nullptr};
		std::string method{"bfgs"};
		uvector gs; // 作業領域
		int cacheSize{0xff};
		int hit{0};
		int miss{0};

	public:

		Algorithm(int flg);
		virtual ~Algorithm();

		virtual void compute() = 0;
		virtual void preProcess (
			const std::string& wfile,
			const std::string& w0file,
			const std::string& w2vfile ) = 0;
		virtual void postProcess(const std::string& wfile) = 0;

		virtual void setDimension(decltype(dim) arg);
		virtual void setMaxLength(int arg);
		virtual void setMaxIteration(int arg);
		virtual void setE0(double arg);
		virtual void setE1(double arg);
		virtual void setRp(double arg);
		virtual void setMethod(const std::string& arg);
		virtual void setDatas(decltype(datas) arg);
		virtual void setLabels(decltype(labels) arg);
		virtual void setWeights(decltype(weights) arg);
		virtual void setFeatureFunction(decltype(ff) arg);

		void setCacheSize(decltype(cacheSize) size) { cacheSize = size; }

	protected:

		double computeWG (
			Label y,
			Label yd,
			int i,
			int d,
			uvector& gs	);
	};

	// 学習器
	class Learner : public Algorithm {
	public:

		friend class Likelihood;

		Learner(int flg);
		virtual ~Learner();

		virtual void compute();
		virtual void preProcess (
			const std::string& wfile,
			const std::string& w0file,
			const std::string& w2vfile );
		virtual void postProcess(const std::string& wfile);

	private:

		void computeGrad(double& L, std::vector<double>& dL, bool grad=true);
		double computeZ();
		std::vector<double> computeG(double& WG);
		std::vector<double> computeGm(double Z);
		double alpha(int i, Label y);
		double eta(int i, Label y, int k);
		SVector eta(int i, Label y);
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
		double L{0.0};
	};

	Optimizer::ObjectiveFunction createLikelihood(Learner* learner);

	// 推論器
	class Predictor : public Algorithm {
	public:

		Predictor(int flg);
		virtual ~Predictor();

		virtual void compute();
		virtual void preProcess (
			const std::string& wfile,
			const std::string& w0file,
			const std::string& w2vfile );
		virtual void postProcess(const std::string& wfile);

	private:

		double V(int i, Label y, int& maxd);
		void backtrack(Label maxy, int maxd);
		void printV();
	};

	decltype( std::make_shared<Algorithm>() ) createPredictor(int arg);
}

#endif // SEMI_CRF__H
