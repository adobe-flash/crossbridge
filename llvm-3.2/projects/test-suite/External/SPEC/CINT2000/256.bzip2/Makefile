LEVEL = ../../../..
RUN_OPTIONS     = `cat $(REF_IN_DIR)control`
include ../../Makefile.spec2000
ifeq ($(RUN_TYPE),test)
STDOUT_FILENAME := input.random.out
else
STDOUT_FILENAME := input.compressed.out
endif
