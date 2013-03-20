#!/usr/bin/env python
# encoding: utf-8
"""
worldbuilder.py

Created by Loïc Matthey on 2008-04-04.
Copyright (c) 2008 Loïc Matthey. All rights reserved.

This file is part of WPWG.

WPWG is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
at your option) any later version.

WPWG is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with WPWG.  If not, see <http://www.gnu.org/licenses/>.

"""

import sys
import os
from string import Template
import math
import datetime
import random
from decimal import *

class WorldBuilder:
	
	#########################################################
	## Init the WorldBuilder, create the output file and put the header if given
	## @param outputFilename	string, without directory
	## @param [headerFilename]	string, without directory
	## @param [footerFilename]	string, without directory
	## @param [debug]			boolean, show additional debug if True.
	def __init__(self, outputFilename, headerFilename = None, footerFilename = None, templateDirectory = 'templates/', debug = False):
		
		self.debug = debug;
		self.usedPositions = [];
		self.placedRobots = 0;
		self.placedPieces = {};
		self.templateDirectory = templateDirectory;
		
		# Create output file
		self.outFile = open("worlds/" + str(outputFilename), 'w');
		
		print "<WorldBuilder> ======= Creating \'" + outputFilename + "\' world ====";
		
		# Put the header, with the date of today
		if headerFilename != None:
			self.outFile.write(Template(open(self.templateDirectory + headerFilename, 'r').read()).substitute(today = datetime.date.today()));
			if self.debug:
				print "<WorldBuilder>  Header written";
		
		if (footerFilename != None):
			self.footerFilename = footerFilename;
	##
	#########################################################
	
	#########################################################
	## Finalize the file, output the footer if given at instantiation.
	def finalize(self):
		"""Output the footer if available and finishes"""
		if (hasattr(self, 'footerFilename')):
			self.outFile.write(open(self.templateDirectory + self.footerFilename, 'r').read());

		print "<WorldBuilder> === File created ===";
	##
	#########################################################
	
	#########################################################
	## Write the Arena to the file, calling subfunction depending on the desired shape
	## @param shapeArena	string, "square" or "circle"
	## @param sizeArena		float, radius of the desired arena
	## @param [wallSize]	float, thickness of wall
	def constructArena(self, shapeArena, sizeArena, wallSize = 0.02):
		"""Output the Webots code to create the desired Arena"""
		
		self.sizeArena = sizeArena*0.95;
		
		if (shapeArena == "square"):
			self.constructSquareArena(sizeArena, wallSize);
	##
	#########################################################
	
	#########################################################
	## Construct a Square arena.
	## @param sizeArena			float, radius (half edge length) of the square
	## @param wallSize			float, thickness of the wall
	## @param [subdivisions]	float, number of divisions of the floor
	def constructSquareArena(self, sizeArena, wallSize, subdivisions = 10.0):
		"""Output the Webots code to create a Square Arena"""
		arenaString = Template(open(self.templateDirectory+ "arena.wbtt",'r').read());
		
		# ===> Construct the modified arena according to the given parameters.
		mapping = dict(size = str(sizeArena),
						groundSize = str(int(2*subdivisions+1)),
						groundSpacing = str(sizeArena/subdivisions),
						size_with_wall = str(sizeArena-wallSize),
						size_doubled = str(2.*sizeArena),
						wall_size = str(wallSize),
						wall_translation = str(sizeArena-wallSize/2.0),
						light_position = str(sizeArena*0.75)
		);
		arenaString = arenaString.substitute(mapping);
		#print arenaString;
	
		self.outFile.write(arenaString);
		if self.debug:
			print "<WorldBuilder>  Square arena created. (size %.1f)" % sizeArena;
	##
	#########################################################
	
	#########################################################
	## Construct the Khepera robots. Put a desired number of robots, if random using a gaussian distribution.
	## @param numberRobots		int, number of robots with the same parameters
	## @param controller		string, controller name
	## @param [initialPosRand]	boolean, if True, place randomly using gaussian distribution with dev=sizeArena
	## @param [x]				float, initial position, if not random
	## @param [y]				float, initial position, if not random
	## @param [theta]			float, initial position, if not random
	def constructRobotsKhepera(self, numberRobots, controller, initialPosRand=True, x=0.0, y=0.0, theta=0.0):
		"""Output the webots code to create the robots, without overlaps"""
		
		# Check that the arena is created already
		assert hasattr(self, 'sizeArena'), '<WorldBuilder>  Please construct the arena before.';
		
		# Load the template
		robotTemplate = Template(open(self.templateDirectory + "khepera.wbtt", 'r').read());
		
		print "<WorldBuilder>  ==== Placing "+ str(numberRobots) + " robot(s), controller:" + controller + " ===="
		
		# Construct all the robots
		for num in range(numberRobots):
			
			# Find a position which isn't used already.
			if (initialPosRand):
				#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
				x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
				#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
				y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
				theta = round(random.uniform(0, 2*math.pi),2);
			
				while ([x,y] in self.usedPositions):
					print '<WorldBuilder>    Random position in use for current robot...';
					#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
					#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
			
				self.usedPositions.append([x,y]);
			else:
				# Given position, verify that it's not already used. If it is, randomize it.
				x = round(x, 2);
				y = round(y, 2);
				theta = round(theta, 2);
			
				while ([x,y] in self.usedPositions):
					print '<WorldBuilder>    Fixed position for robot ' + str(self.placedRobots + num+1) + ' already in use. Going random for him.';
					#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
					#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);

				self.usedPositions.append([x,y]);		
		
			
			# ===> Construct the modified robot according to parameters
			mapping = dict(num = str(self.placedRobots + num+1),
							controller = str(controller),
							x = x,
							y = y,
							theta = theta
			);
			robotString = robotTemplate.substitute(mapping);
		
			#print robotString;
			if self.debug:
				print "<WorldBuilder>   Robot %d placed. (controller: %s, x: %.2f, y: %.2f, theta: %.2f)" % (self.placedRobots + num+1,  controller, x, y, theta);
			
			self.outFile.write(robotString);
		
		# Increment the number of placed robots
		self.placedRobots += numberRobots;
	##
	#########################################################

	#########################################################
	## Construct the pieces. Put a desired number of pieces, if random using a gaussian distribution.
	## @param nbBlocks			int, number of blocks constructing this piece, defines a higher type of piece
	## @param pieceType			string, type of this piece, corresponding to a specific template
	## @param nbPieces			int, number of these pieces
	## @param controller		string, controller name
	## @param [initialPosRand]	boolean, if True, place randomly using gaussian distribution with dev=sizeArena
	## @param [x]				float, initial position, if not random
	## @param [y]				float, initial position, if not random
	## @param [theta]			float, initial position, if not random
	def constructPieces(self, nbBlocks, pieceType, nbPieces, controller, initialPosRand=True, x=0, y=0, theta=0):
		"""Output the Webots code to create the pieces, without overlaps"""
		
		# Check that the arena is created already
		assert hasattr(self, 'sizeArena'), '<WorldBuilder>  Please construct the arena before.';
		
		
		# Load the good template, according to nbBlocks and pieceType
		pieceTemplate = Template(open(self.templateDirectory + \
						"pieces_" + str(nbBlocks) + "blocks/" + \
						"piece_" + str(pieceType) + \
						".wbtt", 'r').read());
						
		print "<WorldBuilder>  ==== Placing "+ str(nbPieces) + " piece(s) of type "+str(pieceType) + ", controller: " + controller + " ===="
		
		# Put all pieces
		for num in range(nbPieces):
			
			# Find a position which isn't used already.
			if (initialPosRand):
				# Random position, choose a free position
				#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
				x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
				#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
				y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
				theta = round(random.uniform(0, 2*math.pi),2);
			
				while ([x,y] in self.usedPositions):
					print '<WorldBuilder>    Random position in use for current piece...';
					#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
					#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
			
				self.usedPositions.append([x,y]);
			else:
				# Given position, verify that it's not already used. If it is, randomize it.
				x = round(x, 2);
				y = round(y, 2);
				theta = round(theta, 2);
			
				while ([x,y] in self.usedPositions):
					print '<WorldBuilder>    Fixed position for piece ' + str(self.placedPieces + num+1) + ' already in use. Going random for that one.';
					#x = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					x = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);
					#y = round(random.uniform(-self.sizeArena, self.sizeArena),2);
					y = math.fmod(round(random.gauss(0.0, self.sizeArena),2), self.sizeArena);

				self.usedPositions.append([x,y]);		
		
			
			# ===> Construct the modified robot according to parameters
			mapping = dict( piece_type = pieceType,
							num = str(self.placedPieces.setdefault(str(pieceType),0) + num+1),
							controller = str(controller),
							x = x,
							y = y,
							theta = theta
			);
		
			pieceString = pieceTemplate.substitute(mapping);
		
			if self.debug:
				print "<WorldBuilder>   Piece type %d, #%d placed. (controller: %s, x: %.2f, y: %.2f, theta: %.2f)" % (pieceType, self.placedPieces[str(pieceType)] + num+1,  controller, x, y, theta);
			
			#print pieceString;
			self.outFile.write(pieceString);
			
		# Increment the number of placed robots
		self.placedPieces[str(pieceType)] += nbPieces;
	##
	#########################################################

	def constructSupervisor(self, controller):
		"""Output the webots code for a Supervisor with given controller"""
		
		# Check that the arena is created already
		assert hasattr(self, 'sizeArena'), '<WorldBuilder>  Please construct the arena before.';
		
		# Load the template
		supervisorTemplate = Template(open(self.templateDirectory + "supervisor.wbtt", 'r').read());
		
		print "<WorldBuilder>  ==== Supervisor with "+ controller + " controller ====";
		
		# ===> Construct the modified Supervisor according to parameters
		mapping = dict( controller = str(controller),
		);
	
		supervisorString = supervisorTemplate.substitute(mapping);
		
		# Write it
		self.outFile.write(supervisorString);
		
def main():
	print """
	USE world_generator.py instead please.
	"""
	
	# builder = WorldBuilder("output.wbt", "header.wbtt");
	# 	builder.constructArena("square", 1.5);
	# 	builder.constructRobots(10, "locking_simple");	
	# 	#builder.constructRobots(1, "braitenberg", False, 0, 0, 0);
	# 	#builder.constructRobots(1, "braitenberg", False, 0, 0, 0);
	# 	builder.constructPieces(5, 1, 10, "piece_simple");
	# 	#builder.constructPieces(5, 2, 1, 1, "piece_simple");
	# 	builder.constructPieces(5, 2, 10, "piece_simple");
	# 	builder.constructPieces(5, 3, 10, "piece_simple");
	# 	builder.constructPieces(5, 4, 10, "piece_simple");
	# 	builder.finalize();
	

if __name__ == '__main__':
	main();