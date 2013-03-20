% Optimization of rates for assembly task 
% includes more reactions
  
test_case = 2;  % 1 = 1 puzzle; 2 = 3 puzzles

switch(test_case),
    case 1,
        arena_radius = 1.85;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        x0 = [1 2 1 1 zeros(1,8)];
	case 2,
        arena_radius = 2.8;
        piece_comm_range = 0.35;
        robot_comm_range = 0.6;
        robot_speed = 0.128;
        s = 20;
        x0 = s*[3 6 3 3 zeros(1,8)];
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
     % extra
p_assemble_11 = sdpvar(1,1);
p_assemble_12 = sdpvar(1,1);
p_assemble_7_b = sdpvar(1,1);
p_assemble_f2_b = sdpvar(1,1);

% probability of breaking pieces apart
p_break_5 = sdpvar(1,1);
p_break_6 = sdpvar(1,1);
p_break_7 = sdpvar(1,1);  % breaks into 5 and 6
p_break_8 = sdpvar(1,1);
p_break_f1 = sdpvar(1,1);
p_break_f2 = sdpvar(1,1); % breaks into 6 and 8
     % extra
p_break_11 = sdpvar(1,1);
p_break_12 = sdpvar(1,1);
p_break_7_b = sdpvar(1,1);  % breaks into 4 and 11
p_break_f2_b = sdpvar(1,1); % breaks into 4 and 12

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
% extra
k13 = p_encounter_robot*p_assemble_11;  
k14 = p_break_11;
k15 = p_encounter_robot*p_assemble_7_b;
k16 = p_break_7_b;
k17 = p_encounter_robot*p_assemble_12;
k18 = p_break_12;
k19 = p_encounter_robot*p_assemble_f2_b;
k20 = p_break_f2_b;

K = [  -k1   k2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 0 0 0 0 0 0; ...
        k1  -k2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 0 0 0 0 0 0; ...
        0,    0,  -k3,   k4,    0,    0,    0,    0,    0,    0,    0,    0  0 0 0 0 0 0; ...
        0,    0,   k3,  -k4,    0,    0,    0,    0,    0,    0,    0,    0 0 0 0 0 0 0; ...
        0,    0,    0,    0,  -k5,   k6,    0,    0,    0,    0,    0,    0 0 0 0 0 0 0; ...
        0,   0,    0,    0,   k5,  -k6,    0,    0,    0,    0,    0,    0 0 0 0 0 0 0; ...
        0,    0,    0,    0,    0,    0,  -k7,   k8,    0,    0,    0,    0 0 0 0 0 0 0; ...
        0,    0,    0,    0,    0,    0,   k7,  -k8-k16,    0,    0,    0,    0 0 0 k15 0 0 0; ...
        0,    0,    0,    0,    0,    0,    0,    0,  -k9,  k10,    0,    0 0 0 0 0 0 0; ...
        0,    0,    0,    0,    0,    0,    0,    0,   k9, -k10,    0,    0 0 0 0 0 0 0; ...
        0    0,    0,    0,    0,    0,    0,    0,    0,    0, -k11,  k12 0 0 0 0 0 0; ...
        0    0,    0,    0,    0,    0,    0,    0,    0,    0,  k11, -k12-k20 0 0 0 0 0 k19; ...
        0 0 0 0 0 0 0 0 0 0 0 0 -k13 k14 0 0 0 0; ...
        0 0 0 0 0 0 0 0 0 0 0 0 k13 -k14 0 0 0 0; ...
        0 0 0 0 0 0 0 k16 0 0 0 0 0 0 -k15 0 0 0; ...
        0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -k17 k18 0; ...
        0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 k17 -k18 0; ...
        0 0 0 0 0 0 0 0 0 0 0 k20 0 0 0 0 0 -k19];
    
Y = [1     0     0     0     0     0     0     0     0     0     0     0; ...
     1     0     0     0     1     0     0     0     1     0     0     0; ...
     0     0     1     0     0     0     0     0     0     0     0     0; ...
     0     0     1     0     0     0     0     0     0     0     0     0; ...
     0     1     0     0     0     0     1     0     1     0     0     0; ...
     0     0     0     1     0     0     1     0     0     0     1     0; ...
     0     0     0     0     1     0     0     1     0     0     0     0; ...
     0     0     0     0     0     0     0     0     0     1     1     0; ...
     0     0     0     0     0     1     0     0     0     0     0     0; ...
     0     0     0     0     0     0     0     0     0     0     0     1; ...
     0     0     0     0     0     0     0     0     0     0     0     0; ...
     0     0     0     0     0     0     0     0     0     0     0     0];
 
Y(1:12,13:18) = [[0 0 1 0 1 0 0 0 0 0 0 0]' [zeros(1,10) 1 0]' [0 0 0 1 0 0 0 0 0 0 1 0]'...
              [0 0 1 0 0 0 0 1 0 0 0 0]' [zeros(1,11) 1]'...
              [0 0 0 1 0 0 0 0 0 0 0 1]'];

% stoichiometric matrix
St = [-1     0     0     0     0     0     0     0     0     0;...
    -1     0    -1     0    -1     0     0     0     0     0;...
     0    -1     0     0     0     0    -1     0    -1     0;...
     0    -1     0     0     0     0     0    -1     0    -1;...
     1     0     0    -1    -1     0    -1     0     0     0;...
     0     1     0    -1     0    -1     0     0     0     0;...
     0     0    -1     1     0     0     0     1     0     0;...
     0     0     0     0     1    -1     0     0    -1     0;...
     0     0     1     0     0     0     0     0     0     0;...
     0     0     0     0     0     1     0     0     0     1;...
     0     0     0     0     0     0     1    -1     0     0;...
     0     0     0     0     0     0     0     0     1    -1];
 
v = null(St');  % conservation laws
v1 = v(:,1);
v2 = v(:,2);
v3 = v(:,3);
v4 = v(:,4);
% dot(vi,x) = constant
N1 = dot(v1,x0);
N2 = dot(v2,x0);
N3 = dot(v3,x0);
N4 = dot(v4,x0);

% units are number of pieces
% 12 variables, 4 conservation laws
% Conservation laws:
% x4 + x6 + x7 + xf1 + xf2 = x4(0)
% x1+x5+x7+x8+x11+x12+xf1+xf2 = x1(0)
% x2+x5+x7+2*x8+x11+2*x12+2*xf1+2*xf2 = x2(0)
% x3+x6+x7+x11+x12+xf1+xf2 = x3(0)

TargetRatio = 30; %F1/F2
alpha = TargetRatio/(1+TargetRatio)
x5 = .5; 
x6 = .5;
x7 = .5;
x8 = .5;
x11 = .5;
x12 = .5;
xf1 = 0.5*(x0(2)-(2+x5+x7+2*x8+x11+2*x12))*alpha;  % x2=2
xf2 = 0.5*(x0(2)-(2+x5+x7+2*x8+x11+2*x12))*(1-alpha);
xf1+xf2

x1 = x0(1)-(x5+x7+x8+x11+x12+xf1+xf2)
x2 = x0(2)-(x5+x7+2*x8+x11+2*x12+2*xf1+2*xf2)
x3 = x0(3)-(x6+x7+x11+x12+xf1+xf2)
x4 = x0(4)-(x6+x7+xf1+xf2)

% xf1 = 20;
% xf2 = xf1/TargetRatio;

%--- Yields all positive equilibrium piece numbers ---%
% x1 = 1; x2 = 5; x3 = 1; x4 = 4.5; x5 = 1; x6 = 2; 
% xf1 = 20.2; % independent (=x9)
% xf2 = 2.8; % independent (=x10)
%-----------------------------------------------------%

% A = [v1(7) v1(8) v1(11) v1(12);...
%      v2(7) v2(8) v2(11) v2(12);...
%      v3(7) v3(8) v3(11) v3(12);...
%      v4(7) v4(8) v4(11) v4(12)];
% b = [N1-v1(1)*x1-v1(2)*x2-v1(3)*x3-v1(4)*x4-v1(5)*x5-v1(6)*x6-v1(9)*xf1-v1(10)*xf2;...
%      N2-v2(1)*x1-v2(2)*x2-v2(3)*x3-v2(4)*x4-v2(5)*x5-v2(6)*x6-v2(9)*xf1-v2(10)*xf2;...
%      N3-v3(1)*x1-v3(2)*x2-v3(3)*x3-v3(4)*x4-v3(5)*x5-v3(6)*x6-v3(9)*xf1-v3(10)*xf2;...
%      N4-v4(1)*x1-v4(2)*x2-v4(3)*x3-v4(4)*x4-v4(5)*x5-v4(6)*x6-v4(9)*xf1-v4(10)*xf2];
% xvec = inv(A)*b;
% x7 = xvec(1)
% x8 = xvec(2)
% x11 = xvec(3)
% x12 = xvec(4)

y = [x1*x2; x5; x3*x4; x6; x2*x7; xf1; x5*x6; x7; x2*x5; x8; x6*x8; xf2;
     x3*x5; x11; x4*x11; x3*x8; x12; x4*x12];

%h = -min([k1+k2 k3+k4 k5+k6 k7+k8 k9+k10 k11+k12]);  % asymptotic rate of
                                                      % convergence
%h = -(k1+k2+k3+k4+k5+k6+k7+k8+k9+k10+k11+k12);  % sum of all eigenvalues
%h = -(k1-k2+k3-k4+k5-k6+k7-k8+k9-k10+k11-k12);  % sum of all eigenvalues
%h = -(k1+k3+k5+k7+k9+k11);
%h =  -S;                  % maximize the norm of Y*K (doesn't work)

% sum of denominators of characteristic times for each reaction
h = -(k1*(x2+x1)+k2 + k3*(x3+x4)+k4 + k5*(x2+x7)+k6 + k7*(x5+x6)+k8 ...
     + k9*(x2+x5)+k10 + k11*(x6+x8)+k12 ...
     + k17*(x3+x8)+k18 + k13*(x3+x5)+k14 + k15*(x4+x11)+k16 + k19*(x4+x12)+k20);  
%h = -min([k1*(x2+x1)+k2 k3*(x3+x4)+k4 k5*(x2+x7)+k6 k7*(x5+x6)+k8 ...
%          k9*(x2+x5)+k10 k11*(x6+x8)+k12]);  % slower convergence

F = set(p_assemble_5 >= 0) + set(p_assemble_5 <= 1) + ...
    set(p_assemble_6 >= 0) + set(p_assemble_6 <= 1) + ...
    set(p_assemble_7 >= 0) + set(p_assemble_7 <= 1) + ...
    set(p_assemble_8 >= 0) + set(p_assemble_8 <= 1) + ...
    set(p_assemble_f1 >= 0) + set(p_assemble_f1 <= 1) + ...
    set(p_assemble_f2 >= 0) + set(p_assemble_f2 <= 1) + ...
    set(p_assemble_11 >= 0) + set(p_assemble_11 <= 1) + ...
    set(p_assemble_12 >= 0) + set(p_assemble_12 <= 1) + ...
    set(p_assemble_7_b >= 0) + set(p_assemble_7_b <= 1) + ...
    set(p_assemble_f2_b >= 0) + set(p_assemble_f2_b <= 1) + ...    
    set(p_break_5 >= 0) + set(p_break_5 <= 1) + ...
    set(p_break_6 >= 0) + set(p_break_6 <= 1) + ...
    set(p_break_7 >= 0) + set(p_break_7 <= 1) + ...
    set(p_break_8 >= 0) + set(p_break_8 <= 1) + ...
    set(p_break_f1 >= 0) + set(p_break_f1 <= 1) + ...
    set(p_break_f2 >= 0) + set(p_break_f2 <= 1) + ...
    set(p_break_11 >= 0) + set(p_break_11 <= 1) + ...
    set(p_break_12 >= 0) + set(p_break_12 <= 1) + ...
    set(p_break_7_b >= 0) + set(p_break_7_b <= 1) + ...
    set(p_break_f2_b >= 0) + set(p_break_f2_b <= 1) + ...
    set(Y*K*y == 0); % + ... 
    %set((Y*K).'*(Y*K) <= -S) + set(S > 0);  % maximize the norm of Y*K

solvesdp(F,h)

%--- Output ---%

[double(p_assemble_5) double(p_assemble_6) double(p_assemble_7)...
 double(p_assemble_8) double(p_assemble_f1) double(p_assemble_f2)...
 double(p_assemble_11) double(p_assemble_12) double(p_assemble_7_b)...
 double(p_assemble_f2_b)...
 double(p_break_5) double(p_break_6) double(p_break_7)...
 double(p_break_8) double(p_break_f1) double(p_break_f2)...
 double(p_break_11) double(p_break_12) double(p_break_7_b)...
 double(p_break_f2_b)]

K = double(K);

k1 = double(k1);
k2 = double(k2);
k3 = double(k3);
k4 = double(k4);
k5 = double(k5);
k6 = double(k6);
k7 = double(k7);
k8 = double(k8);
k9 = double(k9);
k10 = double(k10);
k11 = double(k11);
k12 = double(k12);
k13 = double(k13);
k14 = double(k14);
k15 = double(k15);
k16 = double(k16);
k17 = double(k17);
k18 = double(k18);
k19 = double(k19);
k20 = double(k20);

rates = [k1 k2 k3 k4 k5 k6 k7 k8 k9 k10 k11 k12 k13 k14 k15 k16 k17 k18 k19 k20];

% Simulate the system
T0 = 0;
T1 = 70000;
[t X2] = ode45(@puzzle_ode_norobot_exp, [T0, T1], x0, [], rates);
size(X2)
figure
hold on
plot(t,X2(:,9));
plot(t,X2(:,10),'r');

attained_ratio = X2(end,10)/(X2(end,10)+X2(end,9))
goal_ratio = xf2/(xf1+xf2)

title(['Ratio attained:' num2str(attained_ratio) ', goal:' num2str(goal_ratio)]);

%-----%

% outflux = X2(:,end-1:end).*repmat(rates([6 12]), size(X2,1),1)
% influx = [X2(:,2).*X2(:,7)*rates(5) X2(:,8).*X2(:,6)*rates(11)]
% figure(); plot([influx outflux X2(:,9)/x0(1) X2(:,10)/x0(1)])
% 
% figure;
% plot(t,-k1.*X2(:,1).*X2(:,2)+k2.*X2(:,5))

% Desired number of each piece (continuous)
% heuristic initialization
%rates = [ones(1,12)*p_encounter_robot]
%rates(2*linspace(1,6,6)) = 0.5*rates(2*linspace(1,6,6))

%T0 = 0;
%T1 = 5000;
%[t X] = ode45(@puzzle_ode_norobot, [T0, T1], x0, [], rates);
% 
% x1 = X(end,1);
% x2 = X(end,2);
% x3 = X(end,3);
% x4 = X(end,4);
% x5 = X(end,5);
% x6 = X(end,6);
% x7 = X(end,7);
% x8 = X(end,8);
