# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

STATIC_LIBRARIES += gtest
gtest_BUILD_ALL = 1

gtest_CXXSRCS := $(gtest_CXXSRCS) \
  $(curdir)/gtest_main.cpp \
  $(curdir)/gtest-all.cpp \
  $(NULL)
