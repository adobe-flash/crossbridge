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

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __AVM2 // HACK
double getMS()
{
  double ms;
  __asm__ volatile ("%0 = (new Date).time" : "=r"(ms));
  return ms;
}
#endif

static void nop()
{
}

static const char *nop_ni() __attribute__((noinline));
static const char *nop_ni()
{
  __asm__ volatile (""); // don't outsmart us, compiler!
  nop();
}

static const char *skip1(const char *s)
{
  __asm__ volatile (""); // don't outsmart us, compiler!
  return s + 1;
}

// no-inline wrapper so there's an actual call in both C and AS3 land
// from the test loop
static const char *skip1_ni(const char *s) __attribute__((noinline));
static const char *skip1_ni(const char *s)
{
  return skip1(s);
}

static int noti(int n) __attribute__((always_inline));
static int noti(int n)
{
  __asm__ volatile ("");
  return ~n;
}

static int noti_ni(int n) __attribute__((noinline));
static int noti_ni(int n)
{
  return noti(n);
}

static int xori(int a, int b, int c, int d, int e, int f, int g, int h)
{
  return a ^ b ^ c ^ d ^ e ^ f ^ g ^ h;
}

static int xori_ni(int a, int b, int c, int d, int e, int f, int g, int h) __attribute__((noinline));
static int xori_ni(int a, int b, int c, int d, int e, int f, int g, int h)
{
  __asm__ volatile ("");
  return xori(a, b, c, d, e, f, g, h);
}

static double negd(double d)
{
  __asm__ volatile ("");
  return -d;
}

static double negd_ni(double d) __attribute__((noinline));
static double negd_ni(double d)
{
  return negd(d);
}

static void test1(int n)
{
  int i;
  for(i = 0; i < n; i++)
    skip1_ni("hello, world!");
}

static void test2(int n)
{
  int i;
  for(i = 0; i < n; i++)
    noti_ni(1234);
}

static void test3(int n)
{
  int i;
  for(i = 0; i < n; i++)
    negd_ni(1234.0);
}

static void test4(int n)
{
  int i;
  for(i = 0; i < n; i++)
    nop_ni();
}

static void test5(int n)
{
  int i;
  for(i = 0; i < n; i++)
    xori_ni(-1, 1, 0, 0x80000000, 0x7fffffff, -2, 2, -3);
}

static void runtest(const char *name, void (*fun)(int))
{
  int reps = 4;
  int ms;
  printf("running %s\n", name);
  do
  {
    double start, end;
    reps *= 2;
    start = getMS();
    fun(reps);
    end = getMS();
    ms = end - start;
  }
  while(ms < 1000);
  printf("(%d / s) %d reps %d ms\n", (int)(((double)reps * 1000.0) / (double)ms), reps, ms);
}

#ifdef __AVM2
static void as3nop() __attribute__((used, naked, annotate("as3sig:public function nop():void")));
static void as3nop()
{
  nop();
}

static void as3skip1() __attribute__((used, annotate("as3sig:public function skip1(s:String):String")));
static void as3skip1()
{
  const char *p;
  __asm__("%0 = CModule.allocaLatin1String(s); esp = ESP" : "=r"(p));
  p = skip1(p);
  __asm__("ESP = ebp; return CModule.readLatin1String(%0, s.length-1)" : : "r"(p));
}

static void as3skip1_2() __attribute__((used, annotate("as3sig:public function skip1_2(s:String):String")));
static void as3skip1_2()
{
  int len;
  const char *p;
  __asm__("%0 = s.length" : "=r"(len));
  p = (const char *)alloca(len);
  __asm__("ram.position = %0; ram.writeUTFBytes(s)" : : "r"(p));
  p = skip1(p);
  __asm__("ESP = ebp; ram.position = %0; return ram.readUTFBytes(%1)" : : "r"(p), "r"(len-1));
}

static void as3noti() __attribute__((used, naked, annotate("as3sig:public function noti(n:int):int")));
static void as3noti()
{
  int n;
  __asm__("%0 = n" : "=r"(n));
  n = noti(n);
  __asm__("return %0" : : "r"(n));
}

static void as3xori() __attribute__((used, naked, annotate("as3sig:public function xori(a:int, b:int, c:int, d:int, e:int, f:int, g:int, h:int):int")));
static void as3xori()
{
  int a, b, c, d, e, f, g, h;
  __asm__("%0 = a" : "=r"(a));
  __asm__("%0 = b" : "=r"(b));
  __asm__("%0 = c" : "=r"(c));
  __asm__("%0 = d" : "=r"(d));
  __asm__("%0 = e" : "=r"(e));
  __asm__("%0 = f" : "=r"(f));
  __asm__("%0 = g" : "=r"(g));
  __asm__("%0 = h" : "=r"(h));
  a = xori(a, b, c, d, e, f, g, h);
  __asm__("return %0" : : "r"(a));
}

static void as3negd() __attribute__((used, naked, annotate("as3sig:public function negd(n:Number):Number")));
static void as3negd()
{
  double n;
  __asm__("%0 = n" : "=r"(n));
  n = negd(n);
  __asm__("return %0" : : "r"(n));
}

static void as3test1(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) skip1(\"hello, world!\")" : : "r"(n));
}

static void as3test1_2(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) skip1_2(\"hello, world!\")" : : "r"(n));
}

static void as3test2(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) noti(1234)" : : "r"(n));
}

static void as3test3(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) negd(1234.0)" : : "r"(n));
}

static void as3test4(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) nop()" : : "r"(n));
}

static void as3test5(int n)
{
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) xori(-1, 1, 0, 0x80000000, 0x7fffffff, -2, 2, -3)" : : "r"(n));
}

// AS3 only

static void as3dot() __attribute__((used, naked, annotate("as3sig:public function dot(a:Vector.<Number>, b:Vector.<Number>):Number")));
static void as3dot()
{
  double result = 0;
  int len, i;

  __asm__ ("%0 = a.length" : "=r"(len));
  for(i = 0; i < len; i++)
  {
    double a, b;

    __asm__("%0 = a[%1]" : "=r"(a) : "r"(i));
    __asm__("%0 = b[%1]" : "=r"(b) : "r"(i));
    result += (a*b);
  }
  __asm__("return %0" : : "r"(result));
}

static void as3test6(int n)
{
  __asm__ volatile ("var a:Vector.<Number> = new Vector.<Number>(1000)");
  __asm__ volatile ("var b:Vector.<Number> = new Vector.<Number>(1000)");
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) dot(a, b)" : : "r"(n));
}

static void as3test7(int n)
{
  __asm__ volatile ("var a:Vector.<Number> = new Vector.<Number>(1000000)");
  __asm__ volatile ("var b:Vector.<Number> = new Vector.<Number>(1000000)");
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) dot(a, b)" : : "r"(n));
}

static void as3test8(int n)
{
  __asm__ volatile ("var a:Vector.<Number> = new Vector.<Number>(10000000)");
  __asm__ volatile ("var b:Vector.<Number> = new Vector.<Number>(10000000)");
  __asm__ volatile ("for(var n:int = 0; n < %0; n++) dot(a, b)" : : "r"(n));
}
#endif

int main()
{
  runtest("String", test1);
  runtest("int", test2);
  runtest("Number", test3);
  runtest("void", test4);
  runtest("int x 8", test5);
#ifdef __AVM2
  runtest("AS3 String", as3test1);
  runtest("AS3 String 2", as3test1_2);
  runtest("AS3 int", as3test2);
  runtest("AS3 Number", as3test3);
  runtest("AS3 void", as3test4);
  runtest("AS3 int x 8", as3test5);
  runtest("AS3 dot 1000", as3test6);
  runtest("AS3 dot 1000000", as3test7);
  runtest("AS3 dot 10000000", as3test8);
#endif
  return 0;
}
