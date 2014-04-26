/* APPLE LOCAL file radar 4360010 */
/* Check that illegal usage of storage class 'static' does not ICE. */
/* { dg-do compile { target *-*-darwin* } } */

@interface Foo {
	static  int iFoo; /* { dg-error "storage class specified" } */
}
@end
