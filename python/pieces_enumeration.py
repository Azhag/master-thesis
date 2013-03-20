#!/usr/bin/env python
# encoding: utf-8
"""
pieces_enumeration.py

Created by Loïc on 2008-03-24.
Copyright (c) 2008 EPFL. All rights reserved.
"""

import sys
import os
import copy
import math

pi = math.pi;

def flatten(x):
    """flatten(sequence) -> list

    Returns a single, flat list which contains all elements retrieved
    from the sequence and all recursively contained sub-sequences
    (iterables).

    Examples:
    >>> [1, 2, [3,4], (5,6)]
    [1, 2, [3, 4], (5, 6)]
    >>> flatten([[[1,2,3], (42,None)], [4,5], [6], 7, MyVector(8,9,10)])
    [1, 2, 3, 42, None, 4, 5, 6, 7, 8, 9, 10]"""

    result = []
    for el in x:
        #if isinstance(el, (list, tuple)):
        if hasattr(el, "__iter__") and not isinstance(el, basestring):
            result.extend(flatten(el))
        else:
            result.append(el)
    return result

class Piece:
	def __init__(self, left=None, right=None, top=None, bottom=None):
		self.neigh = [];
		self.neigh.append(left);
		self.neigh.append(right);
		self.neigh.append(top);
		self.neigh.append(bottom);
	
	def addPieceEverywhere(self, father):
		'''Recursively add one piece to every possible anchoring point'''
		children = [];
		
		# For all connection points
		for i in range(4):
			children.extend(flatten(copy.deepcopy(self).addPiece(i, father)));
		
		return children;
		
	def addPiece(self, endpoint, father):
		'''Will connect one piece to the given end point.
		
			If the endpoint is occupied, gives the free piece one step further
		'''
		#print 'Connecting ' + str(id(self)) + ' at ' + str(endpoint);
		extended = [];
		
		if(self.neigh[endpoint] == None):
			# Simple case, just add a new piece at the end.
			self.neigh[endpoint] = Piece();
			
			#print ' -> New piece ' + str(id(self.neigh[endpoint])) + ' at ' + str(endpoint);
			#self.printPiece();
			
			extended.append(self);

		elif(endpoint != self.inverseEndpoint(father)):
			#print ' --- deeper';
			
			# Give the free piece forward, without going back on yourself
			newChildren = self.neigh[endpoint].addPieceEverywhere(endpoint);
			
			# Reconstruct the new pieces
			for extendedChild in newChildren:
				#print extendedChild.attachedPieces() + '\n';
				newItem = copy.deepcopy(self);
				newItem.neigh[endpoint] = extendedChild;
				extended.extend([newItem]);
		return extended;
		
	def inverseEndpoint(self, endpoint):
		''' Give the inverse endpoint
		'''
		if (endpoint == 0):
			return 1;
		if (endpoint == 1):
			return 0;
		if (endpoint == 2):
			return 3;
		if (endpoint == 3):
			return 2;
			
	def attachedPieces(self, deepness = 0):
		'''Return the attached pieces
		'''
		neigh = '';
		#for child in self.neigh:
			#neigh = neigh + str(int(child != None));
		neigh += str(id(self));
		
		for child in self.neigh:	
			if (child != None):
				neigh = neigh + "\n" + ' '*(deepness+1) + child.attachedPieces(deepness+1);
		return neigh;
	
	def descrPiece(self, x=0, y=0):	
		pieces = [];
		pieces.append([x,y]);
		
		# different increments for the anchor points
		increments = ((-1, 0), (1, 0), (0, -1), (0, 1));
		#print self.attachedPieces();
		
		for i in range(4):
			if (self.neigh[i] != None):
				#print str(i) + ' ' + str(x)+' '+ str(increments[i][0]);
				pieces.extend(self.neigh[i].descrPiece(x+increments[i][0], y+increments[i][1]));
		return pieces;
		
	def drawingPiece(self, nb_pieces):
		desc = self.descrPiece(0,0);
		drawing = '';
		for i in range(1, 2*nb_pieces):
			for j in range(1,2*nb_pieces):
				drawing += str(int([i-nb_pieces,j-nb_pieces] in desc));
			drawing += '\n';
		print drawing;
	
	def findRotations(self):
		''' Construct all the rotations by k*pi/2 of this piece
		'''
		list_rotations = [];
		rotate = lambda x, y, angle: [int(round(x*math.cos(angle) -y*math.sin(angle))), int(round(x*math.sin(angle)+y*math.cos(angle)))];
		
		desc = self.descrPiece();

		for angle in [pi/2, pi, 3*pi/2]:
			points = [];
			for piece in desc:
				# print angle;
				# 				print piece;
				# 				print rotate(piece[0], piece[1], angle);
				points.append(rotate(piece[0], piece[1], angle));
			list_rotations.append(points);
		return list_rotations;
			
def main():
	nb_pieces = 4;
	
	all_pieces = [];
	open_pieces = [];
	
	# Initial piece
	open_pieces.append(Piece());

	# Create the combinations
	for i in range(1,nb_pieces):
		working_pieces = open_pieces[:];
		for piece in working_pieces:
			#print i;
			#print '\n---- New open piece ----\n';
			open_pieces.remove(piece);
			open_pieces.extend(piece.addPieceEverywhere(None));
			
			# Filter by rotation invariance
			open_pieces_desc = [n.descrPiece() for n in open_pieces];
			for piece in open_pieces:
				for rot in piece.findRotations():
					if (rot in open_pieces_desc):
						#print rot;
			 			#print open_pieces_desc.index(rot);
			 			open_pieces.remove(open_pieces[open_pieces_desc.index(rot)]);
			 			open_pieces_desc.remove(rot);
			
			# Filter by number of blocks (other bug I think...)
			open_pieces_desc = [n.descrPiece() for n in open_pieces];
			for piece in open_pieces_desc:
				for elem in piece:
					if (piece.count(elem)>1):
						open_pieces.remove(open_pieces[open_pieces_desc.index(piece)]);
						open_pieces_desc.remove(piece);
						break;
	# Show them
	print len(open_pieces);
	
	# Find representations
	open_pieces_desc = [n.descrPiece() for n in open_pieces];
	
	new_pieces = open_pieces[:];
	#for i in range(len(open_pieces)):
		#print open_pieces_desc[i];
		#if (piece in rotations):
		#	new_pieces.remove(piece);
	#print new_pieces;
		
	#[e for e in open_pieces_desc if !(e in rotations)];
	#filter(, open_pieces_desc);
	
	#desc = open_pieces[1].descrPiece();
	#print;
	#print desc;

		
#	print [e for e in desc if (e in desc)];
	#print open_pieces[0].findRotations();
		
	for piece in open_pieces:
		print piece.descrPiece(); 
		#piece.drawingPiece(nb_pieces);
	
	#for piece in open_pieces:
	#	print piece.attachedPieces();
	
if __name__ == '__main__':
	main()

