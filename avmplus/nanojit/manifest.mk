
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

nanojit_cpu_cxxsrc = $(error Unrecognized target CPU.)

ifeq (i686,$(TARGET_CPU))
nanojit_cpu_cxxsrc := Nativei386.cpp
endif

ifeq (x86_64,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeX64.cpp
endif

ifeq (powerpc,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativePPC.cpp
endif

ifeq (ppc64,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativePPC.cpp
endif

ifeq (arm,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeARM.cpp
endif

ifeq (thumb2,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeThumb2.cpp
endif

ifeq (sparc,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeSparc.cpp
endif

ifeq (sh4,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeSH4.cpp
endif

ifeq (mips,$(TARGET_CPU))
nanojit_cpu_cxxsrc := NativeMIPS.cpp
endif

avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/Allocator.cpp \
  $(curdir)/Assembler.cpp \
  $(curdir)/CodeAlloc.cpp \
  $(curdir)/Containers.cpp \
  $(curdir)/Fragmento.cpp \
  $(curdir)/LIR.cpp \
  $(curdir)/njconfig.cpp \
  $(curdir)/RegAlloc.cpp \
  $(curdir)/$(nanojit_cpu_cxxsrc) \
  $(NULL)

ifeq ($(COMPILER),VS)
# Disable the 'cast truncates constant value' warning, incurred by
# macros encoding instruction operands in machine code fields.
$(curdir)/Assembler.obj $(curdir)/Nativei386.obj: avmplus_CXXFLAGS += -wd4310
endif
