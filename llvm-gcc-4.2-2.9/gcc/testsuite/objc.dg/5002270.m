/* APPLE LOCAL file radar 5002270 */
/* { dg-do compile } */
/* { dg-options "-Os" } */
#include <objc/objc.h>
@protocol NSObject
@end
@protocol NSCoding
@end
@interface NSObject < NSObject > {
}
@end
@interface nsi:NSObject < NSCoding > {
}
@end
@interface ns:nsi {
}
-(int) b;
@end
@interface data {
}
@end
@implementation data - (id) init {
}

-(void) foo: (unsigned long)
  dt mt:(unsigned long)
  btfr:(ns *) bm
{
  char ** n;
  char ** e;
  int  i, j, m;
  long k;
  signed char h, f;
  signed char t = (h || [bm b]);
  unsigned char  *src;

  for (i = 0; i < m; i++) {
    char  *p = (char *)k;
    char  *q = ((char *) *n);
    unsigned char  *v = ((unsigned char *) *e) +  m ;
    for (j = 0; j < m; j++) {
      unsigned char r, g, b, a;
      if (f) {
        a = h ? *p++ : 0xFF;
        if (!h && t)
          r = (unsigned int) ((float) r / (float) a);
        g = (unsigned int) ((float) g * 255.0 / (float) a);
      }
      *q++ = r;
      *q++ = g;
      *v++ = a;
    }
  }
}
@end
