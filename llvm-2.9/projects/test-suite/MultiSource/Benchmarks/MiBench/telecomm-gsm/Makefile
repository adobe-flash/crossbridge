LEVEL = ../../../..

PROG     = telecomm-gsm
CPPFLAGS = -DSTUPID_COMPILER -DNeedFunctionPrototypes=1 -DSASR
LDFLAGS  = -lm
RUN_OPTIONS = -fps -c $(PROJ_SRC_DIR)/large.au
HASH_PROGRAM_OUTPUT = 1
include $(LEVEL)/MultiSource/Makefile.multisrc
