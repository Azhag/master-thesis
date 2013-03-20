#!/bin/sh

rm res_finishing_times

for i in 1 5 10 20 30 40 50 60 70 80 90 100
do
	echo "--- Simulating for $i pieces ---"
	sed "s/NB_PUZZLES/$i/g" ProblemDefinition_template.cpp > ProblemDefinition.cpp
	sed "s/PIECE_ENCOUNTER/$i/g" ProblemDefinition.cpp > ProblemDefinition.cpp
	sed "s/ROBOT_ENCOUNTER/$i/g" ProblemDefinition.cpp > ProblemDefinition.cpp
	make clean &> /dev/null
	make &> /dev/null
	mkdir mult_$i
	./PuzzleTestCaseSingleMultiple 100 mult_$i/out
	#tail -n 1 out | sed "s/^\([0-9]*\.[0-9]*\) .*/\1/g" >> res_finishing_times
	# sed "s/^\([0-9]*\.[0-9]*\) .*/\1/g" out >> res_finishing_times
done
