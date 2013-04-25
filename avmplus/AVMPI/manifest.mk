# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

ifeq (windows,$(TARGET_OS))
MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/AvmAssert.cpp \
  $(curdir)/float4Support.cpp \
  $(curdir)/MMgcPortWin.cpp \
  $(curdir)/SpyUtilsWin.cpp \
  $(NULL)
endif

ifeq (darwin,$(TARGET_OS))
MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/AvmAssert.cpp \
  $(curdir)/float4Support.cpp \
  $(curdir)/PosixMMgcPortUtils.cpp \
  $(curdir)/MMgcPortMac.cpp \
  $(curdir)/SpyUtilsPosix.cpp \
  $(NULL)
endif

ifeq (linux,$(TARGET_OS))
MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/AvmAssert.cpp \
  $(curdir)/float4Support.cpp \
  $(curdir)/PosixMMgcPortUtils.cpp \
  $(curdir)/MMgcPortUnix.cpp \
  $(NULL)
endif

ifeq (android,$(TARGET_OS))
MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/AvmAssert.cpp \
  $(curdir)/float4Support.cpp \
  $(curdir)/PosixMMgcPortUtils.cpp \
  $(curdir)/MMgcPortUnix.cpp \
  $(NULL)
endif

ifeq (sunos,$(TARGET_OS))
MMgc_CXXSRCS := $(MMgc_CXXSRCS) \
  $(curdir)/AvmAssert.cpp \
  $(curdir)/float4Support.cpp \
  $(curdir)/PosixMMgcPortUtils.cpp \
  $(curdir)/MMgcPortUnix.cpp \
  $(NULL)
endif


