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

#include <setjmp.h>
#include <stdio.h>

typedef struct sjljLinkStruct
{
  jmp_buf jb;
  struct sjljLinkStruct *prev;
} *sjljLink;

volatile int gR = 0;
volatile int gL = 0;

// iteratively recurse forward 23 frames, longjmp back 19, until reaching
// desired depth, and then print corresponding char
void fwd(int c, int depth, int count, sjljLink prev)
{
  struct sjljLinkStruct sjljL;
  int r;

  gR++;
  sjljL.prev = prev;
  r = setjmp(sjljL.jb);
  if(!r && count)
    fwd(c, depth+1, count-1, &sjljL); // no lonjgmp detected, continue recursing
  else if(depth < c)
    fwd(c, depth+1, 23, &sjljL); // recurse 23 deeper
  else if(depth > c) // too deep!
  {
    int i;

    for(i = 1; i < 19; i++)
      prev = prev->prev;
    gL++;
    longjmp(prev->jb, 1); // longjmp back 19 frames
  }
  else
    putchar(depth); // right depth... print
}

void test2_a(jmp_buf jb1, jmp_buf jb2, int n)
{
  if(n)
    longjmp(jb2, 12);
  longjmp(jb1, 13);
}

void test2(int n)
{
  jmp_buf jb1, jb2;
  int i;

  if((i = setjmp(jb1)))
  {
    printf("jb1 %d\n", i);
    return;
  }
  if((i = setjmp(jb2)))
  {
    printf("jb2 %d\n", i);
    return;
  }
  test2_a(jb1, jb2, n);
}

int main()
{
  const char *s = "Hello, world!\nEach character is printed via a bunch of needless recursion and setjmp/longjmp\n";

  printf("test 1\n");
  setbuf(stdout, NULL);
  while(*s)
    fwd(*s++, 0, 0, NULL);
  printf("%d longjmps, %d fwd invokes\n", gL, gR);

  printf("test 2\n");
  test2(0);
  test2(1);
  return 0;
}
