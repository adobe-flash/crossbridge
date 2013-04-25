# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

STATIC_LIBRARIES += vmbase
vmbase_BUILD_ALL = 1
vmbase_INCLUDES += $(VMBASE_INCLUDES)

ifdef USE_GCC_PRECOMPILED_HEADERS
vmbase_PCH_SRC := $(srcdir)/vmbase.h
vmbase_PCH := $(curdir)/vmbase-precompiled.h
endif

vmbase_CXXSRCS := $(vmbase_CXXSRCS) \
  $(curdir)/VMThread.cpp \
  $(curdir)/Safepoint.cpp \
  $(NULL)

# Bug 668442: WinPortUtils.cpp VMPI_getDaylightSavingsTA issue
# Revision buggy, original breaks gtest; disabling gtest until revision fixed.
# $(call RECURSE_DIRS,unittest)
