##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 h264ref test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

# This test can take more than the default 500s timeout at -O0.
RUNTIMELIMIT:=850

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS     := -d foreman_test_encoder_baseline.cfg
  STDOUT_FILENAME := foreman_test_baseline_encodelog.out
else
  RUN_OPTIONS     := -d foreman_train_encoder_baseline.cfg
  STDOUT_FILENAME := foreman_train_baseline_encodelog.out
endif
