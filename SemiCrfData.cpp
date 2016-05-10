// © 2016 PORT INC.

#include "SemiCrfData.hpp"
#include "Logger.hpp"
#include "Error.hpp"

namespace App {

	// 文字列→ラベル
	Label string2Label(const std::string& str)
	{
		int val;
		try {
			val = boost::lexical_cast<int>(str);
		} catch(...) {
			throw Error("unknown label");
		}
		return ( static_cast<Label>(val) );
	}

	// ラベル→文字列
	std::string label2String(Label label)
	{
		std::string str;
		try {
			str = boost::lexical_cast<std::string>(label);
		} catch(...) {
			throw Error("unknown label");
		}
		return ( std::move(str) );
	}
}

namespace SemiCrf {

	//// Factories ////

	decltype(std::make_shared<Labels>()) createLabels(int size)
	{
		return std::make_shared<Labels>(size);
	}

	decltype(std::make_shared<Segment>()) createSegment(int start, int end, Label label)
	{
		return std::make_shared<Segment>(start, end, label);
	}

	decltype(std::make_shared<Segments>()) createSegments()
	{
		return std::make_shared<Segments>();
	}
	
	//// Labels ////

	Labels::Labels(int size)
	{
		Logger::debug() << "Labels()";
		for( int i = 0; i < size; i++ ) {
			push_back(i);
		}
	}

	Labels::~Labels()
	{
		Logger::debug() << "~Labels()";
	}

	//// Data ////

	Data::Data()
	{
		Logger::debug() << "Data()";
	}

	Data::~Data()
	{
		Logger::debug() << "~Data()";
	}

	void Data::computeMeanLength(
		std::map<int,int>* count_,
		std::map<int,double>* mean_,
		std::map<int,double>* variance_
		)
	{
		count = count_;
		mean = mean_;
		variance = variance_;

		for( auto& s : *segs ) {
			int len = s->getEnd() - s->getStart() + 1;
			int lb = static_cast<int>(s->getLabel());
			(*count)[lb] += 1;
			(*mean)[lb] += len;
			(*variance)[lb] += len*len;
		}
	}

	void Datas::computeMeanLength()
	{
		for( auto& d : *this ) {
			d->computeMeanLength(&count, &mean, &variance);
		}

		for( auto ic : count ) {
			int lb = ic.first;
			int c = ic.second;
			double m = mean[lb]/c;
			mean[lb] = m;
			double v = variance[lb]/c;
			variance[lb] = v - m*m;
		}
	}

	void Datas::readJson(std::istream& is)
	{
		auto object = JsonIO::parse(is);
		title = JsonIO::readString(object, "title");
		auto dims = JsonIO::readIntAry(object, "dimension");
		xDim = dims[0];
		yDim = dims[1];
		feature = JsonIO::readString(object, "feature");
		try {
			labels = JsonIO::readUAry(object, "labels");
		} catch(...) {
			if( feature == "JPN" ) {
				throw Error("no labels specified");
			}
		}
		readJsonData(object);
	}

	void Data::writeJson(ujson::array& ary0) const
	{
		Logger::debug() << "Data::writeJson()";

		ujson::array ary1;

		for( auto s : *segs ) {

			int start = s->getStart();
			int end = s->getEnd();
			auto l = s->getLabel();

			for( int i = start; i <= end; i++ ) {

				ujson::array ary2;

				ary2.push_back(strs->at(i).at(0));

				if( i == start ) {

					if( i == end ) {
						ary2.push_back("S/E");
					} else {
						ary2.push_back("S");
					}

				} else if( start < i && i < end ) {

					ary2.push_back("M");

				} else if( i == end ) {

					ary2.push_back("E");

				} else {
					throw Error("invalid segment");
				}

				ary2.push_back(App::label2String(l));
				if( 1 < strs->at(i).size() ) {
					ary2.push_back(strs->at(i).at(1));
				}
				ary1.push_back(std::move(ary2));
			}
		}

		ary0.push_back(std::move(ary1));
	}

	//// Datas ////

	Datas::Datas()
	{
		Logger::debug() << "Datas()";
	}

	Datas::~Datas()
	{
		Logger::debug() << "~Datas()";
	}

	void Datas::writeJson(std::ostream& output) const {
		Logger::debug() << "Datas::writeJson()";

		ujson::array datas;
		for( auto d : *this ) {
			d->writeJson(datas);
		}

		auto object = ujson::object {
			{ "title", title },
			{ "dimension", std::move(ujson::array{ xDim, yDim }) },
			{ "feature", feature },
			{ "data", datas }
		};

		if( !labels.empty() ) {
			object.push_back( std::move( std::make_pair( "labels", labels ) ) );
		}

		output << to_string(object) << std::endl;
	}

	void Datas::write(std::ostream& output)  const {
		Logger::debug() << "Datas::write()";
		writeJson(output);
	}

	void Datas::readJsonData(JsonIO::Object& object)
	{
		auto it = find(object, "data");
		if( it == object.end() || !it->second.is_array() ) {
			throw std::invalid_argument("'data' with type string not found");
		}

		auto array0 = array_cast(std::move(it->second));
		for( auto& i : array0 ) {

			if( !i.is_array() ) {
				throw std::invalid_argument("invalid data format");
			}

			auto data = std::make_shared<Data>(); Logger::debug() << "BEGIN : data was created.";
			readJsonDataCore(i, *data);
			push_back(std::move(data)); Logger::debug() << "END : data was pushed.";
		}
	}

	void Datas::setMean(const std::map<int ,double>& arg) {
		mean = arg;
		for( auto& d : *this ) {
			d->setMeans(&mean);
		}
	}

	void Datas::setVariance(const std::map<int ,double>& arg) {
		variance = arg;
		for( auto& d : *this ) {
			d->setVariancies(&variance);
		}
	}

	//// TrainingDatas ////

	decltype( std::make_shared<Datas>() )
	createTrainingDatas()
	{
		return std::make_shared<SemiCrf::TrainingDatas>();
	}

	TrainingDatas::TrainingDatas()
	{
		Logger::debug() << "TrainingDatas()";
	}

	TrainingDatas::~TrainingDatas()
	{
		Logger::debug() << "~TrainingDatas()";
	}

	void TrainingDatas::readJsonDataCore(ujson::value& value, Data& data)
	{
		decltype(std::make_shared<Segment>()) seg;
		int counter = -1;
		int seg_start = -1;

		auto array1 = array_cast(std::move(value));
		for( auto& j : array1 ) {

			counter++;

			if( !j.is_array() ) {
				throw std::invalid_argument("invalid data format");
			}

			auto array2 = array_cast(std::move(j));
			auto k = array2.begin();
			if( !k->is_string() ) {
				throw std::invalid_argument("invalid data format");
			}

			auto word_id = string_cast(std::move(*k)); Logger::debug() << word_id;
			std::vector<std::string> vs;
			vs.push_back(std::move(word_id));

			k++;
			if( !k->is_string() ) {
				throw std::invalid_argument("invalid format");
			}

			auto descriptor = string_cast(std::move(*k)); Logger::debug() << descriptor;

			k++;
			if( !k->is_string() ) {
				throw std::invalid_argument("invalid format");
			}

			auto label = string_cast(std::move(*k)); Logger::debug() << label;
			auto lb = App::string2Label(label);

			if( descriptor == "N" ) {

				seg = createSegment(counter, counter, lb);
				data.getSegments()->push_back(seg);

			} else if( descriptor == "S" ) {

				seg_start = counter;

			} else if( descriptor == "M" ) {

				// nothing to do

			} else if( descriptor == "E" ) {

				seg	= createSegment(seg_start, counter, lb);
				data.getSegments()->push_back(seg);
				int length = counter - seg_start + 1;
				if( maxLength < length ) {
					maxLength = length;
				}

			} else if( descriptor == "S/E" ) {

				seg	= createSegment(counter, counter, lb);
				data.getSegments()->push_back(seg);
				if( maxLength < 1 ) {
					maxLength = 1;
				}

			} else {
				Logger::warn() << "unknown descriptor";
			}

			k++; // 第4カラムを保存
			if( k != array2.end() && k->is_string() ) {
				auto word = string_cast(std::move(*k));
				vs.push_back(std::move(word));
			}

			data.getStrs()->push_back(std::move(vs));
		}
	}

	void TrainingDatas::read(std::istream& strm)
	{
		Logger::debug() << "TrainingDatas::read()";

		readJson(strm);
		if( empty() ) {
			throw Error("empty training data");
		}

		computeMeanLength();
	}

	//// PredictionDatas ////

	decltype( std::make_shared<Datas>() )
	createPredictionDatas()
	{
		return std::make_shared<SemiCrf::PredictionDatas>();
	}

	PredictionDatas::PredictionDatas()
	{
		Logger::debug() << "PredictionDatas()";
	}

	PredictionDatas::~PredictionDatas()
	{
		Logger::debug() << "~PredictionDatas()";
	}

	void PredictionDatas::readJsonDataCore(ujson::value& value, Data& data)
	{
		decltype(std::make_shared<Segment>()) seg;
		int counter = -1;
		int seg_start = -1;

		auto array1 = array_cast(std::move(value));
		for( auto& j : array1 ) {

			counter++;

			if( !j.is_array() ) {
				throw std::invalid_argument("invalid format");
			}

			auto array2 = array_cast(std::move(j));
			auto k = array2.begin();
			if( !k->is_string() ) {
				throw std::invalid_argument("invalid format");
			}


			std::vector<std::string> ss;
			auto word_id = string_cast(std::move(*k)); Logger::debug() << word_id;
			ss.push_back(std::move(word_id));

			++k;
			++k;
			++k; // 第4カラムへ移動
			if( k != array2.end() && k->is_string() ) {
				auto word = string_cast(std::move(*k)); Logger::debug() << word;
				ss.push_back(std::move(word));
			}

			data.getStrs()->push_back(std::move(ss));
		}
	}

	void PredictionDatas::read(std::istream& strm)
	{
		Logger::debug() << "PredictionDatas::read()";

		readJson(strm);
		if( empty() ) {
			throw Error("empty prediction data");
		}
	}
}
