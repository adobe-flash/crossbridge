/* APPLE LOCAL file 4430139 */
#include <string.h>

extern void abort();
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */

typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma reverse_bitfields on
#pragma ms_struct on

#define USE_STRUCT_WRAPPER 0

#pragma pack(push, 2)
typedef struct
{
	DWORD	a;
#if USE_STRUCT_WRAPPER
	struct {
#endif
	WORD	b:2,
		c:14;
#if USE_STRUCT_WRAPPER
	};
#endif
	DWORD	d:22,
		e:10;
	DWORD	f;
	DWORD	g:25,
		h:1,
		i:6;
#if USE_STRUCT_WRAPPER
	struct {
#endif
	WORD	j:14,
		k:2;
#if USE_STRUCT_WRAPPER
	};
#endif
} Foo;
typedef union { Foo x; int y[6]; } u1;
#pragma pack(pop)


int main(int argc, char* argv[])
{
    Foo foo;
    int i;
    u1 U;
    memset (&U, 0, sizeof(u1));
    U.x.a = 1;
    U.x.b = 1;
    U.x.c = 1;
    U.x.d = 1;
    U.x.e = 1;
    U.x.f = 1;
    U.x.g = 1;
    U.x.h = 1;
    U.x.i = 1;
    U.x.j = 1;
    U.x.k = 1;

    // s should be == 20.  In the bad case, it ends up as 22
    int s = sizeof(Foo);

    if (sizeof(Foo) != 20
	|| U.y[0] != 0x00000001
	|| U.y[1] != 0x00050040
	|| U.y[2] != 0x00010000
	|| U.y[3] != 0x00010600
	|| U.y[4] != 0x00014001
	|| U.y[5] != 0x00000000)
      abort();
    return 0;
}
