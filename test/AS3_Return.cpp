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

#include <stdio.h>
#include <AS3/AS3.h>

class SomeClass
{
public:
  ~SomeClass() { printf("~SomeClass!\n"); }
};

void foo() __attribute__((used, annotate("as3sig:public function as3foo():int")));
void foo()
{
  SomeClass sc;
  AS3_Return(13);
}

void bar() __attribute((used, annotate("as3sig:public function as3bar():Array")));
void bar()
{
  SomeClass sc;
  AS3_ReturnAS3Var(([3,4,5]));
}

void baz() __attribute((used, annotate("as3sig:public function as3baz():void")));
void baz()
{
  SomeClass sc;
}

int main()
{
  __asm("trace(as3foo());");
  __asm("trace(as3bar());");
  __asm("trace(as3baz());");
  return 0;
}

