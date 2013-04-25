# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

ifeq (sunos,$(TARGET_OS))
mathutils_cxxsrc = MathUtilsSolaris.cpp
unixcpuid_cxxsrc = solariscpuid.cpp
else
mathutils_cxxsrc = MathUtilsUnix.cpp
unixcpuid_cxxsrc = unixcpuid.cpp
endif

avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/$(mathutils_cxxsrc) \
  $(curdir)/$(unixcpuid_cxxsrc) \
  $(curdir)/OSDepUnix.cpp \
  $(NULL)
