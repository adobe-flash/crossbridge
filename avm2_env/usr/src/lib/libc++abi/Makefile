# $FreeBSD$

HDRDIR=		${.CURDIR}/../../contrib/libc++abi/include
HDRDIR+=	-I${.CURDIR}/../../contrib/libc++abi/src
HDRDIR+=	-I${.CURDIR}/../libgcceh
HDRDIR+=	-I${.CURDIR}/../../include/c++/v1
SRCDIR=		${.CURDIR}/../../contrib/libc++abi/src

.PATH: ${SRCDIR}

LIB=		c++abi
SHLIB_MAJOR=	1

SRCS+=		abort_message.cpp\
		abort_message.h\
		cxa_aux_runtime.cpp\
		cxa_default_handlers.cpp\
		cxa_demangle.cpp\
		cxa_exception.cpp\
		cxa_exception.hpp\
		cxa_exception_storage.cpp\
		cxa_guard.cpp\
		cxa_handlers.cpp\
		cxa_handlers.hpp\
		cxa_new_delete.cpp\
		cxa_personality.cpp\
		cxa_unexpected.cpp\
		cxa_vector.cpp\
		cxa_virtual.cpp\
		exception.cpp\
		fallback_malloc.ipp\
		private_typeinfo.cpp\
		private_typeinfo.h\
		stdexcept.cpp\
		typeinfo.cpp

WARNS=		0
CXXFLAGS+=	-I${HDRDIR} -std=c++0x -nostdlib -fPIC

LDFLAGS+=	--verbose

.include <bsd.lib.mk>
