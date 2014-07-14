#include <Foundation/Foundation.h>
// rdar: // 8037512

int main()
{
    int state = 0;
    [NSAutoreleasePool new];

    @try {
        state++;
        @try {
            state++;
            @throw [NSObject new];
        }
        @catch (...) {
            state--;
            @throw;
        }
    }

    @catch (...) {
      state--;
    }

    if (state)
      abort();

    @try {
	state++;
        @try {
	    state++;
            @throw [NSObject new];
        }
        @catch (id e) {
	    state--;
            @throw;
        }
    }
    @catch (id e) {
      state--;
    }
    if (state)
      abort();
    return 0;
}
