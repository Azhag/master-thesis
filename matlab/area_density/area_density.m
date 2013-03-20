%AREA_DENSITY Calculate and show the density of area utilization
%   Shows how the robots travel on the area. Goal is a uniform distribution

%TOPLOT
%desired_plot = 1; %Forward simple, 4 robots, 1x 20min run
desired_plot = 2; %Forward simple, 5 robots, 5x 10min run
%desired_plot = 3; %Forward simple, 5 robots, 10x 1min run
%desired_plot = 4; %Bacterial, 5 robots, 10x 20min run
%desired_plot = 5; %Bacterial FD 5, 5 robots, 5x 10min run
%desired_plot = 6; %Bacterial FD 10, 5 robots, 5x 10min run
desired_plot = 7; %Bacterial new mvt tumble less, FD 10, 5 robots, 5x 10min run
desired_plot = 8; %Bacterial new mvt tumble more, FD 10, 5 robots, 5x 10min run
%desired_plot = 9; %Bacterial mvt, 1 Robot. FD 10, DT 22. 5x 10min run.
%desired_plot = 10; %Bacterial mvt, 5 Robots. FD 10, DT 22. 5x 10min run.

arena_radius = 2;
bin_resolution = 30;
file_prefix = 'output_values_r';
% PARAMETERS
switch(desired_plot),
    case 1,
        folder = 'forw_20min';
        nb_robots = 4;
        nb_experiments = 1;
        title_plot = 'Forward movement, 4 robots, 1x 20mn run';
    case 2,
        folder = 'forw_10min';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Forward movement, 5 robots, 5x 10mn run';
    case 3,
        folder = 'forw_1min';
        nb_robots = 5;
        nb_experiments = 10;
        title_plot = 'Forward movement, 5 robots, 10x 1mn run';
    case 4,
        folder = 'bact_20min_fd10td25';
        nb_robots = 5;
        nb_experiments = 10;
        title_plot = 'Bacterial movement, 5 robots, 10x 20mn run';
    case 5,
        folder = 'bact_10min_fd8td25';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Bacterial movement, 5 robots, 5x 10mn run';
    case 6,
        folder = 'bact_10min_fd10td25';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Bacterial movement, 5 robots, 5x 10mn run';
    case 7,
        folder = 'bact_10min_fd10td25_newmvt_tumbleless';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Bacterial movement v2, 5 robots, 5x 10mn run';
    case 8,
        folder = 'bact_10min_fd10td25_newmvt_tumblemore';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Bacterial movement v2, 5 robots, 5x 10mn run';
    case 9,
        folder = 'bact_10min_1rb';
        nb_robots = 1;
        nb_experiments = 5;
        title_plot = 'Bacterial movement, 1 robot, 5x 10mn run';
	case 10,
        folder = 'bact_10min_5rb';
        nb_robots = 5;
        nb_experiments = 5;
        title_plot = 'Bacterial movement, 5 robots, 5x 10mn run';
end;

load 'coverage_perfect.mat';

bins_vector = linspace(-arena_radius, arena_radius, bin_resolution);
bins_label = 0.5*(bins_vector(1:end-1)+bins_vector(2:end));

% Load everything
positions = [];
for robot=1:nb_robots,
    for exp=1:nb_experiments,
        filename = [folder '/' file_prefix num2str(robot) '_' num2str(exp-1) '.txt'];

        positions = [positions; load(filename)];
        
        %eval(['positions_' num2str(file) ' = load(filename);']);
        %eval(['hist_pos_' num2str(file) '= hist2d(positions_' num2str(file) '(:,2:3), bins_vector, bins_vector);']);
    
    %eval(['positions_' num2str(file) ' = load(cell2mat(files(file)));']);
    end;
end;

hist_pos = hist2d(positions(:,2:3), bins_vector, bins_vector);
hist_pos = hist_pos / sum(sum(hist_pos));

hist_coverage = hist_pos./coverage_perfect;
hist_coverage(find(isnan(hist_coverage))) = 0;
hist_coverage(find(isinf(hist_coverage))) = 0;

mean_coverage = mean(mean(hist_coverage(find(hist_coverage > 0))));
error = sum(sum(abs(hist_pos-coverage_perfect)));

if(desired_plot >= 1 && desired_plot <= 10),
        figure();
        pcolor(bins_label, bins_label, hist_pos);
        title(title_plot);
        xlabel('Arena X axis');
        ylabel('Arena Y axis');
        
        
        figure();
        surfc(bins_label, bins_label, hist_pos);
        title(title_plot);
        xlabel('Arena X axis');
        ylabel('Arena Y axis');
        zlabel('percent of occupancy');
        
        disp(['Error of area fitting: ' num2str(error)]);
        disp(['Average coverage: ' num2str(mean_coverage)]);
end;