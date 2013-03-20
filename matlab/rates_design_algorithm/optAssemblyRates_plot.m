% Optimization of rates for assembly task 
  
function [AssemRates, xd, t, X] ...
          = optAssemblyRates_plot(alphaInput,getRates,simSystem,optType)


% alphaInput: 0 to run a range of alpha, >0 and <1 for a particular alpha
% getRates: 1 if want to obtain rates
% simSystem: 1 to simulate the system for a particular alpha
% optType: 1 for Problem P1, 2 for Problem P2

AssemRates = [];

if (alphaInput==0)  % run for whole range of alpha
    delta = 0.01;
    for alpha = delta:delta:1-delta
        alpha   % F1/F2
        [rates, xd, t, X] = runOpt(alpha,simSystem,optType);
        if (getRates==1)
            AssemRates(end+1,1:length(rates)) = rates;
        end
    end
else
    alpha = alphaInput;
    [rates, xd, t, X] = runOpt(alpha,simSystem,optType);
    if (getRates==1)
        AssemRates(end+1,1:length(rates)) = rates;
    end
end

%----------------%

function [rates, xd, t, X] = runOpt(alpha,simSystem,optType)

test_case = 2;  % 1 = 1 puzzle; 2 = 3 puzzles

switch(test_case),
    case 1,
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        x0 = [1 2 1 1 zeros(1,6)];
	case 2,
        arena_radius = 2.8;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        s = 20;
        x0 = s*[3 6 3 3 zeros(1,6)];
end;

% Initial parameters, using geometric probabilities
arena_size = 3*arena_radius^2*sqrt(3)/2;
timestep = 1.0;
% probability of encountering a piece
p_encounter = robot_speed*timestep*2*piece_comm_range/arena_size;
% probability of two robots encountering each other
p_encounter_robot = robot_speed*timestep*2*robot_comm_range/arena_size;

% probabilities of assembling pieces
p_assemble_5 = sdpvar(1,1);
p_assemble_6 = sdpvar(1,1);
p_assemble_7 = sdpvar(1,1);
p_assemble_8 = sdpvar(1,1);
p_assemble_f1 = sdpvar(1,1);
p_assemble_f2 = sdpvar(1,1);
% probability of breaking pieces apart
p_break_5 = sdpvar(1,1);
p_break_6 = sdpvar(1,1);
p_break_7 = sdpvar(1,1);
p_break_8 = sdpvar(1,1);
p_break_f1 = sdpvar(1,1);
p_break_f2 = sdpvar(1,1);

S = sdpvar(1,1);

k1 = p_encounter_robot*p_assemble_5;  % 1 + 2 -> 5
k2 = p_break_5;                                     % 5 -> 1 + 2 
k3 = p_encounter_robot*p_assemble_6;  % 3 + 4 -> 6
k4 = p_break_6;                                     % 6 -> 3 + 4
k5 = p_encounter_robot*p_assemble_f1;   % 7 + 2 -> f1
k6 = p_break_f1;                                    % f1 -> 7 + 2
k7 = p_encounter_robot*p_assemble_7;                % 5 + 6 -> 7
k8 = p_break_7;                                     % 7 -> 5 + 6
k9 = p_encounter_robot*p_assemble_8;    % 5 + 2 -> 8
k10 = p_break_8;                                    % 8 -> 5 + 2
k11 = p_encounter_robot*p_assemble_f2;              % 8 + 6 -> f2
k12 = p_break_f2;                                   % f2 -> 8 + 6

K = [  -k1,   k2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0; ...
        k1,  -k2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0; ...
        0,    0,  -k3,   k4,    0,    0,    0,    0,    0,    0,    0,    0; ...
        0,    0,   k3,  -k4,    0,    0,    0,    0,    0,    0,    0,    0; ...
        0,    0,    0,    0,  -k5,   k6,    0,    0,    0,    0,    0,    0; ...
        0,    0,    0,    0,   k5,  -k6,    0,    0,    0,    0,    0,    0; ...
        0,    0,    0,    0,    0,    0,  -k7,   k8,    0,    0,    0,    0; ...
        0,    0,    0,    0,    0,    0,   k7,  -k8,    0,    0,    0,    0; ...
        0,    0,    0,    0,    0,    0,    0,    0,  -k9,  k10,    0,    0; ...
        0,    0,    0,    0,    0,    0,    0,    0,   k9, -k10,    0,    0; ...
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0, -k11,  k12; ...
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  k11, -k12];
    
Y = [1     0     0     0     0     0     0     0     0     0     0     0; ...
     1     0     0     0     1     0     0     0     1     0     0     0; ...
     0     0     1     0     0     0     0     0     0     0     0     0; ...
     0     0     1     0     0     0     0     0     0     0     0     0; ...
     0     1     0     0     0     0     1     0     1     0     0     0; ...
     0     0     0     1     0     0     1     0     0     0     1     0; ...
     0     0     0     0     1     0     0     1     0     0     0     0; ...
     0     0     0     0     0     0     0     0     0     1     1     0; ...
     0     0     0     0     0     1     0     0     0     0     0     0; ...
     0     0     0     0     0     0     0     0     0     0     0     1];

% Conservation laws:
% x3=x4
% x1+x5+x7+x8+xf1+xf2=3
% x2+x5+x7+2*(x8+xf1+xf2)=6
% x3+x6+x7+xf1+xf2=3

% units are number of pieces
% x1 = 1; % independent
% x2 = 4; % independent
% x3 = 1; % independent
% x5 = 1; % independent
% x4 = x3;
% x7 = x2-2*x1-x5;
% xf1 = (x0(1)-x1-x5-x7-1)*0.9; % independent
% xf2 = (x0(1)-x1-x5-x7-1)*0.1; % independent
% x8 = x0(1)-x1-x5-x7-xf1-xf2
% x6 = x0(3)-x3-x7-xf1-xf2

% In expanded system (alpha = 0.4):
% x = [0.5000    2.5000    1.0000    2.0000    0.5000    0.5000    0.5000    0.5000
%      22.8000   34.2000    0.5000    0.5000]
% In this system (alpha = 0.4):
%  x = [0.5000    2.5000    1.0000    1.0000    0.5000    1.0000    1.0000    1.0000
%       22.8000   34.2000]

x1 = 0.5; % independent
x2 = 2.5; % independent
x3 = 1; % independent
x5 = 0.5; % independent
x4 = x3;
x7 = x2-2*x1-x5;
xf1 = (x0(1)-x1-x5-x7-1)*alpha; % independent
xf2 = (x0(1)-x1-x5-x7-1)*(1-alpha); % independent
x8 = x0(1)-x1-x5-x7-xf1-xf2;
x6 = x0(3)-x3-x7-xf1-xf2;
xf1+xf2;

xd = [x1 x2 x3 x4 x5 x6 x7 x8 xf1 xf2];

y = [x1*x2; x5; x3*x4; x6; x2*x7; xf1; x5*x6; x7; x2*x5; x8; x6*x8; xf2];

%h = -min([k1+k2 k3+k4 k5+k6 k7+k8 k9+k10 k11+k12]);  % asymptotic rate of
                                                      % convergence
%h = -(k1+k2+k3+k4+k5+k6+k7+k8+k9+k10+k11+k12);  % sum of all eigenvalues
%h = -(k1-k2+k3-k4+k5-k6+k7-k8+k9-k10+k11-k12);  % sum of all eigenvalues
%h = -(k1+k3+k5+k7+k9+k11);
%h =  -S;                  % maximize the norm of Y*K (doesn't work)

% sum of denominators of characteristic times for each reaction
if (optType==1)
    h = -(k1*(x2+x1)+k2 + k3*(x3+x4)+k4 + k5*(x2+x7)+k6 + k7*(x5+x6)+k8 ...
        + k9*(x2+x5)+k10 + k11*(x6+x8)+k12);  
elseif (optType==2)
    h = -min([k1*(x2+x1)+k2 k3*(x3+x4)+k4 k5*(x2+x7)+k6 k7*(x5+x6)+k8 ...
          k9*(x2+x5)+k10 k11*(x6+x8)+k12]);
end

F = set(p_assemble_5 >= 0) + set(p_assemble_5 <= 1) + ...
    set(p_assemble_6 >= 0) + set(p_assemble_6 <= 1) + ...
    set(p_assemble_7 >= 0) + set(p_assemble_7 <= 1) + ...
    set(p_assemble_8 >= 0) + set(p_assemble_8 <= 1) + ...
    set(p_assemble_f1 >= 0) + set(p_assemble_f1 <= 1) + ...
    set(p_assemble_f2 >= 0) + set(p_assemble_f2 <= 1) + ...
    set(p_break_5 >= 0) + set(p_break_5 <= 1) + ...
    set(p_break_6 >= 0) + set(p_break_6 <= 1) + ...
    set(p_break_7 >= 0) + set(p_break_7 <= 1) + ...
    set(p_break_8 >= 0) + set(p_break_8 <= 1) + ...
    set(p_break_f1 >= 0) + set(p_break_f1 <= 1) + ...
    set(p_break_f2 >= 0) + set(p_break_f2 <= 1) + ...
    set(Y*K*y == 0); % + ... 
    %set((Y*K).'*(Y*K) <= -S) + set(S > 0);  % maximize the norm of Y*K

solvesdp(F,h)

%--- Output ---%

[double(p_assemble_5) double(p_assemble_6) double(p_assemble_7)...
 double(p_assemble_8) double(p_assemble_f1) double(p_assemble_f2)...
 double(p_break_5) double(p_break_6) double(p_break_7)...
 double(p_break_8) double(p_break_f1) double(p_break_f2)]

%K = double(K);

k1 = double(k1); k2 = double(k2); k3 = double(k3); k4 = double(k4);
k5 = double(k5); k6 = double(k6); k7 = double(k7); k8 = double(k8);
k9 = double(k9); k10 = double(k10); k11 = double(k11); k12 = double(k12);

rates = [k1 k2 k3 k4 k5 k6 k7 k8 k9 k10 k11 k12];
    
if (simSystem==1)
    T0 = 0;
    T1 = 50000;
    [t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0, [], rates);
    plot(t,X);
    legend({'X_1', 'X_2', 'X_3', 'X_4', 'X_5', 'X_6', 'X_7', 'X_8', 'X_{F1}', 'X_{F2}'}, 'Location', 'EastOutside', 'Orientation', 'vertical');
else
    t = 0; X = 0;
end

