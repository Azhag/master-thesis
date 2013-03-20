#!/usr/bin/env python
# encoding: utf-8
"""
worldhandler.py

Object reading from an XML file to instantiate the world.

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
import xml.dom.minidom
import worldbuilder

class WorldHandler():
	
	#########################################################
	## Init the WorldHandler, will create a world using an XML file
	## @param filename	string, filename of the XML parameter file
	## @param builder	WorldBuilder, instance of a Webots world builder
	def __init__(self, filename, builder):
		# Init stuffs
		print "<WorldHandler> +++++++ Reading from " + filename + " ++++";
		self.doc = xml.dom.minidom.parse(filename);
		self.builder = builder;
	##
	#########################################################
	
	#########################################################
	## Start the parsing of the file and the creation of the world.
	def parse(self):
		print "<WorldHandler> ++++ Parsing...";
		self.handlePuzzleWorld(self.doc);
	##
	#########################################################

	#########################################################
	## Handle root node of the XML.
	## @param doc,	Element, base root node
	def handlePuzzleWorld(self, doc):
		self.handleArena(doc.getElementsByTagName("Arena"));
		self.handleRobots(doc.getElementsByTagName("Robots"));
		self.handlePieces(doc.getElementsByTagName("Pieces"));
		self.handleSupervisor(doc.getElementsByTagName("Supervisor"));
		self.builder.finalize();
	##
	#########################################################

	#########################################################
	## Read the Arena node node and call the Builder accordingly
	## @param arena		Element, node of type Arena
	def handleArena(self, arena):
		
		assert arena.length == 1, "<WorldHandler> Error: only one arena please";

		arena = arena[0];
		
		# Handle the shape
		shapeElem = arena.getElementsByTagName("Shape")[0].childNodes[0];
		if shapeElem.nodeType == shapeElem.TEXT_NODE:
			# Possible shape type
			if (shapeElem.data == "square"):
				shape = shapeElem.data;
				#print self.shape;
			elif (shapeElem.data == "circle"):
				shape = shapeElem.data;
			else:
				raise NameException, 'Unrecognized shape in HandleArena';
			
		# Handle the size
		sizeElem = arena.getElementsByTagName("Size")[0].childNodes[0];
		if sizeElem.nodeType == sizeElem.TEXT_NODE:
			size = float(sizeElem.data);
		
		# Construct the arena
		self.builder.constructArena(shape, size);
	##
	#########################################################
	
	#########################################################
	## Handle the Robots nodes. Call handleRobot for a given
	##	robot declaration.
	## @param robots		Element, node of type Robots
	def handleRobots(self, robotsSet):
		for robots in robotsSet:
			for robot in robots.childNodes:
				if robot.nodeType == robot.ELEMENT_NODE:
					self.handleRobot(robot);
	##
	#########################################################
	
	#########################################################
	## Read the Robot node node and call the Builder accordingly
	## @param robot		Element, node of type Robot
	def handleRobot(self, robot):
		x = 0.0;
		y = 0.0;
		initialPosRand = False;
		numberRobots = int(robot.getElementsByTagName("NumberInstances")[0].lastChild.data);
		controller = robot.getElementsByTagName("Controller")[0].lastChild.data;
		
		initialPositionElems = robot.getElementsByTagName("InitialPosition")[0].childNodes;
		for initPos in initialPositionElems:
			# Random or not ?
			if initPos.nodeType == initPos.TEXT_NODE:
				if initPos.data == "random":
					initialPosRand = True;
			elif (initPos.nodeType == initPos.ELEMENT_NODE and initPos.nodeName == "Coordinates"):
				x = float(initPos.getElementsByTagName("X")[0].lastChild.data);
				y = float(initPos.getElementsByTagName("Y")[0].lastChild.data);
		
		# Place the robots
		self.builder.constructRobotsKhepera(numberRobots, controller, initialPosRand, x, y);
	##
	#########################################################

	#########################################################
	## Handle the Pieces nodes. Call handlePiece for a given
	##	piece declaration.
	## @param pieces		Element, node of type Pieces
	def handlePieces(self, piecesSet):
		for pieces in piecesSet:
			nbBlocks = int(pieces.getAttribute("nbBlocks"));
			for piece in pieces.childNodes:
				if piece.nodeType == piece.ELEMENT_NODE:
					self.handlePiece(piece, nbBlocks);
	##
	#########################################################
	
	#########################################################
	## Read the Piece node and call the Builder accordingly.
	## @param piece		Element, node of type Piece
	## @param nbBlocks	int, number of blocks for the desired set of pieces.
	def handlePiece(self, piece, nbBlocks):
		x = 0.0;
		y = 0.0;
		
		# Read parameters
		pieceType = piece.getElementsByTagName("Type")[0].lastChild.data;
		nbPieces = int(piece.getElementsByTagName("NumberInstances")[0].lastChild.data);
		controller = piece.getElementsByTagName("Controller")[0].lastChild.data;
		initialPositionElems = piece.getElementsByTagName("InitialPosition")[0].childNodes;
		for initPos in initialPositionElems:
			# Random or not ?
			if initPos.nodeType == initPos.TEXT_NODE:
				if initPos.data == "random":
					initialPosRand = True;
			elif (initPos.nodeType == initPos.ELEMENT_NODE and initPos.nodeName == "Coordinates"):
				x = initPos.getElementsByTagName("X")[0].lastChild.data;
				y = initPos.getElementsByTagName("Y")[0].lastChild.data;
		
		# Place the pieces
		self.builder.constructPieces(nbBlocks, pieceType, nbPieces, controller, initialPosRand, x, y);
	##
	#########################################################
	
	#########################################################
	## Read the Arena node node and call the Builder accordingly
	## @param arena		Element, node of type Arena
	def handleSupervisor(self, supervisor):
		
		if supervisor.length > 0:
			
			assert supervisor.length == 1, "<WorldHandler> Error: only one Supervisor please";

			supervisor = supervisor[0];
		
			# Get the controller
			controller = supervisor.getElementsByTagName("Controller")[0].lastChild.data;
		
			#Construct the supervisor
			self.builder.constructSupervisor(controller);
	##
	#########################################################
	

def main():
	print """
	USE world_generator.py instead please."""
	# builder = worldbuilder.WorldBuilder("output.wbt", "header.wbtt");
	# 	handler = WorldHandler("test_world.xml", builder);
	# 	handler.parse();
	

if __name__ == '__main__':
	main()