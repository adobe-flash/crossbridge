/* APPLE LOCAL file radar 4502236 */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run } */
#import <Foundation/Foundation.h>

NSArray * createTestVictim(unsigned capacity) {
    NSMutableArray * arr = [[NSMutableArray alloc] initWithCapacity:capacity];
    int x = 0;

    for(x = 0; x < capacity; x++) {
        NSNumber * num = [NSNumber numberWithInteger:x];
        [arr addObject:num];
    }
    
    NSArray * immutableCopy = [arr copy];
    [arr release];
    
    return immutableCopy;
}

void addStuffUp(NSArray * values) {
    NSInteger accumulator = 0;
//    for (id item in values) {
    id item;
    for (item in values) {
        accumulator += [item integerValue];
    }
}

int main (int argc, char const* argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSArray * target = createTestVictim(10);
    addStuffUp(target);
    [pool release];
    return 0;
}
