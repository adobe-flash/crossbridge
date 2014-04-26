/* APPLE LOCAL file radar 5988451 - nested blocks */
/* This test is to make sure that the inner block uses the value of the
   copied-in variable at its point of declaration and not when block is envoked.
*/
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

extern void abort(void);

void * _NSConcreteStackBlock[32];


void callVoidVoid(void (^closure)(void)) {
    closure();
}

int main(int argc, char *argv[]) {
    int local_i_var = 1;

    void (^vv)(void) = ^{
        if (local_i_var != 1)
	  abort();
        if (argc > 0) {
            callVoidVoid(^{ if (local_i_var != 1) abort();  });
        }
    };

    local_i_var = 2;
    vv();
    return 0;
}
