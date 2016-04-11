template<int N> struct Enumerate{char i[N];};
Enumerate<0> enumerate(const unsigned char&);
Enumerate<1> enumerate(const char&);
Enumerate<2> enumerate(const int&);
Enumerate<3> enumerate(const unsigned int&);
Enumerate<4> enumerate(const float&);
Enumerate<5> enumerate(const double&);
Enumerate<6> enumerate(const std::complex<float>&);
Enumerate<7> enumerate(const std::complex<double>&);
template<int N> struct DeEnumerate{};
template<> struct DeEnumerate<0>{typedef unsigned char type;};
template<> struct DeEnumerate<1>{typedef char type;};
template<> struct DeEnumerate<2>{typedef int type;};
template<> struct DeEnumerate<3>{typedef unsigned int type;};
template<> struct DeEnumerate<4>{typedef float type;};
template<> struct DeEnumerate<5>{typedef double type;};
template<> struct DeEnumerate<6>{typedef std::complex<float> type;};
template<> struct DeEnumerate<7>{typedef std::complex<double> type;};

#ifdef _FADBAD_H
Enumerate<8> enumerate(const ::fadbad::F<float>&);
Enumerate<9> enumerate(const ::fadbad::F<double>&);
template<> struct DeEnumerate<8>{typedef ::fadbad::F<float> type;};
template<> struct DeEnumerate<9>{typedef ::fadbad::F<double> type;};
#endif
