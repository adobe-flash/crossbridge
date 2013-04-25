/* APPLE LOCAL file radar 4449535 */
/* This program tests that if class implements the forwardInvocation method, then
   every method possible is implemented in the class and gcc should not issue
   warning of the "Method definition not found" kind. */
/* { dg-do compile { target *-*-darwin* } } */

@interface NSObject
@end

@interface NSInvocation
@end

@interface NSProxy
@end

@protocol MyProtocol
        -(void) doSomething;
@end

@interface DestinationClass : NSObject<MyProtocol>
        -(void) doSomething;
@end

@implementation DestinationClass
        -(void) doSomething
        {
        }
@end

@interface MyProxy : NSProxy<MyProtocol>
{
        DestinationClass        *mTarget;
}
        - (id) init;
        - (void)forwardInvocation:(NSInvocation *)anInvocation;
@end

@implementation MyProxy
        - (void)forwardInvocation:(NSInvocation *)anInvocation
        {
        }
	- (id) init {}        
@end
