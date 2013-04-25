/* APPLE LOCAL file radar 5928316 */
/* Test for use of block pointer in a conditional expression. */
/* { dg-options "-fblocks" } */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];
extern "C" void abort (void);

typedef int (^myblock)(int);

int main() {
        myblock b = ^(int a){ return a * a; };
        if (1 && (b)) {
                int i = b(3);
	        if (i != 9)
		  abort();
		i = b(10);
		if (i != 100)
		  abort();
        }
        return 0;
}


