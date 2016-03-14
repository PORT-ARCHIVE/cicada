
#include <cstdlib>
#include "MultiByteTokenizer.hpp"

MultiByteIterator::MultiByteIterator(const std::string& arg)
	: str(arg)
{
	buf = new char[MB_CUR_MAX];
	p = const_cast<char*>(str.c_str());
	setBuf();
}

MultiByteIterator::~MultiByteIterator()
{
	delete [] buf;
}

char MultiByteIterator::operator * ()
{
	return buf[0];
}

MultiByteIterator::operator const char* ()
{
	return buf;
}

MultiByteIterator& MultiByteIterator::operator ++()
{
	setBuf();
	return (*this);
}

void MultiByteIterator::setBuf()
{
	int i = 0;
	int s = mblen(p, MB_CUR_MAX);
	for( i = 0; i < s; i++ ) {
		buf[i] = *(p++);
	}
	buf[i] = '\0';
}

MultiByteTokenizer::MultiByteTokenizer(std::string str)
	: itr(str)
{
}

MultiByteTokenizer::~MultiByteTokenizer()
{
}

std::string MultiByteTokenizer::get()
{
	while( strcmp(itr, " ") == 0 ||
		   strcmp(itr, "　") == 0 ||
		   strcmp(itr, ",") == 0 ) {
		++itr;
	}

	std::string token;
	while( strcmp(itr, " ") != 0 &&
		   strcmp(itr, "　") != 0 &&
		   strcmp(itr, ",") != 0 &&
		   strcmp(itr, "\0") != 0 ) {

		const char* p = itr;
		int s = mblen(p, MB_CUR_MAX);
		for( int i = 0; i < s; i++ ) {
			token.push_back(p[i]);
		}

		++itr;
	}

	return token;
}
