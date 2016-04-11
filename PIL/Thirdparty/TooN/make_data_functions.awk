BEGIN{
	N=100
	print "//Mechanically generate code. Do not edit."
	print "namespace TooN {"
	print "//modify make_data.awk and run make internal/data_functions.hh"
	for(i=1; i <= N; i++)
	{
		printf "inline Operator<Internal::Data<" i ", double> > Data(double x1"
		
		for(j=2; j<= i; j++)
			printf ", double x"j

		print "){"
		print  "  Operator<Internal::Data<"i", double> > d;"
		for(j=1; j<= i; j++)
			print "  d.vals["j-1"] = x"j";"

		print "  return d;"
		print "}"

		printf "template<typename Precision> inline Operator<Internal::Data<" i ", Precision> > Data(const Precision& x1"
		
		for(j=2; j<= i; j++)
			printf ", const Precision& x"j

		print "){"
		print  "  Operator<Internal::Data<"i", Precision> > d;"
		for(j=1; j<= i; j++)
			print "  d.vals["j-1"] = x"j";"

		print "  return d;"
		print "}"
	}
	print "}"
}
