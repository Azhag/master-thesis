function vargout = DimerDecaySSAFunction(x_values, parameters)
% Different behavior depending on calling method:
%      - if called with 1 parameter, gives the Stoichiometry matrix ('stoich'), the
%        intial concentrations ('init') or the model parameters ('params').
%      - with parameters, output the current rate propensities, according to actual variables x_values,
%        and the model parameters.
% To be used with SSA.m

if (nargin == 1), 
	% If called with 1 parameter, switch on the string provided.
	if (strcmp(x_values,'stoich'))
		% Return the Stoichiometry Matrix.
		vargout = [-1 -2 2 0;0 1 -1 -1;0 0 0 1];
	elseif(strcmp(x_values,'init'))
		vargout = [100000.000000 0.000000 0.000000 ];
	elseif(strcmp(x_values,'params'))
		vargout = [1.000000 0.002000 0.500000 0.040000 ];
	end;

else
	%--------------------------------------------------------
	% propensity functions

	vargout = [ 
			 parameters(1)*x_values(1); ... 
			 (parameters(2)/2.0)*x_values(1)*(x_values(1)-1); ... 
			 parameters(3)*x_values(2); ... 
			 parameters(4)*x_values(2); ... 
	];
end;