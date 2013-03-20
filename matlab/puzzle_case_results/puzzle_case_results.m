function [hRealGroup final_time] = puzzle_case_results( figures_start, desired_plot)
%PUZZLE_CASE_RESULTS Load and plot the results of Webots simulations
%   Takes a directory full of specially formatted files and show several
%   plots based on them.

%clear;
%nargin = 0;

hRealGroup = [];

%TOPLOT

if(nargin<=1),
    %desired_plot = 1; %5 robots, 1 puzzle, 5 exp
    %desired_plot = 2; %5 robots, 1 puzzle, 20 exp
    %desired_plot = 3; %5 robots, 1 puzzle, 50 exp
    %desired_plot = 4; %15 robots, 3 puzzle, 5 exp
    %desired_plot = 5; %15 robots, 3 puzzle, 10 exp
    
    %desired_plot = 6; %15 robots, 3 puzzle, 50 exp
    %desired_plot = 7; %5 robots, 1 puzzle, 50 exp
    %desired_plot = 8; %15 robots, 3 puzzle, 50 exp
    %desired_plot = 9; %4 robots, 1 puzzle, 100 exp, random initialization, webots precision 32
    %desired_plot = 10; %15 robots, 3 puzzle, 50 exp, random initialization, webots precision 32
    desired_plot = 11; %15 robots, 3 puzzle, 100 exp, random initialization, precision 16 <---
    %desired_plot = 12; % two targets, 1 puzzle, 100 exp
    desired_plot = 13; % two targets, 1 puzzle, 100 exp, new  <---
    %desired_plot = 14; % 1 puzzle, 100 exp, 5 robots <---
    desired_plot = 15; % two targets, 3 puzzle, 50 exp, new  <---
end;

file_prefix = 'output_values_';

linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Final puzzle F1'};
ending_configurations = [12];
lying_pieces = false;

% PARAMETERS
switch(desired_plot),
    case 1,
        folder = '1puzzle_5exp';
        nb_experiments = 5;
        nb_pieces = 4;
        nb_products = 4;
        title_plot = '1 Puzzle, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 2,
        folder = '1puzzle_20exp';
        nb_experiments = 20;
        nb_pieces = 4;
        nb_products = 4;
        title_plot = '1 Puzzle, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 3,
        folder = '1puzzle_50exp';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 1;
        title_plot = '1 Puzzle, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 4,
        folder = '3puzzle_5exp';
        nb_experiments = 5;
        nb_pieces = 4;
        nb_puzzle = 3;
        nb_products = 4;
        title_plot = '3 Puzzle, 15 robots, area radius 3m';
        nb_initial_pieces = [3 6 3 3];
    case 5,
        folder = '3puzzle_10exp';
        nb_experiments = 10;
        nb_pieces = 4;
        nb_puzzle = 3;
        nb_products = 4;
        title_plot = '3 Puzzles, 15 robots, area radius 3m';
        nb_initial_pieces = [3 6 3 3];
    case 6,
        folder = '3puzzle_50exp';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_puzzle = 3;
        nb_products = 4;
        title_plot = '3 Puzzles, 15 robots, area radius 3m';
        nb_initial_pieces = [3 6 3 3];
    case 7,
        folder = '1puzzle_50exp_newtimes';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 1;
        title_plot = '1 Puzzle, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 8,
        folder = '3puzzle_50exp_newtimes';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 3;
        title_plot = '3 Puzzle, 15 robots, area radius 3m';
        nb_initial_pieces = [3 6 3 3];
    case 9,
        folder = '1puzzle_100exp_randominit';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 1;
        title_plot = '1 Puzzle, 4 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 10,
        folder = '3puzzle_50exp_randominit';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 3;
        title_plot = '1 Puzzle, 4 robots, area radius 2m';
        nb_initial_pieces = [3 6 3 3];
    case 11,
        folder = '3puzzle_100exp_rand_prec';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 3;
        title_plot = '3 Puzzle, 15 robots, area radius 3m';
        nb_initial_pieces = [3 6 3 3];
    case 12,
        folder = 'twotargets_1puzzle_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [12 14];
        title_plot = 'Two targets, 5 pieces, 4 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
    case 13,
        folder = 'twotargets_1puzzle_150exp_new';
        nb_experiments = 150;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [13 14];
        title_plot = 'Two targets, 5 pieces, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
     case 14,
        folder = '1puzzle_100exp_5robots';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 4;
        nb_puzzle = 1;
        title_plot = '1 Puzzle, 5 robots, area radius 2m';
        nb_initial_pieces = [1 2 1 1];
    case 15,
        folder = 'twotargets_3puzzle_50exp';
        nb_experiments = 50;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [13 14];
        title_plot = 'Two targets, 15 pieces, 15 robots, area radius 3m';
        nb_initial_pieces = 3*[1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
    case 16,
        folder = 'twotargets_3puzzle_wrongrates';
        nb_experiments = 38;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [13 14];
        title_plot = 'Two targets, 15 pieces, 15 robots, area radius 3m';
        nb_initial_pieces = 3*[1 2 1 1];
        linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.10 0.20 0.50]};
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
        legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
        lying_pieces = true;
end;

% Load everything
populations = {};
final_times = [];
final_population = [];
time = {};
all_reactions = [];

for exp=1:nb_experiments,
    filename = [folder '/' file_prefix num2str(exp-1) '.txt'];
    
    reactions_occured = load(filename);
    %reactions_occured = sortrows(reactions_occured,1);
    
    if (lying_pieces), 
       reactions_occured = reactions_occured(:, [1:7 12:end]);
    end;
    if (size(ending_configurations,2) > 1),
        reactions_occured = reactions_occured(:, [1:(size(reactions_occured,2)-3) size(reactions_occured,2)-1 size(reactions_occured,2)-2 size(reactions_occured,2)]);
    else
        reactions_occured = reactions_occured(:, [1:(size(reactions_occured,2)-2)]);
    end;
    
    all_reactions = [all_reactions; reactions_occured];
    
    populations{exp} = [nb_initial_pieces zeros(1, nb_pieces+nb_products)];
    populations{exp} = [populations{exp}; reactions_occured(:, 4:end)];
    populations{exp} = cumsum(populations{exp});
    
    
    time{exp} = [0; reactions_occured(:, 1)];
    
    final_population(exp,:) = populations{exp}(end,:);
    
    % Keep the times of complete assembly
    if (sum(final_population(exp,ending_configurations)) == nb_puzzle),
        final_times = [final_times; time{exp}(end)];
    end;
        
    if (any(any(populations{exp} <0))),
        disp(['Population error in experiment ' num2str(exp) ]);
    end;
end;

all_reactions = sortrows(all_reactions, 1);
all_populations = cumsum(all_reactions(:,8:end))/nb_experiments;
all_times = all_reactions(:,1);
final_time = all_times(end);

if (nargin > 0),

    % Plot the populations averages
    figure(figures_start);
    hold on;
    %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
    hRealGroup = hggroup;
    for piece=1:(nb_pieces+nb_products),
        hReal = plot(all_times, all_populations(:,piece), 'Color', linecolor{piece}, 'LineWidth', linewidth{piece});
        set(hReal, 'Parent', hRealGroup);
        hold on;
    end;
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');

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
    legend(legends, 'Location', 'East');
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
