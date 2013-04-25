# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

PROGRAMS += gtest_vmbase

gtest_vmbase_BUILD_ALL = 1
gtest_vmbase_INCLUDES += $(VMBASE_INCLUDES) $(GTEST_INCLUDES)
gtest_vmbase_STATIC_LIBRARIES = vmbase gtest

gtest_vmbase_CXXSRCS := $(gtest_vmbase_CXXSRCS) \
  $(curdir)/vmbase_concurrency_test.cpp \
  $(curdir)/vmbase_safepoints_test.cpp \
  $(curdir)/vmbase_threads_test.cpp \
  $(NULL)
