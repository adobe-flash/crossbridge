# MultiSource/VersaBench Makefile:  Build all subdirectories automatically

LEVEL = ../../..
# Get the $(ENDIAN) setting
include $(LEVEL)/Makefile.config

# 8b10b is inherently little-endian, don't bother on big-endian.
ifeq ($(ENDIAN),big)
PARALLEL_DIRS = beamformer bmm dbms ecbdes
else
PARALLEL_DIRS = 8b10b beamformer bmm dbms ecbdes
endif

include $(LEVEL)/Makefile.programs
