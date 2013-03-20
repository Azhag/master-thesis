%  Script to make plots for Loic's masters thesis on assembly systems

function plotScript(opts)

% opts is a vector of the plot option numbers

%opts = 1

for i = 1:length(opts)
    switch opts(i)
        case 1
            plotRatesVsAlpha;
        case 2
            plot_xF1_xF2;
        case 3
            plotNetFluxes;
        case 4
            plotAdaptationToRates;
    end
end

%-- Plot the rates from the optimization programs that vary with alpha --%

function plotRatesVsAlpha

load RatesP1_alpha_pt01pt01pt99.mat
rp1 = ExpAssemRates_TauAve;
load RatesP2_alpha_pt01pt01pt99.mat
rp2 = ExpAssemRates_TauMin;

% parameters
xlab = '\alpha = x_{F1}/(x_{F1}+x_{F2})';
ylab = 'Rates';
title1 = 'Rates from Problem P1 that change with \alpha';
title2 = 'Rates from Problem P2 that change with \alpha';
leg1a = 'k_4^-';
leg1b = 'k_6^-';
leg2a =  'k_4^+';
leg2b =  'k_4^-';
leg2c = 'k_6^+';
leg2d = 'k_6^-';
loc = 'EastOutside';
alpha_vec = 0.01:0.01:0.99;
c1 = 'b';  c3 = 'r';  c2 = 'b--';  c4 = 'r--';
fsize = 11;
lw = 1;

figure;
hold on
set(gca,'Fontsize',fsize)
%plot(alpha_vec,rp1)
plot(alpha_vec,rp1(:,6),c1,'Linewidth',lw)
plot(alpha_vec,rp1(:,12),c2,'Linewidth',lw)
legend(leg1a,leg1b);
xlabel(xlab)
ylabel(ylab)
title(title1);

figure;
hold on
set(gca,'Fontsize',fsize)
plot(alpha_vec,rp2(:,5),c1,'Linewidth',lw)
plot(alpha_vec,rp2(:,6),c2,'Linewidth',lw)
plot(alpha_vec,rp2(:,11),c3,'Linewidth',lw)
plot(alpha_vec,rp2(:,12),c4,'Linewidth',lw)
legend(leg2a,leg2b,leg2c,leg2d,'Location',loc);
xlabel(xlab)
ylabel(ylab)
title(title2);

%-- Compare x_F1, x_F2 from the optimization programs for selected alpha --%

function plot_xF1_xF2

% parameters
alpha = [0.1 0.5 0.9];
c1 = 'b';  c2 = 'r';  c3 = 'b--';  c4 = 'r--';  c5 = 'k-.';  c6 = 'k-.';
xlab = 'Time';
ylab = 'x_{F1} and x_{F2}';
title1 = 'x_{F1} and x_{F2} vs. time, \alpha = ';
leg1 = 'x_{F1}, P1';
leg2 = 'x_{F2}, P1';
leg3 = 'x_{F1}, P2';
leg4 = 'x_{F2}, P2';
fsize = 11;
lw = 1;

for i = 1:length(alpha)
    [rates, xd, t1, X1] ...
          = optAssemblyRates_plot(alpha(i),0,1,1);  % Program P1    
    [rates, xd, t2, X2] ...
          = optAssemblyRates_plot(alpha(i),0,1,2);  % Program P2
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    
    plot(t1,X1(:,9)/(xd(9)+xd(10)),c1,'Linewidth',lw);
    plot(t1,X1(:,10)/(xd(9)+xd(10)),c2,'Linewidth',lw);
    plot(t2,X2(:,9)/(xd(9)+xd(10)),c3,'Linewidth',lw);
    plot(t2,X2(:,10)/(xd(9)+xd(10)),c4,'Linewidth',lw);
    plot([t1(1) t1(end)],[xd(9)/(xd(9)+xd(10)) xd(9)/(xd(9)+xd(10))],c5);
    plot([t1(1) t1(end)],[xd(10)/(xd(9)+xd(10)) xd(10)/(xd(9)+xd(10))],c6);
    legend(leg1,leg2,leg3,leg4)
    xlabel(xlab)
    ylabel(ylab)
    title([title1 num2str(alpha(i))])
    ylim([0 1]);
    xlim([0 30000]);
end

%-- Plot net fluxes from the optimization programs for selected alpha --%

function plotNetFluxes

% parameters
alpha = [0.1 0.5 0.9];
c1 = 'b'; c2 = 'r'; c3 = 'k'; c4 = 'b--'; c5 = 'r--'; c6 = 'k--';
xlab = 'Time';
ylab = 'Net flux';
title1 = 'Net flux for each reaction R_i vs. time for Problem 1, \alpha = ';
title2 = 'Net flux for each reaction R_i vs. time for Problem 2, \alpha = ';
leg1 = 'R_1';
leg2 = 'R_2';
leg3 = 'R_3';
leg4 = 'R_4';
leg5 = 'R_5';
leg6 = 'R_6';
fsize = 11;
lw = 1;

for i = 1:length(alpha)
    [rates, xd, t1, X1] ...
          = optAssemblyRates_plot(alpha(i),1,1,1);  % Program P1          
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    semilogx(t1,rates(1)*X1(:,1).*X1(:,2) - rates(2)*X1(:,5),c1,'Linewidth',lw);
    semilogx(t1,rates(3)*X1(:,3).*X1(:,4) - rates(4)*X1(:,6),c2,'Linewidth',lw);
    semilogx(t1,rates(7)*X1(:,5).*X1(:,6) - rates(8)*X1(:,7),c3,'Linewidth',lw);
    semilogx(t1,rates(5)*X1(:,2).*X1(:,7) - rates(6)*X1(:,9),c4,'Linewidth',lw);
    semilogx(t1,rates(9)*X1(:,2).*X1(:,5) - rates(10)*X1(:,8),c5,'Linewidth',lw);
    semilogx(t1,rates(11)*X1(:,6).*X1(:,8) - rates(12)*X1(:,10),c6,'Linewidth',lw);    
    legend(leg1,leg2,leg3,leg4,leg5,leg6)
    xlabel(xlab)
    ylabel(ylab)
    title([title1 num2str(alpha(i))])
    
    [rates, xd, t2, X2] ...
          = optAssemblyRates_plot(alpha(i),1,1,2);  % Program P2
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    semilogx(t2,rates(1)*X2(:,1).*X2(:,2) - rates(2)*X2(:,5),c1,'Linewidth',lw);
    semilogx(t2,rates(3)*X2(:,3).*X2(:,4) - rates(4)*X2(:,6),c2,'Linewidth',lw);
    semilogx(t2,rates(7)*X2(:,5).*X2(:,6) - rates(8)*X2(:,7),c3,'Linewidth',lw);
    semilogx(t2,rates(5)*X2(:,2).*X2(:,7) - rates(6)*X2(:,9),c4,'Linewidth',lw);
    semilogx(t2,rates(9)*X2(:,2).*X2(:,5) - rates(10)*X2(:,8),c5,'Linewidth',lw);
    semilogx(t2,rates(11)*X2(:,6).*X2(:,8) - rates(12)*X2(:,10),c6,'Linewidth',lw);    
    legend(leg1,leg2,leg3,leg4,leg5,leg6)
    xlabel(xlab)
    ylabel(ylab)
    title([title2 num2str(alpha(i))])
end


function plotAdaptationToRates
    % Plot the behavior when modifying the rates during an experiment.
    clear;
    
    load RatesP1_alpha_pt01pt01pt99.mat
    rates = ExpAssemRates_TauAve;
    
    tTot = [];
    XTot = [];
    T0 = 0;
    T1 = 1000;
    x0 = 5*[3 6 3 3 zeros(1,6)];
    
    % Converge to normal values
    [t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0, [], rates(floor(size(rates, 1)*0.35),:));
    tTot = [tTot; t];
    XTot = [XTot; X];
    
    x0n = X(end,:);
    T0 = t(end);
    T1 = T0 + 5000;
    
    % Converge to 100% F1.
    [t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0n, [], rates(floor(size(rates, 1)*0.999),:));
    tTot = [tTot; t];
    XTot = [XTot; X];
    
    x0n = X(end,:);
    T0 = t(end);
    T1 = T0 + 5000;
    
    % Converge to 0% F1
    [t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0n, [], rates(ceil(size(rates, 1)*0.001),:));
    tTot = [tTot; t];
    XTot = [XTot; X];
    
    
    % Converge to 50% F1
    %x0n = X(end,:);
    %T0 = t(end);
    %T1 = T0 + 1000;
    %[t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0n, [], rates(ceil(size(rates, 1)*0.93),:));
    %tTot = [tTot; t];
    %XTot = [XTot; X];
    x0n = X(end,:);
    T0 = t(end);
    T1 = T0 + 10000;
    [t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0n, [], rates(ceil(size(rates, 1)*0.45),:));
    tTot = [tTot; t];
    XTot = [XTot; X];
    
    
    linecolor = {'b', [0 0.5 0], 'r', [0.87 .5 0], [0.75 0 .75], [.75 .75 0], 'k', [0.90 0.40 0.60], [0.30 0.40 0.60], [0.60 0.60 1.0]};
    linewidth = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0};
    
    figure();
    for i=1:size(XTot, 2),
        plot(tTot,XTot(:,i)/x0(1), 'Color', linecolor{i}, 'LineWidth', linewidth{i});
        hold on;
    end;
    %legend({'Piece 1', 'Piece 2', 'Piece 3', 'Piece 4', 'Piece 5', 'Piece 6', 'Piece 7', 'Piece 8', 'Final puzzle F1', 'Final puzzle F2'}, 'Location', 'EastOutside');
    legend({'X_1', 'X_2', 'X_3', 'X_4', 'X_5', 'X_6', 'X_7', 'X_8', 'X_{F1}', 'X_{F2}'}, 'Location', 'EastOutside', 'Orientation', 'vertical');
    ylim([0 1]);
    xlim([0 tTot(end)]);
    xlabel('Time [s]');
    ylabel('Proportion of final assemblies');
%    title('System adaptation to change of rates');
    
    % Mark down events
    plot_lines(1000, [], ':k');
    plot_lines(6000, [], ':r');
    plot_lines(11000, [], ':r');
    
