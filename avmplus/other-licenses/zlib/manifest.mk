#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.


STATIC_LIBRARIES += zlib

zlib_BUILD_ALL = 1

# When compiling the debug versions on windows, need to set -DHAVE_VSNPRINTF 
# otherwise the following error will be produced when compiling:
#  error C3163: '_vsnprintf': attributes inconsistent with previous declaration
ifdef ENABLE_DEBUG
ifeq ($(COMPILER),VS)
    zlib_EXTRA_CFLAGS += -DHAVE_VSNPRINTF
endif
endif


zlib_CSRCS := $(zlib_CSRCS) \
  $(curdir)/adler32.c \
  $(curdir)/compress.c \
  $(curdir)/crc32.c \
  $(curdir)/deflate.c \
  $(curdir)/gzio.c \
  $(curdir)/infback.c \
  $(curdir)/inffast.c \
  $(curdir)/inflate.c \
  $(curdir)/inftrees.c \
  $(curdir)/trees.c \
  $(curdir)/uncompr.c \
  $(curdir)/zutil.c \
  $(NULL)


