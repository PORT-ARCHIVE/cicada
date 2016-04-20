// © 2016 PORT INC.

#include <cstdlib>
#include "MultiByteTokenizer.hpp"

MultiByteIterator::MultiByteIterator(const std::string& arg)
	: str(arg)
{
	buf = new char[MB_CUR_MAX]; // T.B.D.
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
	int s = mblen(p, MB_CUR_MAX); // T.B.D.
	for( i = 0; i < s; i++ ) {
		buf[i] = *(p++);
	}
	buf[i] = '\0'; // T.B.D.
}

MultiByteTokenizer::MultiByteTokenizer(const std::string& str)
	: itr(str)
{
}

MultiByteTokenizer::~MultiByteTokenizer()
{
}

void MultiByteTokenizer::setSeparator(const std::string& sep)
{
	seps.push_back(sep);
}

bool MultiByteTokenizer::isSep(const char* s)
{
	bool ret = false;
	for( const auto& sep : seps ) {
		if( s == sep ) {
			ret = true;
			break;
		}
	}
	return ret;
}

std::string MultiByteTokenizer::get()
{
	while( isSep(itr) ) {
		++itr;
	}

	std::string token;
	while( !isSep(itr) && strcmp(itr, "\0") != 0 ) {

		const char* p = itr;
		int s = mblen(p, MB_CUR_MAX);
		for( int i = 0; i < s; i++ ) {
			token.push_back(p[i]);
		}

		++itr;
	}

	return ( std::move(token) );
}
