# $FreeBSD: release/9.1.0/lib/libblocksruntime/Makefile 215125 2010-11-11 15:13:11Z ed $

LIB=	BlocksRuntime
SHLIB_MAJOR=0
CFLAGS+=-I${.CURDIR}
WARNS?=	2

.PATH: ${.CURDIR}/../../contrib/compiler-rt/BlocksRuntime

INCS=	Block.h Block_private.h
SRCS=	data.c runtime.c

.include <bsd.lib.mk>
