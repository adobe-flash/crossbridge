# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

ifdef USE_GCC_PRECOMPILED_HEADERS
eval_PCH := $(curdir)/avmplus-precompiled.h
endif

shell_CXXSRCS := $(shell_CXXSRCS) \
  $(curdir)/eval-abc.cpp \
  $(curdir)/eval-avmplus.cpp \
  $(curdir)/eval-cogen.cpp \
  $(curdir)/eval-cogen-expr.cpp \
  $(curdir)/eval-cogen-stmt.cpp \
  $(curdir)/eval-compile.cpp \
  $(curdir)/eval-lex.cpp \
  $(curdir)/eval-lex-xml.cpp \
  $(curdir)/eval-parse.cpp \
  $(curdir)/eval-parse-config.cpp \
  $(curdir)/eval-parse-expr.cpp \
  $(curdir)/eval-parse-stmt.cpp \
  $(curdir)/eval-parse-xml.cpp \
  $(curdir)/eval-unicode.cpp \
  $(curdir)/eval-util.cpp \
  $(NULL)
