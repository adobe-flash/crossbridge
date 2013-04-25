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

static char cond;

static void *threadProc(void *arg)
{
  avm2_wake_one(&cond);
  return NULL;
}

static pthread_t thread;

static var timeoutProc(void *arg, var args)
{
  pthread_create(&thread, NULL, threadProc, NULL);
  return internal::_undefined;
}

int main()
{
  var namespaceClass = internal::getlex(internal::new_String("Namespace"));
  var flash_utils = internal::construct(namespaceClass, internal::new_String("flash.utils"));
  var setTimeout = internal::getlex(flash_utils, internal::new_String("setTimeout"));
  var stArgs[] = { internal::new_Function(timeoutProc, NULL), internal::new_int(3000) };
  internal::call(setTimeout, internal::_undefined, 2, stArgs);
  printf("sleeping!\n");
  avm2_self_msleep(&cond, 0);
  pthread_join(thread, NULL);
  printf("ok!\n");
  return 0;
}
