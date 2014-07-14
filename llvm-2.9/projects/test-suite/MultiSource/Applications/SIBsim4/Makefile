LEVEL = ../../..
PROG = SIBsim4

CPPFLAGS =
LDFLAGS  =

ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS= -A4 $(PROJ_SRC_DIR)/x-small.fa $(PROJ_SRC_DIR)/y-small.fa
else
RUN_OPTIONS= -A4 $(PROJ_SRC_DIR)/x.fa $(PROJ_SRC_DIR)/y.fa
endif

include ../../Makefile.multisrc
