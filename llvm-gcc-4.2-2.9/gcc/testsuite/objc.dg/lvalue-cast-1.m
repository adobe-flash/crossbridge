/* APPLE LOCAL file non lvalue assign */
/* { dg-do compile } */
/* { dg-options "-fnon-lvalue-assign" } */

#include <objc/Object.h>

typedef struct _NSPoint {
    float x; 
    float y;
} NSPoint;
typedef NSPoint *NSPointPointer;
typedef NSPoint *NSPointArray;
typedef struct _NSSize {
    float width;
    float height;
} NSSize;
typedef struct _NSRect {
     NSPoint origin;
     NSSize size;
} NSRect;

@interface NSLayoutManager: Object {
  NSRect *_cachedRectArray;
}
- (void)_growCachedRectArrayToSize:(unsigned)newSize;
@end

@implementation NSLayoutManager
- (void)_growCachedRectArrayToSize:(unsigned)newSize {
  ( NSRect *)_cachedRectArray = nil;  /* { dg-warning "target of assignment not really an lvalue" } */
}
@end
