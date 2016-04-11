#include <TooN/TooN.h>

#include <iostream>

using namespace std;
using namespace TooN;

int main(int argc, char ** argv){
    Matrix<2> t;
    t[0] = makeVector(10.0, 17.182818284590450);
    t[1] = makeVector( 0.0, 27.182818284590450);
    
    Matrix<2> s = sqrt(t);
    Matrix<2> l = log(t);
    
    cout << "input\n" << t << endl;
    cout << "square root\n" << s << endl;
    cout << "log\n" << l << endl;
    cout << "difference exp(l) - t\n" << exp(l) - t << endl;
}
