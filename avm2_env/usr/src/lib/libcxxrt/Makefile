# $FreeBSD: release/9.1.0/lib/libcxxrt/Makefile 236600 2012-06-05 06:41:47Z dim $

SRCDIR=		${.CURDIR}/../../contrib/libcxxrt

SHLIB_MAJOR=	1
SHLIBDIR?=	/lib

.PATH: ${SRCDIR}

LIB=		cxxrt

SRCS+=		libelftc_dem_gnu3.c\
		terminate.cc\
		dynamic_cast.cc\
		memory.cc\
		auxhelper.cc\
		exception.cc\
		stdexcept.cc\
		typeinfo.cc\
		guard.cc

WARNS=		0
CFLAGS+=	-I${SRCDIR}
VERSION_MAP=	${.CURDIR}/Version.map

.include <bsd.lib.mk>
