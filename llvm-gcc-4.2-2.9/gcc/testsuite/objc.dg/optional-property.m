/* APPLE LOCAL file radar 5777307 */
/* This program tests that when a property is @optional, with @synthesize, we still synthesize
   setter/getters. Program should compile and run with no errors. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */
/* APPLE LOCAL ARM not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */

#import <Cocoa/Cocoa.h>


@interface BaseClass : NSObject {
    id _delegate;
}
@end

@protocol MyProtocol<NSObject>
@optional
@property(assign) id delegate;
@end

@interface SubClass : BaseClass <MyProtocol> {

}

@end

@implementation BaseClass @end 

@implementation SubClass
@synthesize delegate = _delegate;
@end

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    id<MyProtocol> sc = [SubClass new];
    id o = [NSObject new];
    
    [sc setDelegate:o];
    
    [pool drain];
    return 0;
}

