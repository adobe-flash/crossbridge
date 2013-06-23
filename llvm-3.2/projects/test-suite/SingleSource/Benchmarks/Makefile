LEVEL = ../..
PARALLEL_DIRS := Dhrystone CoyoteBench Shootout Shootout-C++ Stanford McGill \
              Misc Misc-C++ Misc-C++-EH BenchmarkGame Adobe-C++ Polybench
LDFLAGS += -lm

# Disable Polybench tests on ARM for the time being, they are very long running.
ifeq ($(ARCH),ARM)
PARALLEL_DIRS := $(filter-out Polybench, $(PARALLEL_DIRS))
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc
