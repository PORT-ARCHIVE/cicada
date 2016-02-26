#ifndef SEMI_CRF__H
#define SEMI_CRF__H

#include <memory>
#include <string>
#include <vector>

namespace SemiCrf {

	class Label_ {};
	typedef std::shared_ptr<Label_> Label;
	
	class Campany : public Label_ {};
	class Location : public Label_ {};

	class Segment {
	public:

		Segment(int start_, int end_, Label label_)
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


	class Data_ {
	private:
		std::string str;
	};
	typedef std::shared_ptr<Data_> Data;	

	class TrainingData : public Data_ {
	public:
	};	

	class InferenceData : public Data_ {
	public:		
	};

	//class Parameters {};

	class FeatureFunction_ {};
	typedef std::shared_ptr<FeatureFunction_> FeatureFunction;

	// class Algorithm {
	// public:
	// 	virtual void compute(std::vector<std::pair<FeatureFunction,double>> ffps, Data d) = 0;
	// };

	//class Learner : public Algorithm {
	class Learner {		
	public:
		virtual void compute(std::vector<std::pair<FeatureFunction,double>> ffps, std::vector<Data> ds) {};
	private:
	};

	//class Inferer : public Algorithm {	
	class Inferer {
	public:
		virtual void compute(std::vector<std::pair<FeatureFunction,double>> ffps, std::vector<Data> ds) {};
	private:		
	};		

	class SemiCrf {
	public:
		SemiCrf();
		virtual ~SemiCrf();
	};
}

#endif // SEMI_CRF__H
