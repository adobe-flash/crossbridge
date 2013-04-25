# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Note: this adds the C sources to the vmbase library build.

ifeq (windows,$(TARGET_OS))
vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/WinDebugUtils.cpp \
  $(curdir)/WinPortUtils.cpp \
  $(curdir)/GenericPortUtils.cpp \
  $(curdir)/ThreadsWin.cpp \
  $(NULL)
endif

ifeq (darwin,$(TARGET_OS))
vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/MacDebugUtils.cpp \
  $(curdir)/MacPortUtils.cpp \
  $(curdir)/PosixPortUtils.cpp \
  $(curdir)/GenericPortUtils.cpp \
  $(curdir)/ThreadsPosix.cpp \
  $(NULL)
endif

ifeq (linux,$(TARGET_OS))
vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/PosixPortUtils.cpp \
  $(curdir)/PosixSpecificUtils.cpp \
  $(curdir)/UnixDebugUtils.cpp \
  $(curdir)/GenericPortUtils.cpp \
  $(curdir)/ThreadsPosix.cpp \
  $(NULL)
endif

ifeq (android,$(TARGET_OS))
vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/PosixPortUtils.cpp \
  $(curdir)/PosixSpecificUtils.cpp \
  $(curdir)/UnixDebugUtils.cpp \
  $(curdir)/GenericPortUtils.cpp \
  $(curdir)/ThreadsPosix.cpp \
  $(NULL)
endif

ifeq (sunos,$(TARGET_OS))
vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/PosixPortUtils.cpp \
  $(curdir)/PosixSpecificUtils.cpp \
  $(curdir)/UnixDebugUtils.cpp \
  $(curdir)/GenericPortUtils.cpp \
  $(curdir)/ThreadsPosix.cpp \
  $(NULL)
endif
