#include <TooN/Cholesky.h>

#include <iostream>
#include <iomanip>

using namespace std;
using namespace TooN;

int main(int, char ** ){

	cout << setprecision(10);
    
    Matrix<3> t = Data(
         1,   0.5, 0.5,
         0.5,   2, 0.7,
         0.5, 0.7,   3);
    
    Cholesky<3> chol(t);

    cout << "Check for determinant\n";
    cout << chol.determinant() << endl << endl;

    cout << "Static size checks:\n";
    cout << "Check decomposition, all three matrices should be the same.\n";
    cout << t << endl << chol.get_L()*(chol.get_L().T()) << endl 
      	 << chol.get_unscaled_L()*chol.get_D()*(chol.get_unscaled_L().T()) 
	 << endl;

    cout << "Check inverse, third matrix should be close to identity.\n";
    cout << t << "\n" <<  chol.get_inverse() << "\n" 
	 << t * chol.get_inverse() << endl;

    Matrix<> t2 = t;
    
    Cholesky<Dynamic,float> chol2(t2);
    
    cout << "Dynamic size, single precision checks:\n";
    cout << "Check decomposition, all three matrices should be the same.\n";
    cout << t << endl << chol2.get_L()*(chol2.get_L().T()) << endl 
      	 << chol2.get_unscaled_L()*chol2.get_D()*(chol2.get_unscaled_L().T()) 
	 << endl;

    cout << "Check inverse, third matrix should be close to identity.\n";
    cout << t2 << "\n" <<  chol2.get_inverse() << "\n" 
	 << t2 * chol2.get_inverse() << endl;

    Vector<3> bla = makeVector(1,2,3);

    cout << "Check backsub(), the following two vectors should be the same.\n";
    cout << chol.backsub(bla) << endl;
    cout << chol.get_inverse() * bla << endl << endl;

    cout << "Check mahalanobis(), result should be zero.\n";
    cout << chol.mahalanobis(bla) - bla * chol.backsub(bla) << endl;
    
    return 0;
}
