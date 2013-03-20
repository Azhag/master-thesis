#
# Makefile for a Webots C++ controller
#
# if your controller uses several C++ sources files:
CPP_SOURCES = SpidyRobot.cpp spidy.cpp
# or
# CC_SOURCES = my_controller.cc my_clever_algo.cc my_gui.cc
#
# if your controller needs additional libraries:
# LIBRARIES=-L/path/to/my/library -lmy_library -lmy_other_library
#
# if some special CFLAGS need to be passed, for example to find include
# files, or set optimization level:
# CFLAGS=-O3 -I/my_library_path/include
#
# finally, on Windows, if you want that your controller opens up a DOS
# console to display the printf output:
# DOS_CONSOLE=1

ifndef WEBOTS_HOME_PATH
WEBOTS_HOME_PATH=/usr/local/webots/
endif
ifndef WEBOTS_HOME
WEBOTS_HOME=/usr/local/webots
endif

include $(WEBOTS_HOME_PATH)/projects/default/controllers/Makefile.include
