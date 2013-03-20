function Values = lacOperon_eventAssign(SpeciesValues)
% function lacOperon_eventAssign takes
%
% current simulation time and
%
% vector of current species values
%
% and returns the values assigned by an event assignment
%
% lacOperon_eventAssign should be used with MATLABs odeN functions
% and called to reinitialise values when an event has stopped the integration
%

%--------------------------------------------------------
% floating species concentrations
Idna = SpeciesValues(1);
Irna = SpeciesValues(2);
I = SpeciesValues(3);
Op = SpeciesValues(4);
Rnap = SpeciesValues(5);
Rna = SpeciesValues(6);
Z = SpeciesValues(7);
Lactose = SpeciesValues(8);
ILactose = SpeciesValues(9);
IOp = SpeciesValues(10);
RnapOp = SpeciesValues(11);

%--------------------------------------------------------
% event assignments
Lactose = Lactose+10000;

%--------------------------------------------------------
% output values

Values(1) = Idna;
Values(2) = Irna;
Values(3) = I;
Values(4) = Op;
Values(5) = Rnap;
Values(6) = Rna;
Values(7) = Z;
Values(8) = Lactose;
Values(9) = ILactose;
Values(10) = IOp;
Values(11) = RnapOp;
