%function [ output_args ] = measure_robot_encountering( input_args )
%MEASURE_PIECE_ENCOUNTERING Summary of this function goes here
%   Detailed explanation goes here

%clear;

%nargin = 0;

%TOPLOT
%desired_plot = 1; %Arena size 2, 2 robots
desired_plot = 2; %Arena size 2, 4 robots

%desired_plot = 3; %Arena size 3, 5 robots
%desired_plot = 4; %Arena size 3, 15 robots


file_prefix = 'output_values_';

%linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
%linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
% PARAMETERS
switch(desired_plot),
    case 1,
        folder = 'arena2_2robot';
        nb_experiments = 200;
        title_plot = '2 robots, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.1285;
    case 2,
        folder = 'arena2_4robot';
        nb_experiments = 200;
        title_plot = '4 robots, area 2m';
        arena_radius = 1.7;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.1285;
    case 3,
        folder = 'arena3_2robot';
        nb_experiments = 200;
        title_plot = '2 robots, area 3m';
        arena_radius = 2.77;
        piece_comm_range = 0.4;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
    case 4,
        folder = 'arena3_15robot';
        nb_experiments = 86;
        title_plot = '15 robots, area 3m';
        arena_radius = 2.8;
        piece_comm_range = 0.4;
        robot_comm_range = 0.5;
        robot_speed = 0.128;
end;

% Load everything
populations = [];
all_times = [];

for exp=1:nb_experiments,
    filename = [folder '/' file_prefix num2str(exp-1) '.txt'];
    
    populations = unique(load(filename),'rows');
    
    % We measure the probability, we need only the times
    all_times = [all_times; populations(:,1)];
end;

% Fit a exponential onto the time distributions
[parmhat parmci] = expfit(all_times)
 
% Plot the distribution of the encountering times
figure();
[n, xout] = hist(all_times, 20);
bar(xout, n/sum(n));
title(['Distribution of the Robot encountering times, ' title_plot]);
xlabel('Time [s]');
ylabel('Percentage');

% Plot the fitted exponential on top
%x = linspace(0, max(all_times), 100);
y = exppdf(xout, parmhat);
hold on;
y = y/sum(y);
plot(xout,y, 'r');
legend('Experimental data', ['Fitted exponential, lambda = ' num2str(parmhat)]);


% Calculate the theoretical probability
arena_size = 3*arena_radius^2*sqrt(3)/2;
timestep = 1.0;
p_encounter_robot_theo = (robot_speed*timestep*2*robot_comm_range/arena_size)
p_encounter_robot_mesured = 1/parmhat;

figure();
bar([p_encounter_robot_theo, p_encounter_robot_mesured]);
hold on;
errorbar([1, 2], [p_encounter_robot_theo, p_encounter_robot_mesured], [0 1/parmci(1)-p_encounter_robot_mesured], [0 1/parmci(2)-p_encounter_robot_mesured], 'r.');
title('Comparison between theoretical and measured probability');

%     % Plot the populations averages
%     figure();
%     hold on;
%     %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
%     for piece=1:(nb_pieces+nb_products),
%         plot(all_times, all_populations(:,piece), 'Color', linecolor{piece}, 'LineWidth', linewidth{piece});
%         hold on;
%     end;
%     title([title_plot ' (average)']);
%     xlabel('Time [s]');
%     ylabel('Populations');
% 
%     % Plot the populations, several lines
%     figure();
%     hold on;
%     title(title_plot);
%     xlabel('Time [s]');
%     ylabel('Populations');
%     for exp=1:nb_experiments,
%         stairs(time{exp}, populations{exp}(:,5:end));
%     end;
%     hold off;
% 
%     % Plot the distribution of the finishing times
%     figure();
%     hist(final_times, 20);
%     title('Distribution of the finishing times. Red line is the 75% quantile');
%     xlabel('Finishing time [s]');
%     ylabel('Number of experiments');
% 
%     hold on;
%     plot_lines(quantile(final_times, 0.75));
  
%disp(['Assembly success rate: ' num2str(sum(final_population(:,end) == nb_puzzle)/nb_experiments)]);
