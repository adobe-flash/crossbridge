# SingleSource/UnitTests/Vector/Altivec/Makefile

DIRS = 
LEVEL = ../../../..
# Needed because of MADDs in stepfft.
FP_TOLERANCE := 0.002
include $(LEVEL)/SingleSource/Makefile.singlesrc

TARGET_FLAGS += -maltivec
LCCFLAGS += -maltivec
