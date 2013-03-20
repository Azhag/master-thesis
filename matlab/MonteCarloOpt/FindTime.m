TimeMin=0;
TimeMax=1;
DeltaT=0.001;
% if the system converges to Ydes then it converges to Xdes
% (see discussion in Loic's masters thesis, pp. 53-54)
NormD = norm(Yinit - Ydes,2);  

Bingo=0;
while(Bingo==0)  % added Time part
    Time = TimeMax;
    Evolve;  % run the system
    if(Distance/NormD < TargetDistance) Bingo = 1;
    else TimeMax = TimeMax * 2;   % was TimeMax * 2
    end;    
end;

while(TimeMax - TimeMin > DeltaT)
    Time = (TimeMin + TimeMax)/2.;
    Evolve;
    if( Distance/NormD > TargetDistance) TimeMin = Time;
    else TimeMax=Time;
    end;
end;

