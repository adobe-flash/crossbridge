LEVEL = ../../..
PARALLEL_DIRS := \
	164.gzip \
	175.vpr \
	176.gcc \
	181.mcf \
	186.crafty \
	197.parser \
	252.eon \
	253.perlbmk \
	254.gap \
	255.vortex \
	256.bzip2 \
	300.twolf

# Get the $(ARCH) setting
include $(LEVEL)/Makefile.config

# Disable crafty until it stops infinite-looping on Sparc
ifeq ($(ARCH), Sparc)
PARALLEL_DIRS := $(filter-out 176.gcc, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 186.crafty, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 252.eon, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 253.perlbmk, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 254.gap, $(PARALLEL_DIRS))

endif

ifdef DISABLE_CXX
PARALLEL_DIRS := $(filter-out 252.eon, $(PARALLEL_DIRS))
endif

include $(LEVEL)/Makefile.programs
