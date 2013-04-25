// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <libc_private.h>

#ifndef PTHREAD_WEAK_HACK
// threading calls

int             pthread_once(pthread_once_t *once, void (*proc) (void)) __attribute__((weak));
int             pthread_once(pthread_once_t *once, void (*proc) (void))
{
  if(once->state == PTHREAD_NEEDS_INIT)
  {
    proc();
    once->state = PTHREAD_DONE_INIT;
  }
  return 0;
}

// emulate pthread_specific stuff in ST
typedef struct
{
  const void *value;
  union
  {
    void (*destructor)(void *);
    int nextFree;
  };
} PTSpecificEntry;

static PTSpecificEntry *sEntries = NULL;
static int sFirstFree = -1;
static int sEntryCount = 0;

static int allocEntry()
{
  int result;

  if(sFirstFree < 0)
  {
    result = sEntryCount;
    sEntries = realloc(sEntries, ++sEntryCount * sizeof(PTSpecificEntry));
  }
  else
  {
    result = sFirstFree;
    sFirstFree = sEntries[result].nextFree;
  }
  return result;
}

static void freeEntry(int n)
{
  sEntries[n].nextFree = sFirstFree;
  sFirstFree = n;
}

// note! pthread keys will not have destructors called on exit... works fine for libstdc++

int             pthread_key_create(pthread_key_t *key,
                        void (*destructor) (void *)) __attribute__((weak));
int             pthread_key_create(pthread_key_t *key,
                        void (*destructor) (void *))
{
  int n = allocEntry();
  PTSpecificEntry *entry = n + sEntries;

  entry->value = NULL;
  entry->destructor = destructor;
  *key = n;
  return 0;
}

int             pthread_key_delete(pthread_key_t key) __attribute__((weak));
int             pthread_key_delete(pthread_key_t key)
{
  PTSpecificEntry *entry = key + sEntries;

  if(key < 0 || key >= sEntryCount)
    return -1;
  if(entry->destructor && entry->value)
    entry->destructor(entry->value);
  entry->value = NULL;
  freeEntry(key);
  return 0;
}

int             pthread_setspecific(pthread_key_t key, const void *value) __attribute__((weak));
int             pthread_setspecific(pthread_key_t key, const void *value)
{
  PTSpecificEntry *entry = key + sEntries;

  if(key < 0 || key >= sEntryCount)
    return -1;
  entry->value = value;
  return 0;
}

void            *pthread_getspecific(pthread_key_t key) __attribute__((weak));
void            *pthread_getspecific(pthread_key_t key)
{
  PTSpecificEntry *entry = key + sEntries;

  if(key < 0 || key >= sEntryCount)
    return NULL;
  return entry->value;
}

int             pthread_create(pthread_t *, const pthread_attr_t *,
                        void *(*) (void *), void *) __attribute__((weak));
int             pthread_create(pthread_t *ta, const pthread_attr_t *att,
                        void *(*s) (void *), void *arg)
{
  return ENOSYS;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) __attribute__((weak));
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
  return ENOSYS;
}
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) __attribute__((weak));
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
  return ENOSYS;
}

// all of the __sync_ RTLIB calls built on top of simple int CAS provided by ByteArray
// overridden by thrHelpers.c w/ always cas variant
unsigned int avm2_cmpSwapUns(unsigned int *ptr, unsigned int oldval, unsigned int newval) __attribute__((weak));
unsigned int avm2_cmpSwapUns(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  unsigned int curval;
  if(avm2_haveWorkers())
    __asm __volatile__ ("%0 = casi32(%1, %2, %3)" : "=r"(curval) : "r"(ptr), "r"(oldval), "r"(newval) : "memory");
  else
  {
    curval = *ptr;

    if(curval == oldval)
      *ptr = newval;
  }
  return curval;
}

void avm2_mfence() __attribute__((weak));
void avm2_mfence()
{
  if(avm2_haveWorkers())
    __asm __volatile__ ("mfence()" ::: "memory");
}

void flascc_uiThreadAttrInit(pthread_attr_t *pattr, void *stack, int stackSize) __attribute__((weak));
void flascc_uiThreadAttrInit(pthread_attr_t *pattr, void *stack, int stackSize)
{
}

void flascc_uiThreadAttrDestroy(pthread_attr_t *pattr) __attribute__((weak));
void flascc_uiThreadAttrDestroy(pthread_attr_t *pattr)
{
}

void flascc_pthreadsInit() __attribute__((weak));
void flascc_pthreadsInit()
{
}

#endif
