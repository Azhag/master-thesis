%function [ output_args ] = measure_piece_encountering( input_args )
%MEASURE_PIECE_ENCOUNTERING Summary of this function goes here
%   Detailed explanation goes here

%clear;

%nargin = 0;

%TOPLOT
%desired_plot = 1; %Arena size 2, 1 robot
%desired_plot = 2; %Arena size 2, 2 robot
%desired_plot = 3; %Arena size 2, 3 robot
desired_plot = 4; %Arena size 2, 4 robot
%desired_plot = 5; %Arena size 2, 5 robot


%desired_plot = 6; %Arena size 3, 1 robot
%desired_plot = 7; %Arena size 3, 5 robots

%desired_plot = 8; %All robots, arena size 2

file_prefix = 'output_values_';

%linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
%linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
% PARAMETERS
switch(desired_plot),
    case 1,
        folder = 'arena2_1robot';
        nb_experiments = 200;
        title_plot = '1 robot, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_speed = 0.128; %fit 0.09
    case 2,
        folder = 'arena2_2robot';
        nb_experiments = 200;
        title_plot = '2 robots, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_speed = 0.128;
    case 3,
        folder = 'arena2_3robot';
        nb_experiments = 200;
        title_plot = '3 robots, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_speed = 0.128;
    case 4,
        folder = 'arena2_4robot';
        nb_experiments = 200;
        title_plot = '4 robots, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.35; %% Different here !
        robot_speed = 0.128;
    case 5,
        folder = 'arena2_5robot';
        nb_experiments = 200;
        title_plot = '5 robots, area 2m';
        arena_radius = 1.85;
        piece_comm_range = 0.4;
        robot_speed = 0.128;
    case 6,
        folder = 'arena3_1robot';
        nb_experiments = 1; %200 really, just 1 file
        title_plot = '1 robots, area 3m';
        arena_radius = 3.0;
        piece_comm_range = 0.4;
        robot_speed = 0.128;

    case 7,
        folder = 'arena3_5robot';
        nb_experiments = 200;
        title_plot = '5 robots, area 3m';
        arena_radius = 2.8;
        piece_comm_range = 0.4;
        robot_speed = 0.128;
        
    case 8,
        folder = {'arena2_1robot', 'arena2_2robot', 'arena2_3robot', 'arena2_4robot', 'arena2_5robot'};
        nb_experiments = 200;
        arena_radius = 1.85;
        piece_comm_range = 0.35; %% Different here !
        robot_speed = 0.128;
        title_plot = 'Several robots, area 2m';
end;

% Load everything
populations = [];
all_times = [];

if (iscell(folder)),
    
    for folder_num=1:length(folder),
        for exp=1:nb_experiments,
            filename = [folder{folder_num} '/' file_prefix num2str(exp-1) '.txt'];
    
            populations = unique(load(filename),'rows');
    
            % We measure the probability, we need only the times
            all_times(exp,folder_num) = populations(:,1);
        end;
    end;
else,
    for exp=1:nb_experiments,
        filename = [folder '/' file_prefix num2str(exp-1) '.txt'];
    
        populations = unique(load(filename),'rows');
    
        % We measure the probability, we need only the times
        all_times = [all_times; populations(:,1)];
    end;
end;

% Fit a exponential onto the time distributions
[parmhat parmci] = expfit(all_times)
 
% Plot the distribution of the encountering times
figure();
[n, xout] = hist(all_times, 20);
%xout = repmat(xout, size(all_times,2),1);
bar(xout, n/max(sum(n)), 'group');
title(['Distribution of the Piece encountering times, ' title_plot]);
xlabel('Time [s]');
ylabel('Percentage');

% Plot the fitted exponential on top
%x = linspace(0, max(all_times), 100);
y = [];
for robot_num=1:size(parmhat,2);
    y(:,robot_num) = exppdf(xout, parmhat(robot_num));
    y(:,robot_num) = y(:,robot_num)/sum(y(:,robot_num));
end;

hold on;
plot(xout,y, 'r');
legend('Experimental data', ['Fitted exponential, p_e = ' num2str(1./parmhat,3)]);


% Calculate the theoretical probability
arena_size = 6*arena_radius^2*sqrt(3)/4;
timestep = 1.0;
robot_comm_range = 0.6;
p_encounter_piece_theo = (robot_speed*timestep*2*piece_comm_range/arena_size);
p_encounter_piece_mesured = 1./parmhat;

figure();
bar(linspace(1,size(p_encounter_piece_mesured, 2)+1, size(p_encounter_piece_mesured, 2)+1),  [0 p_encounter_piece_mesured]);
set(gca,'XTickLabel',{'Theoretical', '1  robot', '2 robots', '3 robots', '4 robots', '5 robots'});

hold on;
bar(1, p_encounter_piece_theo, 'r');
ylabel('Time to encounter [s]');

errorbar(linspace(2, size(p_encounter_piece_mesured, 2)+1, size(p_encounter_piece_mesured, 2)), [p_encounter_piece_mesured], [1./parmci(1,:)-p_encounter_piece_mesured], [1./parmci(2,:)-p_encounter_piece_mesured], 'r.');
title('Comparison between theoretical and measured probability');
ylabel('Encountering probability');

% Calculate the excluded volume correction
%volume_1robot = pi*robot_comm_range^2;
%diff_volumes = linspace(0, size(p_encounter_piece_mesured, 2),size(p_encounter_piece_mesured, 2)+1)*volume_1robot;
%p_encounter_piece_theo_corrected = (robot_speed*timestep*2*piece_comm_range./(arena_size+diff_volumes));
%hold on;
%plot(linspace(1,size(p_encounter_piece_mesured, 2)+1, size(p_encounter_piece_mesured, 2)+1),  p_encounter_piece_theo_corrected);

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
