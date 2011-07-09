i = [0 4]

    found = 0;
while(size(i,1) >0 && found == 0)
r = func(i(1,:));
  if (0 < r(1) || 0 > r(2))
      %disp 'dropping ';
  i(1,:);
    i = i(2:end,:);
  else
      rt = funct(i(1,:));
    if(0< rt(1,1) || 0 > rt(1,2))
      %refine
      disp 'refining' 
      i(1,:)
      sub = [[i(1,1)  sum(i(1,:))/2.0]; [sum(i(1,:))/2 i(1,2)]]
      while(size(sub,1)> 0 && found == 0)
        cur = sub(1,:);
        cur
        %if(abs(func(cur(1))) < 0.000001)
        %if(abs(cur(1)-cur(2)) < 0.1)
        if(floor(10*cur(1)) == floor(10*cur(2)))
          disp 'found'
            cur
            func(cur(1))
            found = 1;
        elseif(func(cur(1))*func(cur(2)) <= 0)
            sub = [sub; [cur(1) sum(cur(1,:))/2.0]; [sum(cur(1,:))/2 cur(2)]];
            [sub(:,1), I] = sort(sub(:,1));
            sub(:,2) = sub(I,2);
            
        else
            disp 'dropping in refinement';
            cur
        end
        
        sub = sub(2:end,:);
      end
    else
        i = [i; [i(1,1) sum(i(1,:))/2.0];[sum(i(1,:))/2 i(1,2)]];
        % sort i to search lowest intervalls first
        [i(:,1), I] = sort(i(:,1));
        i(:,2) = i(I,2);
    end
        i = i(2:end,:);
        i
  end
  
  
end

