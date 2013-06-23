##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 gcc test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

include $(LEVEL)/Makefile.config

CPPFLAGS += -DINLINE=__inline__

ifeq ($(TARGET_OS),Darwin)
  CPPFLAGS += -DSPEC_CPU_MACOSX
endif

ifeq ($(ENDIAN),big)
  CPPFLAGS += -DHOST_WORDS_BIG_ENDIAN
endif

HASH_PROGRAM_OUTPUT = 1

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS      = cccp.i -o -
  STDOUT_FILENAME := cccp.s
else
  RUN_OPTIONS      = integrate.i -o -
  STDOUT_FILENAME := integrate.s
endif
