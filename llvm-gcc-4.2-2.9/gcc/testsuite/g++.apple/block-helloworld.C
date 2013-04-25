/* APPLE LOCAL file radar 6040305 */
/* Test that we can run a simple block literal. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

void* _NSConcreteStackBlock[32];

int main()
{
    void (^xxx)(void) = ^{ printf("Hello world!\n"); };
    
    xxx();
    return 0;
}

