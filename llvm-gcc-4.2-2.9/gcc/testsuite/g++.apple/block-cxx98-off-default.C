/* APPLE LOCAL file radar 5811887 */
/* Test that with -std=c++98 blocks is off by default. */
/* { dg-do compile } */
/* { dg-options "-std=c++98" } */

int main() {
  void (^Vblock)(void) = ^{};	/* { dg-error "expected primary" } */
				/* { dg-error "was not declared" "" { target *-*-* } 7 } */
				/* { dg-error "expected" "" { target *-*-* } 7 } */
}
