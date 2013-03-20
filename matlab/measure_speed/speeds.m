%function [ dists ] = speeds( input_args )
%SPEEDS Summary of this function goes here
%   Detailed explanation goes here

%output = load('1robot.txt');
%output = load('5robots.txt');
%output = load('1robot_arena3.txt');
output = load('15robots_arena3.txt');

% Get the pairwise distances
pairdists = pdist(output(:,2:3));
pairtimes = pdist(output(:,1));

% The travelled distances between each point is the 1-upper diagonal
dists = diag(squareform(pairdists), 1);
times = diag(squareform(pairtimes), 1);

% Calculate the mean speed
mean_speed = sum(dists)/sum(times)