/* APPLE LOCAL file radar 5732232 - radar 6230297 - blocks */
/* Modified for radar 6169527 */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void takeblock (void (^)());

int main() {
  int one = 1;
  while (1) {
    takeblock(^{
	break;		/* { dg-error "break statement not within loop or switch" } */
	while (one) break;/* ok */
	goto label1;	/* { dg-error "goto not allowed in block literal" } */
			/* { dg-error "label" "" { target *-*-* } 14 } */
      });	/* { dg-error "enters" } */
  label1:	/* { dg-error "jump to label" } */
    break; /* OK */
    if (1)
      continue; /* OK */
  }

  void (^vcl)(void) =
    ^{
    break; /* { dg-error "break statement not within loop or switch" } */

    while (1) {
      void (^vcl1) (void) = ^{};

      break;
    }
  };

  void (^VCL)(void) =
    ^{
    while (1) {
      int i;
      void (^vcl1) (void) = ^{ continue; }; /* { dg-error "continue statement not within a loop" } */
      break;
      for (i = 0; i < 100; i++)
	if (i == 10)
	  break;
    }
  };
  goto label1; /* { dg-error "from here" } */
}
