#!/usr/bin/env python
# encoding: utf-8
"""
run_all_experiments.py

Created by Loïc on 2007-12-17.
Copyright (c) 2007 Loïc Matthey. All rights reserved.
"""

import sys
import os
import time

def main(argv):
	if (len(argv)<=0):
		print("Wrong arguments");
		print("run_all_experiments.py nb_exp outfile");
		sys.exit(-1);
		
	nb_experiments = int(argv[0]);
	#os.system("echo 'final_position = []' >> exp"+argv[0]+".m");
	
	for i in range(nb_experiments):
		os.system("./puzzletestcasesingle out");
		os.system("cat out | tail -n 1 | sed 's/^\([0-9]*\.[0-9]*\).*/\\1/' >> "+argv[1]);
		time.sleep(1.0);
		#os.system("echo 'webots ./worlds/uneven_tri_1_"+str(0.1*i)+".wbt --batch >> exp"+argv[0]+".m'");
		#print(str(int(100*(j+1)/float(nb_experiments)))+" %");

if __name__ == '__main__':
	main(sys.argv[1:])

