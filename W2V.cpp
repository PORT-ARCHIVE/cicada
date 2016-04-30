// © 2016 PORT INC.

#include <cstdlib>
#include <sstream>
#include "W2V.hpp"
#include "Error.hpp"
#include "Logger.hpp"

namespace W2V {

	Matrix_::Matrix_()
		: words(-1)
		, size(-1)
	{
		Logger::debug() << "Matrix_()";
	}

	Matrix_::~Matrix_()
	{
		Logger::debug() << "~Matrix_()";
	}

	void Matrix_::read(const std::string& file)
 	{
        FILE* f = std::fopen(file.c_str(), "rb");
   	    if( f == NULL ) {
			std::stringstream ss;
			ss << "cannot open such file: " << file;
			throw Error(ss.str());
        }

        //long long words;
     	fscanf(f, "%lld", &words);

        //long long size;
    	fscanf(f, "%lld", &size);

        M.resize(words);

        char c;
		for( long long i = 0; i < words; i++ ) {

            std::string w;

            while(1) {
                c = std::fgetc(f);
                if( c == '\n' ) {
                    continue;
                } else if( std::feof(f) || c == ' ' ) {
                    w2i_[w] = i;
                    i2w_[i] = w;
		 			break;
		 		}
		 		w.push_back(c);
		 	}

            vector& v = M[i];
			v.resize(size);
		 	for( long long j = 0; j < size; j++ ) {
		 		float e;
                std::fread((char*)&e, sizeof(float), 1, f);
		 		v(j) = e;
		 	}

		 	// double n = norm_2(v);
		 	// v = v/n;
		}

        std::fclose(f);
	}

	long long Matrix_::w2i(const std::string& w)
	{
		long long ret = -1;
        auto ip = w2i_.find(w);
        if( ip != w2i_.end() ) {
            ret = ip->second;
        }
		return ret;
	}

	const vector& Matrix_::i2v(int i)
	{
		return M.at(i);
	}
}
