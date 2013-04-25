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

struct Foo
{
  Foo()
  {
    printf("Foo constructor\n");
  }
  ~Foo()
  {
    printf("Foo destructor\n");
  }
};

struct Bar
{
  Bar()
  {
    printf("Bar constructor\n");
  }
  ~Bar()
  {
    printf("Bar destructor\n");
  }
};

struct Baz
{
  Baz()
  {
    printf("Baz constructor\n");
  }
  ~Baz()
  {
    printf("Baz destructor\n");
  }
};

void throwSomething(int n)
{
  if(n == 0)
  {
    printf("throwing Bar\n");
    throw Bar();
  }
  if(n == 1)
  {
    printf("throwing Baz\n");
    throw Baz();
  }
  if(n == 2)
  {
    printf("throwing int (14)\n");
    throw 14;
  }
  printf("throwing long (won't be caught!; 15L)\n");
  throw 15L;
}

struct Bazzle
{
  Bazzle()
  {
    printf("Bazzle constructor\n");
  }

  ~Bazzle()
  {
    printf("Bazzle destructor\n");
  }

} gBazzle;

struct Foozle
{
  Foozle()
  {
    printf("Foozle constructor\n");
  }

  ~Foozle()
  {
    printf("Foozle destructor\n");
  }
} gFoozle;

int main()
{
  for(int n = 0; ; n++)
  {
    printf("\n*** catch test %d\n", n);
    try
    {
      Foo foo;
  
      throwSomething(n);
    }
    catch(Bar &b)
    {
      printf("caught Bar\n");
    }
    catch(Baz &bz)
    {
      printf("caught Baz\n");
    }
    catch(int i)
    {
      printf("caught int (%d)\n", i);
    }
  }
  return (int)&gFoozle ^ (int)&gBazzle; // force linkage
}
