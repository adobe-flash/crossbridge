/* APPLE LOCAL file radar 6268817 */
/* Testing use of block literal expression in ObjC's enumeration foreach-statement loop header. */
/* { dg-options "-mmacosx-version-min=10.6 -ObjC -framework Foundation" { target *-*-darwin* } } */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>

@interface TargetSymbol : NSObject {
    NSString *      _symbolName;
}
@property (copy) NSString *name;
@end

@implementation TargetSymbol
@synthesize name = _symbolName;
@end

static void Looper(NSArray *symbols) {
    // 1. Block literal assigned to a block pointer, which is used in loop: build succeeds
    NSComparator comparator = ^(id target1, id target2){return [((TargetSymbol *)target1).name compare:((TargetSymbol *)target2).name];};
    
    for (id symbol in [symbols sortedArrayUsingComparator:comparator]) {
        // Do something
    }

    // 2. Block expression literal used, but still outside loop: build succeeds
    NSArray *sortedSymbols = [symbols sortedArrayUsingComparator:^(id target1, id target2){return [((TargetSymbol *)target1).name compare:((TargetSymbol *)target2).name];}];
    
    for (id symbol in sortedSymbols) {
        // Do something
    }

    // 3. Block expression literal used directly in loop: build fails
    for (id symbol in [symbols sortedArrayUsingComparator:^(id target1, id target2){return [((TargetSymbol *)target1).name compare:((TargetSymbol *)target2).name];}]) {
        // Do something
    }

    // 3. Block expression literal used directly in loop: build fails
    for (id symbol in [symbols sortedArrayUsingComparator:^(id target1, id target2){return [((TargetSymbol *)target1).name compare:((TargetSymbol *)target2).name];}]) {
        // Do something
    }

    // 3. Block expression literal used directly in loop: build fails
    for (id symbol in [symbols sortedArrayUsingComparator:^(id target1, id target2){return [((TargetSymbol *)target1).name compare:((TargetSymbol *)target2).name];}]) {
        // Do something
    }
}


int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
    [pool drain];
    return 0;
}
