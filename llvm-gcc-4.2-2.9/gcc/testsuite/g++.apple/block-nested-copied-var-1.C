/* APPLE LOCAL file radar 5988451 - nested blocks */
/* This routine tests a nested block when a copied-in variable in the
   inner block must be 'frozen' at the outer block so, it has the value
   at the point of inner block declaration and not when block is envoked.
*/
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

extern "C" void abort(void);

void * _NSConcreteStackBlock[32];


void callVoidVoid(void (^closure)(void)) {
  closure();
}

int main(int argc, char *argv[]) {
  int local_i_var = 1;

  void (^vv)(void) = ^{
    if (argc > 0) {
      callVoidVoid(^{ if (local_i_var != 1) abort();  });
    }
  };

  local_i_var = 2;
  vv();
  return 0;
}
