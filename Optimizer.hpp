// © 2016 PORT INC.

#ifndef OPTIMIZATION__HPP
#define OPTIMIZATION__HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace Optimizer {

	using namespace boost::numeric::ublas;
    typedef boost::numeric::ublas::vector<double> vector;
    typedef boost::numeric::ublas::matrix<double> matrix;

	enum {
		ENABLE_ADAGRAD = 0x1
	};

	////////

	class ObjectiveFunction_ {
	public:
		ObjectiveFunction_(){};
		virtual ~ObjectiveFunction_(){};
		virtual double value(vector& x) = 0;
		virtual double savedValue() = 0;
		virtual vector grad(vector& x) = 0;
		virtual void preProcess(vector& x) = 0;
		virtual void beginLoopProcess(vector& x) = 0;
		virtual void afterUpdateXProcess(vector& x) = 0;
		virtual void endLoopProcess(vector& x) = 0;
		virtual void postProcess(vector& x) = 0;
	};

	typedef std::shared_ptr<ObjectiveFunction_> ObjectiveFunction;

	////////

	class UnconstrainedNLP_ {

	public:

		UnconstrainedNLP_(int dim, ObjectiveFunction ofunc);
		virtual ~UnconstrainedNLP_(){};
		virtual void optimize() = 0;
		virtual bool isConv();
		void setAe(double arg) { ae = arg; };
		void setRe(double arg) { re = arg; };
		void setMaxIteration(int arg) { maxIteration = arg; };
		void setE0(double arg) { e0 = arg; };
		void setFlg(int arg) { flg = arg; };

	protected:

		virtual double avoidDivergence(vector& g, double& f1);
		virtual double linearSearch(vector& g);

	protected:

		int flg;
		int dim;
		int itr;
		int maxIteration;
		ObjectiveFunction ofunc;
		double e0;
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

	typedef std::shared_ptr<UnconstrainedNLP_> UnconstrainedNLP;

	////////

	class SteepestDescent : public UnconstrainedNLP_ {

	public:

		SteepestDescent(int dim, ObjectiveFunction ofunc);
		virtual ~SteepestDescent(){};
		virtual void optimize();
		virtual bool isConv();

	protected:

		vector adagrad;
	};

	UnconstrainedNLP createSteepestDescent(int dim, ObjectiveFunction ofunc);

	////////

	class QuasiNewton_ : public UnconstrainedNLP_ {

	public:

		QuasiNewton_(int dim, ObjectiveFunction ofunc);
		virtual ~QuasiNewton_(){};
		virtual void optimize();
		virtual bool isConv();
		void setBeta(double arg) { beta = arg; };
		void setXi(double arg) { xi = arg; };
		void setTau(double arg) { tau = arg; };

	protected:

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

	////////

	class Bfgs : public QuasiNewton_ {
	public:
		Bfgs(int dim, ObjectiveFunction ofunc);
		virtual void updateMatrix();
	};

	UnconstrainedNLP createBfgs(int dim, ObjectiveFunction ofunc);

	void test();

}

#endif // OPTIMIZATION__HPP
