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

#include <stddef.h>
#include <pthread.h>

unsigned int avm2_cmpSwapUns(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  unsigned int result;
  __asm __volatile__ ("%0 = casi32(%1, %2, %3)" : "=r"(result) : "r"(ptr), "r"(oldval), "r"(newval) : "memory");
  return result;
}

void avm2_mfence()
{
  __asm __volatile__ ("mfence()" ::: "memory");
}

void flascc_uiThreadAttrInit(pthread_attr_t *pattr, void *stack, int stackSize)
{
  _pthread_attr_init(pattr);
  _pthread_attr_setstackaddr(pattr, stack);
  _pthread_attr_setstacksize(pattr, stackSize);
}

void flascc_uiThreadAttrDestroy(pthread_attr_t *pattr)
{
  _pthread_attr_destroy(pattr);
}

extern void _libpthread_init(void *);
extern int avm2_haveWorkers();

void flascc_pthreadsInit()
{
  if(avm2_haveWorkers())
    _libpthread_init(NULL);
}
