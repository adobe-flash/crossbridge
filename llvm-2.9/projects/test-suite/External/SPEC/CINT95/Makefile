LEVEL = ../../..
PARALLEL_DIRS := \
  099.go \
  124.m88ksim \
  126.gcc \
  129.compress \
  130.li \
  132.ijpeg \
  134.perl \
  147.vortex

# Disable 126.gcc when testing with Clang, which doesn't support lvalue casts.
ifdef CC_UNDER_TEST_IS_CLANG
PARALLEL_DIRS := $(filter-out 126.gcc, $(PARALLEL_DIRS))
endif

# Get the $(ARCH) setting
include $(LEVEL)/Makefile.config

ifeq ($(ARCH),x86_64)
PARALLEL_DIRS := $(filter-out 126.gcc, $(PARALLEL_DIRS))
endif

# Include the standard makefiles
include ${LEVEL}/External/Makefile.external
