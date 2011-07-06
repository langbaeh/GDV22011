v = [1 0 0; 0 1 0];


M = [ 1    3/2     3/2    3/2    3    3/2    1    3/2    3/2    1 ]
M = [M ;[lambda(v,0,xi(v, 2,1,0 ))^3    3/2*lambda(v,0,xi(v, 2,1,0 ))^2*lambda(v,1,xi(v, ...
						  2,1,0 ))   0  3/2*lambda(v,0,xi(v, 2,1,0 ))*lambda(v,1,xi(v, 2,1,0 ))^2   0   0   lambda(v,1,xi(v, 2,1,0 ))^3  0  0   0  ]]
M = [M; [lambda(v,0,xi(v, 2,0,1 ))^3   0    3/2*lambda(v,0,xi(v, 2,0,1 ))^2*lambda(v,2,xi(v, 2,0,1 ))   0 0    3/2*lambda(v,0,xi(v, 2,0,1 ))*lambda(v,2,xi(v, 2,0,1 ))^2  0   0  0   lambda(v,2,xi(v, 2,0,1 ))^3 ]]
M = [M; [lambda(v,0,xi(v, 1,2,0 ))^3    3/2*lambda(v,0,xi(v, 1,2,0 ))^2*lambda(v,1,xi(v, ...
						  1,2,0 ))   0 3/2*lambda(v,0,xi(v, 1,2,0 ...
						  ))*lambda(v,1,xi(v, 1,2,0 ))^2   0   0   lambda(v,1,xi(v, 1,2,0 ))^3   0  0   0  ;]]

M = [M; [lambda(v,0,xi(v, 1,1,1 ))^3  3/2*lambda(v,0,xi(v, 1,1,1 ))^2*lambda(v,1,xi(v, 1,1,1 )) 3/2*lambda(v,0,xi(v, 1,1,1 ))^2*lambda(v,2,xi(v, 1,1,1 ))    3/2*lambda(v,0,xi(v, 1,1,1 ))*lambda(v,1,xi(v, 1,1,1 ))^2 3*lambda(v,0,xi(v, 1,1,1))*lambda(v,1,xi(v, 1,1,1 ))*lambda(v,2,xi(v, 1,1,1 ))    3/2*lambda(v,0,xi(v, 1,1,1 ))*lambda(v,2,xi(v, 1,1,1 ))^2  lambda(v,1,xi(v, 1,1,1 ))^3    3/2*lambda(v,1,xi(v, 1,1,1 ))^2*lambda(v,2,xi(v, 1,1,1 ))   3/2*lambda(v,1,xi(v, 1,1,1 ))*lambda(v,2,xi(v, 1,1,1 ))^2   lambda(v,2,xi(v, 1,1,1 ))^3 ]]
M = [M; [lambda(v,0,xi(v, 1,0,2 ))^3   0    3/2*lambda(v,0,xi(v, 1,0,2 ))^2*lambda(v,2,xi(v, 1,0,2 ))   0  0    3/2*  lambda(v,0,xi(v, 1,0,2 ))*lambda(v,2,xi(v, 1,0,2 ))^2  0   0  0   lambda(v,2,xi(v, 1,0,2 ))^3  ]]
M = [M;   [1    3/2     3/2    3/2    3    3/2    1    3/2    3/2    1  ]]
M = [M; [0   0   0   0   0   0   lambda(v,1,xi(v, 0,2,1 ))^3    3/2*lambda(v,1,xi(v, ...
						  0,2,1 ))^2 *lambda(v,2,xi(v, 0,2,1 ))   3/2*lambda(v,1,xi(v, 0,2,1 ))*lambda(v,2,xi(v, 0,2,1 ))^2   lambda(v,2,xi(v, 0,2,1 ))^3  ;]]
M = [M; [0   0   0   0   0   0   lambda(v,1,xi(v, 0,1,2 ))^3 3/2*lambda(v,1,xi(v,0,1,2 ))^2*lambda(v,2,xi(v, 0,1,2 ))   3/2*lambda(v,1,xi(v, 0,1,2 ))*lambda(v,2,xi(v, 0,1,2 ))^2   lambda(v,2,xi(v, 0,1,2 ))^3 ]]
M = [M; [  1    3/2     3/2    3/2   3    3/2    1    3/2    3/2    1  ]]


F = [f(xi(v, 2,1,0));f(xi(v, 2,1,0));f(xi(v,2,0,1));f(xi(v,1,2,0));f(xi(v,1,1,1));f(xi(v,1,0,2)); ...
     f(xi(v,0,3,0));f(xi(v,0,2,1));f(xi(v,0,1,2));f(xi(v,0,0,3)) ];

pinv(M)*F