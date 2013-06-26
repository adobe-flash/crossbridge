##===- projects/llvm-test/Makefile -------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
#
# This recursively traverses the programs, building them as necessary.  This
# makefile also implements 'make report TEST=<x>'.
#
##===----------------------------------------------------------------------===##

LEVEL = .
PARALLEL_DIRS = SingleSource MultiSource External

include $(LEVEL)/Makefile.programs

build-for-llvm-top:
	./configure --with-llvmsrc=$(LLVM_TOP)/llvm --with-llvmobj=$(LLVM_TOP)/llvm --srcdir=$(LLVM_TOP)/test-suite --with-llvmgccdir=$(LLVM_TOP)/install --with-externals=$(LLVM_TOP)/externals
	$(MAKE)
