#!/bin/sh

rm res_finishing_times

for i in {1..10}
do
	sed "s/NB_PUZZLES/$i/g" ProblemDefinition_template.cpp > ProblemDefinition.cpp
	make clean
	make
	./PuzzleTestCaseStats 100 out
	#tail -n 1 out | sed "s/^\([0-9]*\.[0-9]*\) .*/\1/g" >> res_finishing_times
	sed "s/^\([0-9]*\.[0-9]*\) .*/\1/g" out >> res_finishing_times
done
