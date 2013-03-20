function writeSpecificSSA(varargin)
% writeSpecificSSA output a file running a SSA for the specific model. This
%                   is the optimized version of SSA.m for the current
%                   model.
%  
% writeSpecificSSA(model)
%       Use the given model to create the file. Filename is automatically
%       created from the model name.
%
% writeSpecificSSA(model, filename)
%       Use the model to create the file with specified filename.
%
% Author: Loïc Matthey, adapted from WriteODEFunction in SBMLToolbox

switch (nargin)
    case 1
        model = varargin{1};
        filename = '';
    case 2
        model = varargin{1};
        filename = varargin{2};
    otherwise
        error('writeSpecificSSA(model, (optional) filename)\n%s', 'does not take more than two arguments');
end;

% check input is an SBML model
if (~isSBML_Model(model))
    error('writeSpecificSSA(model, (optional) filename)\n%s', 'first argument must be an model structure');
end;


% -------------------------------------------------------------
% get information from the model
species = AnalyseSpecies(model);
[parametersNames parametersValues] = GetAllParameters(model);
numberReactions = length(model.reaction);
speciesNames = GetSpecies(model);
numberSpecies = length(model.species);

%---------------------------------------------------------------
% get the name/id of the model

name = '';
if (model.SBML_level == 1)
    name = model.name;
else
    if (isempty(model.id))
        name = model.name;
    else
        name = model.id;
    end;
end;

if (~isempty(filename))
    name = filename;
elseif (length(name) > 63)
    name = name(1:60);
end;

name = strcat(name, 'SSA');
filename = strcat(name, '.m');
%--------------------------------------------------------------------
% open the file for writing

fileID = fopen(filename, 'w');

% Write the start of the function
fprintf(fileID, 'function varargout = %s(tspan, y0, modelParameters, noZeroPopulations, lowBoundsPopulations)\n', name);
fprintf(fileID, '%%%s Performs a simulation of reaction with Gillespie SSA ...\n', name);
fprintf(fileID, '%%   Given a matrix of effectors and propensity functions,\n');
fprintf(fileID, '%%   simulates the exact system behavior according to\n');
fprintf(fileID, '%%   Gillespie''s Stochastic Simulation Algorithm\n');
fprintf(fileID, '%%\n');
fprintf(fileID, '%%   [y t] = %s([T0 T1])\n',name);
fprintf(fileID, '%%           Use the initial conditions defined this file.\n');
fprintf(fileID, '%%   [y t] = %s([T0 T1], stoichiometryMatrix, initialPopulations, modelParameters)\n', name);
fprintf(fileID, '%%           Full definition.\n');
fprintf(fileID, '%%\n');
fprintf(fileID, '%% e.g. : [y t] = %s([0 10]);\n', name);
fprintf(fileID, '%% @author: Loic Matthey\n');
fprintf(fileID, '\n');
fprintf(fileID, '\n');
fprintf(fileID, '%% Check input\n');
fprintf(fileID, 'if (nargin < 5),\n');
fprintf(fileID, '    if (nargin < 4),\n');
fprintf(fileID, '        noZeroPopulations = false;\n');
fprintf(fileID, '        if(nargin < 3),\n');
fprintf(fileID, '            modelParameters = [ ');
for i = 1:length(parametersValues)
    fprintf(fileID, '%f ', parametersValues(i));
end;
fprintf(fileID, '];\n');
fprintf(fileID, '            if (nargin < 2),\n');
fprintf(fileID, '                y0 = [ \n');
for i = 1:numberSpecies
    fprintf(fileID, '%f ', species(i).initialValue);
end;
fprintf(fileID, '];\n');
fprintf(fileID, '            end;\n');
fprintf(fileID, '        end;\n');
fprintf(fileID, '    end;\n');
fprintf(fileID, '    lowBoundsPopulations = zeros(size(y0));\n');
fprintf(fileID, '    if (isempty(modelParameters)),\n');
fprintf(fileID, '        modelParameters = [ ');
for i = 1:length(parametersValues)
    fprintf(fileID, '%f ', parametersValues(i));
end;
fprintf(fileID, '];\n');

fprintf(fileID, '    end;\n');
fprintf(fileID, 'end;\n');

fprintf(fileID, 'stoichiometryMatrix = %s;\n',  mat2str(GetStoichiometryMatrix(model)));
fprintf(fileID, '\n');
fprintf(fileID, 'if(~islogical(noZeroPopulations)),\n');
fprintf(fileID, '    error(''ssa:noZeroPopulationNotLogical'',...\n');
fprintf(fileID, '        ''NoZeroPopulation should be true or false'');\n');
fprintf(fileID, 'end;\n');
fprintf(fileID, 'to_allocate = 100;\n');
fprintf(fileID, '\n');
fprintf(fileID, '%% Use parameters\n');
fprintf(fileID, 'nb_species = size(y0,2);\n');
fprintf(fileID, 't = zeros(1,to_allocate);\n');
fprintf(fileID, 'allocated = to_allocate;\n');
fprintf(fileID, 't(1) = tspan(1);\n');
fprintf(fileID, 'Tmax = tspan(2);\n');
fprintf(fileID, 'x = zeros(to_allocate, nb_species);\n');
fprintf(fileID, 'x(1,:) = y0;\n');
fprintf(fileID, '\n');
fprintf(fileID, 'i = 1;\n');
fprintf(fileID, '\n');
fprintf(fileID, '%% Change behavior depending on the handling of zeroth populations\n');
fprintf(fileID, 'if (~noZeroPopulations), %%not handling zeroth populations\n');
fprintf(fileID, '    %% Loop until the time is finished\n');
fprintf(fileID, '    while(t(i) < Tmax),\n');
fprintf(fileID, '        %%reallocate\n');
fprintf(fileID, '        if(i > allocated),\n');
fprintf(fileID, '            t = [t zeros(1,to_allocate)];\n');
fprintf(fileID, '            x = [x ; zeros(to_allocate, nb_species)];\n');
fprintf(fileID, '            allocated = allocated + to_allocate;\n');
fprintf(fileID, '            to_allocate = 2*to_allocate;\n');
fprintf(fileID, '        end;\n');
fprintf(fileID, '\n');
fprintf(fileID, '        %% Update the rates\n');

% Construct the cell array of x_values names and of parameters
for i = 1:numberSpecies
    xvaluesNames{i} = sprintf('x(i,%d)', i);
end;
for i = 1:length(parametersNames)
    newParametersNames{i} = sprintf('modelParameters(%d)', i);
end;

% Get the kinematic rules and transform them
fprintf(fileID, '        rate = [ \n');

for i=1:numberReactions,
    % Get the Kinematic law.
    if (~isempty(model.reaction(i).kineticLaw.formula)),
        reactionToChange = model.reaction(i).kineticLaw.formula;
        
        % Change the Species names
        reactionToChange = Substitute(speciesNames, xvaluesNames, reactionToChange);
        
        % Change the parameters names
        reactionToChange = Substitute(parametersNames, newParametersNames, reactionToChange);
        
        % Write the reaction
        fprintf(fileID, '         %s', reactionToChange);
        fprintf(fileID, '; ... \n');
    end;
end;
fprintf(fileID, '        ];\n');
fprintf(fileID, '        sumrate = sum(rate);\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Next time\n');
fprintf(fileID, '        t(i+1) = t(i) - log(rand())/sumrate;\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Find which reaction fired\n');
fprintf(fileID, '        v=rand();\n');
fprintf(fileID, '        cumrate = cumsum(rate)/sumrate;\n');
fprintf(fileID, '        react_occured = find(v < cumrate, 1, ''first'');\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% If no reaction can occur, stop.\n');
fprintf(fileID, '        if (sumrate == 0),\n');
fprintf(fileID, '            disp(''The populations are all 0 and no reaction can occur. Change NoZeroPopulations to handle that.'');\n');
fprintf(fileID, '            break;\n');
fprintf(fileID, '        end;\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Update the populations accordingly\n');
fprintf(fileID, '        x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)'';\n');
fprintf(fileID, '\n');
fprintf(fileID, '        i = i+1;\n');
fprintf(fileID, '    end;\n');
fprintf(fileID, 'else %%Handling zeroth populations\n');
fprintf(fileID, '    %% Loop until the time is finished\n');
fprintf(fileID, '    while(t(i) < Tmax),\n');
fprintf(fileID, '        %%reallocate\n');
fprintf(fileID, '        if(i > allocated),\n');
fprintf(fileID, '            t = [t zeros(1,to_allocate)];\n');
fprintf(fileID, '            x = [x ; zeros(to_allocate, nb_species)];\n');
fprintf(fileID, '            allocated = allocated + to_allocate;\n');
fprintf(fileID, '        end;\n');
fprintf(fileID, '\n');
fprintf(fileID, '        %% Update the rates\n');

fprintf(fileID, '        %% Update the rates\n');

% Construct the cell array of x_values names and of parameters
for i = 1:numberSpecies
    xvaluesNames{i} = sprintf('x(i,%d)', i);
end;
for i = 1:length(parametersNames)
    newParametersNames{i} = sprintf('modelParameters(%d)', i);
end;

% Get the kinematic rules and transform them
fprintf(fileID, '        rate = [ \n');

for i=1:numberReactions,
    % Get the Kinematic law.
    if (~isempty(model.reaction(i).kineticLaw.formula)),
        reactionToChange = model.reaction(i).kineticLaw.formula;
        
        % Change the Species names
        reactionToChange = Substitute(speciesNames, xvaluesNames, reactionToChange);
        
        % Change the parameters names
        reactionToChange = Substitute(parametersNames, newParametersNames, reactionToChange);
        
        % Write the reaction
        fprintf(fileID, '         %s', reactionToChange);
        fprintf(fileID, '; ... \n');
    end;
end;
fprintf(fileID, '        ];\n');

fprintf(fileID, '        sumrate = sum(rate);\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Next time\n');
fprintf(fileID, '        t(i+1) = t(i) - log(rand())/sumrate;\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Find which reaction fired\n');
fprintf(fileID, '        v=rand();\n');
fprintf(fileID, '        cumrate = cumsum(rate)/sumrate;\n');
fprintf(fileID, '        react_occured = find(v < cumrate, 1, ''first'');\n');
fprintf(fileID, '\n');
fprintf(fileID, '        %% Update the populations accordingly\n');
fprintf(fileID, '        x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)'';\n');
fprintf(fileID, '        \n');
fprintf(fileID, '        %% Prevent populations under bounds\n');
fprintf(fileID, '        while(any(x(i+1,:) <= lowBoundsPopulations)),\n');
fprintf(fileID, '            v=rand();\n');
fprintf(fileID, '            react_occured = find(v < cumrate, 1, ''first'');\n');
fprintf(fileID, '            x(i+1,:) = x(i,:) + stoichiometryMatrix(:,react_occured)'';\n');
fprintf(fileID, '        end;\n');
fprintf(fileID, '\n');
fprintf(fileID, '        i = i+1;\n');
fprintf(fileID, '    end;\n');
fprintf(fileID, 'end;\n');
fprintf(fileID, '\n');
fprintf(fileID, '%% Remove trailing zeros, because of preallocation\n');
fprintf(fileID, 'varargout{1} = x(1:i,:);\n');
fprintf(fileID, 'varargout{2} = removetrailzeros(t);\n');
fprintf(fileID, '\n');


fclose(fileID);

%% Taken from SBMLTOOLBOX
function y = Substitute(InitialCharArray, ReplacementParams, Formula)

    Allowed = {'(',')','*','/','+','-','^', ' ', ','};
    % get the number of parameters to be replced
    NumberParams = length(InitialCharArray);


    % want these in order of shortest to longest
    % since shorter may be subsets of longer 
    % ie.  'alpha'  is a subset of 'alpha1'

    % determine length of each parameter
    for i = 1:NumberParams
        NoCharsInParam(i) = length(InitialCharArray{i});
    end;

    % create an array of the index of the shortest to longest
    [NoCharsInParam, Index] = sort(NoCharsInParam);

    % rewrite the arrays of parameters from shortest to longest
    for i = 1:NumberParams
        OrderedCharArray{i} = InitialCharArray{Index(i)};
        OrderedReplacements{i} = ReplacementParams{Index(i)};
    end;

    RevisedFormula = Formula;

    for i = NumberParams:-1:1
        % before replacing a character need to check that it is not part of a
        % word etc 
        NumOccurences = length(findstr(OrderedCharArray{i}, RevisedFormula));
        for j = 1:NumOccurences
            k = findstr(OrderedCharArray{i}, RevisedFormula);
            if (k(1) == 1)
                before = ' ';
            else
                before = RevisedFormula(k(1)-1);
            end;
            if ((k(1)+length(OrderedCharArray{i})) < length(RevisedFormula))
                after = RevisedFormula(k(1)+length(OrderedCharArray{i}));
            else
                after = ' ';
            end;
            if (ismember(after, Allowed) && ismember(before, Allowed))
                RevisedFormula = regexprep(RevisedFormula, OrderedCharArray{i}, OrderedReplacements{i}, 1);
            end;
        end;
    end;

    y = RevisedFormula;