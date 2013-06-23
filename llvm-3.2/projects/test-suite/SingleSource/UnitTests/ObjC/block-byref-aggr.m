#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>

typedef struct { uintptr_t v; } RetType;
#define logRetType(msg, val) printf(msg, val.v);

RetType func(dispatch_block_t block);

RetType func(dispatch_block_t block) {
        block = Block_copy(block);
        block();
        
        RetType ret = {4};
        return ret;
}

int main (int argc, const char * argv[]) {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        __block RetType a = {0};
        dispatch_block_t b = ^{
                logRetType("a was originally: %ld\n", a);
        };
        
        a = func(b);
        logRetType("a is now: %ld\n", a);
        
        // b was Block_copied already, so the result is different.
        
        a = func(b);
        logRetType("a has become: %ld\n", a);
                
        [pool drain];
    return 0;
}

