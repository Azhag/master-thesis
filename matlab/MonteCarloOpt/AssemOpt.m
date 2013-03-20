% Monte Carlo optimization program to minimize the time to convergence
% of the simple assembly model (10 piece types, 12 complexes)
% subject to Y*K*y_target = 0 and 0 <= p <= 1, where p are variable
% probabilities
% by Spring Berman, September 2008
% Modified from code by Adam Halasz

clear;
tic

% generate an initial distribution
InitConds;

% Generates a K matrix for a set of desired occupancies %
load KvecStart2  % specific to each alpha

TargetDistance=0.1;
FindTime;%this is the CORE calculation where you compute the time for convergence to 0.1 distance or whatever

%these will hold the consecutive relevant values
Times = [Time];
BestTimes = [Time];
BestTime = Time;
BestKvec = Kvec;

AcceptanceRate=0;
accepted=0;
trials=0;

Temp=0.01;  % was 0.01  ADJUST 10
StepSize = 0.0001;   % was 0.0001  0.000001   ADJUST

for(iMM = 1:1000); 
 toc
 fprintf(1,'Steps: %8d  Tcur=%8.3f Tmin=%8.3f Ar=%7.4f ( %d / %d ) T=%7.4f Step=%10.6f\n', (iMM-1)*100, Time, BestTime, AcceptanceRate, accepted, trials, Temp, StepSize)
 %fprintf(1,'Steps: %8d  Dcur=%9.6f Dmax=%9.6f Ar=%7.4f ( %d / %d ) T=%7.4f Step=%10.6f\n', iMM*100, Dett, BestDett, AcceptanceRate, accepted, trials, Temp, StepSize)
 %figure(1);
 %PlotK
 trials=0;
 accepted=0;
for(iM=1:100)
    iM
 %fprintf(1,'s=%d ',100*(iMM-1)+iM);

accept=0;
while(accept==0)
    %fprintf(1,'t\n');
    trials = trials+1;
    GetNewK;
    FindTime;
    %fprintf(1,'T=%f\n',Time);
    %Metropolis step: see if we accept this new K
    if (exp(-(Time - oldTime)/Temp) > rand(1,1)) 
    %if(exp( (Dett - oldDett) /Temp) > rand(1,1)) 
        accept=1;
        accepted = accepted+1;
        %fprintf(1,'..yes\n');
    else
        % restore the old K and its derivatives
        Kvec = oldKvec;
        Time = oldTime;
    end;
end;

%disp([iMM iM Time trials])

if(Time < BestTime)
    BestTime = Time;
    BestKvec = Kvec;
end;

Times = [Times Time];
BestTimes = [BestTimes BestTime];

end;

AcceptanceRate=accepted/trials;

if(AcceptanceRate > 0.55) 
    StepSize = StepSize * 1.05;
end;

if(AcceptanceRate < 0.45)
   StepSize = StepSize * 0.95;
end;

end;

