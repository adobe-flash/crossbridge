# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

PROGRAMS += shell
shell_INCLUDES += $(AVM_INCLUDES)

shell_BASENAME = avmshell
shell_INCLUDES += -I$(srcdir) -I$(topsrcdir)/extensions 
shell_DEFINES = -DAVMPLUS_SHELL
shell_STATIC_LIBRARIES = lzma zlib avmplus MMgc vmbase
ifeq (1,$(ENABLE_AOT))
shell_STATIC_LIBRARIES += aotstubs aot
endif
shell_DIR := $(curdir)/
shell_EXTRA_CPPFLAGS := $(AVMSHELL_CPPFLAGS)
shell_EXTRA_LDFLAGS := $(AVMSHELL_LDFLAGS)

ifdef ENABLE_SHELL
shell_BUILD_ALL = 1
endif

shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshell.cpp \
  $(curdir)/ConsoleOutputStream.cpp \
  $(curdir)/DebugCLI.cpp \
  $(curdir)/DomainClass.cpp \
  $(curdir)/FileClass.cpp \
  $(curdir)/FileInputStream.cpp \
  $(curdir)/ShellCore.cpp \
  $(curdir)/SystemClass.cpp \
  $(curdir)/swf.cpp \
  $(curdir)/../extensions/SamplerScript.cpp \
  $(curdir)/../extensions/Selftest.cpp \
  $(curdir)/../extensions/SelftestInit.cpp \
  $(curdir)/../extensions/SelftestExec.cpp \
  $(curdir)/ShellPosixGlue.cpp \
  $(curdir)/ShellWorkerGlue.cpp \
  $(curdir)/ShellWorkerDomainGlue.cpp \
  $(NULL)

ifeq (windows,$(TARGET_OS))
shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshellWin.cpp \
  $(curdir)/WinFile.cpp \
  $(NULL)
endif

ifeq (darwin,$(TARGET_OS))
shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshellMac.cpp \
  $(curdir)/PosixFile.cpp \
  $(curdir)/PosixPartialPlatform.cpp \
  $(NULL)
endif

ifeq (linux,$(TARGET_OS))
shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshellUnix.cpp \
  $(curdir)/PosixFile.cpp \
  $(curdir)/PosixPartialPlatform.cpp \
  $(NULL)
endif

ifeq (android,$(TARGET_OS))
shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshellUnix.cpp \
  $(curdir)/PosixFile.cpp \
  $(curdir)/PosixPartialPlatform.cpp \
  $(NULL)
endif

ifeq (sunos,$(TARGET_OS))
shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/avmshellUnix.cpp \
  $(curdir)/PosixFile.cpp \
  $(curdir)/PosixPartialPlatform.cpp \
  $(NULL)
endif

# See manifest.mk in root directory for the dependencies
# on $(topsrcdir)/generated/shell_toplevel.h

# Use of '%' [to force a pattern-rule] instead of '$(curdir)/..' or
# '$(topsrcdir)' [which would then not be a pattern-rule] is crucial
# (ie "deliberate", ie "hack"); see Bug 632086
%/generated/shell_toplevel.h %/generated/shell_toplevel.cpp %/generated/shell_toplevel.abc: $(topsrcdir)/shell/shell_toplevel.as $(topsrcdir)/core/api-versions.as %/generated/builtin.abc
	cd $(topsrcdir)/shell; python shell_toplevel.py $(shell_BUILTINFLAGS)

.PHONY: shell-tracers
# Order dependence on core-tracers is to force a strict ordering
# because both tracer generation scripts will attempt to recompile
# exactgc.abc if it is missing.
shell-tracers: $(topsrcdir)/generated/builtin.abc $(topsrcdir)/generated/shell_toplevel.abc | core-tracers
ifdef AVMSHELL_TOOL
	$(MSG)true "Generating shell-tracers via AVMSHELL_TOOL=$(AVMSHELL_TOOL)"
	$(CMD)cd $(topsrcdir)/shell; AVM=$(AVMSHELL_TOOL) python ./shell_toplevel-tracers.py
else
	$(MSG)true "Skipping shell-tracers generation since AVMSHELL_TOOL unset"
endif

# 1. Use of '$(topsrcdir)/generated' is deliberate; we use absolute
#    paths for code being generated (or referenced) outside build dir.
#
# 2. Use of '$(curdir)/ShellCore.$(II_SUFFIX)' is also deliberate:
#    preprocessed file as target must be specified via same path that
#    is used in root manifest.mk.
#
# Further discussion at Bug 632086.
$(curdir)/ShellCore.$(II_SUFFIX): $(topsrcdir)/generated/shell_toplevel.cpp
