// © 2016 PORT INC.

#include <map>
#include <set>
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
		Logger::trace() << "Labels()";
		for( int i = 0; i < size; i++ ) {
			push_back(i);
		}
	}

	Labels::~Labels()
	{
		Logger::trace() << "~Labels()";
	}

	//// Segment ////

	std::map<int,int> Segment::lavelHistgram;
	std::map<std::pair<int,int>,int> Segment::lavelLengthHistgram;

	Segment::Segment(
		decltype(start) start_,
		decltype(end) end_,
		decltype(label) label_ )
		: start(start_)
		, end(end_)
		, label(label_)
	{
		lavelHistgram[label]++;
		lavelLengthHistgram[std::make_pair(label,end-start+1)]++;
	}

	Segment::~Segment()
	{
		// lavelHistgram[label]--;
		// lavelLengthHistgram[std::make_pair(label,end-start+1)]--;
	}

	//// Data ////

	Data::Data()
	{
		Logger::trace() << "Data()";
	}

	Data::~Data()
	{
		Logger::trace() << "~Data()";
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
		for( auto& file : *this ) {
			for( auto& data : file.second ) {
				data->computeMeanLength(&count, &mean, &variance);
			}
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
		auto v = JsonIO::parse(is);
		if( !v.is_object() ) {
			throw Error("object expected");
		}
		auto object = object_cast(std::move(v));
		auto array = JsonIO::readUAry(object, "pages");
		preReadJsonData(array); // yDimを決める
		readJsonData(array);
		auto dims = JsonIO::readIntAry(object, "dimension");
		xDim = dims[0];
		//yDim = dims[1]; yDimは preReadJsonData で決まる
		feature = JsonIO::readString(object, "feature");
		try {
			labels = JsonIO::readUAry(object, "labels");
		} catch(...) {
			if( feature == "JPN" ) {
				throw Error("no labels specified");
			}
		}
	}

	void Datas::preReadJsonData(std::vector<ujson::value>& array0)
	{
		for( auto& value0 : array0 ) {

			std::vector<std::shared_ptr<Data>> datas;

			if( !value0.is_object() ) {
				throw Error("invalid data format");
			}

			//auto object = object_cast(std::move(value0));
			auto object = object_cast(value0);
			auto array1 = JsonIO::readUAry(object, "data");

			for( auto& value1 : array1 ) {

				auto data = std::make_shared<Data>();
				preReadJsonDataCore(value1, *data);
			}
		}

		++yDim;
	}

	void Datas::readJsonData(std::vector<ujson::value>& array0)
	{
		for( auto& value0 : array0 ) {

			std::vector<std::shared_ptr<Data>> datas;

			if( !value0.is_object() ) {
				throw Error("invalid data format");
			}

			auto object = object_cast(std::move(value0));
			auto title = JsonIO::readString(object, "title");
			auto array1 = JsonIO::readUAry(object, "data");

			for( auto& value1 : array1 ) {

				auto data = std::make_shared<Data>(); Logger::trace() << "BEGIN : data was created.";
				readJsonDataCore(value1, *data);
				datas.push_back(data);                Logger::trace() << "END : data was pushed.";
			}

			push_back(std::move(std::make_pair(std::move(title),std::move(datas))));
		}
	}

	void Data::writeJson(ujson::array& ary0) const
	{
		Logger::trace() << "Data::writeJson()";

		ujson::array ary1;

		for( auto& s : *segs ) {

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
					ary2.push_back(strs->at(i).at(1)); // 推論では word は第二カラムに入っている
				}
				ary1.push_back(std::move(ary2));
			}
		}

		ary0.push_back(std::move(ary1));
	}

	//// Datas ////

	Datas::Datas()
	{
		Logger::trace() << "Datas()";
	}

	Datas::~Datas()
	{
		Logger::trace() << "~Datas()";
	}

	void Datas::writeJson(std::ostream& output) const
	{
		Logger::trace() << "Datas::writeJson()";

		ujson::array array0;
		for( auto& file : *this ) {

			auto title = file.first;
			ujson::array array1;

			for( auto& data : file.second ) {
				data->writeJson(array1);
			}

			auto obj = ujson::object {
				{ "title", std::move(title) },
				{ "data", std::move(array1) }
			};

			array0.push_back(std::move(obj));
		}

		auto object = ujson::object {
			{ "dimension", std::move(ujson::array{ xDim, yDim }) },
			{ "feature", feature },
			{ "pages", std::move(array0) }
		};

		if( !labels.empty() ) {
			object.push_back( std::move( std::make_pair( "labels", labels ) ) );
		}

		output << to_string(object) << std::endl;
	}

	std::map<int, std::string> Datas::make_labels_map() const
	{
		std::map<int, std::string> labels_map;

		for( auto& i : labels ) {

			if( !i.is_array() ) {
				throw Error("invalid data format");
			}

			auto ary = array_cast(std::move(i));
			auto it = ary.begin();

			if( it == ary.end() || !it->is_string() ) {
				throw Error("invalid data format");
			}

			auto label_id = boost::lexical_cast<int>(string_cast(std::move(*it)));

			++it;

			if( it == ary.end() || !it->is_string() ) {
				throw Error("invalid data format");
			}

			auto japanese_keyword = string_cast(std::move(*it));

			++it;

			if( it == ary.end() ) {
				labels_map.insert( std::make_pair(label_id, japanese_keyword) );
				continue;
			}

			if( !it->is_string() ) {
				throw Error("invalid data format");
			}

			auto english_keyword = string_cast(std::move(*it));
			labels_map.insert( std::make_pair(label_id, english_keyword) );
		}

		return std::move(labels_map);
	}

	void Datas::make_label_word_map(
		std::shared_ptr<Data> data,
		std::map<int, std::string>& labels_map,
		std::multimap<std::string, std::string>& mm,
		std::set<std::pair<std::string,std::string>>& check
		) const
	{
		for( auto& seg : *data->getSegments() ) {

			auto s = seg->getStart();
			auto e = seg->getEnd();
			auto label_id = seg->getLabel(); // (圧縮されていない)元のラベル
			auto label = labels_map[label_id];
			auto strs = data->getStrs();

			std::string word;
			for( int i = s; i <= e; i++ ) {
				if( strs->at(i).size() < 2 ) {
					throw Error("option '--output-format 1' not supported");
				}
				word += strs->at(i).at(1); // 推論では word は第二カラムに入っている
			}

			if( label == "NONE" || label == "なし" ) {
				continue;
			}

			const auto& p = std::make_pair(label, word);
			if( check.find(p) == check.end() ) {
				check.insert(p);
				mm.insert( std::move(std::make_pair(std::move(label), std::move(word))) );
			}
		}
	}

	void Datas::writeSimpleJson(std::ostream& output) const
	{
		Logger::trace() << "Datas::writeSimpleJson()";
		if( labels.empty() ) { throw Error("option '--enable-simple-prediction-output' not supported"); }

		auto labels_map = make_labels_map();
		ujson::array array;

		for( auto& file : *this ) {

			ujson::object crf_estimate;
			auto title = file.first;

			std::multimap<std::string, std::string> mm;
			std::set<std::pair<std::string,std::string>> check;
			for( auto& data : file.second ) {
				make_label_word_map(data, labels_map, mm, check);
			}

			for( auto& p : labels_map ) {

				auto il = mm.lower_bound(p.second);
				auto iu = mm.upper_bound(p.second);
				auto d = std::distance(il, iu);

				if( d == 0 ) {

					if( p.second != "NONE" &&
						p.second != "place_indicator" &&
						p.second != "job_indicator" ) {
						Logger::out()->warn("{} not extracted: {}: {}", p.second, title, file_name );
					}
					continue;

				} else {

					ujson::array inner_array;

					for( auto i = il; i != iu; ++i ) {
						inner_array.push_back(i->second);
					}

					crf_estimate.push_back( std::move( std::make_pair(il->first, std::move(inner_array)) ) );
				}
			}

			auto object = ujson::object {{ "title", title }, { "crf_estimate", std::move(crf_estimate) }};
			array.push_back(object);
		}

		output << to_string(array) << std::endl;
	}

	void Datas::writeDebug(std::ostream& output) const
	{
		for( auto& file : *this ) {

			ujson::object crf_estimate;
			auto title = file.first;
			output << "[" << title  << "]" << std::endl;

			std::multimap<std::string, std::string> mm;
			std::set<std::pair<std::string,std::string>> check;
			for( auto& data : file.second ) {

				for( auto& seg : *data->getSegments() ) {

					auto s = seg->getStart();
					auto e = seg->getEnd();
					auto label_id = seg->getLabel(); // (圧縮されていない)元のラベル
					auto strs = data->getStrs();

					std::string word;
					for( int i = s; i <= e; i++ ) {
						if( strs->at(i).size() < 2 ) {
							throw Error("option '--output-format 2' not supported");
						}
						word += strs->at(i).at(1); // 推論では word は第二カラムに入っている
					}

					output << label_id << ": " << word << std::endl;
				}
			}
		}
	}

	void Datas::write(std::ostream& output, unsigned int flg) const {
		Logger::trace() << "Datas::write()";
		if( flg & ENABLE_SIMPLE_PREDICTION_OUTPUT ) {
			writeSimpleJson(output);
		} else if( flg & ENABLE_DEBUG_PREDICTION_OUTPUT ) {
			writeDebug(output);
		} else {
			writeJson(output);
		}
	}

	void Datas::setMean(const std::map<int ,double>& arg) {
		mean = arg;
		for( auto& file : *this ) {
			for( auto& data : file.second ) {
				data->setMeans(&mean);
			}
		}
	}

	void Datas::setVariance(const std::map<int ,double>& arg) {
		variance = arg;
		for( auto& file : *this ) {
			for( auto& data : file.second ) {
				data->setVariancies(&variance);
			}
		}
	}

	void Datas::setLabelMap(const std::map<int ,int>& arg) {
		label_map = arg;
		int size = 0;
		for( auto& p : label_map ) {
			if( size < p.second ) {
				size = p.second;
			}
		}
		reverse_label_map.resize(size+1);
		for( auto& p : label_map ) {
			reverse_label_map[p.second] = p.first;
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
		Logger::trace() << "TrainingDatas()";
	}

	TrainingDatas::~TrainingDatas()
	{
		Logger::trace() << "~TrainingDatas()";
	}

	void TrainingDatas::preReadJsonDataCore(ujson::value& value, Data& data)
	{
		decltype(std::make_shared<Segment>()) seg;

		auto array1 = array_cast(std::move(value));
		for( auto& j : array1 ) {

			if( !j.is_array() ) {
				throw Error("invalid data format");
			}

			auto array2 = array_cast(std::move(j));
			auto k = array2.begin();
			if( !k->is_string() ) {
				throw Error("invalid data format");
			}

			k++;
			if( !k->is_string() ) {
				throw Error("invalid format");
			}

			k++;
			if( !k->is_string() ) {
				throw Error("invalid format");
			}

			auto label = string_cast(std::move(*k));
			auto lb = App::string2Label(label);

			if( label_map.find(lb) == label_map.end() ) {
				label_map.insert( std::make_pair(lb, ++yDim) );
			}
		}
	}

	void TrainingDatas::readJsonDataCore(ujson::value& value, Data& data)
	{
		decltype(std::make_shared<Segment>()) seg;
		int counter = -1;
		int seg_start = -1;
		std::string centence;

		auto array1 = array_cast(std::move(value));
		for( auto& j : array1 ) {

			counter++;

			if( !j.is_array() ) {
				throw Error("invalid data format");
			}

			auto array2 = array_cast(std::move(j));
			auto k = array2.begin();
			if( !k->is_string() ) {
				throw Error("invalid data format");
			}

			auto word_id = string_cast(std::move(*k)); Logger::trace() << word_id;
			std::vector<std::string> vs;
			vs.push_back(std::move(word_id));

			k++;
			if( !k->is_string() ) {
				throw Error("invalid format");
			}

			auto descriptor = string_cast(std::move(*k)); Logger::trace() << descriptor;

			k++;
			if( !k->is_string() ) {
				throw Error("invalid format");
			}

			auto label = string_cast(std::move(*k)); Logger::trace() << label;
			auto lb = label_map[ App::string2Label(label) ]; // labelを圧縮、学習では segment に圧縮されたレベルが入る

			if( descriptor == "N" ) {

				seg = createSegment(counter, counter, lb);
				data.getSegments()->push_back(seg);

			} else if( descriptor == "S" ) {

				seg_start = counter;
				centence += "[:";
				centence += label;

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
				centence += "[:";
				centence += label;

			} else {
				Logger::warn() << "unknown descriptor";
			}

			k++; // 第4カラムを保存
			if( k != array2.end() && k->is_string() ) {
				auto word = string_cast(std::move(*k));
				centence += " ";
				centence += word;
				if( descriptor == "S/E" || descriptor == "E" ) {
					centence += " ]";
				}
				vs.push_back(std::move(word));
			}

			data.getStrs()->push_back(std::move(vs));

		}
		if( !centence.empty() ) {
			Logger::out()->debug("{}", centence);
		}
	}

	void TrainingDatas::reportStatistcs()
	{
		int nData = 0;
		for( auto& d : *this ) {
			nData += d.second.size();
		}
		Logger::out()->info( "# sentences: {}", nData);

		reverse_label_map.resize(label_map.size()+1);
		for( auto& p : label_map ) {
			reverse_label_map[p.second] = p.first;
		}

		Logger::out()->info( "# lavels" );

		const auto& lavelHistgram = SemiCrf::Segment::getLavelHistgram();
		const auto& lavelLengthHistgram = SemiCrf::Segment::getLavelLengthHistgram();

		for( auto& lv : lavelHistgram ) {
			Logger::out()->info( "{}: {}", reverse_label_map[lv.first], lv.second );
		}

		std::map<int,int> total_length;
		for( auto& lvl : lavelLengthHistgram ) {
			total_length[lvl.first.first] += (lvl.first.second) * (lvl.second);
		}

		Logger::out()->info( "Average length of lavels" );

		for( auto& l : total_length ) {
			int n = lavelHistgram.find(l.first)->second;
			Logger::out()->info( "{}: {}", reverse_label_map[l.first], (double)l.second/n );
		}
	}

	void TrainingDatas::read(std::istream& strm)
	{
		Logger::trace() << "TrainingDatas::read()";

		readJson(strm);
		if( empty() ) {
			throw Error("empty training data");
		}

		computeMeanLength();

		reportStatistcs();
	}

	//// PredictionDatas ////

	decltype( std::make_shared<Datas>() )
	createPredictionDatas()
	{
		return std::make_shared<SemiCrf::PredictionDatas>();
	}

	PredictionDatas::PredictionDatas()
	{
		Logger::trace() << "PredictionDatas()";
	}

	PredictionDatas::~PredictionDatas()
	{
		Logger::trace() << "~PredictionDatas()";
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
				throw Error("invalid format");
			}

			auto array2 = array_cast(std::move(j));
			auto k = array2.begin();
			if( !k->is_string() ) {
				throw Error("invalid format");
			}

			std::vector<std::string> ss;
			auto word_id = string_cast(std::move(*k)); Logger::trace() << word_id;
			ss.push_back(std::move(word_id));

			++k;
			++k;
			++k; // 第4カラムへ移動
			if( k != array2.end() && k->is_string() ) {
				auto word = string_cast(std::move(*k)); Logger::trace() << word;
				ss.push_back(std::move(word));
			}

			data.getStrs()->push_back(std::move(ss));
		}
	}

	void PredictionDatas::read(std::istream& strm)
	{
		Logger::trace() << "PredictionDatas::read()";

		readJson(strm);
		if( empty() ) {
			throw Error("empty prediction data");
		}
	}

	void PredictionDatas::reportStatistcs()
	{
		int nData = 0;
		for( auto& d : *this ) {
			nData += d.second.size();
		}
		Logger::out()->info( "# sentences: {}", nData);

		// reverse_label_map.resize(label_map.size()+1);
		// for( auto& p : label_map ) {
		// 	reverse_label_map[p.second] = p.first;
		// }

		Logger::out()->info( "# lavels" );

		const auto& lavelHistgram = SemiCrf::Segment::getLavelHistgram();
		const auto& lavelLengthHistgram = SemiCrf::Segment::getLavelLengthHistgram();

		for( auto& lv : lavelHistgram ) {
			// Logger::out()->info( "{}: {}", reverse_label_map[lv.first], lv.second );
			Logger::out()->info( "{}: {}", lv.first, lv.second );
		}

		std::map<int,int> total_length;
		for( auto& lvl : lavelLengthHistgram ) {
			total_length[lvl.first.first] += (lvl.first.second) * (lvl.second);
		}

		Logger::out()->info( "Average length of lavels" );

		for( auto& l : total_length ) {
			int n = lavelHistgram.find(l.first)->second;
			// Logger::out()->info( "{}: {}", reverse_label_map[l.first], (double)l.second/n );
			Logger::out()->info( "{}: {}", l.first, (double)l.second/n );
		}
	}
}
