%/*****************************************************************************|
%/*  FILE:    Histogram Plot Function in Matlab
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
%/*	      rounds the data automatically). Thus it doesnot require a binsize
%/*	      to generate the histogram plot. 
%/*  NOTES:
%/*
%/*
%/*  TO DO:
%/*
%/*
%/*****************************************************************************|
%/       1         2         3         4         5         6         7         8
%/345678901234567890123456789012345678901234567890123456789012345678901234567890

function [mean_, variance_, stddev_] = histoplot_int(x, titlestr)

  % Generates a normalized histogram of x, a vector containing N samples of
  % the value.
  if (size(x,1) ~= 1 & size(x,2) ~= 1)
    error 'First argument to distplot must be a vector!';
  end
  
  % We'd like to treat this data as a column vector, even if it's not:
  if (size(x, 1) == 1)
    x = x';
  end

  % We assume all x are integers. If not, round it first. 
  x = round(x); 
  
  % First, we've got to bin the data.  We'll start by determining the min & max
  % values.
  xmin = min(x);
  xmax = max(x);
  
  % Now, go ahead & generate bins. Here we use the fact that x are integers 
  binsize = xmax - xmin +1;
  bins = [xmin:1:xmax]';

  % Now, collect the samples into the bins:
  rawbindata = zeros(binsize, 1); 
  numsamples = size(x, 1);
  
  for (i = 1 : numsamples)
      
    bin = x(i) - xmin +1;
    
    rawbindata(bin) = rawbindata(bin) + 1;
  end
  for ( i = 1: binsize)
      distribution(i) = rawbindata(i)/numsamples;
  end
  
  % Label everything appropriately
  figure;
  xlabel('Final Species Population');
  ylabel('Bin Probability');
  
  plot(bins, distribution);
  
  mean_ = mean(x);
  variance_ = var(x);
  stddev_ = std(x);
  datainfo = sprintf('Samples: %.f Mean: %.6g Variance: %.6g StdDev: %.4g', ...
                      numsamples, mean_, variance_, stddev_);
  
  if (nargin == 1)
    titlestr = '';  
  end
  if (size(titlestr, 2) == 0)
    titlestr = datainfo;
  else
    titlestr = {titlestr, datainfo};
  end
  title(titlestr);

  return;
  
