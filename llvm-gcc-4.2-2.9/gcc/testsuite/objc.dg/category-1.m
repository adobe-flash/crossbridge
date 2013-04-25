/* Test class methods inside categories.  */
/* Author: Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-options "-lobjc" } */
/* { dg-do run } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

#ifdef __NEXT_RUNTIME__
#define SUPERCLASS superclass
#else
#define SUPERCLASS superClass
#endif

extern int strcmp(const char *s1, const char *s2);
extern void abort(void);
#define CHECK_IF(expr) if(!(expr)) abort()

@interface MyObject: Object
+ (Class)whatever1;
@end

@implementation MyObject
+ (Class)whatever1 { return [super SUPERCLASS]; }
@end

@interface MyObject (ThisWontCompile)
+(Class)whatever2;
@end
 
@implementation MyObject (ThisWontCompile)
+(Class)whatever2 { return [super SUPERCLASS]; }
@end

int main (int argc, const char * argv[])
{
  Class w1 = [MyObject whatever1];
  Class w2 = [MyObject whatever2];

/* APPLE LOCAL begin objc2 */
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
  CHECK_IF(!strcmp(object_getClassName(w1), "Object"));
  CHECK_IF(!strcmp(object_getClassName(w2), "Object"));
#else
  CHECK_IF(!strcmp(w1->name, "Object"));
  CHECK_IF(!strcmp(w2->name, "Object"));
#endif
/* APPLE LOCAL end objc2 */
  return 0;
}

