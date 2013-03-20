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
	alphas = [0.4, 0.99, 0.01, 0.5];
	times = [4000, 6000, 6000, 8000];

	
	init = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 6, 3, 3, 0, 0, 0, 0];
	
	nb_experiments = range(1);
	
	problemDefString = Template(open("ProblemDefinition_template.cpp",'r').read());
	singleMultString = Template(open("PuzzleTestCaseSingleMultiple_template.cpp",'r').read());
	
	for exp in nb_experiments:
		print "Experiment "+str(exp);
		init_act = init;
				
		for i in range(len(alphas)):
			print "Simulating for step "+str(i) + ": alpha=" + str(alphas[i])+ ", for "+str(times[i])+ "sec.";
		
			# ===> Construct the modified ProblemDefinition file
			mapping = dict(nb_puzzles = str(3),
						piece_encounter = str(0.004379308708520),
						robot_encounter = str(0.006568963062780),
						alpha = str(alphas[i-1]),
						x1 = str(init_act[0]),
						x2 = str(init_act[1]),
						x3 = str(init_act[2]),
						x4 = str(init_act[3]),
						x5 = str(init_act[4]),
						x6 = str(init_act[5]),
						x7 = str(init_act[6]),
						x8 = str(init_act[7]),
						x9 = str(init_act[8]),
						x10 = str(init_act[9]),
						x11 = str(init_act[10]),
						x12 = str(init_act[11]),
						x13 = str(init_act[12]),
						x14 = str(init_act[13]),
						x15 = str(init_act[14]),
						x16 = str(init_act[15]),
						x17 = str(init_act[16]),
						x18 = str(init_act[17]),
						x19 = str(init_act[18]),
			);
			newProblemDef = problemDefString.substitute(mapping);
		
			mapping2 = dict(time = str(times[i]));
			newSingleMult = singleMultString.substitute(mapping2);
		
			# Write ProblemDefinition to file
			outFile = open("ProblemDefinition.cpp", 'w');
			outFile.write(newProblemDef);
			outFile.close();
		
			# Write singleMultString to file
			outFile = open("PuzzleTestCaseSingleMultiple.cpp", 'w');
			outFile.write(newSingleMult);
			outFile.close();
		
			# Compile
			os.system("make clean &> /dev/null");
			os.system("make &> /dev/null");
			# os.system("mkdir mult_" + str(i));
		
			# Execute the simulation
			os.system("./PuzzleTestCaseSingleMultiple 1 green/out"+str(i));
			os.system("cat green/out"+str(i)+"_0.txt >> green/all.txt")
			# Get the last population
			lastPop =  os.popen("tail -n 1 green/out"+str(i)+"_0.txt").read().split(' ');
			print lastPop;
			for j in range(len(init)):
				init_act[j]=lastPop[j+1];
			

if __name__ == '__main__':
	main()

