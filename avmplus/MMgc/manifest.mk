# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

MMgc_INCLUDES += $(AVM_INCLUDES)

ifdef USE_GCC_PRECOMPILED_HEADERS
MMgc_PCH_SRC := $(srcdir)/MMgc.h
MMgc_PCH := $(curdir)/MMgc-precompiled.h
endif

ifdef MMGC_DYNAMIC
DLLS += MMgc
MMgc_EXTRA_CPPFLAGS += $(DLL_CFLAGS)
else
STATIC_LIBRARIES += MMgc
endif
MMgc_BUILD_ALL = 1

MMgc_EXTRA_CPPFLAGS += $(MMGC_CPPFLAGS)

MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/FixedAlloc.cpp \
  $(curdir)/FixedMalloc.cpp \
  $(curdir)/GC.cpp \
  $(curdir)/GCAlloc.cpp \
  $(curdir)/GCAllocObject.cpp \
  $(curdir)/GCDebug.cpp \
  $(curdir)/GCHashtable.cpp \
  $(curdir)/GCHeap.cpp \
  $(curdir)/GCLargeAlloc.cpp \
  $(curdir)/GCLog.cpp \
  $(curdir)/GCMemoryProfiler.cpp \
  $(curdir)/GCObject.cpp \
  $(curdir)/GCPolicyManager.cpp \
  $(curdir)/GCStack.cpp \
  $(curdir)/GCTests.cpp \
  $(curdir)/GCThreads.cpp \
  $(curdir)/PageMap.cpp \
  $(curdir)/ZCT.cpp \
  $(curdir)/GCGlobalNew.cpp \
  $(NULL)

$(curdir)/GCDebugMac.$(OBJ_SUFFIX): CXXFLAGS += -Wno-deprecated-declarations
