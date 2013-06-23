/*
** Copyright (c) 2013 Adobe Systems Inc

** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:

** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "AS3/AS3.h"
#include <pthread.h>
#include <AS3/AVM2.h>

extern void tprintf(const char*, ...);

void * uiThunk(void *args)
{
	inline_as3("import flash.system.Worker;\n");
  inline_as3("trace('uiThunk -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));
}

void *threadProc1(void *arg)
{
  inline_as3("import flash.system.Worker;\n");
  inline_as3("trace('threadProc1 -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));
	avm2_ui_thunk(uiThunk, NULL);
  return NULL;
}

void *threadProc2(void *arg)
{
  inline_as3("import flash.system.Worker;\n");
  inline_as3("trace('threadProc2 -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));
  avm2_ui_thunk(uiThunk, NULL);
  return NULL;
}

void runOnce()
{
  inline_as3("import flash.system.Worker;\n");
  inline_as3("trace('runOnce -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));
}

int main()
{
  inline_as3("import flash.system.Worker;\n");
  inline_as3("trace('main -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));

  //char someCond;
  //avm2_self_msleep(&someCond, 1);

  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, threadProc1, NULL);
  pthread_create(&thread2, NULL, threadProc2, NULL);

  inline_as3("trace('main (thread started) -- is_ui_worker: ' + %0 + ' isPrimordial: '+Worker.current.isPrimordial);" : : "r"(avm2_is_ui_worker()));

  AS3_GoAsync();
}
