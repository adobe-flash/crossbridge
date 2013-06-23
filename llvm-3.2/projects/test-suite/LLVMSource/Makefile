#                  LLVMSource/Makefile
#
# This directory contains regression tests for LLVM backends written in LLVM
# assembly language.
#
LEVEL = ..
DISABLE_FOR_LLVM_PROGRAMS := 1
REQUIRES_EH_SUPPORT := 1

PROGRAMS_TO_TEST := $(basename $(wildcard *.ll))
include $(LEVEL)/Makefile.programs

LLVM_SOURCE_BYTECODES := $(PROGRAMS_TO_TEST:%=Output/%.llvm.bc)

$(LLVM_SOURCE_BYTECODES): \
Output/%.llvm.bc: %.ll $(LLVMAS) Output/.dir
	$(LLVMAS) $< -f -o $@

ifdef TARGET_HAS_JIT
all:: $(JITOUTPUT)
endif

ifndef DISABLE_LLC_DIFFS
all:: $(LLCOUTPUT)
endif

