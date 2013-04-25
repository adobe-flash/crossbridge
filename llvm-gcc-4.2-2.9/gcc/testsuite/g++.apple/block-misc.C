/* APPLE LOCAL file radar 5732232 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void donotwarn();

int (^IFP) ();
int (^II) (int);
int test1() {
  int (^PFR) (int) = 0;	// OK
  PFR = II;	// OK

  if (PFR == II)	// OK
    donotwarn();

  if (PFR == IFP)  /* { dg-error "comparison between distinct pointer types" } */
    donotwarn();

  if (PFR == (int (^) (int))IFP) // OK
    donotwarn();

  if (PFR == 0) // OK
    donotwarn();

  if (PFR)	// OK
    donotwarn();

  if (!PFR)	// OK
    donotwarn();

  return PFR != IFP;  /* { dg-error "comparison between distinct pointer types" } */
}

int test2(double (^S)()) {
  double (^I)(int)  = (double (^)(int))(void*) S;
  (void*)I = (void *)S;  /* { dg-warning "target of assignment not really an lvalue; this will be a hard error in the future" } */

  void *pv = I;

  pv = S;		

  I(1);

  return (void*)I == (void *)S;
}

int^ x;  /* { dg-error "block pointer to non-function type is invalid" } */
int^^ x1; /* { dg-error "block pointer to non-function type is invalid" } */

int test3() {
  char *^ y;  /* { dg-error "block pointer to non-function type is invalid" } */
}
