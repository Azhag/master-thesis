function [hRealGroup final_time] = optimized_puzzle_case_results( figures_start, desired_plot)
%PUZZLE_CASE_RESULTS Load and plot the results of Webots simulations
%   Takes a directory full of specially formatted files and show several
%   plots based on them.

%clear;
%nargin = 0;

hRealGroup = [];

%TOPLOT

if(nargin<=1),
    desired_plot = 1; % optimized two targets alpha 0.001, 1 puzzle, 10min <<---
    %desired_plot = 2; % optimized two targets alpha 0.001, 1 puzzle, 8 robots
    %desired_plot = 3; % optimized two targets alpha 0.001, 3 puzzle, 15 robots
    %desired_plot = 4; % optimized two targets alpha 0.99, 1 puzzle, 8 robots
    %desired_plot = 5; % optimized two targets alpha 0.5, 1 puzzle, 8 robots <<---
    %desired_plot = 6; % optimized two targets alpha 0.99, 1 puzzle, 8 robots <<---
    desired_plot = 7; % two targets, 3 puzzle, 50 exp, bad rates alpha 0.4 <---
    desired_plot = 8; % two targets, 3 puzzle, 50 exp, alpha 0.4 <---
end;

file_prefix = 'output_values_';

linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.5 0.5 1.0], [0.10 0.20 0.50]};
linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
linestyle = '-';
% PARAMETERS
switch(desired_plot),
    case 1,
        folder = '10min_alpha001_1puzzle';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.01, 5 pieces, 8 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 2,
        folder = '20min_alpha001_1puzzle_8rob';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.01, 5 pieces, 8 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 3,
        folder = '15min_alpha001_3puzzle';
        nb_experiments = 29;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.01, 15 pieces, 15 robots, area radius 3m';
        nb_initial_pieces = 3*[1 2 1 1];
    case 4,
        folder = '20min_alpha099_1puzzle_8rob';
        nb_experiments = 78;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.99, 5 pieces, 8 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 5,
        folder = '10min_alpha05_1puzzle';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.5, 5 pieces, 8 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 6,
        folder = '15min_alpha099_1puzzle_8rob';
        nb_experiments = 68;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Optimized Two targets, \alpha = 0.99, 5 pieces, 8 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 7,
        folder = '30min_alpha04_3puzzle_wrongoptrates';
        nb_experiments = 38;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [13 14];
        title_plot = 'Two targets wrong rates, \alpha = 0.4, 15 pieces, 15 robots, area radius 3m';
        nb_initial_pieces = 3*[1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.10 0.20 0.50], [0.10 0.20 0.50]};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        linestyle = '-.';
    case 8,
        folder = '30min_alpha04_3puzzle';
        nb_experiments = 30;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [13 14];
        title_plot = 'Two targets, \alpha = 0.4, 15 pieces, 15 robots, area radius 3m';
        nb_initial_pieces = 3*[1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], 'k', 'k'};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        %linestyle = '-.';
end;

% Load everything
populations = {};
final_times = [];
final_population = [];
time = {};
all_reactions = [];
reactions_occured = {};

for exp=1:nb_experiments,
    filename = [folder '/' file_prefix num2str(exp-1) '.txt'];
    
    reactions_occured{exp} = load(filename);
    %reactions_occured = sortrows(reactions_occured,1);
    reactions_occured{exp} = reactions_occured{exp}(:, [1:18 20 19 21]);
    % I'm stupid: correction if needed
    if (reactions_occured{exp}(reactions_occured{exp}(:,3)==16,17) == 1), 
       reactions_occured{exp}(reactions_occured{exp}(:,3)==16,17) = -1;
    end;
    if (desired_plot == 7),
        reactions_occured{exp} = reactions_occured{exp}(:, [1:19 21 20]);
    end;
    all_reactions = [all_reactions; reactions_occured{exp}];
    
    populations{exp} = [nb_initial_pieces zeros(1, 4+nb_pieces+nb_products)];
    populations{exp} = [populations{exp}; reactions_occured{exp}(:, 4:end)];
    populations{exp} = cumsum(populations{exp});
    
    
    time{exp} = [0; reactions_occured{exp}(:, 1)];
    
    final_population(exp,:) = populations{exp}(end,:);
    
    % Keep the times of complete assembly
    if (sum(final_population(exp,ending_configurations)) == nb_puzzle),
        final_times = [final_times; time{exp}(end)];
    end;
        
    if (any(any(populations{exp} <0))),
        disp(['Population error in experiment ' num2str(exp) ]);
    end;
end;

% I'm stupid: correction if needed
%if (all_reactions(all_reactions(:,3)==16,17) == 1), 
%   all_reactions(all_reactions(:,3)==16,17) = -1;
%end;

all_reactions = sortrows(all_reactions, 1);
all_populations = cumsum(all_reactions(:,12:end))/nb_experiments;
all_times = all_reactions(:,1);
final_time = all_times(end);

if (nargin > 0),

    % Plot the populations averages
    figure(figures_start);
    hold on;
    %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
    hRealGroup = hggroup;
    for piece=(nb_pieces+nb_products-1):(nb_pieces+nb_products),
        hReal = plot(all_times, all_populations(:,piece)/(all_populations(end,end)+all_populations(end,end-1)), 'Color', linecolor{piece}, 'LineWidth', linewidth{piece}, 'LineStyle', linestyle);
        set(hReal, 'Parent', hRealGroup);
        hold on;
    end;
    ylim([0 0.8]);
    xlim([0 1400]);
    %title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('x_{F1}/(x_{F1}^d+x_{F2}^d)  and  x_{F2}/(x_{F1}^d+x_{F2}^d');
else

    % Plot the populations averages
    figure();
    hold on;
    %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
    for piece=1:(nb_pieces+nb_products),
        plot(all_times, all_populations(:,piece), 'Color', linecolor{piece}, 'LineWidth', linewidth{piece});
        hold on;
    end;
    %plot(all_times, all_populations, 'Color', linecolor{1}, 'LineWidth', linewidth{1});
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');
    legend(legends, 'Location', 'NorthEast');
    %xlim([0 1000]);
    %ylim([0 1.2]);

    % Plot the distribution of the finishing times
    figure();
    hist(final_times, 20);
    title('Distribution of the finishing times. Red line is the 75% quantile');
    xlabel('Finishing time [s]');
    ylabel('Number of experiments');

    hold on;
    plot_lines(quantile(final_times, 0.75));


    % Plot the populations, several lines
    figure();
    hold on;
    title(title_plot);
    xlabel('Time [s]');
    ylabel('Populations');
    for exp=1:nb_experiments,
        stairs(time{exp}, populations{exp}(:,ending_configurations));
    end;
    hold off;

end;


disp(['Assembly success rate: ' num2str(sum(sum(final_population(:,ending_configurations),2) == nb_puzzle)/nb_experiments)]);
