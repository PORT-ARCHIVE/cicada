// © 2016 PORT INC.

#ifndef JSON_IO__H
#define JSON_IO__H

#include <iostream>
#include <string>
#include <vector>
#include "ujson.hpp"

namespace JsonIO {

	using Object = std::vector<std::pair<std::string, ujson::value>>;
	ujson::value parse(std::istream& is);
	std::string readString(Object& object, const std::string& tag);
	int readInt(Object& object, const std::string& tag);
	std::vector<int> readIntAry(Object& object, const std::string& tag);
	std::vector<double> readDoubleAry(Object& object, const std::string& tag);
	std::map<int, double> readIntDoubleMap(Object& object, const std::string& tag);
	std::map<int, int> readIntIntMap(Object& object, const std::string& tag);
	std::vector<ujson::value> readUAry(Object& object, const std::string& tag);
}


#endif // JSON_IO__H
