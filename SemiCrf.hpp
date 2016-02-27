#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace SemiCrf {

	class Label_ {};
	typedef std::shared_ptr<Label_> Label;
	
	class Campany : public Label_ {};
	class Location : public Label_ {};

	class Segment_ {
	public:
		Segment_(int start_, int end_, Label label_)
			: start(start_), end(end_), label(label_) {}

		int getStart() { return start; }
		void setStart(int arg) { start = arg; }

		int getEnd() { return end; }
		void setEnd(int arg) { end = arg; }

		Label getLabel() { return label; }
		void setLabel(Label arg) { label = arg; }
		
	private:
		int start;
		int end;
		Label label;
	};

	typedef std::shared_ptr<Segment_> Segment;

	class Data_ {
	public:
		Data_() { std::cout << "Data_::Data()" << std::endl; }
		virtual ~Data_() { std::cout << "Data_::~Data()" << std::endl; }
		virtual void read() = 0;
		virtual void write() = 0;
	protected:
		std::vector<std::string> strs;
		std::vector<Segment> segs;
	};
	
	typedef std::shared_ptr<Data_> Data;	

	class TrainingData : public Data_ {
	public:
		virtual void read() { std::cout << "TrainingData::read()" << std::endl; };
		virtual void write() { std::cout << "TrainingData::write()" << std::endl; };
	};	

	class InferenceData : public Data_ {
	public:
		virtual void read() { std::cout << "InferenceData::read()" << std::endl; };
		virtual void write() { std::cout << "InferenceData::write()" << std::endl; };
	};

	class FeatureFunction_ {
	public:
		FeatureFunction_() {}
		virtual ~FeatureFunction_() {}
		virtual void operator() (Segment s0, Segment s1, Data d) = 0;
	};

	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;
	typedef std::pair<FeatureFunction,double> Ffp;
	typedef std::vector<Ffp> Ffps;

	class Algorithm_ {
	public:
		Algorithm_() { std::cout << "Algorithm_::Algorithm_()" << std::endl; }
		virtual ~Algorithm_() { std::cout << "Algorithm_::~Algorithm_()" << std::endl; }
		virtual void compute(const Data data, Ffps ffps) = 0;
		virtual void compute(const Ffps ffps, Data data) = 0;		
	};

	typedef std::shared_ptr<Algorithm_> Algorithm;

	class Learner : public Algorithm_ {
	public:
		Learner() { std::cout << "Learner::Learner()" << std::endl; }
		~Learner() { std::cout << "Learner::~Learner()" << std::endl; }
		virtual void compute(const Data data, Ffps ffps)
			{ std::cout << "Learner::compute()" << std::endl; };
		virtual void compute(const Ffps ffps, Data data){}; // error
	private:
	};

	class Inferer : public Algorithm_ {
	public:
		Inferer() { std::cout << "Inferer::Inferer()" << std::endl; }
		~Inferer() { std::cout << "Inferer::~Inferer()" << std::endl; }
		virtual void compute(const Data data, Ffps ffps){}; // error
		virtual void compute(const Ffps ffps, Data data)
			{ std::cout << "Inferer::compute()" << std::endl; };
	private:		
	};
}

#endif // SEMI_CRF__H
