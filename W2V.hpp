// © 2016 PORT INC.

#ifndef W2V__H
#define W2V__H

#include <string>
#include <vector>
#include <map>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace W2V {

    using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;
    typedef boost::numeric::ublas::matrix<double> matrix;

 	class Matrix {
	public:
		Matrix();
        virtual ~Matrix();
		void read(std::string file);
		int w2i(const std::string& w);
		const vector& i2v(int i);
    private:
        std::map<std::string, long long> w2i_;
        std::map<long long, std::string> i2w_;
        std::vector<vector> M;
	};
}

#endif // W2V__H
