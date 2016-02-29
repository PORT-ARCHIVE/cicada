#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace SemiCrf {

	// ラベル
	class Label_ {};
	typedef std::shared_ptr<Label_> Label;

	// セグメント 
	class Segment_{
	public:
		Segment_(int start_, int end_, Label label_)
			: start(start_), end(end_), label(label_) {}

		int getStart() const { return start; }
		void setStart(int arg) { start = arg; }

		int getEnd() const { return end; }
		void setEnd(int arg) { end = arg; }

		Label getLabel() const { return label; } // const?
		void setLabel(Label arg) { label = arg; }
		
	private:
		int start;
		int end;
		Label label;
	};

	typedef std::shared_ptr<Segment_> Segment;

	// データ
	class Data_ {
	public:
		Data_() { std::cout << "Data_()" << std::endl; }
		virtual ~Data_() { std::cout << "~Data_()" << std::endl; }
		virtual void read() = 0;
		virtual void write() const = 0;
		std::vector<std::string>&& getStrs() { return std::move(strs); }		
		std::vector<Segment>&& getSegments() { return std::move(segs); }
	protected:
		std::vector<std::string> strs;
		std::vector<Segment> segs;
	};
	
	typedef std::shared_ptr<Data_> Data;	

	// 教師データ
	class TrainingData : public Data_ {
	public:
		virtual void read();
		virtual void write() const;
	};	

	// 推論データ
	class InferenceData : public Data_ {
	public:
		virtual void read();
		virtual void write() const;
	};

	// 素性関数
	class FeatureFunction_ {
	public:
		FeatureFunction_() { std::cout << "FeatureFunction_()" << std::endl; }
		virtual ~FeatureFunction_() { std::cout << "~FeatureFunction_()" << std::endl; }
		virtual bool operator() (Segment s0, Segment s1, std::vector<std::string>&& strs) = 0;
		virtual void read() = 0;
		virtual void write() = 0;
	};

	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;
	typedef std::pair<FeatureFunction,double> Ffp; // 名前が分かりにくい
	typedef std::vector<Ffp> Ffps; // 名前が分かりにくい

	// 抽象アルゴリズム
	class Algorithm_ {
	public:
		Algorithm_() { std::cout << "Algorithm_()" << std::endl; }
		virtual ~Algorithm_() { std::cout << "~Algorithm_()" << std::endl; }
		virtual void compute(const Data data, Ffps ffps) = 0;
		virtual void compute(const Ffps ffps, Data data) = 0;		
	};

	typedef std::shared_ptr<Algorithm_> Algorithm;

	// 学習器
	class Learner : public Algorithm_ {
	public:
		Learner() { std::cout << "Learner()" << std::endl; }
		~Learner() { std::cout << "~Learner()" << std::endl; }
		virtual void compute(const Data data, Ffps ffps);
	private:
		virtual void compute(const Ffps ffps, Data data){}; // error		
	};

	// 推論器
	class Inferer : public Algorithm_ {
	public:
		Inferer() { std::cout << "Inferer()" << std::endl; }
		~Inferer() { std::cout << "~Inferer()" << std::endl; }
		virtual void compute(const Ffps ffps, Data data);
	private:
		virtual void compute(const Data data, Ffps ffps){}; // error		
	};

	//////////////////////////////////////	
}

#endif // SEMI_CRF__H
