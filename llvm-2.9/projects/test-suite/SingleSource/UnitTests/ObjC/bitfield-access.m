#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

@interface I0 { 
@public
    unsigned a:15;
    unsigned b: 9;
} 
@end

@implementation I0
@end

void f0(I0 *s) {
    s->b = 1;
}

int main() {
    char *p = valloc(4096*2);
    mprotect(p+4096, 4096, 0);
    I0 *s = (I0 *)(p+4096-4);

    f0(s);

    return 0;
}
