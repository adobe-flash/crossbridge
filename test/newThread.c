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

// trivial test for "C_Run.newThread(tid:int, stackPtr:int, entryFunPtr:int, argVector:Vector.<int>)"
// uses trace to not require posix

static volatile int done = 0;
static volatile int bar = 0;

static void foo()
{
  int n;
  tprintf("in foo -- bar: %d done: %d", bar, done);
  n = __sync_add_and_fetch(&done, 1);
  tprintf("(after atomic incr) done: %d", done);
  __asm __volatile__("workerClass.current.terminate()");
}

#define THREADS 4

int main(int argc, char **argv) 
{ 
  int stackSize = 256*1024;
  void *stacks[THREADS];
  int i;

  for(i = 0; i < THREADS; i++)
    stacks[i] = malloc(stackSize);
  __asm("import C_Run.newThread");
  for(i = 0, bar = 123; i < THREADS; i++, bar++)
    __asm("trace('new tid: ' + %2); newThread(%2, %0, %1, new Vector.<int>).start()" : : "r"(stackSize+(int)stacks[i]), "r"(foo), "r"(i+1));
  while(done != THREADS);
  tprintf("done");
  return 0;
} 

