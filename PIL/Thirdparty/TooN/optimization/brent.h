#ifndef TOON_BRENT_H
#define TOON_BRENT_H
#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <iomanip>


namespace TooN
{
	using std::numeric_limits;

	/// brent_line_search performs Brent's golden section/quadratic interpolation search
	/// on the functor provided. The inputs a, x, b must bracket the minimum, and
	/// must be in order, so  that \f$ a < x < b \f$ and \f$ f(a) > f(x) < f(b) \f$.
	/// @param a The most negative point along the line.
	/// @param x The central point.
	/// @param fx The value of the function at the central point (\f$b\f$).
	/// @param b The most positive point along the line.
	/// @param func The functor to minimize
	/// @param maxiterations  Maximum number of iterations
	/// @param tolerance Tolerance at which the search should be stopped (defults to sqrt machine precision)
	/// @param epsilon Minimum bracket width (defaults to machine precision)
	/// @return The minima position is returned as the first element of the vector,
	///         and the minimal value as the second element.
	/// @ingroup gOptimize
	template<class Functor, class Precision> Vector<2, Precision> brent_line_search(Precision a, Precision x, Precision b, Precision fx, const Functor& func, int maxiterations, Precision tolerance = sqrt(numeric_limits<Precision>::epsilon()), Precision epsilon = numeric_limits<Precision>::epsilon())
	{
		using std::min;
		using std::max;

		using std::abs;
		using std::sqrt;

		//The golden ratio:
		const Precision g = (3.0 - sqrt(5))/2;
		
		//The following points are tracked by the algorithm:
		//a, b bracket the interval
		// x   is the best value so far
		// w   second best point so far
		// v   third best point so far
		// These may not be unique.
		
		//The following points are used during iteration
		// u   the point currently being evaluated
		// xm   (a+b)/2
		
		//The updates are tracked as:
		//e is the distance moved last step, or current if golden section is used
		//d is the point moved in the current step
		
		Precision w=x, v=x, fw=fx, fv=fx;
		
		Precision d=0, e=0;
		int i=0;

		while(abs(b-a) > (abs(a) + abs(b)) * tolerance + epsilon && i < maxiterations)
		{
			i++;
			//The midpoint of the bracket
			const Precision xm = (a+b)/2;

			//Per-iteration tolerance 
			const Precision tol1 = abs(x)*tolerance + epsilon;

			//If we recently had an unhelpful step, then do
			//not attempt a parabolic fit. This prevents bad parabolic
			//fits spoiling the convergence. Also, do not attempt to fit if
			//there is not yet enough unique information in x, w, v.
			if(abs(e) > tol1 && w != v)
			{
				//Attempt a parabolic through the best 3 points. The pdata is shifted
				//so that x = 0 and f(x) = 0. The remaining parameters are:
				//
				// xw  = w'    = w-x
				// fxw = f'(w) = f(w) - f(x)
				//
				// etc:
				const Precision fxw = fw - fx;
				const Precision fxv = fv - fx;
				const Precision xw = w-x;
				const Precision xv = v-x;

				//The parabolic fit has only second and first order coefficients:
				//const Precision c1 = (fxv*xw - fxw*xv) / (xw*xv*(xv-xw));
				//const Precision c2 = (fxw*xv*xv - fxv*xw*xw) / (xw*xv*(xv-xw));
				
				//The minimum is at -.5*c2 / c1;
				//
				//This can be written as p/q where:
				const Precision p = fxv*xw*xw - fxw*xv*xv;
				const Precision q = 2*(fxv*xw - fxw*xv);

				//The minimum is at p/q. The minimum must lie within the bracket for it
				//to be accepted. 
				// Also, we want the step to be smaller than half the old one. So:

				if(q == 0 || x + p/q < a || x+p/q > b || abs(p/q) > abs(e/2))
				{
					//Parabolic fit no good. Take a golden section step instead
					//and reset d and e.
					if(x > xm)
						e = a-x;
					else
						e = b-x;

					d = g*e;
				}
				else
				{
					//Parabolic fit was good. Shift d and e
					e = d;
					d = p/q;
				}
			}
			else
			{
				//Don't attempt a parabolic fit. Take a golden section step
				//instead and reset d and e.
				if(x > xm)
					e = a-x;
				else
					e = b-x;

				d = g*e;
			}

			const Precision u = x+d;
			//Our one function evaluation per iteration
			const Precision fu = func(u);

			if(fu < fx)
			{
				//U is the best known point.

				//Update the bracket
				if(u > x)
					a = x;
				else
					b = x;

				//Shift v, w, x
				v=w; fv = fw;
				w=x; fw = fx;
				x=u; fx = fu;
			}
			else
			{
				//u is not the best known point. However, it is within the
				//bracket.
				if(u < x)
					a = u;
				else
					b = u;

				if(fu <= fw || w == x)
				{
					//Here, u is the new second-best point
					v = w; fv = fw;
					w = u; fw = fu;
				}
				else if(fu <= fv || v==x || v == w)
				{
					//Here, u is the new third-best point.
					v = u; fv = fu;
				}
			}
		}

		return makeVector(x, fx);
	}
}
#endif
