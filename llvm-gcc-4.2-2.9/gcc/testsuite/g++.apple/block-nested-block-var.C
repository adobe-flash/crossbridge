/* APPLE LOCAL file radar 6225809 - radar 5847213 */
/* __block var used in a nested block neeb be implicitly declared in
   each intervening block. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 " { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

static void Block_copy (void (^I)(void))
{
	I();
}

int main(int argc, char *argv[]) {
    __block int a = 42;
    int save_a = a; // just to keep the address on the stack.

    void (^b)(void) = ^{
        Block_copy(^{
            a = 2;
        });
    };

    Block_copy(b);

    if(a == save_a) {
        printf("**** __block heap storage should have been created at this point\n");
        return 1;
    }
    printf("%s: Success (old %d new %d)\n", argv[0], save_a, a);
    return 0;
}
