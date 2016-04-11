f=tmp




plot -xr 1e-18:1 -logx -logy -xl 'Delta' -yl 'Error' $f '(1:2)' --,'Lapack LU' $f '(1:3)' --,'Gauss-Jordan' $f '(1:4)' --,'SVD' $f '(1:5)' --,'Gaussian Elimination' $f '(1:6)' --,'Cramer'
