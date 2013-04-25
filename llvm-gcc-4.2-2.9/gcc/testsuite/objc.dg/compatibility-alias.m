/* APPLE LOCAL file radar 5835805 */
/* Test use of alias class name in defining a new class and its super class
   results in proper diagnostics if class/super class is being mis-used.
*/
/* { dg-do compile } */

@interface Super @end

@interface MyWpModule @end

@compatibility_alias  MyAlias MyWpModule;

@compatibility_alias  AliasForSuper Super;

@interface MyAlias : AliasForSuper // expected-error {{duplicate interface declaration for class 'MyWpModule'}}
@end /* { dg-error "duplicate interface declaration for class 'MyWpModule'" } */
     /* { dg-error "redefinition of 'struct MyWpModule'" "" { target *-*-* } 16 } */

@implementation MyAlias : AliasForSuper /* { dg-error "conflicting super class name 'Super'" } */
@end

