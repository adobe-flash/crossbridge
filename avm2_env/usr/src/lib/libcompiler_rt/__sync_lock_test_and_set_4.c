/* $FreeBSD: release/9.1.0/lib/libcompiler_rt/__sync_lock_test_and_set_4.c 236016 2012-05-25 17:50:50Z marius $ */
#define	NAME		__sync_lock_test_and_set_4
#define	TYPE		uint32_t
#define	CMPSET		atomic_cmpset_32
#define	EXPRESSION	value

#include "__sync_fetch_and_op_n.h"
