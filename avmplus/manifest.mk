# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

CPPFLAGS += -DGTEST_HAS_RTTI=0 -D_HAS_EXCEPTIONS=0

GTEST_INCLUDES = \
  -I$(topsrcdir)/gtest/gtest-1.6.0/include \
  $(NULL)
INCLUDES := $(GTEST_INCLUDES)

VMBASE_INCLUDES = \
  -I$(topsrcdir)/platform \
  -I$(topsrcdir)/VMPI \
  -I$(topsrcdir)/vmbase \
  $(NULL)

# shell must be included here because that's where avmshell-features.h lives,
# and in shell builds it is included from platform/AVMPI.h.  That's how it
# is supposed to be.
AVM_INCLUDES = \
  $(VMBASE_INCLUDES) \
  -I$(topsrcdir) \
  -I$(topsrcdir)/MMgc \
  -I$(topsrcdir)/core \
  -I$(topsrcdir)/pcre \
  -I$(topsrcdir)/eval \
  -I$(topsrcdir)/platform \
  -I$(topsrcdir)/other-licenses/zlib \
  -I$(topsrcdir)/other-licenses/lzma \
  -I$(topsrcdir)/shell \
  -I$(topsrcdir)/AVMPI \
  -I$(topsrcdir)/generated \
  -I$(topsrcdir)/aot \
  $(NULL)

# This line could just as easily go into core/manifest.mk, but putting it here
# emphasizes that we should not simply include everything everywhere.
avmplus_INCLUDES += $(AVM_INCLUDES)

$(call RECURSE_DIRS,other-licenses/zlib)
$(call RECURSE_DIRS,other-licenses/lzma)

# Bug 668442: WinPortUtils.cpp VMPI_getDaylightSavingsTA issue
# Revision buggy, original breaks gtest; disabling gtest until revision fixed.
# $(call RECURSE_DIRS,gtest)

$(call RECURSE_DIRS,VMPI)
$(call RECURSE_DIRS,vmbase)

$(call RECURSE_DIRS,AVMPI)
$(call RECURSE_DIRS,MMgc)
$(call RECURSE_DIRS,halfmoon)

ifdef ENABLE_TAMARIN
$(call RECURSE_DIRS,core pcre vprof)
ifeq (1,$(ENABLE_AOT))
aot_INCLUDES += $(AVM_INCLUDES)
$(call RECURSE_DIRS,aot)
endif
ifeq (sparc,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (i686,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (x86_64,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (thumb2,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (arm,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (powerpc,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (ppc64,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (mips,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (sh4,$(TARGET_CPU))
$(call RECURSE_DIRS,nanojit)
endif
ifeq (darwin,$(TARGET_OS))
$(call RECURSE_DIRS,platform/mac)
endif
ifeq (windows,$(TARGET_OS))
$(call RECURSE_DIRS,platform/win32)
endif
ifeq (linux,$(TARGET_OS))
$(call RECURSE_DIRS,platform/unix)
endif
ifeq (android,$(TARGET_OS))
$(call RECURSE_DIRS,platform/unix)
endif
ifeq (sunos,$(TARGET_OS))
$(call RECURSE_DIRS,platform/unix)
endif
endif

$(call RECURSE_DIRS,eval)
$(call RECURSE_DIRS,shell)

# Bug 632086: These definitions must come *after* the foo_CXXSRCS
# variables have been completely populated.
MMgc_PREPROCESSED := $(MMgc_CXXSRCS:.cpp=.$(II_SUFFIX)) $(MMgc_PCH:.h=.$(II_SUFFIX))
avmplus_PREPROCESSED := $(avmplus_CXXSRCS:.cpp=.$(II_SUFFIX)) $(avmplus_PCH:.h=.$(II_SUFFIX))
vmbase_PREPROCESSED := $(vmbase_CXXSRCS:.cpp=.$(II_SUFFIX)) $(vmbase_PCH:.h=.$(II_SUFFIX))
shell_PREPROCESSED := $(shell_CXXSRCS:.cpp=.$(II_SUFFIX))

# Bug 632086: Tie generated code for .h and .cpp together, so that
# requiring the regeneration of the .cpp file will force the
# regeneration of both to happen before the .h file is read.
GENERATED_BUILTIN_CODE := \
 $(topsrcdir)/generated/builtin.h \
 $(topsrcdir)/generated/builtin.cpp \
 $(topsrcdir)/generated/builtin.abc
GENERATED_SHELL_CODE := \
 $(topsrcdir)/generated/shell_toplevel.h \
 $(topsrcdir)/generated/shell_toplevel.cpp \
 $(topsrcdir)/generated/shell_toplevel.abc

$(MMgc_PREPROCESSED): | core-tracers
$(avmplus_PREPROCESSED): $(GENERATED_BUILTIN_CODE) | core-tracers
$(vmbase_PREPROCESSED): $(GENERATED_BUILTIN_CODE) | core-tracers
$(shell_PREPROCESSED): $(GENERATED_BUILTIN_CODE) | core-tracers
$(shell_PREPROCESSED): $(GENERATED_SHELL_CODE) | shell-tracers

$(avmplus_CXXOBJS): $(vmbase_PCH).$(PCH_SUFFIX) $(MMgc_PCH).$(PCH_SUFFIX) $(avmplus_PCH).$(PCH_SUFFIX)
$(MMgc_CXXOBJS): $(vmbase_PCH).$(PCH_SUFFIX) $(MMgc_PCH).$(PCH_SUFFIX) $(avmplus_PCH).$(PCH_SUFFIX)
$(vmbase_CXXOBJS): $(vmbase_PCH).$(PCH_SUFFIX)

echo:
	@echo avmplus_CXXFLAGS = $(avmplus_CXXFLAGS)
	@echo avmplus_CXXSRCS = $(avmplus_CXXSRCS)
	@echo avmplus_CXXOBJS = $(avmplus_CXXOBJS)
	@echo avmplus_OBJS = $(avmplus_OBJS)
	@echo avmplus_NAME = $(avmplus_NAME)
	@echo avmplus_BUILTINFLAGS = $(avmplus_BUILTINFLAGS)
	@echo avmplus_PCH_OBJ = $(avmplus_PCH_OBJ)
	@echo MMgc_CXXSRCS = $(MMgc_CXXSRCS)
	@echo MMgc_CXXOBJS = $(MMgc_CXXOBJS)
	@echo MMgc_OBJS = $(MMgc_OBJS)
	@echo MMgc_NAME = $(MMgc_NAME)
	@echo MMgc_BUILTINFLAGS = $(MMgc_BUILTINFLAGS)
	@echo MMgc_PCH = $(MMgc_PCH)
	@echo MMgc_PCH_OBJ = $(MMgc_PCH_OBJ)
	@echo vmbase_CXXSRCS = $(vmbase_CXXSRCS)
	@echo vmbase_CXXOBJS = $(vmbase_CXXOBJS)
	@echo vmbase_OBJS = $(vmbase_OBJS)
	@echo vmbase_NAME = $(vmbase_NAME)
	@echo vmbase_BUILTINFLAGS = $(vmbase_BUILTINFLAGS)
	@echo vmbase_PCH_OBJ = $(vmbase_PCH_OBJ)
