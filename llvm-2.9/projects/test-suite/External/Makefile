LEVEL = ..

#
# Include the configuration so that we know whether or not to include SPEC
#
include $(LEVEL)/Makefile.config

#
# Create the list of directories to compile
#
PARALLEL_DIRS := SPEC Povray Namd FPGrowth BoxedSim skidmarks10

ifneq (,$(findstring x86,$(ARCH)))
# Nurbs uses SSE and only works on x86 and x86_64.
PARALLEL_DIRS += Nurbs
endif

ifndef USE_POVRAY
PARALLEL_DIRS := $(filter-out Povray, $(PARALLEL_DIRS))
endif

ifndef USE_NAMD
PARALLEL_DIRS := $(filter-out Namd, $(PARALLEL_DIRS))
endif

ifndef USE_FPGROWTH
PARALLEL_DIRS := $(filter-out FPGrowth, $(PARALLEL_DIRS))
endif

ifndef USE_BOXEDSIM
PARALLEL_DIRS := $(filter-out BoxedSim, $(PARALLEL_DIRS))
endif

ifndef USE_NURBS
PARALLEL_DIRS := $(filter-out Nurbs, $(PARALLEL_DIRS))
endif

ifndef USE_HMMER
PARALLEL_DIRS := $(filter-out HMMER, $(PARALLEL_DIRS))
endif

ifndef USE_SKIDMARKS10
PARALLEL_DIRS := $(filter-out skidmarks10, $(PARALLEL_DIRS))
endif

# Sparc can't handle Namd: infinite loop, cause unknown
ifeq ($(ARCH),Sparc)
PARALLEL_DIRS := $(filter-out Namd, $(PARALLEL_DIRS))
endif

ifdef DISABLE_CXX
PARALLEL_DIRS := $(filter-out Nurbs, $(PARALLEL_DIRS))
endif

include Makefile.external
