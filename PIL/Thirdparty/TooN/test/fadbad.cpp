#include <TooN/functions/fadbad.h>

#include <TooN/se2.h>

using namespace std;
using namespace TooN;
using namespace fadbad;

void test() {
    SE3<F<double, 6> > se3id = make_fad_se3<double, 6>();
    cout << "SE3 derivatives\n";
    for(int i = 0; i < 6; ++i)
        cout << get_derivative(se3id.get_rotation().get_matrix(), i) << get_derivative(se3id.get_translation(), i) <<  "\n\n";

    SO3<F<double, 6> > so3id = make_fad_so3<double, 6>();
    cout << "SO3 derivatives\n";
    for(int i = 0; i < 3; ++i)
        cout << get_derivative(so3id.get_matrix(), i) << "\n";

    SE2<F<double> > se2id = make_fad_se2<double>();
    cout << "SE2 derivatives\n";
    for(int i = 0; i < 3; ++i)
        cout << get_derivative(se2id.get_rotation().get_matrix(), i) << get_derivative(se2id.get_translation(), i) <<  "\n\n";
    
    SO2<F<double> > so2id = make_fad_so2<double>();
    cout << "SO2 derivatives\n";
    cout << get_derivative(so2id.get_matrix(), 0) << "\n";
    // cout << get_derivative((so2id * SO2<>(0.1)).get_matrix(), 0) << "\n";
}

int main(int argc, char ** argv){
    test();

#if 0
    SE3<> id(makeVector(1,0,0,0,0,0));
    
    const SE3<F<double> > g = make_left_fad_se3(id);
    for(int i = 0; i < 6; ++i)
        cout << get_derivative(g.get_rotation().get_matrix(), i) << get_derivative(g.get_translation(), i) <<  "\n\n";

    Vector<3> in = makeVector(1,2,3);
    const Vector<3, F<double> > p = g * in;
    cout << p << "\n" << get_jacobian<3,6>(p) << endl;
#endif
}
