function [ B N Q R] = constructMatrices( pa )
%CONSTRUCTMATRICES Construct the canonical form of the transition matrix
%for an unique piece in our test case with two final assemblies

%%% We have two absorbing states and a transient 8 states classe. The
%%% probabilities are calculated as average over all marking possibles.

% Matrix going to absorbing states
R = zeros(8,2);
R(2,1) = 1;
R(7,1) = 1;
R(6,2) = pa;
R(8,2) = pa;
R = R/7;

Q = (1/7)*[0 0 0 0 4 0 0 0;
     0 0 0 0 2 0 0 2;
     0 0 0 0 0 3 0 0;
     0 0 0 0 0 3 0 0;
     0 0 0 0 0 0 1 2;
     0 0 0 0 0 0 1 0;
     0 0 0 0 0 0 0 0;
     0 0 0 0 0 0 0 0];
Q = diag(1-sum([R Q],2))+Q;


N = inv(eye(8) - Q);
B = N*R;

x0 = [1 2 1 1 0 0 0 0];
x0 = x0/sum(x0);

p_abs = x0*B