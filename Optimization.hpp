// © 2016 PORT INC.

#ifndef OPTIMIZATION__HPP
#define OPTIMIZATION__HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace Optimization {

	using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;
    typedef boost::numeric::ublas::matrix<double> matrix;

	class ObjectFunction_ {
	public:
		ObjectFunction_(){};
		virtual ~ObjectFunction_(){};
		virtual double value(vector& x) = 0;
		virtual vector grad(vector& x) = 0;
		virtual void preProcess(vector& x) = 0;
		virtual void postProcess(vector& x) = 0;
	};

	typedef std::shared_ptr<ObjectFunction_> ObjectFunction;

	class QuasiNewton_ {
	public:

		QuasiNewton_(int dim, ObjectFunction ofunc);
		virtual ~QuasiNewton_();
		void optimize();
		void setAe(double ae);
		void setRe(double re);
		void setMaxIteration(int limit);
		virtual void updateDx() = 0;

	protected:

		virtual bool isConv();
		virtual double linearSearch(vector& g);

	protected:

		int itr;
		int dim;
		ObjectFunction ofunc;
		double re;
		double ae;
		double r0;
		int maxIteration;
		vector x;
		vector dx;
		vector g0;
		vector g1;
		vector y;
		identity_matrix<double> I;
		matrix H0;
		matrix H1;
		matrix A;
		matrix B;
	};

	typedef std::shared_ptr<QuasiNewton_> QuasiNewton;

	class Bfgs : public QuasiNewton_ {
	public:
		Bfgs(int dim, ObjectFunction ofunc);
		virtual void updateDx();
	};

	QuasiNewton createBfgs(int dim, ObjectFunction ofunc);

	void test();

}

#endif // OPTIMIZATION__HPP
