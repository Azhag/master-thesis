function vargout = LotkaVolterraSSAFunction(x_values, parameters)
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
		vargout = [1 -1 0;0 1 -1];
	elseif(strcmp(x_values,'init'))
		vargout = [50.000000 100.000000 ];
	elseif(strcmp(x_values,'params'))
		vargout = [1.000000 0.005000 0.600000 ];
	end;

else
	%--------------------------------------------------------
	% propensity functions

	vargout = [ 
			 parameters(1)*x_values(1); ... 
			 parameters(2)*x_values(1)*x_values(2); ... 
			 parameters(3)*x_values(2); ... 
	];
end;