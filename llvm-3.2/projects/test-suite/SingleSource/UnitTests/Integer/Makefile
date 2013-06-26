# SingleSource/UnitTests/Integer/Makefile
LEVEL = ../../..

# You can use this seed value to get a different test
# each day, but don't forget to 'make clean' To use it,
# change RUN_OPTIONS (below) to be $(SEED)
SEED = $(shell date +%j)

# Use a consistent seed value for the tests so they produce
# the same results regardless of whether you make clean or not.
RUN_OPTIONS := 31415926

# Define this to 1 to test larger integer (>64 bits) types.
ENABLE_LARGE_INTEGERS := 0

# None of these test cases can use a native code generator because
# those targets are not set up to deal with arbitrary precision
# integer types. Consequently, we must disable the LLI, LLC, 
# LLC-BETA, and JIT test cases.

DISABLE_LLC := 1
DISABLE_JIT := 1
ENABLE_LLI  := 1

# Define USE_REFERENCE_OUTPUT which is a signal to Makefile.programs
# that the native compiler should not be run but that its output
# should be "faked" by copying the reference output. This is 
# necessary because gcc doesn't support the bitwidth attribute and 
# therefore can't generate correct output.

USE_REFERENCE_OUTPUT := 1


LDFLAGS += -lstdc++
ifeq (1,$(ENABLE_LARGE_INTEGERS))
CPPFLAGS += -DENABLE_LARGE_INTEGERS
endif
include $(LEVEL)/SingleSource/Makefile.singlesrc
