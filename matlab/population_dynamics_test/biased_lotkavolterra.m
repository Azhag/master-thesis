%function [t x] = biased_lotkavolterra(Tmax, x0)
% Call example : > biased_lotkavolterra(1, [5000 1000 0])
% Tmax : maximum time of running
% x0 : initial points. [V, P, S]
% Loic Matthey

clear;
Tmax = 10;
%x0 = [3000 100 0];
x0 = [15 0 0];

t(1) = 0;
V(1) = x0(1);
P(1) = x0(2);
S(1) = x0(3);

% Vbirth = 500;
% VeatenP = 1;
% Pdeath = 50;
Vcap = 20; 
Vbirth = 10;
VeatenP = 0.0;
Pdeath = 0.0;


VeatenS = 0.1;
Sbirth = 0.0;

i=2;
while(t < Tmax),
    rate = abs([
        Vbirth*(Vcap-V(i-1))/Vcap;
        VeatenP*P(i-1)*V(i-1);
        VeatenS*S(i-1)*V(i-1);
        Pdeath*P(i-1);
        Sbirth]);
    
    eff = [ sign(Vbirth*(Vcap-V(i-1))/Vcap) 0 0; % Reaction 1, effect on V, P, S
        -1 1 0;
        -1 0 1;
        0 -1 0;
        0 1 0;
        0 0 1];
    
    sumrate = sum(rate);
    t(i) = t(i-1) - log(rand(1))/sumrate;
    
    if (t(i) < Tmax),
        v= rand(1);
        for j=1:size(rate,1),
            if (v < sum(rate(1:j))/sumrate),
                % Reaction j occured !
                %j
                V(i) = V(i-1) + eff(j,1);
                P(i) = max(P(i-1) + eff(j,2),0);
                S(i) = S(i-1) + eff(j,3);
                break;
            end;
        end;
    end;
    i = i+1;
end;
t = t(1:size(t,2)-1);

figure();
plot(t, V, t, P, t, S);
title(['Biased LotkaVolterra, tmax = ' num2str(Tmax) ' , T0 = ' num2str(x0(1)) ' , I0 = ' num2str(x0(2)) ', E0 = ' num2str(x0(3))]);
xlabel('Time');
ylabel('Population');
legends = {'Vegeterian', 'Predator', 'Super predator'};
legend(legends);
x = [V ;P; S];