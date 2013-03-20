% Construct vectors whose orthogonality to the
% flat form Kvec of the rates k_ij enforces
% that Ydes remains a zero eigenvector to K

% for some reason this turns out to be rank deficient
% by 1

% NOTE: We use the fact that for a network of deficiency zero,
% Y*K*y = 0 iff Y*x = 0; i.e. the null space of K coincides with
% the null space of Y*K 
% - from "The Existence and Uniqueness of Steady States for a 
%  Class of Chemical Reaction Networks," Martin Feinberg 1995

% columns of this matrix will hold the vectors
ConsVec = zeros(NConnections, NComplexes);

for(IS = 1:NComplexes);
    %set of connections whose target is site IS
    I1 = find(Connections(:,2) == IS);
    %set of sites which are the sources of these connections
    Sources = Connections(I1,1);
    %set of connections whose source is site IS
    I2 = find(Connections(:,1) == IS);
    ConsVec(I1,IS) = Ydes(Sources);
    ConsVec(I2,IS) = - Ydes(IS);
end;

% SVD the constraint vector set
[UC SC VC] = svd(ConsVec);

% UC(:,1:NComplexes-1) contains an orthonormal basis of the hyperplane spanned
% by the constraint vectors

% US(1:,NComplexes:NConnections) spans the rest
% will use the latter to project proposed changes to Kvec 
% so that we maintain Kvec' * ConsVec(:,l)  = 0 for any l=1:NComplexes
ConProj = UC(:,NComplexes:NConnections) * UC(:,NComplexes:NConnections)';
