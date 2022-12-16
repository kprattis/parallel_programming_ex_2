function [arrP, arrDir] = arrow_distribute(X, opt , val)
    %X(:,1) = x , X(:, 2) = y
    if nargin < 1 || nargin == 2 || nargin > 3
        disp(['correct use: [arrP, arrDir] = arrow_distribute(X, opt , val), where X: vector of points, opt: \"N\" or \"step\", val: corresponding value depending on opt ']);
        return
    end

    n = length(X(:,1));

    dists = zeros(n, 1);
    for i = 2:n
        dists(i) = dists(i - 1) + norm( X(i, :) - X(i - 1, :) );
    end


    if strcmp(opt, 'N') 
        step = dists(n) / val;
        N = val;
    elseif strcmp(opt, 'step')
        N = floor(dists(n) / val);
        step = val;
    end

    arrP = zeros(N ,2);
    arrDir = zeros(N ,2); 

    intervalID = 2;
    for i = 1:N
        
        len = min(step * i, dists(n));
        while len > dists(intervalID)
            intervalID = intervalID + 1;
        end

        arrDir(i, :) = X(intervalID, :) - X(intervalID - 1, :);
        arrP(i, :) = (dists(intervalID) - len)/(dists(intervalID) - dists(intervalID - 1)) * (X(intervalID, :) - X(intervalID - 1, :)) + X(intervalID - 1, :);

    end


end 
