/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in
** accordance with the terms of the Adobe license agreement accompanying it.
** If you have received this file from a source other than Adobe, then your use,
** modification, or distribution of it requires the prior written permission of Adobe.
*/

#ifndef _AVM2_H
#define _AVM2_H

#ifdef __cplusplus
extern "C" {
#endif

struct __avm2_arglist
{
  void *arg_ptr;
};

typedef struct __avm2_arglist *avm2_arglist_t;

struct __avm2_retdata
{
  int eax;
  int edx;
  double st0;
};

#define __avm2_apply_args() \
({ \
  struct __avm2_arglist *arglist = (struct __avm2_arglist *)__builtin_alloca(sizeof(struct __avm2_arglist)); \
  __asm("%0 = ebp" : "=r"(arglist->arg_ptr)); \
  (void *)arglist; \
})

void __avm2_apply2(struct __avm2_retdata *ret, void *f, void *a, int s);

#define __avm2_apply(F, D, S) \
({ \
  void *__f = (F); \
  struct __avm2_arglist *__d = (struct __avm2_arglist *)(D); \
  int __s = (S); \
  struct __avm2_retdata *__ret = (struct __avm2_retdata *)__builtin_alloca(sizeof(struct __avm2_retdata)); \
  __avm2_apply2(__ret, __f, __d->arg_ptr, (__s+3)/4); \
  (void *)__ret; \
})
  
#define __avm2_return(R) \
({ \
  struct __avm2_retdata *__ret = (struct __avm2_retdata *)(R); \
  __asm __volatile__ ("eax = %0; edx = %1; st0 = %2; ESP = ebp; return" :: "r"(__ret->eax), "r"(__ret->edx), "r"(__ret->st0)); \
  (void)0; \
})

#if OVERRIDE__BUILTIN_APPLY_XXX
#define __builtin_va_arg_pack *$error$*
#define __builtin_va_arg_pack_len *$error$*
/*#define arglist_t avm2_arglist_t*/
#define __builtin_apply_args __avm2_apply_args
#define __builtin_apply __avm2_apply
#define __builtin_return __avm2_return
#endif

/* super low-level conc primitives -- expect hangs or worse if not used properly */

/* non-recursive mutexes */

unsigned avm2_locked_id(unsigned *mtx); /* id of lock owner or 0 */
void avm2_lock(unsigned *mtx); /* lock with thread id */
void avm2_lock_id(unsigned *mtx, unsigned id); /* lock with any non-0, non-set-high-bit id */
void avm2_unlock(unsigned *mtx); /* unlock */

/* address token-based conditions */

/* sleep on address -- mtx must be locked on entry
** waits for addr to be woken for timo ms (or forever if timo==0)
** releases mutex on entry, re-acquired before return
** 0 on timeout, non-zero on wake
*/
int avm2_msleep(void *addr, unsigned *mtx, int timo);
int avm2_wake(void *addr); /* wake any threads msleeping on this address, returns non-zero if a thread was woken  */
int avm2_wake_one(void *addr); /* wake zero (if none msleeping on this address) or one threads msleeping on this address, returns non-zero if any thread was woken */

/* call proc(arg) while impersonating specified thread */
void *avm2_thr_impersonate(long tid, void *(*proc)(void *), void *arg);

/* call proc(arg) on the ui Worker as calling thread
** note that doing C I/O in "proc" is unsafe and may result in deadlock!
*/
void *avm2_ui_thunk(void *(*proc)(void *), void *arg);

/* returns non-zero if running on the ui Worker */
int avm2_is_ui_worker();

/* wait for a frame event or until timeout -- return 0 on timeout */
int avm2_wait_for_ui_frame(int timo);

/* lock this thread to this Worker -- recursive ok */
void avm2_self_lock();
void avm2_self_unlock();
/* like avm2_msleep with "self" as lock -- "self" must be locked via avm2_self_lock */
int avm2_self_msleep(void *addr, int timo);

/* trampoline support -- see sdk/usr/share/avm2_tramp.cpp */
void *avm2_tramp_alloc(void *funPtr, int sret, void *arg0, void *arg1);
void avm2_tramp_free(void *trampFunPtr);

#ifdef __cplusplus
}
#endif

#endif /* _AVM2_H */

