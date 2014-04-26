/* APPLE LOCAL file radar 5928316 */
/* Test for use of block pointer in a conditional expression. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
void * _NSConcreteStackBlock[32];

typedef int (^myblock)(int);

int main() {
        myblock b = ^(int a){ return a * a; };
        if (1 && (b)) {
                int i = b(3);
                printf("i = %d\n", i);
        }
        return 0;
}


