// © 2016 PORT INC.

#include <boost/numeric/ublas/io.hpp>
#include "Optimizer.hpp"
#include "Error.hpp"
#include "Logger.hpp"

namespace Optimizer {

	using namespace boost::numeric::ublas;

	QuasiNewton_::QuasiNewton_(int d, ObjectiveFunction f)
		: dim(d)
		, itr(0)
		, maxIteration(256)
		, ofunc(f)
		, re(1.0e-6)
		, ae(1.0e-6)
		, beta(1.0)
		, minBeta(1.0e-2) // T.B.D.
		, xi(0.5)
		, tau(0.5)
		, r0(0.0)
		, x(dim)
		, dx(dim)
		, g0(dim)
		, g1(dim)
		, y(dim)
		, I(dim)
		, H0(dim, dim)
		, H1(dim, dim)
		, A(dim, dim)
		, B(dim, dim)
	{
	}

	void QuasiNewton_::optimize()
	{
		ofunc->preProcess(x);

		H0 = I;
		g0 = ofunc->grad(x);                 Logger::out(2) << "g0=" << g0 << std::endl;
		double alpha = 1.0;

		while(1) {

			ofunc->beginLoopProcess(x);

			d = - prod(H0, g0);              Logger::out(2) << "d=" << d << std::endl;
			alpha = linearSearch(d);         Logger::out(2) << "alpha=" << alpha << std::endl;
			dx = alpha * d;                  Logger::out(2) << "dx=" << dx << std::endl;
			x = x + dx;                      Logger::out(2) << "x=" << x << std::endl;
			ofunc->afterUpdateXProcess(x);
			if( isConv() ) break;
			g1 = ofunc->grad(x);             Logger::out(2) << "g1=" << g1 << std::endl;
			y = g1 - g0;                     Logger::out(2) << "y=" << y << std::endl;
			g0 = g1;
			updateMatrix();                  Logger::out(2) << "H=" << H0 << std::endl;

			ofunc->endLoopProcess(x);

			++itr;
		}

		ofunc->postProcess(x);
	}

	double QuasiNewton_::linearSearch(vector& d)
	{
		double beta = 1.0;
		double xi = 0.5;
		double tau = 0.5;

		double gd = inner_prod(g0, d);
		double f0 = ofunc->value(x);
		vector x1 = x + beta*d;
		double f1 = ofunc->value(x1);

		while( xi*beta*gd < f1 - f0 ) { // Armijo's rule
			beta *= tau;
			x1 = x + beta*d;
			f1 = ofunc->value(x1);
			if( beta < minBeta ) {
				break;
			}
		}

		return beta;
	}

	bool QuasiNewton_::isConv()
	{
		bool flg = false;
		double r = sqrt(inner_prod(dx, dx));

		if( itr == 0 ) {

			r0 = r;

		} else {

			double err = r/(r0*re + ae);
			flg = ( err < 1.0 );
			double f = ofunc->value(x);
			Logger::out(1) << "f=" << f << " err=" << err << std::endl;
		}

		if( !flg && itr == maxIteration ) {
			throw Error("iteration limit");
		}

		return flg;
	}

	QuasiNewton createBfgs(int dim, ObjectiveFunction ofunc)
	{
		return std::shared_ptr<QuasiNewton_>( new Bfgs(dim, ofunc) );
	}

	Bfgs::Bfgs(int dim, ObjectiveFunction ofunc)
		: QuasiNewton_(dim, ofunc)
	{
	}

	void Bfgs::updateMatrix()
	{
		double p = inner_prod(y, dx);                     Logger::out(3) << "p=" << p << std::endl;
		A = outer_prod(dx, dx)/p; 	                      Logger::out(3) << "A=" << A << std::endl;
		B = I - outer_prod(y, dx)/p;                      Logger::out(3) << "B=" << B << std::endl;
		H1 = prod(trans(B), matrix(prod(H0, B))) + A;     Logger::out(3) << "H=" << H1 << std::endl;
		H0 = H1;
	}

	class test1 : public ObjectiveFunction_ {
	public:
		test1(){}
		virtual ~test1(){}
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
			Logger::out(1) << " " << x[0] << " " << x[1] << std::endl;
		}
		virtual void beginLoopProcess(vector& x){}
		virtual void afterUpdateXProcess(vector& x){}
		virtual void endLoopProcess(vector& x){}
		virtual void postProcess(vector& x){}
	private:
		double x0;
		double x1;
	};

	class test2 : public ObjectiveFunction_ {
	public:
		test2(){}
		virtual ~test2(){}
		virtual double value(vector& x){
			double a = (x[1]-x[0]*x[0]);
			double b = (1-x[0]);
			return ( 100*a*a + b*b );
		}
		virtual vector grad(vector& x){
			vector g(2);
			g(0) = -400*(x[1]-x[0]*x[0])*x[0] - 2*(1-x[0]);
			g(1) = 200*(x[1]-x[0]*x[0]);
			return std::move(g);
		}
		virtual void preProcess(vector& x){
			x[0] = -1.9;
			x[1] = 2;
			Logger::out(1) << " " << x[0] << " " << x[1] << std::endl;
		}
		virtual void beginLoopProcess(vector& x){}
		virtual void afterUpdateXProcess(vector& x){
			Logger::out(1) << " " << x[0] << " " << x[1] << std::endl;
		}
		virtual void endLoopProcess(vector& x){}
		virtual void postProcess(vector& x){
			Logger::out(1) << " " << x[0] << " " << x[1] << std::endl;
		}
	private:
		double x0;
		double x1;
	};

	void test() {
		//QuasiNewton qn = createBfgs(2, std::shared_ptr<ObjectiveFunction_>(new test1()) );
		QuasiNewton qn = createBfgs(2, std::shared_ptr<ObjectiveFunction_>(new test2()) );
		qn->setRe(1.0e-4);
		qn->setAe(1.0e-4);
		qn->setMaxIteration(10000);
		qn->optimize();
	}
}
