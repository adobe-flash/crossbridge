/* APPLE LOCAL file radar 6083666 */
/* Test to check that 1) no warning/error is issued when an 'ivar' which has
   a matching name with property has a type mismatch with that property. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

@interface NSArray @end

@interface NSMutableArray : NSArray
@end

@interface Class1 
{
 NSMutableArray* pieces;
 double* unrelated;
}

@property (readonly) NSArray* pieces;
@property (readonly) NSArray* unrelated;
@property (readonly) NSMutableArray* prop_user_getter;

@end

@interface Class2  {
 Class1* container;
}

@end

@implementation Class2

- (void) lastPiece
{
 container.pieces;
 container.unrelated;
 container.prop_user_getter;
}

@end
