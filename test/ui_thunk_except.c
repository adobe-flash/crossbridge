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

// compile using -emit-swf -swf-version=18 -pthread

#include <AS3/AVM2.h>
#include <AS3/AS3.h>
#include <stdio.h>

static volatile int throwCount = 0;
void *sometimesThrowsThunk(void *data)
{
  int n = (int)data;

  if(rand() & 1) // 50% of the time
  {
    throwCount++;
    inline_as3("throw 'foo'");
  }
  printf("%d\n", n);
  return NULL;
}

int main()
{
  int i;
  for(i = 0; i < 100; i++)
    avm2_ui_thunk(sometimesThrowsThunk, (void *)i);
  printf("%d throws\n", throwCount);
  return 0;
}
