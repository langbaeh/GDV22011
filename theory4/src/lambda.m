function r = lambda(v, i,  z)
i = i +1;
vt = v;
vt(:,i) = z;

D1 = [1 1 1; vt];
D0 = [1 1 1; v];

r = det(D1)/det(D0);



