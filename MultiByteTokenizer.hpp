// © 20116 PORT INC.
#ifndef MULTI_BYTE_TOKENIZER__H
#define MULTI_BYTE_TOKENIZER__H

#include <cstring>
#include <string>

class MultiByteIterator {
public:
	MultiByteIterator(const std::string& arg);
	virtual ~MultiByteIterator();
	char operator * ();
	operator const char* ();
	MultiByteIterator& operator ++();
	void setBuf();

private:
	std::string str;
	char* buf;
	char* p;
};

class MultiByteTokenizer
{
public:
	MultiByteTokenizer(const std::string& str);
	virtual ~MultiByteTokenizer();
	std::string get();

private:
	MultiByteIterator itr;
};

#endif // MULTI_BYTE_TOKENIZER__H
