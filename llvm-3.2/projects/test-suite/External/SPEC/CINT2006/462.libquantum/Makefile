##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 libquantum test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

ifdef LARGE_PROBLEM_SIZE
RUN_TYPE := ref
else
# small problem size should use train: test is too small.
RUN_TYPE := train
endif

include ../../Makefile.spec2006

ifeq ($(TARGET_OS),Darwin)
  CPPFLAGS += -DSPEC_CPU_MACOSX
endif

ifeq ($(TARGET_OS),Linux)
  CPPFLAGS += -DSPEC_CPU_LINUX
endif

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS     := 33 5
  STDOUT_FILENAME := test.out
else
  RUN_OPTIONS     := 143 25
  STDOUT_FILENAME := train.out
endif
