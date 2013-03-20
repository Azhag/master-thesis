function [diffarea, plotdata] = histodistance_int(x1, x2, titlestr)
% Generates Normalized Difference Area of Two Distributions & plot
%
% Computes normalized "difference area" between two distributions and generates
% a plot overlaying the envelopes of the binned distributions.
% The difference between this function and the function histodistance_real
% is that here we assume the two vectors are all integers. 
% 
% Usage: [diffarea plotdata] = histodistance_int(x1, x2, title)
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
%     A number between 0 and 2 representing the area of the absolute difference
%     of the two normalized distributions.  The closer the value to zero, the
%     more alike the distributions are.  A value of 2 would indicate that the
%     distributions were completely disjoint, while 0 would imply that the two
%     were identical.

% Revision History:
%
%      When     Who   What
%   ----------|-----|-----------------------------------------
%     Sep/03    YC  Initial release
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%            
%        1         2         3         4         5         6         7         8
%2345678901234567890123456789012345678901234567890123456789012345678901234567890

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

  x1 = round(x1); 
  x2 = round(x2); 
  
  % First, we've got to bin the data.  We'll start by determining the min & max
  % values.
  xmin = min(min(x1), min(x2));
  xmax = max(max(x1), max(x2));
  
  % Bin the data.  Each bin's value = percentage of normalized samples of x that
  % fall in bin's interval.
  bins1 = bin_data(x1, xmin, xmax);
  bins2 = bin_data(x2, xmin, xmax);
  
  % Now, we'll integrate the difference between the bins to get a measure of the
  % distance between the distributions.
  % (binsize & numbins cancel each other here, so the integral is a simple sum)
  diffarea = sum(abs(bins1 - bins2));
  
  % Now, let's generate a normalized envelope plot of each of the distributions:
  binsize = xmax - xmin +1;
  for (i=0:binsize - 1)
      bins(i+1) = xmin + i;
  end
  
  figure;
  plot(bins, bins1, 'bo-', bins, bins2, 'r+--');
  bins = bins'; 
  plotdata(:, 1) = bins; 
  plotdata(:, 2) = bins1; 
  plotdata(:, 3) = bins2; 
  
  % Label everything appropriately
  xlabel(' X Value');
  ylabel('Bin Probability');
  
  if (isempty(titlestr)) 
    titlestr = sprintf('Density Distance Area = %f', diffarea);
  else
    titlestr = sprintf('%s\nDensity Distance Area = %f', titlestr, diffarea);
  end
  title(titlestr);
  return;
  
  
function bindata = bin_data(x, xmin, xmax)
  binsize = xmax - xmin + 1;
  bins = [xmin : 1 : xmax]';
  bindata = zeros(binsize, 1);
  
  numsamples = size(x, 1);

  % First, we'll collect all the points into the bins & just get a raw
  % count of the bin population
  for (i = 1 : numsamples)
    bin = x(i) - xmin + 1; 

    bindata(bin) = bindata(bin) + 1;
  end
  
  % Now, we'll normalize the bin population to a percentage of the points that
  % lie in that bin:
  bindata = bindata / numsamples; 

  return
