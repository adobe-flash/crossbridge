LEVEL = ../../..

PROG     = fhourstones3.1
#CPPFLAGS = -DUNIX
# -DTRANSIZE=1050011 -DPROBES=8 -DREPORTPLY=8

# Specify which file provides the contents of stdin for the test run 
ifdef LARGE_PROBLEM_SIZE
STDIN_FILENAME = $(PROJ_SRC_DIR)/inputs_large
else
STDIN_FILENAME = $(PROJ_SRC_DIR)/inputs
endif

include	../../Makefile.multisrc
