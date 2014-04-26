/* APPLE LOCAL file 5724385 */
/* { dg-options "-std=c99 -mmacosx-version-min=10.5 -m64 -lobjc" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-std=c99 -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/runtime.h>
#include <stdint.h>
#include <stdio.h>

@interface Foo {
@private
    int first;
    int :1;
    int third :1;
    int :1;
    int fifth :1;
}
@end
@implementation Foo 
+(void)initialize { }
@end

int main()
{
    Class cls = objc_getClass("Foo");
    unsigned int count;
    Ivar *iv_list = class_copyIvarList(cls, &count);
    fprintf(stderr, "count %d\n", count);
    for (unsigned int i = 0; i < count; i++) {
        const char *ivarname = ivar_getName(iv_list[i]);
        fprintf(stderr, "ivar: %s (%p)\n", ivarname ? ivarname : "", iv_list[i]);
        const char *ivartype = ivar_getTypeEncoding(iv_list[i]);
        fprintf(stderr, "type: %s\n", ivartype);
        ptrdiff_t ivaroffset = ivar_getOffset(iv_list[i]);
	/* APPLE LOCAL default to Wformat-security 5764921 */
        fprintf(stderr, "offset: %x\n", (int)ivaroffset);
        fprintf(stderr, "\n");
    }
    return 0;
}
    
