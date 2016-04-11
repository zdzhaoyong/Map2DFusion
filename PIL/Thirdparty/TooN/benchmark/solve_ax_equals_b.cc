#include <TooN/TooN.h>
#include <TooN/LU.h>
#include <TooN/helpers.h>
#include <TooN/gaussian_elimination.h>
#include <TooN/gauss_jordan.h>
#include <tr1/random>
#include <sys/time.h>  //gettimeofday
#include <vector>
#include <utility>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>


using namespace TooN;
using namespace std;
using namespace tr1;

double get_time_of_day()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec+tv.tv_usec * 1e-6;
}

std::tr1::mt19937 eng;
std::tr1::uniform_real<double> rnd;
double global_sum;

#include "solvers.cc"


struct UseCompiledCramer
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{	
		solve_direct(a, b, x);
	}

	static string name()
	{
		return "CC";
	}
};

struct UseLU
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{
		LU<R> lu(a);

		x = lu.backsub(b);
	}

	static string name()
	{
		return "LU";
	}
};

struct UseLUInv
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{
		LU<R> lu(a);

		x = lu.get_inverse() * b;
		//x = lu.backsub(b);
	}

	static string name()
	{
		return "LI";
	}
};


struct UseGaussianElimination
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{
		x = gaussian_elimination(a, b);
	}

	static string name()
	{
		return "GE";
	}
};

struct UseGaussianEliminationInverse
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{
		Matrix<R> i, inv;
		i = Identity;
		inv = gaussian_elimination(a, i);
		x = inv * b;
	}

	static string name()
	{
		return "GI";
	}
};

struct UseGaussJordanInverse
{
	template<int R, int C> static void solve(const Matrix<R, R>& a, const Matrix<R, C>& b, Matrix<R, C>& x)
	{
		Matrix<R, 2*R> m;
		m.template slice<0,0,R,R>() = a;
		m.template slice<0,R,R,R>() = Identity;
		gauss_jordan(m);
		x = m.template slice<0,R,R,R>() * b;
	}

	static string name()
	{
		return "GJ";
	}
};


template<int Size, int Cols, class Solver> void benchmark_ax_eq_b(map<string, vector<double> >& results)
{
	double time=0, t_tmp, start = get_time_of_day(), t_tmp2;
	double sum=0;
	int n=0;

	while(get_time_of_day() - start < .1)
	{
		Matrix<Size> a;
		for(int r=0; r < Size; r++)
			for(int c=0; c < Size; c++)
				a[r][c] = rnd(eng);
			

		Matrix<Size, Cols> b, x;

		for(int r=0; r < Size; r++)
			for(int c=0; c < Cols; c++)
				b[r][c] = rnd(eng);
		
		a[0][0] += (t_tmp=get_time_of_day()) * 1e-20;
		Solver::template solve<Size, Cols>(a, b, x);
		global_sum += (t_tmp2=get_time_of_day())*x[Size-1][Cols-1];
			
		time += t_tmp2 - t_tmp;
		n++;
	}

	results[Solver::name()].push_back(n/time);

	global_sum += sum;	
}




template<int Size, int Cols, typename Solver, bool Use> struct Optional
{
	static void solve(map<string, vector<double> >& r)
	{
		benchmark_ax_eq_b<Size, Cols, Solver>(r);
	}
};


template<int Size, int Cols, typename Solver > struct Optional<Size, Cols, Solver, 0>
{
	static void solve(map<string, vector<double> >&)
	{
	}
};

template<int Size, int C=1, bool End=0> struct ColIter
{
	static void iter()
	{
		static const int Lin = Size*2;
		static const int Grow = 1;
		static const int Cols = C + (C<=Lin?0:(C-Lin)*(C-Lin)*(C-Lin)/Grow);
		map<string, vector<double> > results;
		cout << Size << "\t" << Cols << "\t";
		
		//Run each menchmark 10 times and select the median result
		for(int i=0; i < 10; i++)
		{
			benchmark_ax_eq_b<Size, Cols, UseGaussJordanInverse>(results);
			benchmark_ax_eq_b<Size, Cols, UseGaussianElimination>(results);
			benchmark_ax_eq_b<Size, Cols, UseGaussianEliminationInverse>(results);
			benchmark_ax_eq_b<Size, Cols, UseLUInv>(results);
			benchmark_ax_eq_b<Size, Cols, UseLU>(results);
			Optional<Size, Cols, UseCompiledCramer, (Size<=highest_solver)>::solve(results);
		}
		
		vector<pair<double, string> > res;
		for(map<string, vector<double> >::iterator i=results.begin(); i != results.end(); i++)
		{
			sort(i->second.begin(), i->second.end());
			res.push_back(make_pair(i->second[i->second.size()/2], i->first));
		}



		sort(res.begin(), res.end());
		for(unsigned int i=0; i < res.size(); i++)
			cout << res[i].second << " " << setprecision(5) << setw(10) << res[i].first << "            ";
		cout << endl;
		ColIter<Size, C+1, (Cols> Size*1000)>::iter();
	}
};

template<int Size, int C> struct ColIter<Size, C, 1> 
{

	static void iter()
	{
	}
};

#ifndef SIZE
	#define SIZE 2
#endif

int main()
{
	ColIter<SIZE>::iter();
	
	return global_sum != 123456789.0;
}
