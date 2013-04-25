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
#include <AS3/AS3++.h>
#include <AS3/AVM2.h>

static char mouseThreadExitCond;

using namespace AS3::ui;

static var mouseMoveEventListener(void *data, var args)
{
  var event = args[0];
  printf("mouseMove!\n");
  return internal::_undefined;
}

static void *mouseThreadProc(void *arg)
{
  var stage = internal::get_Stage();
  var elArgs[] = { internal::new_String("mouseMove"), internal::new_Function(mouseMoveEventListener, NULL) };
  internal::call(stage["addEventListener"], stage, 2, elArgs);

  avm2_self_msleep(&mouseThreadExitCond, 0);

  internal::call(stage["removeEventListener"], stage, 2, elArgs);
  return NULL;
}

int main()
{
  pthread_t mouseThread;
  pthread_create(&mouseThread, NULL, mouseThreadProc, NULL);
  pthread_join(mouseThread, NULL);
  return 0;
}
