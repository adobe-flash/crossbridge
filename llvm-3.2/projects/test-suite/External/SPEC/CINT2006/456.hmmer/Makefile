##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 hmmr test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 0.002
FP_ABSTOLERANCE = 0.00001

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS      = --fixed 0 --mean 325 --num 45000 --sd 200 --seed 0 bombesin.hmm
  STDOUT_FILENAME := bombesin.out
else
  RUN_OPTIONS      = --fixed 0 --mean 425 --num 85000 --sd 300 --seed 0 leng100.hmm
  STDOUT_FILENAME := leng100.out
endif
