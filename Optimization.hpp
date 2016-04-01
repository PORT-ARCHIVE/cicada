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
		ObjectFunction_();
		virtual ~ObjectFunction_();
		virtual double value() = 0;
		virtual vector grad() = 0;
		virtual void update(const vector& dx) = 0;
		virtual void preProcess() = 0;
		virtual void postProcess() = 0;
	};

	typedef std::shared_ptr<ObjectFunction_> ObjectFunction;

	class QuasiNewton_ {
	public:

		QuasiNewton_(int dim, ObjectFunction ofunc);
		virtual ~QuasiNewton_();
		void iterate();
		void setAe(double ae);
		void setRe(double re);
		void setMaxIteration(int limit);

	protected:

		virtual void updateDx() = 0;
		virtual bool isConv();

	protected:

		bool first;
		int dim;
		ObjectFunction ofunc;
		double alpha;
		double re;
		double ae;
		double r0;
		int maxIteration;
		vector dx;
		vector g0;
		vector g1;
		vector y;
		identity_matrix<double> I;
		matrix H0;
		matrix H1;
	};

	typedef std::shared_ptr<QuasiNewton_> QuasiNewton;

	class Bfgs : public QuasiNewton_ {
	public:
		Bfgs(int dim, ObjectFunction ofunc);
	protected:
		virtual void updateDx();
	};

	QuasiNewton createBfgs(int dim, ObjectFunction ofunc) { return std::shared_ptr<QuasiNewton_>( new Bfgs(dim, ofunc) ); }

}

#endif // OPTIMIZATION__HPP
