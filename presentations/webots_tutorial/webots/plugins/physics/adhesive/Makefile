#
# Standard Makefile for a Webots physics shared library
#
# Author: Olivier.Michel@cyberbotics.com
# Date:   August 30th, 2006.
# Modified: Simon Ruffieux, February 19th. 2008 
#

ifndef WEBOTS_HOME_PATH
WEBOTS_HOME_PATH=/usr/local/webots/
endif
ifndef WEBOTS_HOME
WEBOTS_HOME=/usr/local/webots
endif

GLINC=/usr/include/
GLLIB=/usr/lib/

CFLAGS  = -I$(GLINC) -g
#LIBRARIES    =  -L -lm 
CPP_SOURCES = adhesive_main.cpp adhesive.cpp Vector3D.cpp StickyObj.cpp

include $(WEBOTS_HOME_PATH)/projects/default/plugins/physics/Makefile.include
