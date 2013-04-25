/* APPLE LOCAL file radar 5453108 */
/* Check that no warning is issued when protocl's method matches a
   @dynamic implementation of a property with same name. */
/* { dg-options "-Wall -Werror" } */

@protocol QueryItem

- (id) children;

@end


@interface QueryNode <QueryItem>

@property (retain) id children;

@end


@implementation QueryNode

@dynamic children;

@end

