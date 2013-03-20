%/*****************************************************************************|
%/*  FILE:    Histogram Plot Function in Matlab
%/*
%/*  AUTHOR:  Andrew Hall
%/*
%/*  CREATED: January, 2003 
%/*
%/*  LAST MODIFIED: August 12, 2004
%/*             BY: Yang Cao
%/*             TO: Add comments and changed the function name. 
%/*
%/*  SUMMARY: This function generates the histogram for a vector (contains
%/*           samples of one random variable). It assumes data are provided 
%/*           in real number and a number of bins is required. 
%/*
%/*  NOTES:
%/*
%/*
%/*  TO DO:
%/*
%/*
%/*****************************************************************************|
%/       1         2         3         4         5         6         7         8
%/345678901234567890123456789012345678901234567890123456789012345678901234567890

function [mean_, variance_, stddev_] = histoplot_real(x, numbins, titlestr)

  global binsize;
  global numsamples;
  
  % Generates a normalized histogram of x, a vector containing N samples of
  % the value.
  if (size(x,1) ~= 1 & size(x,2) ~= 1)
    error 'First argument to distplot must be a vector!';
  end
  
  % We'd like to treat this data as a column vector, even if it's not:
  if (size(x, 1) == 1)
    x = x';
  end
  
  % First, we've got to bin the data.  We'll start by determining the min & max
  % values.
  xmin = norm(x, -inf);
  xmax = norm(x, inf);
  
  % Also, we'd like the bins to be centered around the mean--so we need an odd
  % number of bins:
  if (nargin == 1) 
	numbins = 50; 
  end
  if (mod(numbins, 2) == 0)
    numbins = numbins + 1;
  end
  
  % Now, go ahead & generate bins
  binsize = (xmax - xmin) / (numbins  - 1);
  bins = [xmin : binsize : xmax]';
  
  % Now, collect the samples into the bins:
  rawbindata = zeros(size(bins));
  numsamples = size(x, 1);
  for (i = 1 : numsamples)
    bin = findbin(x(i), bins);
    rawbindata(bin) = rawbindata(bin) + 1;
  end
  
  % Now, what actually gets displayed is the centers of the bins, so we've
  % got to shift them:
  bins = bins + (binsize/2);
  
  % And, we'd like to show probability, rather than raw sample count, so...
  normbindata = normalize(rawbindata);
  yourplot = bar(bins, normbindata, 1);
  hold on
  
  errbars = normalize(sqrt(rawbindata));
  errorbar(bins, normbindata, errbars);
  
  % Label everything appropriately
  xlabel('Final Species Population');
  ylabel('Bin Probability');
  
  mean_ = mean(x);
  variance_ = var(x);
  stddev_ = std(x);
  datainfo = sprintf('Samples: %.f Mean: %.6g Variance: %.6g StdDev: %.4g', ...
                      numsamples, mean_, variance_, stddev_);
  
  if (nargin <= 2)
    titlestr = '';  
  end
  if (size(titlestr, 2) == 0)
    titlestr = datainfo;
  else
    titlestr = {titlestr, datainfo};
  end
  
  title(titlestr);
  hold off;

  clear global numsamples;
  clear global binsize;
  return;
  
function bin = findbin(dataval, bins)
  for (i = 1 : size(bins, 1))
    if (bins(i) > dataval)
      i = i - 1;
      break;
    end
  end

  bin = i;
  return;
  
  
function normdata = normalize(rawdata)
  global numsamples;
  global binsize;
  
  normdata = rawdata  / (numsamples);% * binsize);
