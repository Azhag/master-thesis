%function hStochGroup = optimized_stochmodel_puzzle_case( figures_start , desired_plot, final_time)
%MODEL_PUZZLE_CASE Load and plot the results of StochKit simulations
%   Takes a directory full of specially formatted files.

%clear;
%hStochGroup = [];

nargin = 0;

%TOPLOT
if(nargin<=1),
    %desired_plot = 1; % OPTIMIZED alpha = 0.01 two final puzzles, no robots, 60 puzzle, 100 exp
    %desired_plot = 2; % OPTIMIZED alpha = 0.01 two final puzzles, no robots, 3 puzzle, 100 exp
    %desired_plot = 3; % OPTIMIZED alpha = 0.01 two final puzzles, no robots, 1 puzzle, 100 exp
    %desired_plot = 4; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 60 puzzle, 100 exp
    %desired_plot = 5; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 3 puzzle, 100 exp
    %desired_plot = 6; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 1 puzzle, 100 exp
    %desired_plot = 7; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 60 puzzle, 100 exp
    %desired_plot = 8; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 3 puzzle, 100 exp
    %desired_plot = 9; % OPTIMIZED alpha = 0.5 two final puzzles, no robots, 1 puzzle, 100 exp
    desired_plot = 10; % WITH 5 ROBOTS OPTIMIZED alpha = 0.01 two final puzzles, 1 puzzle, 100 exp
    desired_plot = 11; % WITH 5 ROBOTS OPTIMIZED alpha = 0.5 two final puzzles, 1 puzzle, 100 exp
    desired_plot = 12; % WITH 5 ROBOTS OPTIMIZED alpha = 0.99 two final puzzles, 1 puzzle, 100 exp
    desired_plot = 13; % WITH 8 ROBOTS OPTIMIZED alpha = 0.01 two final puzzles, 1 puzzle, 100 exp
    %desired_plot = 14; % WITH 8 ROBOTS OPTIMIZED alpha = 0.01 two final puzzles, 3 puzzle, 100 exp
    desired_plot = 15; % WITH 8 ROBOTS OPTIMIZED alpha = 0.5 two final puzzles, 1 puzzle, 100 exp
    desired_plot = 16; % WITH 8 ROBOTS OPTIMIZED alpha = 0.99 two final puzzles, 1 puzzle, 100 exp
    desired_plot = 17; % 15 robots, green manufacturing, 100 exp
end;

file_prefix = 'out_';

linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0};
linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.30 0.40 0.60]};
legends = {'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'};
linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.5 0.5 1.0], [0.10 0.20 0.50]};
% PARAMETERS
switch(desired_plot),
    case 1,
        folder = '60puzzle_alpha001_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 60;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 2,
        folder = '3puzzle_alpha001_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 3,
        folder = '1puzzle_alpha001_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 4,
        folder = '60puzzle_alpha05_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 60;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.5 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 5,
        folder = '3puzzle_alpha05_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.5 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 6,
        folder = '1puzzle_alpha05_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.5 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 7,
        folder = '60puzzle_alpha099_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 60;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 8,
        folder = '3puzzle_alpha099_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99 for two final assemblies, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 9,
        folder = '1puzzle_alpha099_100exp';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99, 60 Puzzle, area radius 3m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 10,
        folder = 'robots5_1puzzle_alpha001';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01, 5 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 11,
        folder = 'robots5_1puzzle_alpha05';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.5, 5 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 12,
        folder = 'robots5_1puzzle_alpha099';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99, 5 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 13,
        folder = 'robots8_1puzzle_alpha001_bis';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01, 8 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 14,
        folder = 'robots5_3puzzle_alpha001';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.01, 5 robots, 3 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 15,
        folder = 'robots8_1puzzle_alpha05';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.5, 8 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 16,
        folder = 'robots8_1puzzle_alpha099';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 1;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99, 8 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    case 17,
        folder = 'green_manufacturing';
        nb_experiments = 100;
        nb_pieces = 4;
        nb_products = 6;
        nb_puzzle = 3;
        ending_configurations = [8 9];
        title_plot = 'SSA: Optimized \alpha = 0.99, 8 robots, 1 Puzzle, area radius 2m';
        linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
end;

% Load everything
reactions_occured = [];
populations = {};
final_times = [];
final_population = [];
time = {};
all_reactions = [];
exp_finished = [];

for exp=1:nb_experiments,
    filename = [folder '/' file_prefix num2str(exp-1) '.txt'];
    
    populations{exp} = unique(load(filename),'rows');
    time{exp} = populations{exp}(:,1); 
    
    
    % Convert back to reactions for summing
    reactions_occured = diff(populations{exp});
    all_reactions = [all_reactions; time{exp}(2:end) reactions_occured(:,2:end)];

   
    populations{exp} = populations{exp}(:,2:end);    
    
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
all_reactions = [[0 populations{1}(1,:)*nb_experiments]; all_reactions];
all_populations = cumsum(all_reactions(:,2:end))/nb_experiments;
all_times = all_reactions(:,1);

%final_times = all_times(all_reactions(:,end)>0);

% 
% % Interpolation for average behaviors
% time_interp = linspace(0, max(final_times), 100);
% pop_interp = {};
% for exp=1:nb_experiments,
%     exp
%     pop_interp{exp} = interp1(time{exp}, populations{exp}, time_interp, 'nearest', 'extrap');
% end;
% pop_piece = {};
% pop_mean = [];
% pop_std = [];
% for piece=1:(nb_pieces+nb_products),
%     pop_piece{piece} = [];
%     for exp=1:nb_experiments,
%         pop_piece{piece} = [pop_piece{piece} pop_interp{exp}(:,4+piece)];
%     end;
%     pop_mean(:, piece) = mean(pop_piece{piece},2);
%     pop_std(:, piece) = var(pop_piece{piece},1,2);
% end;




if (nargin > 0),
     % Plot the populations averages
    figure(figures_start);
    hStochGroup = hggroup;
    hold on;
    %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
    
    if (all_times(end) < final_time),
        all_times= [all_times; final_time];
        all_populations = [all_populations; all_populations(end, :)];
    end;
    for piece=1:(nb_pieces+nb_products),
        hStoch = plot(all_times, all_populations(:,piece), '-.', 'Color', linecolor{piece}, 'LineWidth', linewidth{piece});
        set(hStoch, 'Parent', hStochGroup);
        hold on;
    end;
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');
    xlim([0 final_time]);

else
    % Plot the populations averages
    figure();
    hold on;
    %errorbar(repmat(time_interp', 1, size(pop_mean,2)), pop_mean, sqrt(pop_std));
    for piece=1:(nb_pieces+nb_products),
        plot(all_times, all_populations(:,piece), 'Color', linecolor{piece}, 'LineWidth', linewidth{piece});
        hold on;
    end;
    title([title_plot ' (average)']);
    xlabel('Time [s]');
    ylabel('Populations');
    legend(legends);
    %xlim([0 3600]);
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

    % Plot the distribution of the finishing times
%     figure();
%     hist(final_times, 15);
%     title('Distribution of the finishing times. Red line is the 75% quantile');
%     xlabel('Finishing time [s]');
%     ylabel('Number of experiments');
%     xlim([100 1000]);
% 
%     hold on;
%     plot_lines(quantile(final_times, 0.75));
  
end;

disp(['Assembly success rate: ' num2str(sum(sum(final_population(:,ending_configurations),2) == nb_puzzle)/nb_experiments)]);
