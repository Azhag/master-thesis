% Generates Kolmogorov Difference of Two Distributions & plot
%
% Computes kolmogorov distance between two distributions and generates
% a plot overlaying the envelopes of the binned distributions.
%
% Usage: [diffarea plotdata] = kolmogorovdistance(x1, x2, title)
%
%   Arguments:
%   
%   x1 -> A vector (row or column) of samples from distribution 1.
%
%   x2 -> A vector (row or column) of samples from distribution 2.
%
%   title -> An optional string to prepend to the plot title.  A string
%            describing the difference area & number of bins will automatically
%            be included in the title whether or not this parameter is included.
% 
%   Return Value:
%
%     A number between 0 and 1 representing the kolmogorov difference
%     of the two distributions.  The closer the value to zero, the
%     more alike the distributions are.  A value of 1 would indicate that the
%     distributions were completely disjoint, while 0 would imply that the two
%     were identical.
% Revision History:
%
%      When     Who   What
%   ----------|-----|-----------------------------------------
%      Sep-03   YC  Initial release
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%            
%        1         2         3         4         5         6         7         8
%2345678901234567890123456789012345678901234567890123456789012345678901234567890

function [diffarea, plotdata] = kolmogorovdistance(x1, x2, titlestr)
  if (size(x1,1) ~= 1 & size(x1,2) ~= 1)
    error 'First argument to comparedists must be a vector!';
  end
  if (size(x2,1) ~= 1 & size(x2,2) ~= 1)
    error 'Second argument to comparedists must be a vector!';
  end
  
  % We'd like to treat this data as a column vector, even if it's not:
  if (size(x1, 1) == 1)
    x1 = x1';
  end
  if (size(x2, 1) == 1)
    x2 = x2';
  end
  
  if (nargin == 2) 
    titlestr = '';
  end
  
  
  % First, we've got to bin the data.  We'll start by determining the min & max
  % values.
  xmin = min(min(x1), min(x2)); 
  xmax = max(max(x1), max(x2)); 

  sortedx1 = sort(x1); 
  sortedx2 = sort(x2); 
  sortedx = sort([x1; x2]); 
  
  % Bin the data.  Each bin's value = percentage of normalized samples of x that
  % fall in bin's interval.
  sizex1 = size(x1, 1); 
  sizex2 = size(x2, 1); 
  position_x1 = 1; 
  position_x2 = 1; 
  bins = zeros(sizex1 + sizex2, 1);
  cdfx1 = zeros(sizex1 + sizex2, 1);
  cdfx2 = zeros(sizex1 + sizex2, 1);
  
  i = 1;  
  while ( i <= sizex1 + sizex2 ) 
    bins(i) = sortedx(i); 
    if (position_x2 > sizex2)
	cdfx2(i) = 1; 
        if (position_x1 > sizex1) 
	   cdfx1(i) = 1; 
	   i = i + 1;  
        else 
	   cdfx1(i) = position_x1/sizex1; 
	   position_x1 = position_x1 + 1;
	   i = i + 1 ;
	end
    elseif ( position_x1 > sizex1) 
	cdfx1(i) = 1; 
        cdfx2(i) = position_x2/sizex2;
	position_x2 = position_x2 + 1;
	i = i + 1; 
    elseif ( sortedx1(position_x1) < sortedx2(position_x2) )
	    cdfx1(i) = position_x1/sizex1; 
	    cdfx2(i) = (position_x2 - 1)/sizex2; 
	    position_x1 = position_x1 + 1;
	    i = i + 1; 
    elseif ( sortedx1(position_x1) == sortedx2(position_x2) )
	    cdfx1(i) = position_x1/sizex1;
	    cdfx1(i+1) = position_x1/sizex1;
	    cdfx2(i) = position_x2/sizex2;
	    cdfx2(i + 1) = position_x2/sizex2;
	    bins(i+1) = bins(i); 

	    position_x2 = position_x2 + 1;
	    position_x1 = position_x1 + 1;
	    i = i + 2; 
    else 
	    cdfx1(i) = ( position_x1 - 1)/sizex1;
	    cdfx2(i) = position_x2/sizex2;
	    position_x2 = position_x2 + 1;
	    i = i + 1;
    end
  end
  diffarea = max(abs(cdfx1 - cdfx2));

  plotdata = zeros(sizex1 + sizex2, 3);
  plotdata(:,1) = bins; 
  plotdata(:,2) = cdfx1; 
  plotdata(:,3) = cdfx2; 
  
  figure;
  plot(bins, cdfx1, 'b-', bins, cdfx2, 'r-');
  
  % Label everything appropriately
  xlabel('X Value');
  ylabel('Culmulative Distribution Function');
  
  if (isempty(titlestr)) 
    titlestr = sprintf('Kolmogrov Difference = %f', diffarea);
  else
    titlestr = sprintf('%s\nDifference Area = %f', titlestr, diffarea);
  end
  title(titlestr);
  return;
