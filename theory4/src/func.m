function r = func(t)
if (size(t,2)==2)
    r = imult(t,imult(t,t))+iminus(8*t,(4*imult(t,t)+[2 2]));
else
    r = t^3+8*t-4*t^2-2;
end