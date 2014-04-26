/* APPLE LOCAL file radar 4505126 */
/* Test lookup of properties in protocol */
/* Program should compile with no error or warning. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@protocol Rectangle
@property int foo;
@end

@protocol Square
@end

@interface Label <Square, Rectangle>
@end;

int  main ()
{
	Label <Square, Rectangle> * p;
	int gg = p.foo;
	p.foo = 200;
}
