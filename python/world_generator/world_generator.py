#!/usr/bin/env python
# encoding: utf-8
"""
world_generator.py

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
import worldhandler
import worldbuilder

def main():
	''' Construct a given world using the parameters defined in a XML file
	'''
	builder = worldbuilder.WorldBuilder("output.wbt", "header.wbtt", None, "templates/");
	handler = worldhandler.WorldHandler("test_world.xml", builder);
	handler.parse();


if __name__ == '__main__':
	main()
