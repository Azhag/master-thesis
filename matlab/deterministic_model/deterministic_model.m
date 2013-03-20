%function hODEGroup = deterministic_model( figures_start, test_case, final_time)
%DETERMINISTIC_MODEL Summary of this function goes here
%   Detailed explanation goes here

%clear;
hODEGroup = [];

nargin = 0;
if(nargin<=1),
    % Choose the plot
    test_case = 1; % Comparison

end;

concentration = 15/(3*3^2*sqrt(3)/2);

switch(test_case),
    case 1,
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        time_cost_piece_handling = 0;
        
        nb_assembly_steps = 4;
        nb_puzzles = [1:10];
        
        file_stoch = 'res_finishing_times';
        nb_exp=100;
        
        title_plot = 'Deterministic: 1 Puzzle, 4 robots, time assembly ';
%        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
 %       linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0  2.0};
  %      linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
  
end;

% Initial parameters, using geometric probabilities
nb_robots = 5*nb_puzzles;
%arena_radius = sqrt(2*nb_robots/(3*concentration*sqrt(3)))
arena_radius = 3;
arena_size = 3*arena_radius.^2*sqrt(3)/2
timestep = 1.0;

covered_space_robots = 0.5*2*nb_robots;
arena_size = (arena_size./covered_space_robots(1)).*covered_space_robots
arena_size_det = (3*sqrt(2)*2)^2*nb_puzzles;

p_encounter = robot_speed*timestep*2*piece_comm_range./arena_size
p_encounter_robot = robot_speed*timestep*2*robot_comm_range./arena_size

%p_assembly = ones(1,6);
p_assembly = [0.97777 0.9074 0.9636 0.9737 0.8333 1.0];

% Calculate the parallelizability of deterministic case
nb_processing_units = ones(size(nb_puzzles));
for i=2:size(nb_puzzles,2),
    nb_processing_units(find(arena_size_det(i) < arena_size, 1, 'first'):end) = nb_processing_units(find(arena_size_det(i) < arena_size, 1, 'first'):end)+1;
end;

% Calculate the times
time_travel = (1./(p_encounter_robot))+time_cost_piece_handling;
%time_travel = (1./(p_encounter_robot(1)))+time_cost_piece_handling;
%time_travel = arena_radius*sqrt(2);
t_assembly = 2*time_travel*nb_assembly_steps;
%time_total = nb_puzzles.*t_assembly;
%time_total = t_assembly.*nb_puzzles./nb_processing_units;
time_total = t_assembly;
%time_total = t_assembly./nb_puzzles;

% Time stochastic (measured manually)
%stoch_times = [4765,4297,3625,3405,3000,2434,2280,2230,2055,2012];
stoch_times = 3070*ones(size(nb_puzzles));

% Plot the results
if (nargin >0),
    figure(figures_start);
    hODEGroup = hggroup;
    hold on;
    %for piece=6:length(y0),
        hODE = plot(nb_puzzles, time_total); %, nb_puzzles, stoch_times(1:size(nb_puzzles,2))); %, '--', 'Color', linecolor{piece-5}, 'LineWidth', linewidth{piece-5});
        set(hODE, 'Parent', hODEGroup);
        hold on;
    %end;
    title([title_plot num2str(t_assembly)]);
    xlabel('Nb puzzles to build');
    ylabel('Time to construction [s]');
    %legend(legends, 'Location', 'NorthEast');
else
    figure();
    %hold on;
    %for piece=6:length(y0),
        plot(nb_puzzles, time_total, nb_puzzles, stoch_times(1:size(nb_puzzles,2))); %, '--', 'Color', linecolor{piece-5}, 'LineWidth', linewidth{piece-5});
        hold on;
    %end;
    title([title_plot]);
    xlabel('Nb puzzles to build');
    ylabel('Time to construction [s]');
    %legend(legends, 'Location', 'NorthEast');
end;