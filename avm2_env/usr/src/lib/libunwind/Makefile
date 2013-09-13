# $FreeBSD$

HDRDIR=		${.CURDIR}/../../contrib/libunwind/include
HDRDIR+=	-I${.CURDIR}/../../contrib/libunwind/include/tdep
HDRDIR+=	-I${.CURDIR}/../../contrib/libunwind/src
SRCDIR=		${.CURDIR}/../../contrib/libunwind/src


SHLIB_MAJOR=	1
SHLIBDIR?=	/lib

.PATH: ${SRCDIR}

LIB=		unwind

SRCS+=		mi/init.c \
	        mi/flush_cache.c \
	        mi/mempool.c \
	        mi/strerror.c \
	        dwarf/global.c \
	        mi/backtrace.c \
	        mi/dyn-cancel.c \
	        mi/dyn-info-list.c \
	        mi/dyn-register.c \
	        mi/Ldyn-extract.c \
	        mi/Lfind_dynamic_proc_info.c \
	        mi/Lget_accessors.c \
	        mi/Lget_proc_info_by_ip.c \
	        mi/Lget_proc_name.c \
	        mi/Lput_dynamic_unwind_info.c \
	        mi/Ldestroy_addr_space.c \
	        mi/Lget_reg.c \
	        mi/Lset_reg.c \
	        mi/Lget_fpreg.c \
	        mi/Lset_fpreg.c \
	        mi/Lset_caching_policy.c \
	        mi/_ReadULEB.c \
	        mi/_ReadSLEB.c \
	        unwind/Backtrace.c \
	        unwind/DeleteException.c \
	        unwind/FindEnclosingFunction.c \
	        unwind/ForcedUnwind.c \
	        unwind/GetBSP.c \
	        unwind/GetCFA.c \
	        unwind/GetDataRelBase.c \
	        unwind/GetGR.c \
	        unwind/GetIP.c \
	        unwind/GetLanguageSpecificData.c \
	        unwind/GetRegionStart.c \
	        unwind/GetTextRelBase.c \
	        unwind/RaiseException.c \
	        unwind/Resume.c \
	        unwind/Resume_or_Rethrow.c \
	        unwind/SetGR.c \
	        unwind/SetIP.c \
	        unwind/GetIPInfo.c \
	        dwarf/Lexpr.c \
	        dwarf/Lfde.c \
	        dwarf/Lparser.c \
	        dwarf/Lpe.c \
	        dwarf/Lstep.c \
	        dwarf/Lfind_proc_info-lsb.c

#Add x86 specific files
SRCS+=		elf32.c \
	        x86/is_fpreg.c \
	        x86/regname.c \
	        x86/Lcreate_addr_space.c \
	        x86/Lget_save_loc.c \
	        x86/Lglobal.c \
	        x86/Linit.c \
	        x86/Linit_local.c \
	        x86/Linit_remote.c \
	        x86/Lget_proc_info.c \
	        x86/Lregs.c \
	        x86/Lresume.c \
	        x86/Lstep_.c

#Add freebsd specific file
SRCS+=		os-freebsd.c \
		x86/Los-freebsd.c

WARNS=		0
CFLAGS+=	-I${HDRDIR} -fPIC -D_GNU_SOURCE -DHAVE_DL_ITERATE_PHDR
CFLAGS+=	-DHAVE_SYS_ENDIAN_H -DPIC -fexceptions -m32

.include <bsd.lib.mk>
