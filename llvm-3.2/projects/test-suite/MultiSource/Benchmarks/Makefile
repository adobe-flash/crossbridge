# MultiSource/Benchmarks Makefile:  Build all subdirectories automatically

LEVEL = ../..

include $(LEVEL)/Makefile.config

# Disabled: OptimizerEval.
PARALLEL_DIRS := Fhourstones Fhourstones-3.1 \
                 McCat Olden Ptrdist llubenchmark \
                 sim FreeBench MallocBench Prolangs-C SciMark2-C mediabench\
                 nbench ASCI_Purple MiBench Trimaran VersaBench NPB-serial\
                 BitBench ASC_Sequoia TSVC

# Disable TSVC on Darwin until the tests support SMALL_PROBLEM_SIZE=1.
ifeq ($(TARGET_OS),Darwin)
ifeq ($(ARCH),ARM)
PARALLEL_DIRS := $(filter-out TSVC,$(PARALLEL_DIRS))
endif
endif

ifndef DISABLE_CXX
PARALLEL_DIRS += Prolangs-C++ PAQ8p tramp3d-v4 Bullet
endif

ifndef SMALL_PROBLEM_SIZE
PARALLEL_DIRS += \
		mafft
endif

include $(LEVEL)/MultiSource/Makefile.multisrc
