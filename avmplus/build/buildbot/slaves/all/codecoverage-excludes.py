#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# the script outputs a list of exclude patterns

# the exclude pattern to print
exclude_pattern=""

# document each exclude with a reason

# exclude the self tests, they are not part of the source code
exclude_pattern+="!extensions/ST_*.cpp !extensions/Selftest*.cpp "

# exclude generated code
exclude_pattern+="!generated/shell_toplevel.h !generated/shell_toplevel.cpp !generated/shell_toplevel*.hh "

# exclude Memory Profiler not part of flash player shipping code
exclude_pattern+="!MMgc/GCMemoryProfiler.h !MMgc/GCMemoryProfiler.cpp "

# exclude other-licenses, zlib and pcre, they are external packages
exclude_pattern+="!other-licenses/ !pcre/ "

# exclude the shell code, it is not part of the flash player shipping code
exclude_pattern+="!shell/ "

# exclude debugging utilities
exclude_pattern+="!vmbase/AvmAssert.cpp !VMPI/MacDebugUtils.cpp !VMPI/SpyUtils*.cpp !VMPI/*DebugUtils.cpp "

# exclude Sampler
exclude_pattern+="!extensions/SamplerScript.h "

# exclude vprof and eval tools, not part of flash player shipping code
exclude_pattern+="!vprof/ !eval/ "

# exclude gtest
exclude_pattern+="!gtest/ "

# The following functions in MathClass and MathUtils have been excluded
# from the coverage calculation as there are currently not reachable
# (not called from any other source):
# https://bugzilla.mozilla.org/show_bug.cgi?id=592728
exclude_pattern+="'!avmplus::MathClass::max2(double,double)' "
exclude_pattern+="'!avmplus::MathClass::min2(double,double)' "
exclude_pattern+="'!avmplus::MathUtils::equals(double,double)' "
exclude_pattern+="'!avmplus::MathUtils::infinity()' "
exclude_pattern+="'!avmplus::MathUtils::max(double,double)' "
exclude_pattern+="'!avmplus::MathUtils::min(double,double)' "
exclude_pattern+="'!avmplus::MathUtils::nan()' "
exclude_pattern+="'!avmplus::MathUtils::neg_infinity()' "
exclude_pattern+="'!avmplus::MathUtils::nextDigit(double*)' "
exclude_pattern+="'!avmplus::MathUtils::powerOfTen(int32_t,double)' "
exclude_pattern+="'!avmplus::MathUtils::roundInt(double)' "

# print exclude pattern
print(exclude_pattern)
