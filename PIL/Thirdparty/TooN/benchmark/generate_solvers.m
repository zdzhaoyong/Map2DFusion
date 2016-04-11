clear all 
out={};
symmetric=1

for S=5:5
	T = num2str(S);

	m=sym(zeros(S,S));
	for r=1:S
		for c=1:S
			if r >= c | symmetric == 0
				v = [ 'A' num2str(r-1) num2str(c-1) ];
			else
				v = [ 'A' num2str(c-1) num2str(r-1) ];
			end
			m(r, c) = sym(v);
		end
	end

	m
	pause

	inverse =simplify(inv(m) * det(m));
	idet = ccode(det(m));


	out{end+1} = [ 'template<int Cols> void solve_direct(const Matrix<' T '>& A, const Matrix<' T ', Cols>& b, Matrix<' T ',Cols>& x)' ];
	out{end+1} = [ '{' ];

	idet = [ idet   '                                      '];
	q=[];
	generated = zeros(1,100);

	for i=1:length(idet)
		if idet(i) >= '0' & idet(i) <= '9'  & (idet(i-1) == 'A' | idet(i-2) == 'A')
			q = [q '[' idet(i) ']' ];
		elseif idet(i) == 'M' & idet(i+13) == '=' | idet(i) == 't'
			genvar_num  = str2num(idet(i+11));

			if generated(genvar_num) == 1
				q = [q idet(i)];
			else
				q = [q 'double ' idet(i)];
				generated(genvar_num)=1;
			end

		elseif idet(i) == ';'
			out{end+1} = [q ';'];
			q=[];
		else
			q = [q idet(i)];
		end

	end

	out{end+1} = '	double idet = 1/t0;';

	for r=1:S
		for c=1:S
			
			p = ccode(inverse(r,c));
			p = [p '                                            '];

			for i=1:length(p)
				if p(i) >= '0' & p(i) <= '9'  & (p(i-1) == 'A' | p(i-2) == 'A')
					q = [q '[' p(i) ']' ];
				elseif p(i) == ';'
					out{end+1} = [q ';'];
					q=[];
				else
					q = [q p(i)];
				end

			end


			out{end+1} = [ '	double i' num2str(r-1) num2str(c-1) ' = t0*idet;'];
		end
	end
	
	out{end+1} = '	for(unsigned int i=0; i < x.num_cols(); i++)';
	out{end+1} = '	{';
	for r=1:S
		row = ['		x[' num2str(r-1) '][i] = '];
		for c=1:S
			row = [row 'i' num2str(r-1) num2str(c-1) '*b[' num2str(c-1) '][i] + ' ];
		end
		out{end+1} = [row(1:end-3) ';'];
	end
	out{end+1} = '	}';
	out{end+1} = '}';
end

out{end+1} = [ 'static const int highest_solver= ' T ';'];

slv = fopen('solvers.cc', 'w');
for i=1:length(out)
	fprintf(slv, '%s\n', out{i});
end
fclose(slv);

