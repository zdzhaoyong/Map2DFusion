BEGIN{
	types[n++] = "unsigned char"
	types[n++] = "char"
	types[n++] = "int"
	types[n++] = "unsigned int"
	types[n++] = "float"
	types[n++] = "double"
	types[n++] = "std::complex<float>"
	types[n++] = "std::complex<double>"

	print "template<int N> struct Enumerate{char i[N];};"

	for(i=0; i < n; i++)
		print "Enumerate<"i"> enumerate(const "types[i]"&);"

	
	print "template<int N> struct DeEnumerate{};"
	for(i=0; i < n; i++)
		print "template<> struct DeEnumerate<"i">{typedef "types[i]" type;};"

	
		

}
