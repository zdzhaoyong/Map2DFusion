####
#### Compile cg_test.cc to cg_test
####

set multiplot
set nocontour
set pm3d
unset surface
set view 0,0
set isosample 100

splot [-2:2] [-2:2] sin(20*sqrt(x**2 + y**2) + atan(y/x)*2) + 1*(x**2 + y**2)
unset pm3d
set surface
splot [-2:2] [-2:2] '< ./cg_test' using 1:2:3 with linespoints title 'Conjugate Gradient path'
unset multiplot



