%/*****************************************************************************|
%/*  FILE:    Distance and histogram plots for two different samples
%/*
%/*  AUTHOR:  Yang Cao
%/*
%/*  CREATED: September, 2003
%/*
%/*  LAST MODIFIED:
%/*             BY:
%/*             TO:
%/*
%/*  SUMMARY: This function assume the vector contains only integers (or it
%/*           rounds the data automatically). Thus it doesnot require a binsize
%/*           to generate the histogram plot.
%/*  NOTES:
%/*
%/*
%/*  TO DO:
%/*
%/*
%/*****************************************************************************|
%/       1         2         3         4         5         6         7         8
%/345678901234567890123456789012345678901234567890123456789012345678901234567890

function [diffarea, plotdata] = histodistance_real(x1, x2, numbins, titlestr)
% Generates Normalized Difference Area of Two Distributions & plot
%
% Computes normalized "difference area" between two distributions and generates
% a plot overlaying the envelopes of the binned distributions.
%
% Usage: [diffarea, plotdata] = histodistance_real(x1, x2, numbins, title)
%
%   Arguments:
%   
%   x1 -> A vector (row or column) of samples from distribution 1.
%
%   x2 -> A vector (row or column) of samples from distribution 2.
%
%   numbins -> The number of bins (k) into which to group the normalized
%              data.
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
%   11-Nov-02   ASH   Initial release
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

  if (nargin <= 2)
    numbins = 50; 
  end
  
  if (nargin <= 3) 
    titlestr = '';
  end

  % First, we've got to bin the data.  We'll start by determining the min & max
  % values.
  xmin = min(min(x1), min(x2)); 
  xmax = max(max(x1), max(x2)); 
  
  % Bin the data.  Each bin's value = percentage of normalized samples of x that
  % fall in bin's interval.
  bins1 = bin_data(x1, numbins, xmin, xmax);
  bins2 = bin_data(x2, numbins, xmin, xmax);
  
  % Now, we'll integrate the difference between the bins to get a measure of the
  % distance between the distributions.
  % (binsize & numbins cancel each other here, so the integral is a simple sum)
  diffarea = sum(abs(bins1 - bins2))*(xmax - xmin)/numbins;

  % Now, let's generate a normalized envelope plot of each of the distributions:
  plotdata = zeros(numbins, 3);
  for (i = 1 : numbins)
    lowIndex = i;
    
    plotdata(lowIndex,  1) = xmin + (i-0.5)/numbins*(xmax - xmin);
    
    plotdata(lowIndex, 2) = bins1(i);
    
    plotdata(lowIndex, 3) = bins2(i);
  end
  
  figure;
  plot(plotdata(:,1), plotdata(:,2), 'b-', plotdata(:,1), plotdata(:,3), 'r-');
  
  % Label everything appropriately
  xlabel(' X Value');
  ylabel('Bin Probability');
  
  if (isempty(titlestr)) 
    titlestr = sprintf('Difference Area = %f (%d bins)', diffarea, numbins);
  else
    titlestr = sprintf('%s\nDifference Area = %f (%d bins)', titlestr, diffarea, numbins);
  end
  title(titlestr);
  return;
  
  
function bindata = bin_data(x, numbins, xmin, xmax)
  bindata = zeros(numbins, 1);
  binsize = 1/numbins;
  bins = [0 : binsize : 1-binsize]';
  
  dx = xmax - xmin;
  numsamples = size(x, 1);

  % First, we'll collect all the points into the bins & just get a raw
  % count of the bin population
  for (i = 1 : numsamples)
    normval = (x(i)-xmin)/(dx);
    bin = floor((normval) / binsize) + 1; 
    
    if (bin > numbins) 
      bin = numbins; % last point will be mapped to last bin (oh well),
                     % even though it shouldn't be (all intervals should be half-open).
    end

    bindata(bin) = bindata(bin) + 1;
  end
  
  % Now, we'll normalize the bin population to a percentage of the points that
  % lie in that bin:
  bindata = bindata / numsamples * numbins/(xmax - xmin);

  return
