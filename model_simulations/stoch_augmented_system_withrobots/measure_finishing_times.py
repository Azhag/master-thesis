#!/usr/bin/env python
# encoding: utf-8
"""
untitled.py

Created by Loïc on 2008-09-09.
Copyright (c) 2008 EPFL. All rights reserved.
"""

import sys
import os
from string import Template

def main():
	nb_puzzles = range(1,11);
	prob_pieces = [0.013137926125559,   0.006568963062780,   0.004379308708520,   0.003284481531390,   0.002627585225112,   0.002189654354260, 0.001876846589366,   0.001642240765695,   0.001459769569507,   0.001313792612556];
	prob_robots = [0.019706889188339,   0.009853444594170,   0.006568963062780,   0.004926722297085,   0.003941377837668,   0.003284481531390, 0.002815269884048,   0.002463361148542,   0.002189654354260,   0.001970688918834];
	
	problemDefString = Template(open("ProblemDefinition_template.cpp",'r').read());
	
	for i in nb_puzzles:
		print "Simulating for "+str(i) + " puzzles";
		
		# ===> Construct the modified ProblemDefinition file
		mapping = dict(nb_puzzles = str(i),
					piece_encounter = str(prob_pieces[i-1]),
					robot_encounter = str(prob_robots[i-1])
		);
		newProblemDef = problemDefString.substitute(mapping);
		
		# Write to file
		outFile = open("ProblemDefinition.cpp", 'w');
		outFile.write(newProblemDef);
		outFile.close();
		
		# Compile
		os.system("make clean &> /dev/null");
		os.system("make &> /dev/null");
		os.system("mkdir mult_" + str(i));
		
		# Execute the simulation
		os.system("./PuzzleTestCaseSingleMultiple 100 mult_"+str(i)+"/out");


if __name__ == '__main__':
	main()

