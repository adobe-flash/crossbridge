# MultiSource/Applications/Makefile:  Build all subdirectories automatically

LEVEL = ../..

include $(LEVEL)/Makefile.config

PARALLEL_DIRS  = Burg aha sgefa siod d spiff treecc SPASS \
                 oggenc JM viterbi SIBsim4 ClamAV sqlite3 lemon
ifndef DISABLE_CXX
PARALLEL_DIRS += lambda-0.1.3 hbd hexxagon minisat 
endif

ifndef SMALL_PROBLEM_SIZE
PARALLEL_DIRS += lua
endif

# Obsequi uses Linux-only features; need to fix that
ifeq ($(TARGET_OS),Linux)
PARALLEL_DIRS += obsequi
endif

# kimwitu++ uses multi-byte chars, not available on sparc/solaris
ifneq ($(TARGET_OS),SunOS)
ifndef DISABLE_CXX
PARALLEL_DIRS += kimwitu++
endif
endif

ifeq ($(TARGET_OS),SunOS)
PARALLEL_DIRS := $(filter-out SPASS, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out hexxagon, $(PARALLEL_DIRS))
endif

include $(LEVEL)/Makefile.programs
