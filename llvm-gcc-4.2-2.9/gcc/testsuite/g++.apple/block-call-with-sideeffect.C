/* APPLE LOCAL file radar 6396238 */
/* { dg-options "-mmacosx-version-min=10.6 -Wall" { target *-*-darwin* } } */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

typedef void (^P)(void);

static P p = ^{ printf("inside block\n"); };

static int count;

extern "C" void abort(void);
void * _NSConcreteGlobalBlock;

P mkblock(void)
{
        printf("outside block\n");
        ++count;
	return p;
}

int main(void)
{
        mkblock()();
        if (count != 1)
          abort();
        return 0;
}

