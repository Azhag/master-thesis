done=0;
oldKvec = Kvec;
oldTime = Time;

% apply constraints on rates
while(done == 0)
    done;
    % random vector step
    dKvec = StepSize * (rand(NConnections,1) - 0.5*ones(NConnections,1));
    
    % multiply the forward probabilities by the appropriate probabilities
    %p_assembly = [0.97777 0.9074 0.9636 0.9737 0.8333 1.0];
    dKvec(1) = dKvec(1)*p_encounter_robot*0.97777;
    dKvec(3) = dKvec(3)*p_encounter_robot*0.9074;
    dKvec(5) = dKvec(5)*p_encounter_robot*0.9737;
    dKvec(7) = dKvec(7)*p_encounter_robot*0.9636;
    dKvec(9) = dKvec(9)*p_encounter_robot*0.8333;
    dKvec(11) = dKvec(11)*p_encounter_robot*1.0;

    % project onto the piece orthogonal to the constraint vectors
    %dKvec = ConProj' * dKvec;  % null space constraint
    dKvec = ProjNull * dKvec;   % null space constraint
    % now make sure that the new Kvec is all positive, add upper limit
    % constraints
    KvecT = Kvec + dKvec;
    if ( (KvecT > 0) & (KvecT(2)<=1) & (KvecT(4)<=1) & (KvecT(6)<=1)...
      & (KvecT(8)<=1) & (KvecT(10)<=1) & (KvecT(12)<=1) ...
      & (KvecT(1)<=p_encounter_robot*0.97777) & (KvecT(3)<=p_encounter_robot*0.9074) ...
      & (KvecT(5)<=p_encounter_robot*0.9737) & (KvecT(7)<=p_encounter_robot*0.9636) ...
      & (KvecT(9)<=p_encounter_robot*0.8333) & (KvecT(11)<=p_encounter_robot*1.0) )
        done=1;
    end;
end;

% make the change
Kvec = Kvec + dKvec;
