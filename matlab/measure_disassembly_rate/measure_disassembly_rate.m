%function [ output_args ] = measure_piece_encountering( input_args )
%MEASURE_PIECE_ENCOUNTERING Summary of this function goes here
%   Detailed explanation goes here


clear;

% We measure the probability, we need only the times
all_times = load('disassemblies_times_2_1.txt');

% Fit a exponential onto the time distributions
[parmhat parmci] = expfit(all_times);
 
% Plot the distribution of the encountering times
figure();
[n, xout] = hist(all_times, 20);
%xout = repmat(xout, size(all_times,2),1);
bar(xout, n/max(sum(n)), 'group');
title(['Distribution of the Disassembly times']);
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
p_disassembly_theo = 0.01;
p_disassembly_mesured = 1./parmhat;

figure();
bar(linspace(1,size(p_disassembly_mesured, 2)+1, size(p_disassembly_mesured, 2)+1),  [0 p_disassembly_mesured]);
set(gca,'XTickLabel',{'Theoretical', 'Measured'});

hold on;
bar(1, p_disassembly_theo, 'r');
errorbar(linspace(2, size(p_disassembly_mesured, 2)+1, size(p_disassembly_mesured, 2)), p_disassembly_mesured, [1./parmci(1,:)-p_disassembly_mesured], [1./parmci(2,:)-p_disassembly_mesured], 'r.');
title('Comparison between theoretical and measured rate');
ylabel('Disassembly probability');

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
