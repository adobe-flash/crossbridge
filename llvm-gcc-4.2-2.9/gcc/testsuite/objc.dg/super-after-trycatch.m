/* APPLE LOCAL file radar 6023694 */
/* Test that no bogus warning is issued as the result of
   'super' being 'volatized' for a try-catch statement. */
/* { dg-options "-Wall" } */
/* { dg-do compile } */

@interface NSObject {
}
- (unsigned)retainCount;
@end

@interface Foo : NSObject {
}
@end

@implementation Foo

- (unsigned)retainCount
{
        [super retainCount];
 @try {
 } @catch(id anything) {
 }
 return [super retainCount];
}

@end
