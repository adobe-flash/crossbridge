/* APPLE LOCAL file radar 5732232 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */


int (^IFP) ();
int (^II) (int);
int main() {
	int (^PFR) (int) = IFP;	/* { dg-error "incompatible block pointer types initializing" } */
	PFR = II;


	int (^IFP) () = PFR;	/* This is now ok in c (radar 6196572) */


	const int (^CIC) () = IFP;

	const int (^CICC) () = CIC;

	int * const (^IPCC) () = 0;

	int * const (^IPCC1) () = IPCC;

	int * (^IPCC2) () = IPCC;	

	int (^IPCC3) (const int) = PFR;	

	int (^IPCC4) (int, char (^CArg) (double));


	int (^IPCC5) (int, char (^CArg) (double)) = IPCC4;

	int (^IPCC6) (int, char (^CArg) (float))  = IPCC4; /* { dg-error "incompatible block pointer types initializing" } */

	IPCC2 = 0;
	IPCC2 = 1; /* { dg-error "invalid conversion assigning integer" } */
	int (^x)() = 0;
	int (^y)() = 3;   /* { dg-error "invalid conversion initializing integer" } */
	int a = 1;
	int (^z)() = a+4;   /* { dg-error "invalid conversion initializing integer" } */
	return 0;
}

int blah() {
	int (^IFP) (float);
	char (^PCP)(double, double, char);

	IFP(1.0);
	IFP (1.0, 2.0);	/* { dg-error "too many arguments to block call" } */

	char ch = PCP(1.0, 2.0, 'a');
	return PCP(1.0, 2.0);	/* { dg-error "too few arguments to block" } */
}

void foo() {

  /*  compare with: int (*xx1)(char *s);
      int (*xx2)(const char *s) = xx1; */
  /* APPLE LOCAL radar 6246965 */
  int (^xx)(const char *s) = ^(char *s) { return 1; };  /* { dg-error "incompatible block pointer types initializing" } */

}

