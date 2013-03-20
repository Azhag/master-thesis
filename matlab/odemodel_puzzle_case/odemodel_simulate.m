%function hODEGroup = odemodel_simulate( figures_start, test_case, final_time)
%SIMULATE Summary of this function goes here
%   Detailed explanation goes here

%clear;
hODEGroup = [];

nargin = 0;
if(nargin<=1),
    % Choose the Puzzle test case to simulate
    test_case = 1; % 4 robots, 1 puzzle
    %test_case = 2; % 15 robots, 3 puzzle
    %test_case = 3; % 5 robots, 1 puzzle, two final puzzles
    test_case = 4; % 15 robots, 3 puzzle, two final puzzles
    
    final_time = 5000;

end;




switch(test_case),
    case 1,
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        dy = @puzzlecase_ode;
        y0 = [5 1 2 1 1 zeros(1,8)];
        
        title_plot = 'ODE Model: 1 Puzzle, 5 robots, area radius 2m';
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Final puzzle F1'};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
	case 2,
        arena_radius = 2.8;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        dy = @puzzlecase_ode;
        y0 = [15 3 6 3 3 zeros(1,8)];
        
        title_plot = 'ODE Model: 3 Puzzle, 15 robots, area radius 3m';
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Final puzzle F1'};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
    case 3,
        arena_radius = 1.85;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        dy = @puzzlecase_ode_twopuzzles;
        y0 = [5 1 2 1 1 zeros(1,10)];
        
        title_plot = 'ODE Model: 1 Puzzle, 5 robots, area radius 2m';
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0  2.0};
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
    case 4,
        arena_radius = 3.0;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        dy = @puzzlecase_ode_twopuzzles;
        y0 = 3*[5 1 2 1 1 zeros(1,10)];
        
        title_plot = 'ODE Model: 3 Puzzle, 15 robots, area radius 3m';
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0  2.0};
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
end;

% Initial parameters, using geometric probabilities
arena_size = 3*arena_radius^2*sqrt(3)/2;
timestep = 1.0;

p_encounter = robot_speed*timestep*2*piece_comm_range/arena_size
p_encounter_robot = robot_speed*timestep*2*robot_comm_range/arena_size

%p_assembly = ones(1,6);
p_assembly = [0.97777 0.9074 0.9636 0.9737 0.8333 1.0];

params = [p_encounter p_encounter_robot*p_assembly];

T0 = 0;
T1 = final_time;

% Simulate the system
[t x] = ode45(dy, [T0, T1], y0, [], params);

% Plot the results

if (nargin >0),
    figure(figures_start);
    hODEGroup = hggroup;
    hold on;
    for piece=6:length(y0),
        hODE = plot(t, x(:,piece), '--', 'Color', linecolor{piece-5}, 'LineWidth', linewidth{piece-5});
        set(hODE, 'Parent', hODEGroup);
        hold on;
    end;
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');
    %legend(legends, 'Location', 'NorthEast');
else
    figure();
    %hold on;
    for piece=6:length(y0),
        plot(t, x(:,piece), 'Color', linecolor{piece-5}, 'LineWidth', linewidth{piece-5});
        hold on;
    end;
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');
    %legend(legends, 'Location', 'NorthEast');
end;