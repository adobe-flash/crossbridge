/* APPLE LOCAL file radar 6267049 */
typedef int MyTypedef;

@interface MyClass

@property MyTypedef <Gobbledygook> myProperty; /* { dg-error "qualified type is not a valid object" } */
	/* { dg-error "cannot find protocol declaration for \\'Gobbledygook\\'" "" { target *-*-* } 6 } */

@end

