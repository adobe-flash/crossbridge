# SingleSource/UnitTests/Vector/Makefile
LEVEL = ../../..

include $(LEVEL)/Makefile.config

DIRS = 

ifeq ($(ARCH),PowerPC)
DIRS += Altivec
endif

ifeq ($(ARCH),x86)
DIRS += SSE
endif
ifeq ($(ARCH),x86_64)
DIRS += SSE
endif

# Assume ARMv7 implies NEON.
ifneq ($(CC_UNDER_TEST_TARGET_IS_THUMBV7),)
DIRS += NEON
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc


