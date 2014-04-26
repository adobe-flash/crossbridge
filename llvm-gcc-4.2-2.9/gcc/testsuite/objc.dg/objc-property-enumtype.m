/* APPLE LOCAL file radar 6264448 */
/* Test that property of enum types don't cause error
   when setter call is generated. */
/* { dg-do run { target *-*-darwin* } } */ 
/* { dg-options "-framework Foundation -Werror" { target arm*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -Werror" { target i?86-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -Werror" { target powerpc*-*-darwin* } } */

#import <Foundation/Foundation.h>

enum TimeType {
    TimeTypeZero = 0,
    TimeTypeOne = 1,
    TimeTypeTwo = 2
};

typedef enum TimeType TimeType;
 

@interface A : NSObject { 
    TimeType tt; 
}

@property (nonatomic) TimeType tt;

@end

@interface B : NSObject { 
    TimeType tt; 
}

@property (nonatomic) TimeType tt;

@end

@implementation A

@synthesize tt;

@end

@implementation B

@synthesize tt;

@end

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    A* a = [[A alloc] init];
    B* b = [[B alloc] init];
    
    a.tt = TimeTypeOne;
    // failure here:
    b.tt = a.tt;
    
    b.tt = [a tt];
    
    [b setTt: [a tt]];

    [pool drain];
    return 0;
}
