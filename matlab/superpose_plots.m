%function [ output_args ] = superpose_plots( input_args )
%SUPERPOSE_PLOTS Plot superpositions of models and simulations
%   Allows to create superposed plots showing the fitting

% addpath('./odemodel_puzzle_case');
% addpath('./stochmodel_puzzle_case');
% addpath('./puzzle_case_results');

nb_figure = 1;
%%
% 1 Puzzle
% 
% % First figure: ODE Model and Webots
% figure(1);
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup final_time] = puzzle_case_results(1, 14);
% cd '..'
% 
% % Ode model
% cd './odemodel_puzzle_case'
% hODEGroup = odemodel_simulate(1, 1, final_time);
% cd '..'
% 
% 
% set(get(get(hODEGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','ODE Model');
% title('Physical simulation and ODE Model: 1 Puzzle, 5 robots, arena radius 2m');
% ylim([0 1.301]);
% xlim([0 1000]);
% 
% % % Second figure: Stoch Model and Webots
% figure(2);
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup2 final_time] = puzzle_case_results(2, 14);
% cd '..'
% 
% % Ode model
% cd './stochmodel_puzzle_case'
% hStochGroup = stochmodel_puzzle_case(2, 7, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup2,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: 1 Puzzle, 5 robots, arena radius 2m');
% ylim([0 1.301]);
% xlim([0 1000]);
% 
% %%
% % 3 Puzzle
% % First figure: ODE Model and Webots
% figure(3);
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup final_time] = puzzle_case_results(3, 11);
% cd '..'
% 
% % Ode model
% cd './odemodel_puzzle_case'
% hODEGroup = odemodel_simulate(3, 2, final_time);
% cd '..'
% 
% set(get(get(hODEGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','ODE Model');
% title('Physical simulation and ODE Model: 3 Puzzles, 15 robots, arena radius 3m');
% ylim([0 4]);
% 
% 
% 
% 
% % Second figure: Stoch Model and Webots
% figure(4);
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup final_time] = puzzle_case_results(4, 11);
% cd '..'
% 
% % Ode model
% cd './stochmodel_puzzle_case'
% hStochGroup = stochmodel_puzzle_case(4, 6, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: 3 Puzzles, 15 robots, arena radius 3m');
% ylim([0 4]);




% %%
% % 1 Puzzle, two final assemblies
% 
% % First figure: ODE Model and Webots
% %figure(5);
% figure(5)
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup final_time] = puzzle_case_results(5, 13);
% cd '..'
% 
% % Ode model
% cd './odemodel_puzzle_case'
% hODEGroup = odemodel_simulate(5, 3, final_time);
% cd '..'
% 
% 
% set(get(get(hODEGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','ODE Model');
% title('Physical simulation and ODE Model: Two final puzzles, 1 Puzzle, 5 robots, arena radius 2m');
% ylim([0 1.301]);
% xlim([0 565]);
% 
% % Second figure: Stoch Model and Webots
% figure(6);
% close;
% 
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup2 final_time] = puzzle_case_results(6, 13);
% cd '..'
% 
% % Ode model
% cd './stochmodel_puzzle_case'
% hStochGroup = stochmodel_puzzle_case(6, 8, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup2,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: Two final puzzles, 1 Puzzle, 5 robots, arena radius 2m');
% ylim([0 1.301]);
% xlim([0 565]);

%%
% 3 Puzzle, two final assemblies
% First figure: ODE Model and Webots
figure(nb_figure);
close;

% Webots results
cd './puzzle_case_results'
[hRealGroup final_time] = puzzle_case_results(nb_figure, 15);
cd '..'

% Ode model
cd './odemodel_puzzle_case'
hODEGroup = odemodel_simulate(nb_figure, 4, final_time);
cd '..'

% Stochastic model
cd './stochmodel_puzzle_case'
hStochGroup = stochmodel_puzzle_case(nb_figure, 9, final_time);
cd '..'


set(get(get(hODEGroup,'Annotation'),'LegendInformation'),...
     'IconDisplayStyle','on'); % Include this hggroup in the legend
set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
    'IconDisplayStyle','on'); % Include this hggroup in the legend
set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
     'IconDisplayStyle','on'); % Include this hggroup in the legend

legend('Microscopic continuous','Macro continuous', 'Macro discrete');
%title('3 final assemblies, 15 robots, arena radius 3m');
title('');
ylim([0 3.5]);
xlim([0 1050]);
nb_figure = nb_figure+1;

% % Second figure: Stoch Model and Webots
% figure(nb_figure);
% close;
% 
% % Webots results
% cd './puzzle_case_results'
% [hRealGroup final_time] = puzzle_case_results(nb_figure, 15);
% cd '..'
% 
% % Stochastic model
% cd './stochmodel_puzzle_case'
% hStochGroup = stochmodel_puzzle_case(nb_figure, 9, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: Two final puzzles, 3 Puzzles, 15 robots, arena radius 3m');
% ylim([0 3.5]);
% xlim([0 1050]);
% nb_figure = nb_figure +1;


% %%
% % 3 Puzzle, two final assemblies
% % First figure: ODE Model and Webots
% figure(nb_figure);
% close;
% 
% % Webots results
% % cd './puzzle_case_results'
% % [hRealGroup final_time] = puzzle_case_results(nb_figure, 15);
% % cd '..'
% % 
% % % Ode model
% % cd './odemodel_puzzle_case'
% % hODEGroup = odemodel_simulate(nb_figure, 4, final_time);
% % cd '..'
% % 
% % set(get(get(hODEGroup,'Annotation'),'LegendInformation'),...
% %      'IconDisplayStyle','on'); % Include this hggroup in the legend
% % set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
% %     'IconDisplayStyle','on'); % Include this hggroup in the legend
% % legend('Physic Simulation','ODE Model');
% % title('Physical simulation and ODE Model: Two final puzzles, 3 Puzzles, 15 robots, arena radius 3m');
% % ylim([0 3.5]);
% % xlim([0 1050]);
% % nb_figure = nb_figure+1;
% 
% % Stoch Model and Webots alpha 0.01
% figure(nb_figure);
% close;
% 
% % Webots results
% cd './optimized_webots_results'
% [hRealGroup final_time] = optimized_puzzle_case_results(nb_figure, 1);
% cd '..'
% 
% % Stochastic model
% cd './optimized_stochmodel_puzzle_case'
% hStochGroup = optimized_stochmodel_puzzle_case(nb_figure, 13, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: \alpha = 0.01, 1 Puzzle, 8 robots');
% %ylim([0 3.5]);
% %xlim([0 1050]);
% nb_figure = nb_figure +1;
% 
% 
% % Stoch Model and Webots alpha 0.5
% figure(nb_figure);
% close;
% 
% % Webots results
% cd './optimized_webots_results'
% [hRealGroup final_time] = optimized_puzzle_case_results(nb_figure, 5);
% cd '..'
% 
% % Stochastic model
% cd './optimized_stochmodel_puzzle_case'
% hStochGroup = optimized_stochmodel_puzzle_case(nb_figure, 15, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: \alpha = 0.5, 1 Puzzle, 8 robots');
% %ylim([0 3.5]);
% xlim([0 900]);
% nb_figure = nb_figure +1;
% 
% % Stoch Model and Webots alpha 0.99
% figure(nb_figure);
% close;
% 
% % Webots results
% cd './optimized_webots_results'
% [hRealGroup final_time] = optimized_puzzle_case_results(nb_figure, 6);
% cd '..'
% 
% % Stochastic model
% cd './optimized_stochmodel_puzzle_case'
% hStochGroup = optimized_stochmodel_puzzle_case(nb_figure, 16, final_time);
% cd '..'
% 
% set(get(get(hStochGroup,'Annotation'),'LegendInformation'),...
%      'IconDisplayStyle','on'); % Include this hggroup in the legend
% set(get(get(hRealGroup,'Annotation'),'LegendInformation'),...
%     'IconDisplayStyle','on'); % Include this hggroup in the legend
% legend('Physic Simulation','Stochastic Model');
% title('Physical simulation and Stochastic Model: \alpha = 0.99, 1 Puzzle, 8 robots');
% %ylim([0 3.5]);
% xlim([0 900]);
% nb_figure = nb_figure +1;