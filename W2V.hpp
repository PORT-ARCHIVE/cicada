// © 2016 PORT INC.

#ifndef W2V__H
#define W2V__H

#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace W2V { // word2vecのWrapper

	using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;
    typedef boost::numeric::ublas::matrix<double> matrix;

 	class Matrix {
	public:
		Matrix();
		virtual ~Matrix();
		void read(std::string file);
		int w2i(std::string w);
		vector i2v(int i);
	private:
		matrix m;
	};
}

#endif // W2V__H
