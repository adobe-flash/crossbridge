/* APPLE LOCAL file radar 5811887 */
/* Test that with -std=c99 blocks is off by default. */
/* { dg-do compile } */
/* { dg-options "-std=c99" } */

int main()
{
	void (^Vblock)(void) = ^{};	/* { dg-error "expected identifier" } */
}
