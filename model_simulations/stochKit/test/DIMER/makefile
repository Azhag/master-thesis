Dimer = dmdimer odmdimer exptaudimer imptaudimer sssadimer adimer

all: $(Dimer)
dmdimer:
	cd ./DM; make;

odmdimer:
	cd ./ODM; make;

exptaudimer:
	cd ./ExpTau; make;

imptaudimer:
	cd ./ImpTau; make;

sssadimer:
	cd ./SSSA; make;
	
adimer:
	cd ./Adaptive; make;

clean:
	cd ./DM; make clean;
	cd ./ODM; make clean;
	cd ./ExpTau; make clean;
	cd ./ImpTau; make clean;
	cd ./SSSA; make clean;
	cd ./Adaptive; make clean;
