function varargout = DimerDecaySSA(tspan, y0, modelParameters, noZeroPopulations, lowBoundsPopulations)
%DimerDecaySSA Performs a simulation of reaction with Gillespie SSA ...
%   Given a matrix of effectors and propensity functions,
%   simulates the exact system behavior according to
%   Gillespie's Stochastic Simulation Algorithm
%
%   [y t] = DimerDecaySSA([T0 T1])
%           Use the initial conditions defined this file.
%   [y t] = DimerDecaySSA([T0 T1], stoichiometryMatrix, initialPopulations, modelParameters)
%           Full definition.
%
% e.g. : [y t] = DimerDecaySSA([0 10]);
% @author: Loic Matthey


% Check input
if (nargin < 5),
    if (nargin < 4),
        noZeroPopulations = false;
        if(nargin < 3),
            modelParameters = [ 1.000000 0.002000 0.500000 0.040000 ];
            if (nargin < 2),
                y0 = [ 
100000.000000 0.000000 0.000000 ];
            end;
        end;
    end;
    lowBoundsPopulations = zeros(size(y0));
    if (isempty(modelParameters)),
        modelParameters = [ 1.000000 0.002000 0.500000 0.040000 ];
    end;
end;
stoichiometryMatrix = [-1 -2 2 0;0 1 -1 -1;0 0 0 1];

if(~islogical(noZeroPopulations)),
    error('ssa:noZeroPopulationNotLogical',...
        'NoZeroPopulation should be true or false');
end;
to_allocate = 100;

% Use parameters
nb_species = size(y0,2);
t = zeros(1,to_allocate);
allocated = to_allocate;
t(1) = tspan(1);
Tmax = tspan(2);
x = zeros(to_allocate, nb_species);
x(1,:) = y0;

i = 1;

% Change behavior depending on the handling of zeroth populations
if (~noZeroPopulations), %not handling zeroth populations
    % Loop until the time is finished
    while(t(i) < Tmax),
        %reallocate
        if(i > allocated),
            t = [t zeros(1,to_allocate)];
            x = [x ; zeros(to_allocate, nb_species)];
            allocated = allocated + to_allocate;
            to_allocate = 2*to_allocate;
        end;

        % Update the rates
        rate = [ 
         modelParameters(1)*x(i,1); ... 
         (modelParameters(2)/2.0)*x(i,1)*(x(i,1)-1); ... 
         modelParameters(3)*x(i,2); ... 
         modelParameters(4)*x(i,2); ... 
        ];
        sumrate = sum(rate);
        
        % Next time
        t(i+1) = t(i) - log(rand())/sumrate;
        
        % Find which reaction fired
        v=rand();
        cumrate = cumsum(rate)/sumrate;
        react_occured = find(v < cumrate, 1, 'first');
        
        % If no reaction can occur, stop.
        if (sumrate == 0),
            disp('The populations are all 0 and no reaction can occur. Change NoZeroPopulations to handle that.');
            break;
        end;
        
        % Update the populations accordingly
        x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)';

        i = i+1;
    end;
else %Handling zeroth populations
    % Loop until the time is finished
    while(t(i) < Tmax),
        %reallocate
        if(i > allocated),
            t = [t zeros(1,to_allocate)];
            x = [x ; zeros(to_allocate, nb_species)];
            allocated = allocated + to_allocate;
        end;

        % Update the rates
        % Update the rates
        rate = [ 
         modelParameters(1)*x(i,1); ... 
         (modelParameters(2)/2.0)*x(i,1)*(x(i,1)-1); ... 
         modelParameters(3)*x(i,2); ... 
         modelParameters(4)*x(i,2); ... 
        ];
        sumrate = sum(rate);
        
        % Next time
        t(i+1) = t(i) - log(rand())/sumrate;
        
        % Find which reaction fired
        v=rand();
        cumrate = cumsum(rate)/sumrate;
        react_occured = find(v < cumrate, 1, 'first');

        % Update the populations accordingly
        x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)';
        
        % Prevent populations under bounds
        while(any(x(i+1,:) <= lowBoundsPopulations)),
            v=rand();
            react_occured = find(v < cumrate, 1, 'first');
            x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)';
        end;

        i = i+1;
    end;
end;

% Remove trailing zeros, because of preallocation
varargout{1} = x(1:i,:);
varargout{2} = removetrailzeros(t);

