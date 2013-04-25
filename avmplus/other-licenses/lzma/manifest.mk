#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

STATIC_LIBRARIES += lzma

lzma_BUILD_ALL = 1

lzma_CSRCS := $(lzma_CSRCS) \
  $(curdir)/Alloc.c \
  $(curdir)/Bcj2.c \
  $(curdir)/Bra.c \
  $(curdir)/Bra86.c \
  $(curdir)/LzFind.c \
  $(curdir)/Lzma2Dec.c \
  $(curdir)/LzmaDec.c \
  $(curdir)/LzmaLib.c \
  $(curdir)/LzmaEnc.c \
  $(NULL)


