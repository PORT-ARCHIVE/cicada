// © 2016 PORT INC.

#ifndef OPTIMIZATION__HPP
#define OPTIMIZATION__HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace Optimizer {

	using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;
    typedef boost::numeric::ublas::matrix<double> matrix;

	class ObjectiveFunction_ {
	public:
		ObjectiveFunction_(){};
		virtual ~ObjectiveFunction_(){};
		virtual double value(vector& x) = 0;
		virtual vector grad(vector& x) = 0;
		virtual void preProcess(vector& x) = 0;
		virtual void beginLoopProcess(vector& x) = 0;
		virtual void afterUpdateXProcess(vector& x) = 0;
		virtual void endLoopProcess(vector& x) = 0;
		virtual void postProcess(vector& x) = 0;
	};

	typedef std::shared_ptr<ObjectiveFunction_> ObjectiveFunction;

	class UnconstrainedNLP_ {

	public:

		UnconstrainedNLP_(int dim, ObjectiveFunction ofunc);
		virtual ~UnconstrainedNLP_(){};
		virtual void optimize() = 0;
		virtual bool isConv();
		void setAe(double arg) { ae = arg; };
		void setRe(double arg) { re = arg; };
		void setMaxIteration(int arg) { maxIteration = arg; };
		virtual double linearSearch(vector& g);

	protected:

		int dim;
		int itr;
		int maxIteration;
		ObjectiveFunction ofunc;
		double beta;
		double minBeta;
		double xi;
		double tau;
		double r0;
		double re;
		double ae;
		vector x;
		vector dx;
		vector g0;
		vector g1;
		vector d;
	};

	class SteepestDescent_ : public UnconstrainedNLP_ {

	public:

		SteepestDescent_(int dim, ObjectiveFunction ofunc);
		virtual ~SteepestDescent_(){};
		virtual void optimize();

	protected:

		// T.B.D.
		// virtual double linearSearch(vector& g);
		// virtual bool isConv();
	};

	class QuasiNewton_ : public UnconstrainedNLP_ {

	public:

		QuasiNewton_(int dim, ObjectiveFunction ofunc);
		virtual ~QuasiNewton_(){};
		virtual void optimize();
		void setBeta(double arg) { beta = arg; };
		void setXi(double arg) { xi = arg; };
		void setTau(double arg) { tau = arg; };

	protected:

		// virtual double linearSearch(vector& g);
		// virtual bool isConv();
		virtual void updateMatrix() = 0;

	protected:

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
		Bfgs(int dim, ObjectiveFunction ofunc);
		virtual void updateMatrix();
	};

	QuasiNewton createBfgs(int dim, ObjectiveFunction ofunc);

	void test();

}

#endif // OPTIMIZATION__HPP
