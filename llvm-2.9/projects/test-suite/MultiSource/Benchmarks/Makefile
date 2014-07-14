# MultiSource/Benchmarks Makefile:  Build all subdirectories automatically

LEVEL = ../..
# Disabled: OptimizerEval.
PARALLEL_DIRS := Fhourstones Fhourstones-3.1 \
                 McCat Olden Ptrdist llubenchmark \
                 sim FreeBench MallocBench Prolangs-C SciMark2-C\
                 mediabench ASCI_Purple MiBench Trimaran VersaBench NPB-serial\
                 BitBench ASC_Sequoia

ifndef DISABLE_CXX
PARALLEL_DIRS += Prolangs-C++ PAQ8p tramp3d-v4 Bullet
endif

ifndef SMALL_PROBLEM_SIZE
PARALLEL_DIRS += \
		mafft
endif

include $(LEVEL)/Makefile.programs
