/* APPLE LOCAL file radar 5732232 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

typedef void (^CL)(void);

CL foo() {
  short y;
  short (^add1)(void) = ^{ return y+1; };  /* { dg-error "incompatible block pointer types initializing" } */

  CL X = ^{
    if (2)
      return;
    return 1;		/* { dg-error "void block should not return a value" } */
  };

  int (^Y) (void)  = ^{
    if (3)
      return 1;
    else
      return;		/* { dg-error "non-void block should return a value" } */
  };

  char *(^Z)(void) = ^{
    if (3)
      return "";
    else
      return (char*)0;
  };

  double (^A)(void) = ^ {
    if (1)
      return (float)1.0;
    else
      if (2)
	return (double)2.0;	/* { dg-error "incompatible type returning" } */
    return 1;		/* { dg-error "incompatible type returning" } */
  };			/* { dg-error "incompatible block pointer types initializing" } */
  char *(^B)(void) = ^{
    if (3)
      return "";
    else
      return 2;		/* { dg-error "incompatible type returning" } */
			/* { dg-warning "return makes pointer from integer without a cast" "" { target *-*-* } 43 } */
  };

  return ^{ return 1; };	/* { dg-error "incompatible block pointer types initializing" } */
}
