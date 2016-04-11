#ifndef TOON_DOWNHILL_SIMPLEX_H
#define TOON_DOWNHILL_SIMPLEX_H
#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <algorithm>
#include <cstdlib>

namespace TooN
{

/** This is an implementation of the Downhill Simplex (Nelder & Mead, 1965)
    algorithm. This particular instance will minimize a given function.
	
	The function maintains \f$N+1\f$ points for a $N$ dimensional function, \f$f\f$
	
	At each iteration, the following algorithm is performed:
	- Find the worst (largest) point, \f$x_w\f$.
	- Find the centroid of the remaining points, \f$x_0\f$.
	- Let \f$v = x_0 - x_w\f$
	- Compute a reflected point, \f$ x_r = x_0 + \alpha v\f$
	- If \f$f(x_r)\f$ is better than the best point
	  - Expand the simplex by extending the reflection to \f$x_e = x_0 + \rho \alpha v \f$
	  - Replace \f$x_w\f$ with the best point of \f$x_e\f$,  and \f$x_r\f$.
	- Else, if  \f$f(x_r)\f$ is between the best and second-worst point
	  - Replace \f$x_w\f$ with \f$x_r\f$.
	- Else, if  \f$f(x_r)\f$ is better than \f$x_w\f$
	  - Contract the simplex by computing \f$x_c = x_0 + \gamma v\f$
	  - If \f$f(x_c) < f(x_r)\f$
	    - Replace \f$x_w\f$ with \f$x_c\f$.
	- If \f$x_w\f$ has not been replaced, then shrink the simplex by a factor of \f$\sigma\f$ around the best point.

	This implementation uses:
	- \f$\alpha = 1\f$
	- \f$\rho = 2\f$
	- \f$\gamma = 1/2\f$
	- \f$\sigma = 1/2\f$
	
	Example usage:
	@code
#include <TooN/optimization/downhill_simplex.h>
using namespace std;
using namespace TooN;

double sq(double x)
{
	return x*x;
}

double Rosenbrock(const Vector<2>& v)
{
		return sq(1 - v[0]) + 100 * sq(v[1] - sq(v[0]));
}

int main()
{
		Vector<2> starting_point = makeVector( -1, 1);

		DownhillSimplex<2> dh_fixed(Rosenbrock, starting_point, 1);

		while(dh_fixed.iterate(Rosenbrock))
		{
			cout << dh.get_values()[dh.get_best()] << endl;
		}
		
		cout << dh_fixed.get_simplex()[dh_fixed.get_best()] << endl;
}

	@endcode


    @ingroup gOptimize
	@param   N The dimension of the function to optimize. As usual, the default value of <i>N</i> (-1) indicates
	         that the class is sized at run-time.


**/
template<int N=-1, typename Precision=double> class DownhillSimplex
{
	static const int Vertices = (N==-1?-1:N+1);
	typedef Matrix<Vertices, N, Precision> Simplex;
	typedef Vector<Vertices, Precision> Values;

	public:
		/// Initialize the DownhillSimplex class. The simplex is automatically
		/// generated. One point is at <i>c</i>, the remaining points are made by moving
		/// <i>c</i> by <i>spread</i> along each axis aligned unit vector.
		///
		///@param func       Functor to minimize.
		///@param c          Origin of the initial simplex. The dimension of this vector
		///                  is used to determine the dimension of the run-time sized version.
		///@param spread     Size of the initial simplex.
		template<class Function> DownhillSimplex(const Function& func, const Vector<N>& c, Precision spread=1)
		:simplex(c.size()+1, c.size()),values(c.size()+1)
		{
			alpha = 1.0;
			rho = 2.0;
			gamma = 0.5;
			sigma = 0.5;

			using std::sqrt;
			epsilon = sqrt(numeric_limits<Precision>::epsilon());
			zero_epsilon = 1e-20;

			restart(func, c, spread);
		}
		
		/// This function sets up the simplex around, with one point at \e c and the remaining
		/// points are made by moving by \e spread along each axis aligned unit vector.
		///
		///@param func       Functor to minimize.
		///@param c          \e c corner point of the simplex
		///@param spread     \e spread simplex size
		template<class Function> void restart(const Function& func, const Vector<N>& c, Precision spread)
		{
			for(int i=0; i < simplex.num_rows(); i++)
				simplex[i] = c;

			for(int i=0; i < simplex.num_cols(); i++)
				simplex[i][i] += spread;

			for(int i=0; i < values.size(); i++)
				values[i] = func(simplex[i]);
		}
		
		///Check to see it iteration should stop. You probably do not want to use
		///this function. See iterate() instead. This function updates nothing.
		///The termination criterion is that the simplex span (distancve between
		///the best and worst vertices) is small compared to the scale or 
		///small overall.
		bool finished()
		{
			Precision span =  norm(simplex[get_best()] - simplex[get_worst()]);
			Precision scale = norm(simplex[get_best()]);

			if(span/scale < epsilon || span < zero_epsilon)
				return 1;
			else 
				return 0;
		}
		
		/// This function resets the simplex around the best current point.
		///
		///@param func       Functor to minimize.
		///@param spread     simplex size
		template<class Function> void restart(const Function& func, Precision spread)
		{
			restart(func, simplex[get_best()], spread);
		}

		///Return the simplex
		const Simplex& get_simplex() const
		{
			return simplex;
		}
		
		///Return the score at the vertices
		const Values& get_values() const
		{
			return values;
		}
		
		///Get the index of the best vertex
		int get_best() const 
		{
			return std::min_element(&values[0], &values[0] + values.size()) - &values[0];
		}
		
		///Get the index of the worst vertex
		int get_worst() const 
		{
			return std::max_element(&values[0], &values[0] + values.size()) - &values[0];
		}

		///Perform one iteration of the downhill Simplex algorithm
		///@param func Functor to minimize
		template<class Function> void find_next_point(const Function& func)
		{
			//Find various things:
			// - The worst point
			// - The second worst point
			// - The best point
			// - The centroid of all the points but the worst
			int worst = get_worst();
			Precision second_worst_val=-HUGE_VAL, bestval = HUGE_VAL, worst_val = values[worst];
			int best=0;
			Vector<N> x0 = Zeros(simplex.num_cols());


			for(int i=0; i < simplex.num_rows(); i++)
			{
				if(values[i] < bestval)
				{
					bestval = values[i];
					best = i;
				}

				if(i != worst)
				{
					if(values[i] > second_worst_val)
						second_worst_val = values[i];

					//Compute the centroid of the non-worst points;
					x0 += simplex[i];
				}
			}
			x0 *= 1.0 / simplex.num_cols();


			//Reflect the worst point about the centroid.
			Vector<N> xr = (1 + alpha) * x0 - alpha * simplex[worst];
			Precision fr = func(xr);

			if(fr < bestval)
			{
				//If the new point is better than the smallest, then try expanding the simplex.
				Vector<N> xe = rho * xr + (1-rho) * x0;
				Precision fe = func(xe);

				//Keep whichever is best
				if(fe < fr)
				{
					simplex[worst] = xe;
					values[worst] = fe;
				}
				else
				{
					simplex[worst] = xr;
					values[worst] = fr;
				}

				return;
			}

			//Otherwise, if the new point lies between the other points
			//then keep it and move on to the next iteration.
			if(fr < second_worst_val)
			{
				simplex[worst] = xr;
				values[worst] = fr;
				return;
			}


			//Otherwise, if the new point is a bit better than the worst point,
			//(ie, it's got just a little bit better) then contract the simplex
			//a bit.
			if(fr < worst_val)
			{
				Vector<N> xc = (1 + gamma) * x0 - gamma * simplex[worst];
				Precision fc = func(xc);

				//If this helped, use it
				if(fc <= fr)
				{
					simplex[worst] = xc;
					values[worst] = fc;
					return;
				}
			}
			
			//Otherwise, fr is worse than the worst point, or the fc was worse
			//than fr. So shrink the whole simplex around the best point.
			for(int i=0; i < simplex.num_rows(); i++)
				if(i != best)
				{
					simplex[i] = simplex[best] + sigma * (simplex[i] - simplex[best]);
					values[i] = func(simplex[i]);
				}
		}

		///Perform one iteration of the downhill Simplex algorithm, and return the result
		///of not DownhillSimplex::finished.
		///@param func Functor to minimize
		template<class Function> bool iterate(const Function& func)
		{
			find_next_point(func);
			return !finished();
		}

		Precision alpha; ///< Reflected size. Defaults to 1.
		Precision rho;   ///< Expansion ratio. Defaults to 2.
		Precision gamma; ///< Contraction ratio. Defaults to .5.
		Precision sigma; ///< Shrink ratio. Defaults to .5.
		Precision epsilon;  ///< Tolerance used to determine if the optimization is complete. Defaults to square root of machine precision.
		Precision zero_epsilon; ///< Additive term in tolerance to prevent excessive iterations if \f$x_\mathrm{optimal} = 0\f$. Known as \c ZEPS in numerical recipies. Defaults to 1e-20

	private:

		//Each row is a simplex vertex
		Simplex simplex;

		//Function values for each vertex
		Values values;


};
}
#endif
