function [ output_args ] = simulate( input_args )
%SIMULATE Summary of this function goes here
%   Detailed explanation goes here

clear;

% Choose the Puzzle test case to simulate
test_case = 1; % 4 robots, 1 puzzle
test_case = 2; % 15 robots, 3 puzzle

legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 1,2', 'Piece 3,4', 'Piece 1,2,3,4', 'Assembled'};
linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};

switch(test_case),
    case 1,
        arena_radius = 2;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.15;

        y0 = [4 1 2 1 1 zeros(1,8)];
        
        title_plot = 'ODE Model: 1 Puzzle, 5 robots, area radius 2m';
        
	case 2,
        arena_radius = 3;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.15;
                
        y0 = [15 3 6 3 3 zeros(1,8)];
        
        title_plot = 'ODE Model: 3 Puzzle, 15 robots, area radius 3m';
        
end;

% Initial parameters, using geometric probabilities
arena_size = 3*arena_radius^2*sqrt(3)/2;
timestep = 1.0;

p_encounter = robot_speed*timestep*2*piece_comm_range/arena_size;
p_encounter_robot = robot_speed*timestep*2*robot_comm_range/arena_size;
p_encounter_robot_11 = p_encounter_robot;

params = [p_encounter p_encounter_robot p_encounter_robot_11];

T0 = 0;
T1 = 1200;

% Simulate the system
[t x] = ode45(@puzzlecase_ode, [T0, T1], y0, [], params);

% Plot the results
figure(1);
hold on;
for piece=6:length(y0),
    plot(t, x(:,piece), '--', 'Color', linecolor{piece-5}, 'LineWidth', linewidth{piece-5});
    hold on;
end;
title([title_plot ' (average)']);
xlabel('Time [s]');
ylabel('Populations');
legend(legends, 'Location', 'NorthEast')
