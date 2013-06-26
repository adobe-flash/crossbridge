##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 GemsFTDT test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 1.0e-9
FP_ABSTOLERANCE = 1.0e-10

include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

ifeq ($(RUN_TYPE),test)
  STDOUT_FILENAME := test.log
else
  STDOUT_FILENAME := train.log
endif

GemsFDTD.c: parameter.c readdata.c leapfrog.c
NFT.c: parameter.c globalvar.c posvector.c fourier_transf.c errorcheck.c huygens.c excite.c
PEC.c: parameter.c errorcheck.c readline.c globalvar.c
UPML.c: parameter.c globalvar.c errorcheck.c PEC.c
calcflops.c: globalvar.c parameter.c huygens.c UPML.c NFT.c
excite.c: parameter.c globalvar.c
fourier_transf.c: parameter.c
globalvar.c: parameter.c
huygens.c: parameter.c globalvar.c excite.c errorcheck.c
leapfrog.c:  errorcheck.c parameter.c huygens.c update.c UPML.c PEC.c NFT.c calcflops.c globalvar.c progress.c
posvector.c: parameter.c
progress.c: parameter.c globalvar.c
readdata.c: parameter.c globalvar.c errorcheck.c huygens.c UPML.c excite.c NFT.c PEC.c progress.c
update.c: parameter.c globalvar.c
