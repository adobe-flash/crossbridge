# SingleSource/UnitTests/Vector/Altivec/Makefile

DIRS = 
LEVEL = ../../../..
# Needed because of MADDs in stepfft.
FP_TOLERANCE := 0.002
include $(LEVEL)/SingleSource/Makefile.singlesrc

# FIXME: CBE doesn't support vectors: PR1126
DISABLE_CBE=1

TARGET_FLAGS += -maltivec -faltivec
LCCFLAGS += -maltivec -faltivec
