LEVEL = ../../../..

ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = -scanfile c756hel.in -trainfile1 a10.img -stride 2 -startx 134 -starty 220 -endx 184 -endy 240 -objects 3
else
RUN_OPTIONS = -scanfile c756hel.in -trainfile1 a10.img -stride 2 -startx 134 -starty 220 -endx 154 -endy 230 -objects 3
endif

#STDIN_FILENAME = $(RUN_TYPE).in
STDOUT_FILENAME = $(RUN_TYPE).out
CPPFLAGS =

include ../../Makefile.spec2000
