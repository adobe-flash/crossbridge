/* APPLE LOCAL file radar 5988451 */
/* More testing of copied in variables in nested blocks. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks -mmacosx-version-min=10.5 -ObjC++ -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>
void *_NSConcreteStackBlock[32];

id Global = nil;

void callVoidVoid(void (^closure)(void)) {
    closure();
}

int main(int argc, char *argv[]) {
    id x = [[NSObject alloc] init];
    id initial_x = x;

    void (^vv)(void) = ^{
        if (argc > 0) {
            callVoidVoid(^{ Global = x; });
        }
    };

    x = nil;
    vv();
    if (Global != initial_x) {
        exit(1);
    }
    return 0;
}
