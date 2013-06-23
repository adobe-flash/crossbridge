/* $FreeBSD: release/9.1.0/lib/libcompiler_rt/__sync_val_compare_and_swap_8.c 236016 2012-05-25 17:50:50Z marius $ */
#define	NAME		__sync_val_compare_and_swap_8
#define	TYPE		uint64_t
#define	CMPSET		atomic_cmpset_64

#include "__sync_val_compare_and_swap_n.h"
