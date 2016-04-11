#include <iostream>
#include <TooN/TooN.h>
#include <TooN/SymEigen.h>

using namespace std;
using namespace TooN;

int main() {
  // construct M
  Matrix<3> M = Data(4,0,2,
                     0,5,3,
                     2,3,6);
 
  Vector<3> dg(makeVector(2,4,9));
  // create the eigen decomposition of M
  SymEigen<3> eigM(M);
  cout << M << endl;

  cout << endl;

  // Print the eigenvalues and eigenvectors
  cout << eigM.get_evalues() << endl;
  cout << eigM.get_evectors() << endl << endl;

  // print the square root of the matrix.
  cout << eigM.get_sqrtm() << endl << endl;

  // print the square root of the matrix squared.
  cout << eigM.get_sqrtm().T() * eigM.get_sqrtm() << endl << endl;

  // print the inverse of the matrix.
  cout << eigM.get_pinv() << endl;

  // print the inverse square root of the matrix.
  cout << eigM.get_isqrtm() << endl;

  // print the inverse square root of the matrix squared.
  cout << eigM.get_isqrtm().T() * eigM.get_isqrtm() << endl;
  
  return 0;
}
