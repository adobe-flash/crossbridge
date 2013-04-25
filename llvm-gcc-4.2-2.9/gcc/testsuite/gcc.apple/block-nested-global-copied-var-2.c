/* APPLE LOCAL file radar global blocks */
/* More testing of copied in variables in nested blocks. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks -mmacosx-version-min=10.6 -ObjC -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>
void *_NSConcreteGlobalBlock;

id Global = nil;

void callVoidVoid(void (^closure)(void)) {
    closure();
}

int argc = 1;

id x;

void (^vv)(void) = ^{
        if (argc > 0) {
            callVoidVoid(^{ Global = x; });
        }
};

int main() {
    x = [[NSObject alloc] init];
    id initial_x = x;

    vv();

    if (Global != initial_x) {
        exit(1);
    }
    return 0;
}
