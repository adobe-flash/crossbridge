# $FreeBSD$

# Path from build/mac/lib/.... to llvm-gcc
HDRDIR=		${.CURDIR}
HDRDIR+=	-I${.CURDIR}/../../../../../../llvm-gcc-4.2-2.9/gcc
HDRDIR+=	${.CURDIR}/../../../../../../llvm-gcc-4.2-2.9/include
SRCDIR=		${.CURDIR}


SHLIB_MAJOR=	1
SHLIBDIR?=	/lib

.PATH: ${SRCDIR}

LIB=		gcceh

SRCS+=		unwind-c.c \
		unwind-dw2-fde.c \
		unwind-dw2.c \
		unwind-sjlj.c \
		unwind.inc \
		gthr-gnat.c

WARNS=		0
CFLAGS+=	-I${HDRDIR} -m32 -fPIC -O2 -DIN_GCC -D__USING_SJLJ_EXCEPTIONS__
CFLAGS+=	-DHAVE_GTHR_DEFAULT -DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED
.include <bsd.lib.mk>

