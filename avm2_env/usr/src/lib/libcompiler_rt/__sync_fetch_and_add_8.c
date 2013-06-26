/* $FreeBSD: release/9.1.0/lib/libcompiler_rt/__sync_fetch_and_add_8.c 236016 2012-05-25 17:50:50Z marius $ */
#define	NAME		__sync_fetch_and_add_8
#define	TYPE		uint64_t
#define	FETCHADD(x, y)	atomic_fetchadd_64(x, y)

#include "__sync_fetch_and_op_n.h"
