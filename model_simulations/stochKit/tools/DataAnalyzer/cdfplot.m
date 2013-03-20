%/*****************************************************************************|
%/*  FILE:    Culmulative Distribution Plot Function in Matlab
%/*
%/*  AUTHOR:  Yang Cao
%/*
%/*  CREATED: Aug 12, 2004
%/*
%/*  LAST MODIFIED:
%/*             BY:
%/*             TO:
%/*
%/*  SUMMARY: This function generates the cdf(Culmulative distribution function) 
%/*  		for a vector (contains samples of one random variable). It 
%/* 		assumes data are provided in real number and a number of bins 
%/* 		is required.
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

function [mean_, variance_, stddev_] = cdfplot(x, titlestr)

  % Generates a cdf of x, a vector containing N samples of
  % the value.
  if (size(x,1) ~= 1 & size(x,2) ~= 1)
    error 'First argument to distplot must be a vector!';
  end
  
  % We'd like to treat this data as a column vector, even if it's not:
  if (size(x, 1) == 1)
    x = x';
  end
  
  sortedx = sort(x); 
  numsamples = size(x, 1);
  bins = zeros(numsamples, 1); 
  distribution = zeros(numsamples, 1); 
  
  for (i = 1 : numsamples)
      
    bins(i) = sortedx(i);
    distribution(i) = i/numsamples;

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
  
