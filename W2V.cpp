// © 2016 PORT INC.

#include <W2V.hpp>
#include <cstdlib>
#include "Error.hpp"

namespace W2V {

	Matrix::Matrix()
	{
	}

	Matrix::~Matrix()
	{
	}

	void Matrix::read(std::string file)
 	{
        FILE* f = std::fopen(file.c_str(), "rb");
   	    if( f == NULL ) {
			throw Error("cannot open such file");
        }

        long long words;
     	fscanf(f, "%lld", &words);

        long long size;
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

		 	double n = norm_2(v);
		 	v = v/n;
		}

        std::fclose(f);
	}

	int Matrix::w2i(const std::string& w)
	{
        auto ip = w2i_.find(w);
        if( ip != w2i_.end() ) {
            return ip->second;
        } else {
            throw Error("");
        }
	}

	const vector& Matrix::i2v(int i)
	{
		const vector& v = M[i];
		return v;
	}
}
