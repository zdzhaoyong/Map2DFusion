rand('state', 1)
o = fopen('tmpfile', 'w')
for r=[2 3 4 5 6]
	
	for n =1:10
		m = rand(r,r);
		m = m + m';

		ml = m';
		ml = ml(:);

		%fprintf('test<%i>(Data(%.15e', r, ml(1));
		%fprintf(',%.15e', ml(2:end));
		%fprintf('));\n');

		[vec, tmp] = eig(m);
		vals = eig(m);

		for i=1:r
			fprintf(o, '%.15e ', vec(:,i))
			fprintf(o, '\n');
		end
		fprintf(o, '%.15e ', vals)
		fprintf(o, '\n\n\n\n\n');

		for i=1:r
			fprintf(o, '%.15e ', vec(:,i))
			fprintf(o, '\n');
		end
		fprintf(o, '%.15e ', vals)
		fprintf(o, '\n\n\n\n\n');


	end
end
