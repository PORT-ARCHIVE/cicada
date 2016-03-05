#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace AppReqs {

	// ラベル
	enum class Label : int;

}

namespace SemiCrf {

	// ラベル集合
	class Labels_ : public std::vector<AppReqs::Label> {};
	typedef std::shared_ptr<Labels_> Labels;	

	// セグメント 
	class Segment_ {
	public:
		Segment_(int start_, int end_, AppReqs::Label label_)
			: start(start_), end(end_), label(label_) {}

		int getStart() const { return start; }
		void setStart(int arg) { start = arg; }

		int getEnd() const { return end; }
		void setEnd(int arg) { end = arg; }

		AppReqs::Label getLabel() const { return label; } // const?
		void setLabel(AppReqs::Label arg) { label = arg; }
		
	private:
		int start;
		int end;
		AppReqs::Label label;
	};

	typedef std::shared_ptr<Segment_> Segment;

	// セグメント集合
	class Segments_ : public std::vector<Segment> {};

	typedef std::shared_ptr<Segments_> Segments;

	// 文字列集合
	class Strs_ : public std::vector<std::string> {};

	typedef std::shared_ptr<Strs_> Strs;

	// データ
	class Data_ {
	public:
		Data_() : strs( new Strs_() ), segs( new Segments_() )
			{ std::cout << "Data_()" << std::endl; }
		virtual ~Data_() { std::cout << "~Data_()" << std::endl; }
		virtual void read();
		virtual void write() const;
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
		Datas_() {};
		virtual ~Datas_() {};
		virtual void read();
		virtual void write() const;
	};

	typedef std::shared_ptr<Datas_> Datas;

	// 重みベクトル
	class Weights_ : public std::vector<double> {
	public:
		Weights_() { std::cout << "Weights()" << std::endl; }
		virtual ~Weights_() { std::cout << "~Weights()" << std::endl; }
		void read();
		void write();		
	};

	typedef std::shared_ptr<Weights_> Weights;	

	// 素性関数
	class FeatureFunction_ {
	public:
		FeatureFunction_() { std::cout << "FeatureFunction_()" << std::endl; }
		virtual ~FeatureFunction_() { std::cout << "~FeatureFunction_()" << std::endl; }
		virtual double operator() (Segment s0, Segment s1, Strs strs) = 0;
		virtual double operator() (AppReqs::Label y, AppReqs::Label yd, Data x, int j, int i) = 0;
		virtual void read() = 0;
		virtual void write() = 0;
	};

	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;

	// 素性関数の集合
	class FeatureFunctions_ : public std::vector<FeatureFunction> {
	public:
		FeatureFunctions_() { std::cout << "FeatureFunctions_()" << std::endl; }
		virtual ~FeatureFunctions_() { std::cout << "~FeatureFunctions_()" << std::endl; }
		void read();
		void write();
	};

	typedef std::shared_ptr<FeatureFunctions_> FeatureFunctions;


	// チェックテーブル
	typedef std::tuple<bool,double,int> CheckTuple;
	typedef std::vector<CheckTuple> CheckTable_;
	typedef std::shared_ptr<CheckTable_> CheckTable;


	// 抽象アルゴリズム
	class Algorithm_ {
	public:
		Algorithm_() :
			labels( new Labels_() ),
			ffs( new FeatureFunctions_() ),
			weights( new Weights_() ),
			datas( new Datas_() )
			{ std::cout << "Algorithm_()" << std::endl; }
		virtual ~Algorithm_() { std::cout << "~Algorithm_()" << std::endl; }
		virtual void setLabels(Labels arg) { labels = arg; }
		virtual void setMaxLength(int arg) { maxLength = arg; }
		virtual void setDatas(Datas arg) { datas = arg; }
		virtual void setFeatureFunctions(FeatureFunctions arg) { ffs = arg; }
		virtual void setWeights(Weights arg) { weights = arg; }		
		virtual void compute() = 0;

	protected:
		Labels labels;
		FeatureFunctions ffs;
		Weights weights;
		Datas datas;
		int maxLength; // 最大セグメント長
		Data current_data;
		Segments segs;
		CheckTable current_vctab;
		CheckTable current_actab;
		CheckTable current_ectab;
	};

	typedef std::shared_ptr<Algorithm_> Algorithm;

	// 学習器
	class Learner : public Algorithm_ {
	public:
		Learner() { std::cout << "Learner()" << std::endl; }
		virtual ~Learner() { std::cout << "~Learner()" << std::endl; }
		virtual void compute();

	private:
		void computeG(std::vector<double>& Gs);
		void computeZ(double& Z);
		void computeGm(std::vector<double>& Gms, double Z);
		double alpha(int i, AppReqs::Label y);
		double eta(int i, AppReqs::Label y, int k);
	};

	// 推論器
	class Inferer : public Algorithm_ {
	public:
		Inferer() { std::cout << "Inferer()" << std::endl; }
		virtual ~Inferer() { std::cout << "~Inferer()" << std::endl; }
		virtual void compute();
		
	private:
		double V(int i, AppReqs::Label y, int& maxd);
	};
}

#endif // SEMI_CRF__H
