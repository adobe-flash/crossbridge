/* APPLE LOCAL file radar 6545782 */
/* Test that no trampoline is generated for this test case. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <Block.h>

extern "C" void __enable_execute_stack ()
{
        abort();
}

extern "C" uint64_t
dispatch_benchmark(size_t count, void (^block)(void));

class A {
public:
    void foo();
};

void A::foo() {}

class B {
public:
    void createBlockInMethod(A &a);
    void callBlockPassedIn(A &a, void(^aBlock)());
};

void B::createBlockInMethod(A &a) {
    void (^block) () = ^{ a.foo(); };
    block();
}

void B::callBlockPassedIn(A &a, void(^aBlock)()) {
    aBlock();
}


int main(int argc __attribute__((unused)), char **argv) {
    __block B b;
    __block A a;
    
    __block int x = 0;
    
    uint64_t cyclesPerBlock1 = dispatch_benchmark(1000000, ^{
        b.createBlockInMethod(a);
    });
        
    uint64_t cyclesPerBlock2 = dispatch_benchmark(1000000, ^{
        b.callBlockPassedIn(a, ^{ a.foo(); });
    });
    
    fprintf(stdout, "%s: %llu cycles/iter vs. %llu cycles/iter: %5.5f slower\n", argv[0],
            cyclesPerBlock1, cyclesPerBlock2,
            ((double)cyclesPerBlock1) / ((double)cyclesPerBlock2));
    
    return 0;
}


