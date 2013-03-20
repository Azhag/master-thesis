[tSim XSim] = ode45(@puzzle_ode_norobot, [0, Time], Xinit, [], Kvec);
EI = size(XSim,1);
YSim = [XSim(EI,1)*XSim(EI,2); XSim(EI,5); XSim(EI,3)*XSim(EI,4); XSim(EI,6); ...
        XSim(EI,2)*XSim(EI,7); XSim(EI,9); XSim(EI,5)*XSim(EI,6); XSim(EI,7); ...
        XSim(EI,2)*XSim(EI,5); XSim(EI,8); XSim(EI,6)*XSim(EI,8); XSim(EI,10)];  % x(9) = xf1; x(10) = xf2
%deficit of occupancy
Distance = norm(YSim - Ydes,2);
