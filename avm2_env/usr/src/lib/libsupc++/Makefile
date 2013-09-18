# $FreeBSD$

HDRDIR=		${.CURDIR}/../../../../../../llvm-gcc-4.2-2.9/libstdc++-v3/libsupc++
HDRDIR+=	-I${.CURDIR}/../../../../../../llvm-gcc-4.2-2.9/libstdc++-v3/include
HDRDIR+=	-I${.CURDIR}/../libgcceh
SRCDIR=		${.CURDIR}/../../../../../../llvm-gcc-4.2-2.9/libstdc++-v3/libsupc++

.PATH: ${SRCDIR}

LIB=		supc++
SHLIB_MAJOR=	1

SRCS+=		del_op.cc\
		del_opnt.cc\
		del_opv.cc\
		del_opvnt.cc\
		eh_alloc.cc\
		eh_arm.cc\
		eh_aux_runtime.cc\
		eh_call.cc\
		eh_catch.cc\
		eh_exception.cc\
		eh_globals.cc\
		eh_personality.cc\
		eh_term_handler.cc\
		eh_terminate.cc\
		eh_throw.cc\
		eh_type.cc\
		eh_unex_handler.cc\
		guard.cc\
		new_op.cc\
		new_opnt.cc\
		new_opv.cc\
		new_opvnt.cc\
		pure.cc\
		tinfo.cc\
		tinfo2.cc\
		vec.cc
		#new_handler.cc\
		#vterminate.cc

WARNS=		0
CXXFLAGS+=	-I${HDRDIR} -m32 -std=c++0x -nostdlib -fPIC -D_GLIBCXX_SJLJ_EXCEPTIONS
CXXFLAGS+=	-stdlib=libstdc++

LDFLAGS+=	--verbose

.include <bsd.lib.mk>

