/* APPLE LOCAL file radar 6096219 */
/* Test that __block attribute can be used directly. */
/* { dg-options "-mmacosx-version-min=10.5 -fblocks" { target *-*-darwin* } } */
/* { dg-do compile } */

int main()
{
        __block int JJJJ;
	__attribute__((__blocks__(byref))) int III;

	int (^XXX)(void) = ^{ return III+JJJJ; };
}

