/*-
 * Copyright (c) 2002 Daniel Eischen <deischen@freebsd.org>.
 * Copyright (c) 2005 David Xu <davidxu@freebsd.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/libthr/arch/i386/include/pthread_md.h,v 1.9.2.1.6.1 2010/12/21 17:09:25 kensmith Exp $
 */

/*
 * Machine-dependent thread prototypes/definitions.
 */
#ifndef _PTHREAD_MD_H_
#define	_PTHREAD_MD_H_

#include <stddef.h>
#include <sys/types.h>
#include <machine/sysarch.h>

#define	CPU_SPINWAIT		//TODO
#define	DTV_OFFSET		offsetof(struct tcb, tcb_dtv)

/*
 * Variant II tcb, first two members are required by rtld,
 * tcbp points to the structure.
 */
struct tcb {
	struct tcb		*tcb_self;	/* required by rtld */
	void			*tcb_dtv;	/* required by rtld */
	struct pthread		*tcb_thread;
	void			*tcb_kthread; /* struct thread kernel thread */
	unsigned		tcb_lock; /* lock for avm2_self_lock */
	unsigned		tcb_locked_count; /* avm2_self_lock recurses! */
};

/*
 * The constructors.
 */
struct tcb	*_tcb_ctor(struct pthread *, int);
void		_tcb_dtor(struct tcb *tcb);

/* Called from the thread to set its private data. */
static __inline void
_tcb_set(struct tcb *tcb)
{
	__asm volatile ("tcbp = %0" : : "r"(tcb)); // keep in sync w/ libcHack.c thread_run
}

/*
 * Get the current tcb.
 */
static __inline struct tcb *
_tcb_get(void)
{
	struct tcb *ret;
	__asm("%0 = tcbp" : "=r"(ret));
	return ret;
}

extern struct pthread *_thr_initial;

static __inline struct pthread *
_get_curthread(void)
{
        if (_thr_initial)
                return (_tcb_get()->tcb_thread);
        return (NULL);
}

#ifndef HAS__UMTX_OP_ERR
#undef HAS__UMTX_OP_ERR
#endif

#endif
