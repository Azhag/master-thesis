Hsr = dmhsr sssahsr

all: $(Hsr)
dmhsr:
	cd ./DM; make;

odmhsr:
	cd ./ODM; make;

sssahsr:
	cd ./SSSA; make;

clean:
	cd ./DM; make clean;
	cd ..;
	cd ./SSSA; make clean; 
	cd ..; 
