#include <iostream>
#include <sstream>

using namespace std;

#include <TooN/so2.h>
#include <TooN/se2.h>
#include <TooN/so3.h>
#include <TooN/se3.h>
#include <TooN/sim2.h>
#include <TooN/sim3.h>

void test_so2(){
    cout << "---------------SO2 Tests---------------\n";
    
    TooN::SO2<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SO2<int> r2;
    cout << r2 << endl;
    TooN::SO2<> r(0.1);
    cout << "constructor with 0.1\n";
    cout << r << endl;
    cout << "generator\n";
    cout << r.generator() << endl;
    cout << "ln()\n";
    cout << r.ln() << endl;
    cout << "inverse\n";
    cout << r.inverse() << endl;
    cout << "times\n";
    cout << r * r.inverse() << endl;
    cout << (r *= r.inverse()) << endl;
    r = TooN::SO2<>::exp(0.1);

    TooN::Vector<2> t = TooN::makeVector(0,1);
    cout << "right and left multiply with vector " << t << "\n";
    cout << r * t << endl;
    cout << t * r << endl;
    TooN::Matrix<2> l = TooN::Identity;
    cout << "right and left multiply with matrix\n" << l << "\n";
    cout << r * l << endl;
    cout << l * r << endl;
    TooN::Matrix<2,3> l2(TooN::Zeros);
    l2[0] = TooN::makeVector(0,1,2);
    cout << "right with rectangular matrix\n";
    cout << r * l2 << endl;
    cout << l2.T() * r << endl;

    TooN::Matrix<2> m;
    m[0] = TooN::makeVector(0.5, 1);
    m[1] = TooN::makeVector(1,1);
    cout << "set from matrix (uses coerce) " << m << "\n";
    r = m;
    cout << r << endl;

    cout << "read from istream\n";
    istringstream is("0 -1 1 0");
    is >> r;
    cout << r << endl;
}

void test_se2(){
    cout << "---------------SE2 Tests---------------\n";

    TooN::SE2<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SE2<int> r2;
    cout << r2 << endl;
    
    cout << "from vector 1 1 0\n";
    cout << TooN::SE2<>::exp(TooN::makeVector(1,1,0)) << endl;
    
    TooN::SE2<> r3(TooN::makeVector(1,1,1));
    cout << "from vector 1 1 1\n";
    cout << r3 << endl;
    cout << r3.ln() << endl;
    
    cout << "generators 0,1,2\n";
    cout << TooN::SE2<>::generator(0) ;
    cout << TooN::SE2<>::generator(1) ;
    cout << TooN::SE2<>::generator(2) << endl;

    TooN::Vector<2> t1 = TooN::makeVector(0,1);
    TooN::Vector<> t2(3); t2 = TooN::makeVector(1,0,1);
    cout << "se2 * vector\n";
    cout << r3 * t1 << endl;
    cout << r3 * t2 << endl;
    cout << "vector * se3\n";    
    // cout << t1 * r3 << endl; // this is not well defined, should the output be a 3 vector ?
    cout << t2 * r3 << endl;    

    TooN::Matrix<3> m1;
    TooN::Matrix<> m2(3,3);
    TooN::Matrix<3,10> m3;
    cout << "se2 * matrix\n";
    cout << r3 * m1 << endl;
    cout << r3 * m2 << endl;
    cout << r3 * m3 << endl;
    cout << "matrix * se2\n";
    cout << m1 * r3 << endl;
    cout << m2 * r3 << endl;
    cout << m3.T() * r3 << endl;

    TooN::SO2<> r(-1);
    cout << "so2 * se2\n";
    cout << r * r3 << endl;
    
    cout << "read from istream\n";
    istringstream is("0 -1 2 1 0 3");
    is >> r3;
    cout << r3 << endl;
}

void test_so3(){
    cout << "---------------SO3 Tests---------------\n";
    
    TooN::SO3<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SO3<int> r2;
    cout << r2 << endl;
    TooN::SO3<> r(TooN::makeVector(0.1, 0.1, 0.1));
    cout << "constructor with 0.1\n";
    cout << r << endl;
    cout << "generator 0,1,2\n";
    cout << TooN::SO3<>::generator(0) ;
    cout << TooN::SO3<>::generator(1) ;
    cout << TooN::SO3<>::generator(2) << endl;
    cout << "ln()\n";
    cout << r.ln() << endl;
    cout << "inverse\n";
    cout << r.inverse() << endl;
    cout << "times\n";
    cout << r * r.inverse() << endl;
    cout << (r *= r.inverse()) << endl;
    r = TooN::SO3<>::exp(TooN::makeVector(0.1, 0.1, 0.1));

    TooN::Vector<3> t = TooN::makeVector(0,1,2);
    cout << "right and left multiply with vector " << t << "\n";
    cout << r * t << endl;
    cout << t * r << endl;
    TooN::Matrix<3> l = TooN::Identity;
    cout << "right and left multiply with matrix\n" << l << "\n";
    cout << r * l << endl;
    cout << l * r << endl;
    TooN::Matrix<3,6> l2(TooN::Zeros);
    l2[0] = TooN::makeVector(0,1,2,3,4,5);
    cout << "right with rectangular matrix\n";
    cout << r * l2 << endl;
    cout << l2.T() * r << endl;

    TooN::Matrix<3> m;
    m[0] = TooN::makeVector(0.5, 1,2);
    m[1] = TooN::makeVector(1,1,0);
    m[2] = TooN::makeVector(0,1,0);
    cout << "set from matrix (uses coerce)\n" << m << "\n";
    r = m;
    cout << r << endl;
    TooN::SO3<> r5(m);
    cout << r5 << endl;

    cout << "read from istream\n";
    istringstream is("0 -1 0 1 0 0 0 0 1");
    is >> r;
    cout << r << endl;

    cout << "test rotation constructor\n";
    TooN::Vector<3> a = TooN::makeVector(1,0,0), b = TooN::makeVector(0,1,1);
    TooN::SO3<> rr(a,b);
    cout << "a " << a << " to b " << b << " is\n" << rr << endl;
    cout << "R * a " << rr * a << endl;
    cout << "a " << a << " to itself is\n" << TooN::SO3<>(a,a) << endl;
}

void test_se3(){
    cout << "---------------SE3 Tests---------------\n";
    
    TooN::SE3<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SE3<int> r2;
    cout << r2 << endl;
    TooN::SE3<> r(TooN::makeVector(0.1, 0.1, 0.1, 0.2, -0.2, 0.2));
    cout << "constructor\n";
    cout << r << endl;
    cout << "generator 0,1,2,3,4,5\n";
    cout << TooN::SE3<>::generator(0) ;
    cout << TooN::SE3<>::generator(1) ;
    cout << TooN::SE3<>::generator(2) << endl;
    cout << TooN::SE3<>::generator(3) ;
    cout << TooN::SE3<>::generator(4) ;
    cout << TooN::SE3<>::generator(5) << endl;
    cout << "ln()\n";
    cout << r.ln() << endl;
    cout << "inverse\n";
    cout << r.inverse() << endl;
    cout << "times\n";
    cout << r * r.inverse() << endl;
    cout << (r *= r.inverse()) << endl;
    r = TooN::SE3<>::exp(TooN::makeVector(0.1, 0.1, 0.1, 0.2, -0.2, 0.2));

    TooN::Vector<4> t = TooN::makeVector(0,1,2,3);
    cout << "right and left multiply with vector " << t << "\n";
    cout << r * t << endl;
    cout << t * r << endl;
    TooN::Vector<3> t3 = TooN::makeVector(0,1,2);
    cout << "right with a 3 vector " << t3 << "\n";
    cout << r * t3 << endl;

    TooN::Matrix<4> l = TooN::Identity;
    cout << "right and left multiply with matrix\n" << l << "\n";
    cout << r * l << endl;
    cout << l * r << endl;
    TooN::Matrix<4,6> l2(TooN::Zeros);
    l2[0] = TooN::makeVector(0,1,2,3,4,5);
    cout << "right with rectangular matrix\n";
    cout << r * l2 << endl;
    cout << l2.T() * r << endl;
    
    TooN::SO3<> rot(TooN::makeVector(-0.2, 0.2, -0.2));
    cout << "mult with SO3\n";
    cout << rot * r << endl;
    
    TooN::Vector<6> a = TooN::makeVector(0,1,2,0.1, 0.2, 0.3);
    cout << "adjoint with a " << a << "\n";
    cout << r.adjoint(a) << endl;
    cout << "0 0 0 0 0 0 = " << r.adjoint(a) - (r * TooN::SE3<>(a) * r.inverse()).ln() << endl;
    cout << "trinvadjoint with a " << a << "\n";
    cout << r.trinvadjoint(a) << endl;
    cout << "0 = " << r.trinvadjoint(a) * r.adjoint(a) - a * a << endl; 

    TooN::Matrix<6> ma(TooN::Identity);
    ma[0] = TooN::makeVector(0.1, 0.2, 0.1, 0.2, 0.1, 0.3);
    ma = ma.T() * ma;
    cout << "adjoint with ma\n" << ma << "\n";
    cout << r.adjoint(ma) << endl;
    cout << "trinvadjoint with ma\n";
    cout << r.trinvadjoint(ma) << endl;

    cout << "read from istream\n";
    istringstream is("0 -1 0 1 1 0 0 2 0 0 1 3");
    is >> r;
    cout << r << endl;
}

void test_sim3(){
    cout << "---------------SIM3 Tests---------------\n";
    
    TooN::SIM3<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SIM3<int> r2;
    cout << r2 << endl;
    TooN::SIM3<> r(TooN::makeVector(0.1, 0.1, 0.1, 0.2, -0.2, 0.2, 0.5));
    cout << "constructor\n";
    cout << r << endl;
    cout << "different cases\n";
    cout << TooN::SIM3<>(TooN::makeVector(0.0, 0, 0, 0, 0, 0, 0)) << endl;
    cout << TooN::SIM3<>(TooN::makeVector(0.0001, 0, 0, 0, 0, 0, 0)) << endl;
    cout << TooN::SIM3<>(TooN::makeVector(0.0, 0, 0, 0.01, 0, 0, 0)) << endl;
    cout << TooN::SIM3<>(TooN::makeVector(0.0, 0, 0, 0.0, 0, 0, 0.0001)) << endl;
    cout << "generator 0,1,2,3,4,5,6\n";
    cout << TooN::SIM3<>::generator(0) ;
    cout << TooN::SIM3<>::generator(1) ;
    cout << TooN::SIM3<>::generator(2) << endl;
    cout << TooN::SIM3<>::generator(3) ;
    cout << TooN::SIM3<>::generator(4) ;
    cout << TooN::SIM3<>::generator(5) << endl;
    cout << TooN::SIM3<>::generator(6) << endl;
    cout << "ln()\n";
    cout << r.ln() << endl;
    cout << "inverse\n";
    cout << r.inverse() << endl;
    cout << "inverse.ln()\n";
    cout << r.inverse().ln() << endl;
    cout << "times\n";
    cout << r * r.inverse() << endl;
    cout << (r *= r.inverse()) << endl;
    r = TooN::SIM3<>::exp(TooN::makeVector(0.1, 0.1, 0.1, 0.2, -0.2, 0.2, 0.5));

    TooN::Vector<4> t = TooN::makeVector(0,1,2,3);
    cout << "right and left multiply with vector " << t << "\n";
    cout << r * t << endl;
    cout << t * r << endl;
    TooN::Vector<3> t3 = TooN::makeVector(0,1,2);
    cout << "right with a 3 vector " << t3 << "\n";
    cout << r * t3 << endl;

    TooN::Matrix<4> l = TooN::Identity;
    cout << "right and left multiply with matrix\n" << l << "\n";
    cout << r * l << endl;
    cout << l * r << endl;
    TooN::Matrix<4,6> l2(TooN::Zeros);
    l2[0] = TooN::makeVector(0,1,2,3,4,5);
    cout << "right with rectangular matrix\n";
    cout << r * l2 << endl;
    cout << l2.T() * r << endl;

#if 0    
    TooN::SO3<> rot(TooN::makeVector(-0.2, 0.2, -0.2));
    cout << "mult with SO3\n";
    cout << rot * r << endl;
    
    TooN::Vector<6> a = TooN::makeVector(0,1,2,0.1, 0.2, 0.3);
    cout << "adjoint with a " << a << "\n";
    cout << r.adjoint(a) << endl;
    cout << "0 0 0 0 0 0 = " << r.adjoint(a) - (r * TooN::SE3<>(a) * r.inverse()).ln() << endl;
    cout << "trinvadjoint with a " << a << "\n";
    cout << r.trinvadjoint(a) << endl;
    cout << "0 = " << r.trinvadjoint(a) * r.adjoint(a) - a * a << endl; 

    TooN::Matrix<6> ma(TooN::Identity);
    ma[0] = TooN::makeVector(0.1, 0.2, 0.1, 0.2, 0.1, 0.3);
    ma = ma.T() * ma;
    cout << "adjoint with ma\n" << ma << "\n";
    cout << r.adjoint(ma) << endl;
    cout << "trinvadjoint with ma\n";
    cout << r.trinvadjoint(ma) << endl;
#endif

    cout << "read from istream\n";
    istringstream is("0 -1 0 1 1 0 0 2 0 0 1 3");
    is >> r;
    cout << r << endl;
    cout << r.get_rotation() << "\n" << r.get_translation() << "\n" << r.get_scale() << endl;
}

void test_sim2(){
    cout << "---------------SIM2 Tests---------------\n";
    
    TooN::SIM2<> r1;
    cout << "default constructor\n";
    cout << r1 << endl;
    cout << "default constructor <int>\n";
    TooN::SIM2<int> r2;
    cout << r2 << endl;
    TooN::SIM2<> r(TooN::makeVector(0.1, 0.1, 0.1, 0.2));
    cout << "constructor\n";
    cout << r << endl;
    cout << "different cases\n";
    cout << TooN::SIM2<>(TooN::makeVector(0.0, 0, 0, 0)) << endl;
    cout << TooN::SIM2<>(TooN::makeVector(0.0001, 0, 0, 0)) << endl;
    cout << TooN::SIM2<>(TooN::makeVector(0.0, 0, 0.0001, 0)) << endl;
    cout << TooN::SIM2<>(TooN::makeVector(0.0, 0, 0, 0.0001)) << endl;
    cout << "generator 0,1,2,3\n";
    cout << TooN::SIM2<>::generator(0);
    cout << TooN::SIM2<>::generator(1);
    cout << TooN::SIM2<>::generator(2) << endl;
    cout << TooN::SIM2<>::generator(3) << endl;
    cout << "ln()\n";
    cout << r.ln() << endl;
    cout << "inverse\n";
    cout << r.inverse() << endl;
    cout << "inverse.ln()\n";
    cout << r.inverse().ln() << endl;
    cout << "times\n";
    cout << r * r.inverse() << endl;
    cout << (r *= r.inverse()) << endl;
    r = TooN::SIM2<>::exp(TooN::makeVector(0.1, 0.1, 0.1, 0.2));

    TooN::Vector<3> t = TooN::makeVector(0,1,2);
    cout << "right and left multiply with vector " << t << "\n";
    cout << r * t << endl;
    cout << t * r << endl;
    TooN::Vector<2> t3 = TooN::makeVector(0,1);
    cout << "right with a 2 vector " << t3 << "\n";
    cout << r * t3 << endl;

    TooN::Matrix<3> l = TooN::Identity;
    cout << "right and left multiply with matrix\n" << l << "\n";
    cout << r * l << endl;
    cout << l * r << endl;
    TooN::Matrix<3,6> l2(TooN::Zeros);
    l2[0] = TooN::makeVector(0,1,2,3,4,5);
    cout << "right with rectangular matrix\n" << l2 << "\n";
    cout << r * l2 << endl;
    cout << l2.T() * r << endl;

#if 0    
    TooN::SO3<> rot(TooN::makeVector(-0.2, 0.2, -0.2));
    cout << "mult with SO3\n";
    cout << rot * r << endl;
    
    TooN::Vector<6> a = TooN::makeVector(0,1,2,0.1, 0.2, 0.3);
    cout << "adjoint with a " << a << "\n";
    cout << r.adjoint(a) << endl;
    cout << "0 0 0 0 0 0 = " << r.adjoint(a) - (r * TooN::SE3<>(a) * r.inverse()).ln() << endl;
    cout << "trinvadjoint with a " << a << "\n";
    cout << r.trinvadjoint(a) << endl;
    cout << "0 = " << r.trinvadjoint(a) * r.adjoint(a) - a * a << endl; 

    TooN::Matrix<6> ma(TooN::Identity);
    ma[0] = TooN::makeVector(0.1, 0.2, 0.1, 0.2, 0.1, 0.3);
    ma = ma.T() * ma;
    cout << "adjoint with ma\n" << ma << "\n";
    cout << r.adjoint(ma) << endl;
    cout << "trinvadjoint with ma\n";
    cout << r.trinvadjoint(ma) << endl;
#endif

    cout << "read from istream\n";
    istringstream is("0 -1 0 1 1 0");
    is >> r;
    cout << r << endl;
    cout << r.get_rotation() << "\n" << r.get_translation() << "\n" << r.get_scale() << endl;
}

void test_se2_exp(){
    cout << "------------------SE2 check------------------\n";

    TooN::SE2<> s2(TooN::makeVector(1,0,1));
    TooN::SE3<> s3(TooN::makeVector(0,1,0,1,0,0));
    cout << s2 << endl;
    cout << s3 << endl;
    
    cout << s2.ln() << "\tvs\t" << s3.ln() << endl;
    
}

int main(int, char* *){
 
 #if 0
    test_so2();
    test_so3();
    test_se2();
    test_se3();
    test_sim2();
    test_sim3();
 
    test_se2_exp();
#endif

    test_sim3();
    test_sim2();
    
    return 0;
}
