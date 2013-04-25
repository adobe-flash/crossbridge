#
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

MODULES_ZLIB_SRC_LCSRCS = \
		adler32.c \
		compress.c \
		crc32.c \
		deflate.c \
		gzio.c \
		infback.c \
		inffast.c \
		inflate.c \
		inftrees.c \
		trees.c \
		uncompr.c \
		zutil.c \
		$(NULL)

MODULES_ZLIB_SRC_LEXPORTS = zlib.h zconf.h mozzconf.h

MODULES_ZLIB_SRC_CSRCS := $(addprefix $(topsrcdir)/modules/zlib/src/, $(MODULES_ZLIB_SRC_LCSRCS))
MODULES_ZLIB_SRC_EXPORTS := $(addprefix $(topsrcdir)/modules/zlib/src/, $(MODULES_ZLIB_SRC_LEXPORTS))

