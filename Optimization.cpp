// © 2016 PORT INC.

#include <boost/numeric/ublas/io.hpp>
#include "Optimization.hpp"
#include "Error.hpp"
#include "Logger.hpp"

namespace Optimization {

	using namespace boost::numeric::ublas;

	QuasiNewton_::QuasiNewton_(int d, ObjectFunction f)
		: itr(0)
		, dim(d)
		, ofunc(f)
		, r0(0.0)
		, x(dim)
		, dx(dim)
		, g0(dim)
		, g1(dim)
		, I(dim)
		, H0(dim, dim)
		, H1(dim, dim)
		, A(dim, dim)
		, B(dim, dim)
	{
	}

	QuasiNewton_::~QuasiNewton_()
	{
	}

	void QuasiNewton_::setAe(double arg)
	{
		ae = arg;
	}

	void QuasiNewton_::setRe(double arg)
	{
		re = arg;
	}

	void QuasiNewton_::setMaxIteration(int arg)
	{
		maxIteration = arg;
	}

	void QuasiNewton_::optimize()
	{
		ofunc->preProcess(x);

		H0 = I;
		g0 = ofunc->grad(x);

		do {

			updateDx();
			x = x + dx;          Logger::out(2) << "x=" << x << std::endl;
			g1 = ofunc->grad(x); Logger::out(2) << "g1=" << g1 << std::endl;
			y = g1 - g0;         Logger::out(2) << "y=" << y << std::endl;
			g0 = g1;

			if( maxIteration == itr ) {
				throw Error("iteration limit");
			}

		} while( !isConv() );

		ofunc->postProcess(x);
	}

	bool QuasiNewton_::isConv()
	{
		bool flg = false;
		double r = sqrt(inner_prod(dx, dx));

		if( itr++ == 0 ) {

			r0 = r;

		} else {

			double err = r/(r0*re + ae);
			flg = ( err < 1.0 );
			double f = ofunc->value(x);
			Logger::out(1) << "f=" << f << " err=" << err << std::endl;
		}

		return flg;
	}

	double QuasiNewton_::linearSearch(vector& d)
	{
		double beta = 1.0;
		double phy = 0.8;
		double tau = 0.3;

		double gd = inner_prod(g0, d);
		double f0 = ofunc->value(x);
		vector x1 = x + beta*d;
		double f1 = ofunc->value(x1);

		while( phy*beta*gd < f1 - f0 ) {
			beta *= tau;
			x1 = x + beta*d;
			f1 = ofunc->value(x1);
		}

		return beta;
	}

	QuasiNewton createBfgs(int dim, ObjectFunction ofunc)
	{
		return std::shared_ptr<QuasiNewton_>( new Bfgs(dim, ofunc) );
	}

	Bfgs::Bfgs(int dim, ObjectFunction ofunc)
		: QuasiNewton_(dim, ofunc)
	{
	}

	void Bfgs::updateDx()
	{
		vector d(dim);

		if( itr == 0 ) {

			d = - g0;

		} else {

			double p = inner_prod(y, dx);                 Logger::out(2) << "p=" << p << std::endl;
			A = outer_prod(dx, dx)/p; 	                  Logger::out(2) << "A=" << A << std::endl;
			B = I - outer_prod(y, dx)/p;                  Logger::out(2) << "B=" << B << std::endl;
			H1 = prod(trans(B), matrix(prod(H0, B))) + A; Logger::out(2) << "H=" << H1 << std::endl;
			H0 = H1;
			d = - prod(H1, g0);
		}

		double alpha = linearSearch(d);
		dx = alpha * d;                                   Logger::out(2) << "dx=" << dx << std::endl;
	}

	class Obj_ : public ObjectFunction_ {
	public:
		Obj_(){}
		virtual ~Obj_(){}
		virtual double value(vector& x){
			double a = (x[0]-x[1]*x[1]);
			double b = (x[1]-2);
			return ( 0.5*a*a + 0.25*b*b*b*b );
		}
		virtual vector grad(vector& x){
			vector g(2);
			g(0) = x[0]-x[1]*x[1];
			double a = (x[1]-2);
			g(1) = -2.0*x[1]*(x[0]-x[1]*x[1])+a*a*a;
			return std::move(g);
		}
		virtual void preProcess(vector& x){
			x[0] = 0;
			x[1] = 0;
		}
		virtual void postProcess(vector& x){}
	private:
		double x0;
		double x1;
	};

	void test() {
		QuasiNewton qn = createBfgs(2, std::shared_ptr<ObjectFunction_>(new Obj_()) );
		qn->setRe(1.0e-8);
		qn->setAe(1.0e-8);
		qn->setMaxIteration(10000);
		qn->optimize();
	}
}
