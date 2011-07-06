i = [0 4]

while(size(i,2) >0)
  r = func(i(:,1))
  if (0 < r(1) or 0 > r(2))
    print 'dropping '
    i(:,1)
    i = i(:,2:end)
  else
    rt = funct(i(:,1))
    if(0< rt(1) or 0 > r(2))
      %refine
      sub = [i(1) sum(i(:,1))/2.0] [sum(i(:,1))/2, i(2)]
      while(size(sub,2)>0)
	if(func(sub(:,1))
	
      end
    else
      i = [i [i(1) sum(i(:,1))/2.0] [sum(i(:,1))/2, i(2)]]
      i = i(:,2:end)
    end
  end
  
  
end

