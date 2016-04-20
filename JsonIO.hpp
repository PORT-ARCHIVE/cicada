// © 2016 PORT INC.

#ifndef JSON_IO__H
#define JSON_IO__H

#include <string>
#include <vector>
#include "ujson.hpp"

namespace JsonIO {

	typedef std::vector<std::pair<std::string, ujson::value>> Object;

	std::string readString(Object& object, const std::string& tag);
	int readInt(Object& object, const std::string& tag);
	std::vector<int> readIntAry(Object& object, const std::string& tag);
	std::vector<double> readDoubleAry(Object& object, const std::string& tag);
	std::map<int, double> readIntDoubleMap(Object& object, const std::string& tag);
}


#endif // JSON_IO__H
