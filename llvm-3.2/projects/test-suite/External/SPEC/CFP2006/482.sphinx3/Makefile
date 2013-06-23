##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 sphinx3 test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE = 0.001

include ../../Makefile.spec2006

CPPFLAGS += -I$(SPEC_BENCH_DIR)/src/libutil -DHAVE_CONFIG_H

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS      = `cp an406-fcaw-b.be.raw an406-fcaw-b.raw` \
                     `cp an407-fcaw-b.be.raw an407-fcaw-b.raw` \
                     `echo "an406-fcaw-b 128000" > ctlfile`    \
                     `echo "an407-fcaw-b 131200" >> ctlfile` ctlfile . args.an4
  STDOUT_FILENAME := an4.log
else
  RUN_OPTIONS      = leng100.hmm
  STDOUT_FILENAME := leng100.out
endif
