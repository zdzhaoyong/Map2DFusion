
#include <iostream>

using namespace std;

#include <TooN/SymEigen.h>

int main(int, char **){

    TooN::Matrix<> mt(3,3);
    cout << mt * TooN::makeVector(0,1,2) << endl;

    TooN::Matrix<2> tt;
    tt[0] = TooN::makeVector(1,2);
    tt[1] = TooN::makeVector(2,1);
    
    TooN::SymEigen<2> symt(tt);
    cout << symt.get_evectors() << symt.get_evalues() << endl;

    TooN::Matrix<3> t;

    t[0] = TooN::makeVector(1,0.5, 0.5);
    t[1] = TooN::makeVector(0.5, 2, 0.7);
    t[2] = TooN::makeVector(0.5,0.7, 3);
    
    TooN::SymEigen<3> sym(t);
    cout << sym.backsub(TooN::makeVector(0,1,2)) << endl;
    
    TooN::Matrix<> t2(3,3);

    t2[0] = TooN::makeVector(1,0.5, 0.5);
    t2[1] = TooN::makeVector(0.5, 2, 0.7);
    t2[2] = TooN::makeVector(0.5,0.7, 3);
    
    TooN::SymEigen<> sym2(t2);
    cout << sym2.backsub(TooN::makeVector(0,1,2)) << endl;

    
    TooN::Matrix<-1, -1, float> t2f(3,3);

    t2f[0] = TooN::makeVector(1,0.5, 0.5);
    t2f[1] = TooN::makeVector(0.5, 2, 0.7);
    t2f[2] = TooN::makeVector(0.5,0.7, 3);
    
    TooN::SymEigen<TooN::Dynamic,float> symf(t2f);
    cout << symf.backsub(TooN::makeVector(0,1,2)) << endl;
}
