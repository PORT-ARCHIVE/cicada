// © 2016 PORT INC.

#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include "Error.hpp"
#include "FileIO.hpp"

namespace App {

	// ラベル
	enum class Label : int;

	// 文字列→ラベル
	Label string2Label(const std::string& str);

	// ラベル→文字列
	std::string label2String(Label label);
}

namespace SemiCrf {

	enum {
		DISABLE_ADAGRAD = 0x1,
		DISABLE_DATE_VERSION = (0x1 << 1),
		ENABLE_LIKELIHOOD_ONLY = (0x1 << 2)
	};

	// ラベル集合
	class Labels_ : public std::vector<App::Label> {
	public:
		Labels_();
		virtual ~Labels_();
	};
	typedef std::shared_ptr<Labels_> Labels;
	Labels	createLabels();

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
		virtual void read();
		virtual void write(std::ostream& output) const;
		Strs getStrs() { return strs; }
		Segments getSegments() { return segs; }
		void setSegments(Segments arg) { segs = arg; }
	protected:
		Strs strs;
		Segments segs;
	};
	
	typedef std::shared_ptr<Data_> Data;

	// データ集合
	class Datas_ : public std::vector<Data> {
	public:
		Datas_();
		virtual ~Datas_();
		virtual void read(std::istream& input) = 0;
		virtual void write(std::ostream& output) const;
		virtual int getDim() { return (( xDim + yDim ) * yDim); }
		virtual int getXDim() { return xDim; }
		virtual int getYDim() { return yDim; }
		virtual void setXDim(int arg) { xDim = arg; }
		virtual void setYDim(int arg) { yDim = arg; }
		virtual int getMaxLength() { return maxLength; }
	protected:
		int xDim;
		int yDim;
		int maxLength;
	};

	typedef std::shared_ptr<Datas_> Datas;

	// 教師データ集合
	class TrainingDatas_ : public Datas_ {
	public:
		TrainingDatas_();
		virtual ~TrainingDatas_();
		virtual void read(std::istream& input);
	};

	Datas createTrainingDatas();

	// 推論データ集合
	class PredictionDatas_ : public Datas_ {
	public:
		PredictionDatas_();
		virtual ~PredictionDatas_();
		virtual void read(std::istream& input);
	};

	Datas createPredictionDatas();
  
	// 重みベクトル
	class Weights_ : public std::vector<double> {
	public:
		Weights_(int dim);
		virtual ~Weights_();
		void read(std::ifstream& ifs);
		void write(std::ofstream& ofs);
		void setXDim(int arg) { xDim = arg; }
		void setYDim(int arg) { yDim = arg; }
		int getXDim() { return xDim; }
		int getYDim() { return yDim; }
	protected:
		int xDim;
		int yDim;
	};

	typedef std::shared_ptr<Weights_> Weights;
	Weights createWeights(int dim = 0);

	// 素性関数
	class FeatureFunction_ {
	public:
		FeatureFunction_();
		virtual ~FeatureFunction_();
		virtual double operator() (int k, App::Label y, App::Label yd, Data x, int j, int i) = 0;
		virtual void read() = 0;
		virtual void write() = 0;
		void setXDim(int arg) { xDim = arg; }
		void setYDim(int arg) { yDim = arg; }
	protected:
		int xDim;
		int yDim;
	};

	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;

	FeatureFunction createFeatureFunction();

	// チェックテーブル
	typedef std::tuple<bool,double,int,App::Label> CheckTuple;
	typedef std::vector<CheckTuple> CheckTable_;
	typedef std::shared_ptr<CheckTable_> CheckTable;
	CheckTable createCheckTable(int capacity);

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
		virtual void setDatas(Datas arg);
		virtual void setFeatureFunction(FeatureFunction arg);
		virtual void setWeights(Weights arg);
		virtual void setDimension(int arg);
		virtual void compute() = 0;
		virtual void preProcess(const std::string& wfile, const std::string& w0file) = 0;
		virtual void postProcess(const std::string& wfile) = 0;
		void setFlg(int flg);

	protected:
		double computeWG(App::Label y, App::Label yd, int i, int d);

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
		Data current_data;
		CheckTable current_vctab;
		CheckTable current_actab;
		CheckTable current_ectab;
		int flg;
	};

	typedef std::shared_ptr<Algorithm_> Algorithm;

	// 学習器
	class Learner : public Algorithm_ {
	public:
		Learner(int arg);
		virtual ~Learner();
		virtual void compute();
		virtual void preProcess(const std::string& wfile, const std::string& w0file);
		virtual void postProcess(const std::string& wfile);

	private:
		bool isConv(double L, const std::vector<double>& dL, double& tdl0, double& rerr, bool& isfirst);
		double computeZ();
		std::vector<double> computeG(double& WG);
		std::vector<double> computeGm(double Z);
		double alpha(int i, App::Label y);
		double eta(int i, App::Label y, int k);
	};

	Algorithm createLearner(int arg);

	// 推論器
	class Predictor : public Algorithm_ {
	public:
		Predictor(int arg);
		virtual ~Predictor();
		virtual void compute();
		virtual void preProcess(const std::string& wfile, const std::string& w0file);
		virtual void postProcess(const std::string& wfile);
		
	private:
		double V(int i, App::Label y, int& maxd);
		void backtrack(App::Label maxy, int maxd);
		void printV();
	};

	Algorithm createPredictor(int arg);
}

#endif // SEMI_CRF__H
