# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/coff.cpp \
  $(curdir)/MathUtilsWin.cpp \
  $(curdir)/NativeFunctionWin.cpp \
  $(curdir)/OSDepWin.cpp \
  $(NULL)

ifeq (i686,$(TARGET_CPU))
avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/win32cpuid.cpp \
  $(NULL)
endif

ifeq (x86_64,$(TARGET_CPU))
ifeq (windows,$(TARGET_OS))
avmplus_MASMSRCS := $(avmplus_MASMSRCS) \
  $(curdir)/win64setjmp.asm
  $(NULL)
endif
endif


ifeq (thumb2,$(TARGET_CPU))
ifeq (windows,$(TARGET_OS))
avmplus_ASMSRCS := $(avmplus_ASMSRCS) \
  $(curdir)/win32armsetjmp.armasm
  $(NULL)
endif
endif
