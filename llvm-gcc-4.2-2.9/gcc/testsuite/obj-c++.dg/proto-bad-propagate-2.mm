/* APPLE LOCAL file radar 5676962 */
/* { dg-options "-Wall -Werror" } */
/* { dg-do compile } */

#import <Foundation/Foundation.h>

@class Foo;

#define FooA Foo<AProtocol>
#define FooB Foo<BProtocol>

@protocol AProtocol
- (id) initWithState:(id)state foo:(FooA*)obj;
@end

@protocol BProtocol
- (id) initWithState:(id)state anotherFoo:(FooB*)obj;
@end

@interface Foo : NSObject
- (id) initWithState:(id)state;
@end

@interface Bar : FooB
{
	FooB*		myFoo;
}
@end

@implementation Foo

- (id) initWithState:(id)state
{
	return [super init];
}

@end

@implementation Bar

- (id) initWithState:(id)state anotherFoo:(FooB*)obj
{
	self = [super initWithState:state];
	myFoo = obj;
	return self;
}
@end
