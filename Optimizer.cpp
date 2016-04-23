// © 2016 PORT INC.

#include <boost/numeric/ublas/io.hpp>
#include <boost/format.hpp>
#include "Optimizer.hpp"
#include "Error.hpp"
#include "Logger.hpp"

namespace Optimizer {

	using namespace boost::numeric::ublas;

	////////

	UnconstrainedNLP_::UnconstrainedNLP_(int d, ObjectiveFunction f)
		: flg(0x0)
		, dim(d)
		, itr(0)
		, maxIteration(256)
		, ofunc(f)
		, e0(1.0e-2)
		, beta(1.0)
		, minBeta(1.0e-2)
		, xi(0.5)
		, tau(0.5)
		, r0(0.0)
		, re(1.0e-6)
		, ae(1.0e-6)
		, x(dim)
		, dx(dim)
		, g0(dim)
		, g1(dim)
		, d(dim)
	{
	}

	double UnconstrainedNLP_::avoidDivergence(vector& d, double& f1)
	{
		double beta1 = beta;
		vector x1 = x + beta1*d;

		while(1) {
			try {
				f1 = ofunc->value(x1);
				break;
			} catch(Error& e) {
				beta1 *= tau;
				x1 = x + beta1*d;
			}
		}

		return beta1;
	}

	double UnconstrainedNLP_::linearSearch(vector& d)
	{
		vector x1;
		double gd = inner_prod(g0, d);
		double f0 = ofunc->value(x);
		double f1 = 0.0;
		double beta1 = avoidDivergence(d, f1);

		while( xi*beta1*gd < f1 - f0 ) { // Armijo's rule
			beta1 *= tau;
			x1 = x + beta1*d;
			f1 = ofunc->value(x1);
			if( beta1 < minBeta ) {
				break;
			}
		}

		return beta1;
	}

	bool UnconstrainedNLP_::isConv()
	{
		bool flg = false;
		double r = sqrt(inner_prod(g0, g0));


		if( itr == 0 ) {

			r0 = r;

		} else {

			double err = r/(r0*re + ae);
			flg = ( err < 1.0 );
			double f = ofunc->value(x);
			Logger::info() << boost::format("f= %10.6e |∇f|= %10.6e") % f % err;
		}

		if( !flg && itr == maxIteration ) {
			throw Error("iteration limit");
		}

		return flg;
	}

	////////

	SteepestDescent::SteepestDescent(int d, ObjectiveFunction f)
		: UnconstrainedNLP_(d, f)
		, adagrad(dim)
	{
	}

	UnconstrainedNLP createSteepestDescent(int dim, ObjectiveFunction ofunc)
	{
		return std::shared_ptr<SteepestDescent>( new SteepestDescent(dim, ofunc) );
	}

	void SteepestDescent::optimize()
	{
		ofunc->preProcess(x);

		g0 = ofunc->grad(x);                 Logger::debug() << "g0=" << g0;
		double alpha = 1.0;

		while(1) {

			ofunc->beginLoopProcess(x);

			d = - g0;                        Logger::debug() << "d=" << d;

			if( flg & ENABLE_ADAGRAD ) {
				auto iad = adagrad.begin();
				auto id = d.begin();
				for( auto& idx : dx ) {
					idx = e0/(1.0+sqrt(*iad++))*(*id++);
				}                            Logger::debug() << "dx=" << dx;
				double f1 = 0.0;
				alpha = avoidDivergence(d, f1);
			} else {
				alpha = linearSearch(d);     Logger::debug() << "alpha=" << alpha;
			}

			dx = alpha * d;                  Logger::debug() << "dx=" << dx;
			x = x + dx;                      Logger::debug() << "x=" << x;
			ofunc->afterUpdateXProcess(x);
			if( isConv() ) break;
			g0 = ofunc->grad(x);             Logger::debug() << "g0=" << g0;

			ofunc->endLoopProcess(x);

			++itr;
		}

		ofunc->postProcess(x);
	}

	bool SteepestDescent::isConv()
	{
		bool flg = false;
		vector g02 = element_prod(g0, g0);
		double r = sqrt( sum( g02 ) );
		adagrad += g02;

		if( itr == 0 ) {

			r0 = r;

		} else {

			double err = r/(r0*re + ae);
			flg = ( err < 1.0 );
			double f = ofunc->savedValue();
			Logger::info() << boost::format("f= %10.6e |∇f|= %10.6e") % f % err;
		}

		if( !flg && itr == maxIteration ) {
			throw Error("iteration limit");
		}

		return flg;
	}

	////////

	QuasiNewton_::QuasiNewton_(int d, ObjectiveFunction f)
		: UnconstrainedNLP_(d, f)
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
		g0 = ofunc->grad(x);                 Logger::debug() << "g0=" << g0;
		double alpha = 1.0;

		while(1) {

			ofunc->beginLoopProcess(x);

			d = - prod(H0, g0);              Logger::debug() << "d=" << d;
			alpha = linearSearch(d);         Logger::debug() << "alpha=" << alpha;
			dx = alpha * d;                  Logger::debug() << "dx=" << dx;
			x = x + dx;                      Logger::debug() << "x=" << x;
			ofunc->afterUpdateXProcess(x);
			if( isConv() ) break;
			g1 = ofunc->grad(x);             Logger::debug() << "g1=" << g1;
			y = g1 - g0;                     Logger::debug() << "y=" << y;
			g0 = g1;
			updateMatrix();                  Logger::debug() << "H=" << H0;

			ofunc->endLoopProcess(x);

			++itr;
		}

		ofunc->postProcess(x);
	}

	bool QuasiNewton_::isConv()
	{
		bool flg = false;
		double r = sqrt(inner_prod(g0, g0));


		if( itr == 0 ) {

			r0 = r;

		} else {

			double err = r/(r0*re + ae);
			flg = ( err < 1.0 );
			double f = ofunc->savedValue();
			Logger::info() << boost::format("f= %10.6e |∇f|= %10.6e") % f % err;
		}

		if( !flg && itr == maxIteration ) {
			throw Error("iteration limit");
		}

		return flg;
	}

	////////

	UnconstrainedNLP createBfgs(int dim, ObjectiveFunction ofunc)
	{
		return std::shared_ptr<UnconstrainedNLP_>( new Bfgs(dim, ofunc) );
	}

	Bfgs::Bfgs(int dim, ObjectiveFunction ofunc)
		: QuasiNewton_(dim, ofunc)
	{
	}

	void Bfgs::updateMatrix()
	{
		double p = inner_prod(y, dx);                     Logger::trace() << "p=" << p;
		A = outer_prod(dx, dx)/p; 	                      Logger::trace() << "A=" << A;
		B = I - outer_prod(y, dx)/p;                      Logger::trace() << "B=" << B;
		H1 = prod(trans(B), matrix(prod(H0, B))) + A;     Logger::trace() << "H=" << H1;
		H0 = H1;
	}

	////////

	class test1 : public ObjectiveFunction_ {
	public:
		test1(){}
		virtual ~test1(){}
		virtual double savedValue(){
			return f;
		}
		virtual double value(vector& x){
			double a = (x[0]-x[1]*x[1]);
			double b = (x[1]-2);
			f = ( 0.5*a*a + 0.25*b*b*b*b );
			return f;
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
			Logger::info() << " " << x[0] << " " << x[1];
		}
		virtual void beginLoopProcess(vector& x){}
		virtual void afterUpdateXProcess(vector& x){}
		virtual void endLoopProcess(vector& x){}
		virtual void postProcess(vector& x){}
	private:
		double x0;
		double x1;
		double f;
	};

	class test2 : public ObjectiveFunction_ {
	public:
		test2(){}
		virtual ~test2(){}
		virtual double savedValue(){
			return f;
		}
		virtual double value(vector& x){
			double a = (x[1]-x[0]*x[0]);
			double b = (1-x[0]);
			f = ( 100*a*a + b*b );
			return f;
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
			Logger::info() << " " << x[0] << " " << x[1];
		}
		virtual void beginLoopProcess(vector& x){}
		virtual void afterUpdateXProcess(vector& x){
			Logger::info() << " " << x[0] << " " << x[1];
		}
		virtual void endLoopProcess(vector& x){}
		virtual void postProcess(vector& x){
			Logger::info() << " " << x[0] << " " << x[1];
		}
	private:
		double x0;
		double x1;
		double f;
	};

	void test() {
		UnconstrainedNLP qn = createBfgs(2, std::shared_ptr<ObjectiveFunction_>(new test2()) );
		qn->setRe(1.0e-4);
		qn->setAe(1.0e-4);
		qn->setMaxIteration(10000);
		qn->optimize();
	}
}
