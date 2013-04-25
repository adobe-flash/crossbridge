/* APPLE LOCAL file ObjC GC */
/* A compile-only test for insertion of write barriers. */
/* Developed by Ziemowit Laski  <zlaski@apple.com>  */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc -Wno-non-lvalue-assign -Wassign-intercept" } */
/* { dg-require-effective-target objc_gc } */

#include <objc/objc.h>

struct NSRect {
  void *a;
  id b;
  __strong void *c;
  __strong id *d;
};
  
@protocol WebCoreImageRenderer
- (void)drawImageInRect:(NSRect)ir fromRect:(NSRect)fr;
- (void)release;
- (void)new;
- (BOOL)isNull;
- (id <WebCoreImageRenderer>)copyWithZone:(int)z;
- (id <WebCoreImageRenderer>)retainOrCopyIfNeeded;
@end

typedef id <WebCoreImageRenderer> WebCoreImageRendererPtr;

class QPixmap {
public:
    QPixmap() {}
    ~QPixmap() {}

    void resize(int w, int h);

public:
    WebCoreImageRendererPtr imageRenderer;
    __strong void *somePtr;
    mutable bool needCopyOnWrite;
};

void inline_func(void) {
  QPixmap *pix;

  pix->imageRenderer = 0; /* { dg-warning "strong\\-cast assignment" } */
  pix->somePtr = 0; /* { dg-warning "strong\\-cast assignment" } */
  (__strong id)pix->somePtr = 0; /* { dg-warning "strong\\-cast assignment" } */
  (__strong id)pix->imageRenderer = 0; /* { dg-warning "strong\\-cast assignment" } */
}

void QPixmap::resize(int w, int h)
{
    if (needCopyOnWrite) {
        id <WebCoreImageRenderer> newImageRenderer = [imageRenderer copyWithZone:0];
        [imageRenderer release];
        imageRenderer = newImageRenderer; /* { dg-warning "strong\\-cast assignment" } */
        somePtr = 0; /* { dg-warning "strong\\-cast assignment" } */
        (__strong id)imageRenderer = newImageRenderer; /* { dg-warning "strong\\-cast assignment" } */
        needCopyOnWrite = false;
    }
    [imageRenderer new];
}
