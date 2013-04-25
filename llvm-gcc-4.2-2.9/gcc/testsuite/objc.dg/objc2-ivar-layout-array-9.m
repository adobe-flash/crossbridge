/* APPLE LOCAL file radar 5082000 */
/* Test of ivar layour involving large array ivar. */
/* { dg-do compile { target *-*-darwin* } } */

typedef struct _NSAttributeDictionaryElement {
    unsigned int hash;
    unsigned key;
    unsigned value;
} NSAttributeDictionaryElement;

@interface NSTableOptions {
    unsigned numElements;
    unsigned refCount;
    NSAttributeDictionaryElement elements[1024];
}
@end

@implementation NSTableOptions
@end
