/* APPLE LOCAL file radar 5390587 */
/* Test that use of property name as 'getter' attribute of a 'readonly' property
   does not confuse gcc into issuing bogus error. */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */


@protocol Test
  @property (readonly, getter = PROPERTY1) int ANOTHER_READONLY_PROPERTY;

  @property int PROPERTY1;

  @property (readonly, getter = PROPERTY1) int READONLY_PROPERTY;

@end

@interface Test <Test> {
  int ivar;
}
- (int) PROPERTY1;
- (void) setPROPERTY1:(int)value;
@end

int main ()
{
	Test *x;

	x.PROPERTY1 = 200 + x.READONLY_PROPERTY;	/* OK */
	x.READONLY_PROPERTY = x.PROPERTY1 - 100; /* { dg-error "object cannot be set - either readonly property or no setter found" }  */
	x.ANOTHER_READONLY_PROPERTY = x.PROPERTY1 - 100; /* { dg-error "object cannot be set - either readonly property or no setter found" }  */
	return 0;
}

