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

/***

Trampoline functionality

Compile this file as part of your project -- no pre-built .o or .a is provided as trampoline count is chosen at compile time

The number of total trampolines is fixed at runtime but selectable at compile time with the AVM2_TRAMP_COUNT macro

C++ implementation, but does not require libstdc++ to link! (can be compile and linked w/ gcc -- g++ not needed)
provides:

***

extern "C" void *avm2_tramp_alloc(void *funPtr, int sret, void *arg0, void *arg1);

If a trampoline cannot be allocated, returns NULL.
Otherwise, returns a function pointer that is of type:

funPtrRetType (*)(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)

Where:

funPtrRetType is the return type of "funPtr"
trampFunPtr is the trampoline function ptr as returned from avm2_tramp_alloc
args points to any arguments passed to the trampoline
arg0 and arg1 are the same values passed to avm2_tramp_alloc
... represents that arguments passed to the trampoline are also passed to funPtr

sret must zero for a non-structure return type, the size of the structure return type for POD,
or the negative size of the structure for non-POD.

***

extern "C" void avm2_tramp_free(void *trampFunPtr);

Frees a previously allocated trampoline

***/

#include <stdlib.h>
#include <stdint.h>
#include <AS3/AVM2.h>
#include <AS3/AS3.h>

#ifdef AVM2_TRAMP_TEST
#include <stdio.h>
#include <stdarg.h>
#endif

#ifndef AVM2_TRAMP_COUNT
#define AVM2_TRAMP_COUNT 1024 // default to 1024 trampolines
#endif

struct TrampState
{
  int sret; // returns a struct (via hidden ptr)?
  void *funPtr; // function to trampoline to
  void *arg0; // client arg0
  void *arg1; // client arg1
  TrampState *nextFree; // next free tramp
};

static TrampState s_trampStates[AVM2_TRAMP_COUNT];
static void *s_trampImpls[AVM2_TRAMP_COUNT] = { 0 };
static TrampState *s_nextFreeTrampState;

static int ptrcomp(const void *a, const void *b)
{
  return *(const intptr_t *)a - *(const intptr_t *)b;
}

static void findTrampState() __attribute__((used, annotate("as3sig:public function avm2FindTrampState(funPtr:int):int")));
static void findTrampState() // find the TrampState * for a given tramp's fun ptr
{
  void *funPtr;
  AS3_GetScalarFromVar(funPtr, funPtr);
  void *entry = bsearch(&funPtr, s_trampImpls, AVM2_TRAMP_COUNT, sizeof(void *), ptrcomp);
  TrampState *state = s_trampStates + ((void **)entry - s_trampImpls);
#ifdef AVM2_TRAMP_TEST_LOG
  printf("findTrampState: funptr: %p entry: %p state: %p\n", funPtr, entry, state);
#endif
  AS3_Return(state);
}

__asm(
"#package private\n"
// make a tramp for a given tramp's fun ptr + state
"public function avm2MakeTramp(funPtr:int, trampState:int):Function { "
"  return function():void { "
"    var oldPos:int = ram.position; "
"    ram.position = trampState; "
"    var sret:int = ram.readInt(); " // do we return a struct (via hidden ptr)?
"    var fun:int = ram.readInt(); " // read the fun to delegate to
"    var arg0:int = ram.readInt(); " // read arg0
"    var arg1:int = ram.readInt(); " // read arg1
#ifdef AVM2_TRAMP_TEST_LOG
"    trace('avm2MakeTramp/tramp(' + funPtr + ', ' + trampState + '): fun: ' + fun + ' arg0: ' + arg0 + ' arg1: ' + arg1); "
#endif
"    if(sret) { "
"      ram.position = ESP; "
"      sret = ram.readInt(); "
"    } "
"    ram.position = ESP - 16; "
"    if(sret) "
"      ram.writeInt(sret); "
"    ram.writeInt(funPtr); " // pass the tramp fun ptr
"    ram.writeInt(ESP + (sret ? 4 : 0)); " // pass the addr of the args
"    ram.writeInt(arg0); " // pass arg 0
"    ram.writeInt(arg1); " // pass arg 1
"    ram.position = oldPos; " // restore ram.position
"    ESP -= 16; " // new args
"    ptr2fun[fun](); " // invoke function
"    ESP += 16; " // restore stack
"  }; "
"} "
// implements the tramp init
"public function avm2TrampInit(funPtr:int):void { "
"  var trampState:int = avm2FindTrampState(funPtr); "
#ifdef AVM2_TRAMP_TEST_LOG
"  trace('avm2TrampInit(' + funPtr + '): trampState: ' + trampState); "
#endif
"  var tramp:Function = avm2MakeTramp(funPtr, trampState); "
"  ptr2fun[funPtr] = tramp; " // overwrite stub with new tramp!
"  tramp(); "
"} "
);

template <int N> struct TrampImpl
{
  __attribute__((naked)) static void trampImpl()
  {
    __asm __volatile__ ("avm2TrampInit(%0)" : : "i"(trampImpl));
  }
};

template <> class TrampImpl<-1>
{
};

// initialize tramp list by divide and conquer (to not blow through template nesting limit)
template <int MIN, int MAX> struct TrampImplInitHelper
{
  enum { MID = (MIN + MAX) / 2 };

  static /*__attribute__((always_inline))*/ void init()
  {
    if(MIN == MID)
    {
#ifdef AVM2_TRAMP_TEST_LOG
      printf("init # %d\n", MIN);
#endif
      s_trampImpls[MIN] = (void *)&TrampImpl<MIN>::trampImpl;
    }
    else
    {
      TrampImplInitHelper<MIN, MID>::init();
      TrampImplInitHelper<MID, MAX>::init();
    }
  }
};

static void init()
{
#ifdef AVM2_TRAMP_TEST_LOG
  printf("init\n");
#endif
  TrampImplInitHelper<0, AVM2_TRAMP_COUNT>::init(); // initalize array of tramps

  qsort(s_trampImpls, AVM2_TRAMP_COUNT, sizeof(void *), ptrcomp); // sort array by ptr

  // set up linked list
  TrampState **cur = &s_nextFreeTrampState;

  for(int i = 0; i < AVM2_TRAMP_COUNT; i++)
  {
    *cur = s_trampStates + i;
    cur = &(s_trampStates[i].nextFree);
  }
  *cur = NULL;
}

// serialize access to the free list
static unsigned sMutex = 0;

extern "C" void *avm2_tramp_alloc(void *funPtr, int sret, void *arg0, void *arg1)
{
  avm2_lock(&sMutex);

  if(!s_trampImpls[0]) // not initialized?
    init();

  TrampState *state = s_nextFreeTrampState;

  if(state)
  {
    // take it off free list
    s_nextFreeTrampState = state->nextFree;
    avm2_unlock(&sMutex);
    state->funPtr = funPtr;
    state->sret = (int)(sret > 8 || (sret & (sret-1)) || sret < 0); // hidden ptr for structs that can't fit in ret regs
    state->arg0 = arg0;
    state->arg1 = arg1;
    // return actual thunk
    return s_trampImpls[state - s_trampStates];
  }
  avm2_unlock(&sMutex);
  return NULL;
}

extern "C" void avm2_tramp_free(void *trampFunPtr)
{
  void *entry = bsearch(&trampFunPtr, s_trampImpls, AVM2_TRAMP_COUNT, sizeof(void *), ptrcomp);

  if(entry)
  {
    TrampState *state = s_trampStates + ((void **)entry - s_trampImpls);

    avm2_lock(&sMutex);
    state->nextFree = s_nextFreeTrampState;
    s_nextFreeTrampState = state;
    avm2_unlock(&sMutex);
  }
}

#ifdef AVM2_TRAMP_TEST
static int trampTest0(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  printf("trampFunPtr: %p args: %p args[0] as int: (%d) arg0: %p arg1: %p\n", trampFunPtr, args, *(int *)args, arg0, arg1);
  return 6677;
}

struct Test1Struct
{
  double d;
  int i;
};

static Test1Struct trampTest1(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test1Struct ret = { 12.34, i0 };
  return ret;
}

struct Test2Struct
{
  int n;
  int i;
};

static Test2Struct trampTest2(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test2Struct ret = { 1234, i0 };
  return ret;
}

struct Test3Struct
{
  int i;
};

static Test3Struct trampTest3(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test3Struct ret = { i0 };
  return ret;
}

struct Test4Struct
{
  int i;
  char c;
};

static Test4Struct trampTest4(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test4Struct ret = { 1234, i0 };
  return ret;
}

struct Test5Struct
{
  double d;
};

static Test5Struct trampTest5(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test5Struct ret = { i0 };
  return ret;
}

struct Test6Struct
{
  char c;
};

static Test6Struct trampTest6(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test6Struct ret = { i0 };
  return ret;
}

struct Test7Struct
{
  char c;
  ~Test7Struct() {} // non-POD!
};

static Test7Struct trampTest7(void *trampFunPtr, void *args, void *arg0, void *arg1, ...)
{
  va_list ap;
  va_start(ap, arg1);
  int i0 = va_arg(ap, int);
  va_end(ap);
  int i1 = *(int *)args;
  printf("trampFunPtr: %p args: %p args[0] as int: (%d %d) arg0: %p arg1: %p\n", trampFunPtr, args, i0, i1, arg0, arg1);
  Test7Struct ret = { i0 };
  return ret;
}

static void allocAndFreeAllTramps(int n = 0)
{
  void *tramp = avm2_tramp_alloc(NULL, 0, NULL, NULL);
  if(tramp)
  {
    printf("alloced tramp # %d\n", n);
    allocAndFreeAllTramps(n + 1);
  }
  avm2_tramp_free(tramp);
}

int main()
{
  int (*test0)(int);
  Test1Struct (*test1)(int);
  Test1Struct test1Struct;
  Test2Struct (*test2)(int);
  Test2Struct test2Struct;
  Test3Struct (*test3)(int);
  Test3Struct test3Struct;
  Test4Struct (*test4)(int);
  Test4Struct test4Struct;
  Test5Struct (*test5)(int);
  Test5Struct test5Struct;
  Test6Struct (*test6)(int);
  Test6Struct test6Struct;
  Test7Struct (*test7)(int);
  Test7Struct test7Struct;

  allocAndFreeAllTramps();
  allocAndFreeAllTramps();

  *(void **)&test0 = avm2_tramp_alloc((void *)trampTest0, 0, (void *)0x1234, (void *)0x5678);
  *(void **)&test1 = avm2_tramp_alloc((void *)trampTest1, sizeof(Test1Struct), (void *)0x12, (void *)0x56);
  *(void **)&test2 = avm2_tramp_alloc((void *)trampTest2, sizeof(Test2Struct), (void *)0x12, (void *)0x56);
  *(void **)&test3 = avm2_tramp_alloc((void *)trampTest3, sizeof(Test3Struct), (void *)0x12, (void *)0x56);
  *(void **)&test4 = avm2_tramp_alloc((void *)trampTest4, sizeof(Test4Struct), (void *)0x12, (void *)0x56);
  *(void **)&test5 = avm2_tramp_alloc((void *)trampTest5, sizeof(Test5Struct), (void *)0x12, (void *)0x56);
  *(void **)&test6 = avm2_tramp_alloc((void *)trampTest6, sizeof(Test6Struct), (void *)0x12, (void *)0x56);
  *(void **)&test7 = avm2_tramp_alloc((void *)trampTest7, -sizeof(Test7Struct), (void *)0x12, (void *)0x56);

  printf("%d\n", test0(987));
  printf("%d\n", test0(456));

  test1Struct = test1(987);
  printf("%f %d\n", test1Struct.d, test1Struct.i);
  test1Struct = test1(456);
  printf("%f %d\n", test1Struct.d, test1Struct.i);

  test2Struct = test2(987);
  printf("%d %d\n", test2Struct.n, test2Struct.i);
  test2Struct = test2(456);
  printf("%d %d\n", test2Struct.n, test2Struct.i);

  test3Struct = test3(987);
  printf("%d\n", test3Struct.i);
  test3Struct = test3(456);
  printf("%d\n", test3Struct.i);

  test4Struct = test4('a');
  printf("%d %c\n", test4Struct.i, test4Struct.c);
  test4Struct = test4('b');
  printf("%d %c\n", test4Struct.i, test4Struct.c);

  test5Struct = test5(123);
  printf("%f\n", test5Struct.d);
  test5Struct = test5(456);
  printf("%f\n", test5Struct.d);

  test6Struct = test6('a');
  printf("%c\n", test6Struct.c);
  test6Struct = test6('b');
  printf("%c\n", test6Struct.c);

  test7Struct = test7('a');
  printf("%c\n", test7Struct.c);
  test7Struct = test7('b');
  printf("%c\n", test7Struct.c);

  return 0;
}
#endif
