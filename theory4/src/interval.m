i = [0 4]

while(size(i,1) >0)
r = func(i(1,:));
  if (0 < r(1) || 0 > r(2))
      %disp 'dropping ';
  i(1,:);
    i = i(2:end,:);
  else
      rt = funct(i(1,:));
    if(0< rt(1,1) || 0 > rt(1,2))
      %refine
      sub = [[i(1,1)  sum(i(1,:))/2.0]; [sum(i(1,:))/2 i(1,2)]];
      while(size(sub,1)> 0)
        cur = sub(1,:);
        if(abs(func(cur(1))) < 0.000001)
            disp 'found'
            cur(1)
            func(cur(1))
        elseif(func(cur(1))*func(cur(2)) <= 0)
        sub = [sub; [cur(1) sum(cur(1,:))/2.0]; [sum(cur(1,:))/2 cur(2)]];
        else
            %disp 'dropping in refinement';
        cur;
        end
        
        sub = sub(2:end,:);
      end
    else
    i = [i; [i(1,1) sum(i(1,:))/2.0];[sum(i(1,:))/2 i(1,2)]];
    end
        i = i(2:end,:);
    
  end
  
  
end

