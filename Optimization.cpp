// © 2016 PORT INC.

#include "Optimization.hpp"

namespace Optimization {

	using namespace boost::numeric::ublas;

	QuasiNewton::QuasiNewton(int d, ObjectFunction f)
		: first(true)
		, dim(d)
		, ofunc(f)
		, alpha(1.0)
		, r0(0.0)
		, dx(dim)
		, g0(dim)
		, g1(dim)
		, I(dim)
		, H0(dim, dim)
		, H1(dim, dim)
	{
	}

	QuasiNewton::~QuasiNewton()
	{
	}

	void QuasiNewton::setAe(double arg)
	{
		ae = arg;
	}

	void QuasiNewton::setRe(double arg)
	{
		re = arg;
	}

	void QuasiNewton::setMaxIteration(int arg)
	{
		maxIteration = arg;
	}

	void QuasiNewton::iterate()
	{
		ofunc->preProcess();

		H1 = H0 = I;
		g1 = g0 = ofunc->grad();

		int itr = 0;

		while( isConv() ) {

			if( maxIteration == itr ) {
				throw "iteration limit"; // T.B.D.
			}

			auto tg = g1;
			auto tH = H1;

			updateDx();
			ofunc->update(dx);
			g1 = ofunc->grad();
			y = g1 - g0;

			g0 = tg;
			H0 = tH;

			if( first ) first = false;
			++itr;
		}

		ofunc->postProcess();
	}

	bool QuasiNewton::isConv()
	{
		bool flg = false;
		double r = sqrt(inner_prod(dx, dx));
		double err;

		if( first ) {
			r0 = r;
		} else {
			err = r / ( re*r0 + ae );
			flg = ( err < 1 );
		}

		double f = ofunc->value();
		// Logging f, err

		return flg;
	}

	Bfgs::Bfgs(int dim, ObjectFunction ofunc)
		: QuasiNewton(dim, ofunc)
	{
	}

	void Bfgs::updateDx()
	{
		if( first ) {

			dx = g0;

		} else {

			auto p = inner_prod(y, dx);
			auto A = outer_prod(dx, dx)/p;
			auto B = I - outer_prod(y, dx)/p;
			H1 = prod(B, matrix(prod(H0, B))) + A;
			dx = prod(H1, g0);
		}
	}
}
