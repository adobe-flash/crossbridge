##===- TEST.vtl.Makefile -----------------------------------*- Makefile -*-===##
#
# Makefile for getting performance metrics using Intel's VTune.
#
##===----------------------------------------------------------------------===##

TESTNAME = $*

VTL := /opt/intel/vtune/bin/vtl

#
# Events: These will need to be modified for every different CPU that is used
# (i.e. the Pentium 3 on Cypher has a different set of available events than
# the Pentium 4 on Zion).
#
P4_EVENTS := "-ec en='2nd Level Cache Read Misses' en='2nd-Level Cache Read References'"
P3_EVENTS := "-ec en='L2 Cache Request Misses (highly correlated)'"

EVENTS := $(P4_EVENTS)
