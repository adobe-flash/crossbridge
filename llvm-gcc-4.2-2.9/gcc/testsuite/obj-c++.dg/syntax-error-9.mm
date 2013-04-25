/* APPLE LOCAL file radar 6271728 */
/* Check for error reporting of bad method definition. */
@interface MyClass 
@end


@implementation MyClass
- (void) ni
@end	/* { dg-error "expected `\{' before \\'end\\'" } */
