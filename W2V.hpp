// © 2016 PORT INC.

#ifndef W2V__H
#define W2V__H

#include <string>
#include <vector>
#include <map>
#include <boost/numeric/ublas/vector.hpp>

namespace W2V {

    using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;

 	class Matrix_ {
	public:
		Matrix_();
        virtual ~Matrix_();
		void read(const std::string& file);
		int w2i(const std::string& w);
		const vector& i2v(int i);
		long long getNumWords() { return words; }
		long long getSize() { return size; }
    private:
		long long words;
		long long size;
        std::map<std::string, long long> w2i_;
        std::map<long long, std::string> i2w_;
        std::vector<vector> M;
	};

	typedef std::shared_ptr<Matrix_> Matrix;
}

#endif // W2V__H
