#include <TooN/helpers.h>

#include <iostream>

using namespace std;

int main(int, char **) {
    TooN::Vector<3> t = TooN::makeVector(0,1,2);
    TooN::Vector<> t2 = TooN::makeVector(0,1,2);    
    
    cout << t << "\t" << TooN::project(t) << "\t" << TooN::unproject(t) << endl;
    cout << t2 << "\t" << TooN::project(t2) << "\t" << TooN::unproject(t2) << endl;
    
    return 0;
}
