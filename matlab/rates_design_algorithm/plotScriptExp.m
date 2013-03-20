%  Script to make plots for Loic's masters thesis on assembly systems

function plotScriptExp(opts)

% opts is a vector of the plot option numbers

for i = 1:length(opts)
    switch opts(i)
        case 1
            plotRatesVsAlpha;
        case 2
            plot_xF1_xF2;
        case 3
            plotNetFluxes;
    end
end

%-- Plot the rates from the optimization programs that vary with alpha --%

function plotRatesVsAlpha

load RatesP1_alpha_pt01pt01pt99_exp.mat
load RatesP2_alpha_pt01pt01pt99_exp.mat

% parameters
xlab = '\alpha = x_{F1}/(x_{F1}+x_{F2})';
ylab = 'Rates';
title1 = 'Rates from Problem P1 that change with \alpha';
title2a = '      Forward rates from Problem P2 that change with \alpha';
title2b = '      Reverse rates from Problem P2 that change with \alpha';
loc = 'EastOutside';
alpha_vec = 0.01:0.01:0.99;
c1 = 'b';  c2 = 'r';  c3 = 'k';  c4 = 'g';
c5 = 'm';  c6 = 'b--';  c7 = 'r--';  c8 = 'k--'; c9 = 'g--'; c10 = 'm--';
c11 = 'c--'; c12 = 'y--';
fsize = 11;
lw = 1;

figure;
hold on
set(gca,'Fontsize',fsize)
plot(alpha_vec,r1(:,4),c1,'Linewidth',lw)
plot(alpha_vec,r1(:,8),c3,'Linewidth',lw)
plot(alpha_vec,r1(:,6),c2,'Linewidth',lw)
plot(alpha_vec,r1(:,12),c4,'Linewidth',lw)
plot(alpha_vec,r1(:,14),c5,'Linewidth',lw)
plot(alpha_vec,r1(:,16),c6,'Linewidth',lw)
plot(alpha_vec,r1(:,18),c7,'Linewidth',lw)
plot(alpha_vec,r1(:,20),c8,'Linewidth',lw)
legend('k_2^-','k_3^-','k_4^-','k_6^-','k_7^-','k_8^-','k_9^-','k_{10}^-','Location',loc);
xlabel(xlab)
ylabel(ylab)
title(title1);

figure;
hold on
set(gca,'Fontsize',fsize)
plot(alpha_vec,r2(:,1),c1,'Linewidth',lw)
plot(alpha_vec,r2(:,3),c2,'Linewidth',lw)
plot(alpha_vec,r2(:,5),c3,'Linewidth',lw)
plot(alpha_vec,r2(:,9),c9,'Linewidth',lw)
plot(alpha_vec,r2(:,13),c5,'Linewidth',lw)
plot(alpha_vec,r2(:,15),c7,'Linewidth',lw)
plot(alpha_vec,r2(:,19),c8,'Linewidth',lw)
legend('k_1^+','k_2^+','k_4^+','k_5^+','k_7^+','k_8^+','k_{10}^+','Location',loc);
xlabel(xlab)
ylabel(ylab)
title(title2a);

figure;
hold on
set(gca,'Fontsize',fsize)
plot(alpha_vec,r2(:,2),c1,'Linewidth',lw)
plot(alpha_vec,r2(:,4),c10,'Linewidth',lw)
plot(alpha_vec,r2(:,8),c3,'Linewidth',lw)
plot(alpha_vec,r2(:,6),c2,'Linewidth',lw)
plot(alpha_vec,r2(:,10),c11,'Linewidth',lw)
plot(alpha_vec,r2(:,12),c12,'Linewidth',lw)
plot(alpha_vec,r2(:,14),c7,'Linewidth',lw)
plot(alpha_vec,r2(:,16),c4,'Linewidth',lw)
plot(alpha_vec,r2(:,18),c9,'Linewidth',lw)
plot(alpha_vec,r2(:,20),c5,'Linewidth',lw)
legend('k_1^-','k_2^-','k_3^-','k_4^-','k_5^-','k_6^-','k_7^-','k_8^-','k_9^-','k_{10}^-',...
        'Location',loc);
xlabel(xlab)
ylabel(ylab)
title(title2b);

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
          = optAssemblyRates_exp_plot(alpha(i),0,1,1);  % Program P1    
    [rates, xd, t2, X2] ...
          = optAssemblyRates_exp_plot(alpha(i),0,1,2);  % Program P2
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    plot(t1,X1(:,9)/(xd(9)+xd(10)),c1,'Linewidth',lw);
    plot(t1,X1(:,10)/(xd(9)+xd(10)),c2,'Linewidth',lw);
    plot(t2,X2(:,9)/(xd(9)+xd(10)),c3,'Linewidth',lw);
    plot(t2,X2(:,10)/(xd(9)+xd(10)),c4,'Linewidth',lw);
    plot([t1(1) t1(end)],[xd(9)/(xd(9)+xd(10)) xd(9)/(xd(9)+xd(10))],c5);
    plot([t1(1) t1(end)],[xd(10)/(xd(9)+xd(10)) xd(10)/(xd(9)+xd(10))],c6);
    legend(leg1,leg2,leg3,leg4,'Location','East')
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
c1 = 'b'; c2 = 'r'; c3 = 'k'; c4 = 'g'; c5 = 'm'; c6 = 'b--';
c7 =  'r--'; c8 = 'k--'; c9 = 'g--'; c10 = 'm--';
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
leg7 = 'R_7';
leg8 = 'R_8';
leg9 = 'R_9';
leg10 = 'R_{10}';
fsize = 11;
lw = 1;

for i = 1:length(alpha)
    [rates, xd, t1, X1] ...
          = optAssemblyRates_exp_plot(alpha(i),1,1,1);  % Program P1          
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    semilogx(t1,rates(1)*X1(:,1).*X1(:,2) - rates(2)*X1(:,5),c1,'Linewidth',lw);
    semilogx(t1,rates(3)*X1(:,3).*X1(:,4) - rates(4)*X1(:,6),c2,'Linewidth',lw);
    semilogx(t1,rates(7)*X1(:,5).*X1(:,6) - rates(8)*X1(:,7),c3,'Linewidth',lw);
    semilogx(t1,rates(5)*X1(:,2).*X1(:,7) - rates(6)*X1(:,9),c4,'Linewidth',lw);
    semilogx(t1,rates(9)*X1(:,2).*X1(:,5) - rates(10)*X1(:,8),c5,'Linewidth',lw);
    semilogx(t1,rates(11)*X1(:,6).*X1(:,8) - rates(12)*X1(:,10),c6,'Linewidth',lw); 
    semilogx(t1,rates(13)*X1(:,3).*X1(:,5) - rates(14)*X1(:,11),c7,'Linewidth',lw); 
    semilogx(t1,rates(15)*X1(:,4).*X1(:,11) - rates(16)*X1(:,7),c8,'Linewidth',lw);
    semilogx(t1,rates(17)*X1(:,3).*X1(:,8) - rates(18)*X1(:,12),c9,'Linewidth',lw); 
    semilogx(t1,rates(19)*X1(:,4).*X1(:,12) - rates(20)*X1(:,10),c10,'Linewidth',lw); 
    legend(leg1,leg2,leg3,leg4,leg5,leg6,leg7,leg8,leg9,leg10)
    xlabel(xlab)
    ylabel(ylab)
    title([title1 num2str(alpha(i))])   
    
    [rates, xd, t2, X2] ...
          = optAssemblyRates_exp_plot(alpha(i),1,1,2);  % Program P2
    figure;
    hold on;
    set(gca,'Fontsize',fsize)
    semilogx(t2,rates(1)*X2(:,1).*X2(:,2) - rates(2)*X2(:,5),c1,'Linewidth',lw);
    semilogx(t2,rates(3)*X2(:,3).*X2(:,4) - rates(4)*X2(:,6),c2,'Linewidth',lw);
    semilogx(t2,rates(7)*X2(:,5).*X2(:,6) - rates(8)*X2(:,7),c3,'Linewidth',lw);
    semilogx(t2,rates(5)*X2(:,2).*X2(:,7) - rates(6)*X2(:,9),c4,'Linewidth',lw);
    semilogx(t2,rates(9)*X2(:,2).*X2(:,5) - rates(10)*X2(:,8),c5,'Linewidth',lw);
    semilogx(t2,rates(11)*X2(:,6).*X2(:,8) - rates(12)*X2(:,10),c6,'Linewidth',lw); 
    semilogx(t2,rates(13)*X2(:,3).*X2(:,5) - rates(14)*X2(:,11),c7,'Linewidth',lw);     
    semilogx(t2,rates(15)*X2(:,4).*X2(:,11) - rates(16)*X2(:,7),c8,'Linewidth',lw); 
    semilogx(t2,rates(17)*X2(:,3).*X2(:,8) - rates(18)*X2(:,12),c9,'Linewidth',lw); 
    semilogx(t2,rates(19)*X2(:,4).*X2(:,12) - rates(20)*X2(:,10),c10,'Linewidth',lw); 
    legend(leg1,leg2,leg3,leg4,leg5,leg6,leg7,leg8,leg9,leg10)
    xlabel(xlab)
    ylabel(ylab)
    title([title2 num2str(alpha(i))])
end

