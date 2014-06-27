LEVEL = ../../../..
PROG = make
CPPFLAGS += -DHAVE_SIGLIST -DNO_LDAV -DNOMEMOPT -DPOSIX
Source=commands.c job.c dir.c file.c load.c misc.c main.c read.c \
       remake.c remote.c rule.c implicit.c default.c variable.c expand.c \
       function.c vpath.c version.c arscan.c ar.c

RUN_OPTIONS = -n -f $(PROJ_SRC_DIR)/INPUT/GNUmakefile.make USEROPT=BWGC VPATH=$(PROJ_SRC_DIR)
include ../../../Makefile.multisrc
