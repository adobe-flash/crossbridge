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
extern "C" {
#include <sys/thr.h>
}
#include <AS3/AS3++.h>
#include <AS3/AVM2.h>

using namespace AS3::ui;

int expectedDepth = 0;
int successes = 0;
int EXPECTED_SUCCESSES = 20;

static var recurseProc(void *arg, var args)
{
  var callee = args[0];
  int depth = internal::int_valueOf(args[1]);

  if(depth)
  {
    printf("depth: %d\n", depth);
	  
    if( depth == expectedDepth-- ){
      ++successes;
    }
	  
    var rargs[] = { callee, internal::new_int(depth-1) };
    internal::call(callee, internal::_undefined, 2, rargs);
  }
  return internal::_undefined;
}

static var fun = internal::new_Function(recurseProc, NULL);
static var fargs[] = { fun, internal::new_int(10) };

static char cond;

static void *threadProc(void *arg)
{
  internal::call(fun, internal::_undefined, 2, fargs);
  avm2_wake_one(&cond);
  return NULL;
}

int main()
{
  printf("self\n");
	expectedDepth = 10;
  internal::call(fun, internal::_undefined, 2, fargs);
  printf("thread\n");
	expectedDepth = 10;
  pthread_t thread;
  pthread_create(&thread, NULL, threadProc, NULL);
  avm2_self_msleep(&cond, 0);
  pthread_join(thread, NULL);
  printf("ok!\n");
  if( successes == EXPECTED_SUCCESSES ){
    printf("RESULT=PASS");
  }else{
    printf("RESULT=FAIL");
  }
  return 0;
}
