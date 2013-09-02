/*-
 * Copyright (C) 2003 David Xu <davidxu@freebsd.org>
 * Copyright (c) 2001,2003 Daniel Eischen <deischen@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 * $FreeBSD: src/lib/libthr/arch/i386/i386/pthread_md.c,v 1.7.10.1.6.1 2010/12/21 17:09:25 kensmith Exp $
 */

#include <stdlib.h>
#include <sys/types.h>
#include <machine/segments.h>
#include <machine/sysarch.h>
#include <string.h>
// #include <rtld_tls.h>

#include "pthread_md.h"

extern void *_kthread_ctor();
extern void _kthread_dtor(void *k);

struct tcb *
_tcb_ctor(struct pthread *thread, int initial)
{
        struct tcb *tcb;

        tcb = calloc(1, sizeof(struct tcb));
        if (tcb)
	{
                tcb->tcb_thread = thread;
		tcb->tcb_kthread = _kthread_ctor();
	}
        return (tcb);
}

void
_tcb_dtor(struct tcb *tcb)
{
	_kthread_dtor(tcb->tcb_kthread);
        free(tcb);
}
