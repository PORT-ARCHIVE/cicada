// © 2016 PORT INC.

#ifndef SEMI_CRF_DATA__H
#define SEMI_CRF_DATA__H

#include <string>
#include <boost/numeric/ublas/vector.hpp>
#include <map>
#include "ujson.hpp"
#include "JsonIO.hpp"

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

}

#endif // SEMI_CRF_DATA__H
