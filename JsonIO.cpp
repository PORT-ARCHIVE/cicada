// © 2016 PORT INC.

#include <sstream>
#include "JsonIO.hpp"
#include "Error.hpp"

namespace JsonIO {

	ujson::value parse(std::istream& is)
	{
		std::string jsonstr;
		jsonstr.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
		return std::move(ujson::parse(jsonstr));
	}

	std::string readString(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_string() ) {
			std::stringstream ss;
			ss << "'" << tag << "' with type string not found";
			throw Error(ss.str());
		}
		auto str = string_cast(std::move(it->second));
		return std::move(str);
	}

    int readInt(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_number() ) {
			std::stringstream ss;
			ss << "'" << tag << "' with type number not found";
			throw Error(ss.str());
		}
		int value = int32_cast(std::move(it->second));
		return value;
	}

	std::vector<int> readIntAry(Object& object, const std::string& tag)
	{
		std::vector<int> ary;

		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_array() ) {
			std::stringstream ss;
			ss << "'" << tag << "' with type array not found";
			throw Error(ss.str());
		}

		auto array = array_cast(std::move(it->second));
		for( auto i = array.begin(); i != array.end(); ++i ) {

			if( !i->is_number() ) {
				throw Error("invalid data format");
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
			std::stringstream ss;
			ss << "'" << tag << "' with type string not found";
			throw Error(ss.str());
		}

		auto array = array_cast(std::move(it->second));
		for( auto i = array.begin(); i != array.end(); ++i ) {

			if( !i->is_number() ) {
				throw Error("invalid data format");
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
			std::stringstream ss;
			ss << "'" << tag << "' with type array not found";
			throw Error(ss.str());
		}

		auto array0 = array_cast(std::move(it->second));
		for( auto i = array0.begin(); i != array0.end(); ++i ) {

			auto array1 = array_cast(std::move(*i));
			auto j = array1.begin();

			if( !j->is_number() ) {
				throw Error("invalid data format");
			}

			int lb = int32_cast(std::move(*j));

			j++;

			if( !j->is_number() ) {
				throw Error("invalid data format");
			}

			double m = double_cast(std::move(*j));

			idm[lb] = m;
		}

		return std::move(idm);
	}

	std::map<int, int> readIntIntMap(Object& object, const std::string& tag)
	{
		std::map<int,int> iim;

		auto it = find(object, tag.c_str());
		if( it == object.end() || !it->second.is_array() ) {
			std::stringstream ss;
			ss << "'" << tag << "' with type array not found";
			throw Error(ss.str());
		}

		auto array0 = array_cast(std::move(it->second));
		for( auto i = array0.begin(); i != array0.end(); ++i ) {

			auto array1 = array_cast(std::move(*i));
			auto j = array1.begin();

			if( !j->is_number() ) {
				throw Error("invalid data format");
			}

			int lb = int32_cast(std::move(*j));

			j++;

			if( !j->is_number() ) {
				throw Error("invalid data format");
			}

			double m = int32_cast(std::move(*j));

			iim[lb] = m;
		}

		return std::move(iim);
	}

	std::vector<ujson::value> readUAry(Object& object, const std::string& tag)
	{
		auto it = find(object, tag.c_str());
		if( it == object.end() ) {
			std::stringstream ss;
			ss << "'" << tag << "' not found";
			throw Error(ss.str());
		}

		auto ary = array_cast(std::move(it->second));
		return std::move(ary);
	}
}
