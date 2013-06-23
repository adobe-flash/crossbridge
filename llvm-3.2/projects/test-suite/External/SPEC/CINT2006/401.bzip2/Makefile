##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 bzip2 test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

include ../../Makefile.spec2006

## FIXME: The control file has more than one line in it.
RUN_OPTIONS = `head -n 1 $(REF_IN_DIR)control`

ifeq ($(RUN_TYPE),test)
  STDOUT_FILENAME := input.program.out
else
  STDOUT_FILENAME := input.combined.out
endif
