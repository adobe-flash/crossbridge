##===- lib/Target/AVM2/Makefile ---------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file was developed by the LLVM research group and is distributed under
# the University of Illinois Open Source License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../../..
LIBRARYNAME = LLVMAVM2CodeGen
TARGET = AVM2

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = AVM2GenRegisterInfo.h.inc AVM2GenRegisterNames.inc \
                AVM2GenRegisterInfo.inc AVM2GenInstrNames.inc \
                AVM2GenInstrInfo.inc AVM2GenAsmWriter.inc \
                AVM2GenDAGISel.inc AVM2GenSubtarget.inc \
                AVM2GenCallingConv.inc

DIRS = AsmPrinter TargetInfo

include $(LEVEL)/Makefile.common

