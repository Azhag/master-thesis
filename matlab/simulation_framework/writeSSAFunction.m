function writeSSAFunction(varargin)
% writeSSAFunction output a file defining a SSAfunction for
%       the given model. To use with SSA.m.
%       This file will provide the propensity rates and the Stoichiometry
%       matrix depending on its calling type.
%  
% writeSSAFunction(model)
%       Use the given model to create the file. Filename is automatically
%       created from the model name.
%
% writeSSAFunction(model, filename)
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
        error('writePropensityFunction(model, (optional) filename)\n%s', 'does not take more than two arguments');
end;

% check input is an SBML model
if (~isSBML_Model(model))
    error('writePropensityFunction(model, (optional) filename)\n%s', 'first argument must be an model structure');
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

name = strcat(name, 'SSAFunction');
filename = strcat(name, '.m');
%--------------------------------------------------------------------
% open the file for writing

fileID = fopen(filename, 'w');


% write the function declaration
fprintf(fileID,  'function vargout = %s(x_values, parameters)\n', name);

% need to add comments to output file
fprintf(fileID, '%% Different behavior depending on calling method:\n');
fprintf(fileID, '%%      - if called with 1 parameter, gives the Stoichiometry matrix (''stoich''), the\n');
fprintf(fileID, '%%        intial concentrations (''init'') or the model parameters (''params'').\n');
fprintf(fileID, '%%      - with parameters, output the current rate propensities, according to actual variables x_values,\n');
fprintf(fileID, '%%        and the model parameters.\n');
fprintf(fileID, '%% To be used with SSA.m\n\n');


fprintf(fileID, 'if (nargin == 1), \n');
fprintf(fileID, '\t%% If called with 1 parameter, switch on the string provided.\n');

% Output the Stoichiometry matrix
fprintf(fileID, '\tif (strcmp(x_values,''stoich''))\n');
fprintf(fileID, '\t\t%% Return the Stoichiometry Matrix.\n');
fprintf(fileID, '\t\tvargout = %s;\n', mat2str(GetStoichiometryMatrix(model)));

% Output the initial concentrations
fprintf(fileID, '\telseif(strcmp(x_values,''init''))\n');
fprintf(fileID, '\t\tvargout = [');
for i = 1:numberSpecies
    fprintf(fileID, '%f ', species(i).initialValue);
end;
fprintf(fileID, '];\n');

% Output the model parameters
fprintf(fileID, '\telseif(strcmp(x_values,''params''))\n');
fprintf(fileID, '\t\tvargout = [');
for i = 1:length(parametersValues)
    fprintf(fileID, '%f ', parametersValues(i));
end;
fprintf(fileID, '];\n');
fprintf(fileID, '\tend;\n\n');

fprintf(fileID, 'else');

% Construct the cell array of x_values names and of parameters
for i = 1:numberSpecies
    xvaluesNames{i} = sprintf('x_values(%d)', i);
end;
for i = 1:length(parametersNames)
    newParametersNames{i} = sprintf('parameters(%d)', i);
end;

% Get the kinematic rules and transform them
fprintf(fileID, '\n\t%%--------------------------------------------------------\n');
fprintf(fileID, '\t%% propensity functions\n\n');

fprintf(fileID, '\tvargout = [ \n');

for i=1:numberReactions,
    % Get the Kinematic law.
    if (~isempty(model.reaction(i).kineticLaw.formula)),
        reactionToChange = model.reaction(i).kineticLaw.formula;
        
        % Change the Species names
        reactionToChange = Substitute(speciesNames, xvaluesNames, reactionToChange);
        
        % Change the parameters names
        reactionToChange = Substitute(parametersNames, newParametersNames, reactionToChange);
        
        % Write the reaction
        fprintf(fileID, '\t\t\t %s', reactionToChange);
        fprintf(fileID, '; ... \n');
    end;
end;

fprintf(fileID, '\t];\n');

fprintf(fileID, 'end;');

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