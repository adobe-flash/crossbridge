#include <stdlib.h>

#define ATTR __attribute__((__ms_struct__))

// GCC only implements #pragma ms_struct on Darwin.
#define HAVE_PRAGMA_MS_STRUCT (defined(__clang__) || defined(__MACH__))

struct {
                   unsigned int bf_1 : 12;
                   unsigned int : 0;
                   unsigned int bf_2 : 12;
} ATTR t1 = {1,2};
static int a1[(sizeof(t1) == 8) -1];

struct
{
                   char foo : 4;
                   short : 0;
                   char bar;
} ATTR t2 = {3,4};
static int a2[(sizeof(t2) == 4) -1];

#if HAVE_PRAGMA_MS_STRUCT
#pragma ms_struct on
struct
{
                   char foo : 4;
                   short : 0;
                   char bar;
} t3 = {5,6};
#pragma ms_struct off
static int a3[(sizeof(t3) == 4) -1];
#endif

struct
{
  char foo : 6;
  int : 0;
} ATTR t4;
static int a4[(sizeof(t4) == 4) -1];

struct
{
  char foo : 4;
  short : 0;
  char bar : 8;
} ATTR t5 = {7,8};
static int a5[(sizeof(t5) == 4) -1];

struct
{
                   char foo : 4;
                   short : 0;
                   long  :0;
                   char bar;
} ATTR t6 = {5, 10};
static int a6[(sizeof(t6) == 4) -1];

struct
{
                   char foo : 4;
                   int  :0;
                   short : 0;
                   char bar;
} ATTR t7 = {5, 10};
static int a7[(sizeof(t7) == 8) -1];

struct
{
                   char foo : 4;
                   short : 0;
                   int  :0;
                   char bar:7;
} ATTR t8 = {5, 10};
static int a8[(sizeof(t8) == 4) -1];

struct
{
                   char foo : 4;
                   char : 0;
                   short : 0;
                   int : 0;
                   long  :0;
                   char bar;
} ATTR t10 = {5, 10};
static int a10[(sizeof(t10) == 2) -1];

int main() {
  if (t1.bf_1 != 1 || t1.bf_2 != 2)
    abort();
  if (t2.foo != 3 || t2.bar != 4)
    abort();
#if HAVE_PRAGMA_MS_STRUCT
  if (t3.foo != 5 || t3.bar != 6)
    abort();
#endif
  if (t5.foo != 7 || t5.bar != 8)
    abort();
  if (t6.foo != 5 || t6.bar != 10)
    abort();
  if (t7.foo != 5 || t7.bar != 10)
    abort();
  if (t8.foo != 5 || t8.bar != 10)
    abort();
  if (t10.foo != 5 || t10.bar != 10)
    abort();
  return 0;
}

