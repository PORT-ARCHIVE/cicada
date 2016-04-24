// © 2016 PORT INC.

#include "JsonIO.hpp"

namespace JsonIO {

	Object parse(std::istream& is)
	{
		std::string jsonstr;
		jsonstr.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

		auto v = ujson::parse(jsonstr);

		if( !v.is_object() ) {
			throw std::invalid_argument("object expected");
		}

		Object object = object_cast(std::move(v));
		return std::move(object);
	}

	std::string readString(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_string() ) {
			throw std::invalid_argument("title' with type string not found"); // T.B.D.
		}
		std::string str = string_cast(std::move(it->second));
		return std::move(str);
	}

    int readInt(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_number() ) {
			throw std::invalid_argument("max_length' with type string not found"); // T.B.D
		}
		int value = int32_cast(std::move(it->second));
		return value;
	}

	std::vector<int> readIntAry(Object& object, const std::string& tag)
	{
		std::vector<int> ary;

		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_array() ) {
			throw std::invalid_argument("'dimention' with type array not found"); // T.B.D
		}

		std::vector<ujson::value> array = array_cast(std::move(it->second));
		for( auto i = array.begin(); i != array.end(); ++i ) {

			if( !i->is_number() ) {
				throw std::invalid_argument("invalid data format");
			}

			ary.push_back(int32_cast(std::move(*i)));
		}

		return std::move(ary);
	}

	std::vector<double> readDoubleAry(Object& object, const std::string& tag)
	{
		std::vector<double> ary;

		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_array() ) {
			throw std::invalid_argument("'weights' with type object not found"); // T.B.D
		}

		std::vector<ujson::value> array = array_cast(std::move(it->second));
		for( auto i = array.begin(); i != array.end(); ++i ) {

			if( !i->is_number() ) {
				throw std::invalid_argument("invalid data format");
			}

			ary.push_back(double_cast(std::move(*i)));
		}

		return std::move(ary);
	}

	std::map<int, double> readIntDoubleMap(Object& object, const std::string& tag)
	{
		std::map<int,double> idm;

		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_array() ) {
			throw std::invalid_argument("'mean' with type object not found"); // T.B.D
		}

		std::vector<ujson::value> array0 = array_cast(std::move(it->second));
		for( auto i = array0.begin(); i != array0.end(); ++i ) {

			std::vector<ujson::value> array1 = array_cast(std::move(*i));
			auto j = array1.begin();

			if( !j->is_number() ) {
				throw std::invalid_argument("invalid data format");
			}

			int lb = int32_cast(std::move(*j));

			j++;

			if( !j->is_number() ) {
				throw std::invalid_argument("invalid data format");
			}

			double m = double_cast(std::move(*j));

			idm[lb] = m;
		}

		return std::move(idm);
	}

	std::vector<ujson::value> readUAry(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() ) {
			throw std::invalid_argument("labels' with type string not found"); // T.B.D
		}

		std::vector<ujson::value> ary = array_cast(std::move(it->second));
		return std::move(ary);
	}
}
