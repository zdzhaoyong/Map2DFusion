out = fopen('determinant.txt', 'w')
cc  = fopen('determinant.cc', 'w')

fprintf(cc, '#include "regressions/regression.h"\n');
fprintf(cc, 'int main(){\n');

fprintf(out, '# Automatically built from determinant.m\n');
fprintf(out, '# Format is:\n');
fprintf(out, '# <generic fixed>   <GE fixed>   <LU fixed>\n');
fprintf(out, '# <generic dynamic> <GE dynamic> <LU dynamic>\n');
fprintf(out, '\n');
fprintf(out, '#> t 1e-10\n');
fprintf(out, '\n');


for i=2:6

	fprintf(out, '# %ix%i\n', i, i)
	
	for j=1:10

		M = rand(i,i);
		v = M(:);

		fprintf(cc, '	{\n');
		fprintf(cc, '		Matrix<%i> m = Data(%.14e', i, v(1));
		fprintf(cc, ', %.14e', v(2:end));
		fprintf(cc, ');\n');
		fprintf(cc, '		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;\n');
		fprintf(cc, '		Matrix<> n = m;\n');
		fprintf(cc, '		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;\n');
		fprintf(cc, '	}\n');
		fprintf(cc, '\n');

		d = repmat(det(M), 1, 6);

		fprintf(out, '%.14e %.14e %.14e\n', d);
	end

	fprintf(out, '\n');
end

fprintf(cc, '}\n');

fclose(out)
fclose(cc)

