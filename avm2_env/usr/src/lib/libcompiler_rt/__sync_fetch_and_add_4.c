/* $FreeBSD: release/9.1.0/lib/libcompiler_rt/__sync_fetch_and_add_4.c 236016 2012-05-25 17:50:50Z marius $ */
#define	NAME		__sync_fetch_and_add_4
#define	TYPE		uint32_t
#define	FETCHADD(x, y)	atomic_fetchadd_32(x, y)

#include "__sync_fetch_and_op_n.h"
