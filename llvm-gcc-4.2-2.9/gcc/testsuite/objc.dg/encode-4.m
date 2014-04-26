/* Encoding tests for ObjC class layouts.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-options "-lobjc" } */
/* { dg-do run } */

/* APPLE LOCAL objc2 */
#include <stddef.h>
#include <objc/Object.h>
#ifdef __NEXT_RUNTIME__
#include <objc/objc-class.h>
#define OBJC_GETCLASS objc_getClass
#else
#include <objc/objc-api.h>
#define OBJC_GETCLASS objc_get_class
#endif

extern void abort(void);
extern int strcmp(const char *s1, const char *s2);
#define CHECK_IF(expr) if(!(expr)) abort()

@class Int1, Int2;
struct Nested;

struct Innermost {
  unsigned char a, b;
  struct Nested *encl;
};

struct Nested {
  float a, b;
  Int1 *next;
  struct Innermost innermost;
};

@interface Int1: Object {
  signed char a, b;
  Int2 *int2;
  struct Nested nested;
}
@end

@interface Int2: Int1 {
  struct Innermost *innermost;
  Int1 *base;
}
@end

@implementation Int1
@end

@implementation Int2
@end

/* APPLE LOCAL begin objc2 */
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
Ivar *ivar;
#else
struct objc_ivar *ivar;
#endif

static void check_ivar(const char *name, const char *type) {
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
  CHECK_IF(!strcmp(ivar_getName(*ivar), name));
  CHECK_IF(!strcmp(ivar_getTypeEncoding(*ivar), type));
#else
  CHECK_IF(!strcmp(ivar->ivar_name, name));
  CHECK_IF(!strcmp(ivar->ivar_type, type));
#endif
  ivar++;
}
/* APPLE LOCAL end objc2 */

int main(void) {
/* APPLE LOCAL begin objc2 */
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
  ivar = class_copyIvarList ((Class)OBJC_GETCLASS("Int1"), NULL);
#else
  ivar = ((Class)OBJC_GETCLASS("Int1"))->ivars->ivar_list;
#endif
/* APPLE LOCAL end objc2 */
  check_ivar("a", "c");
  check_ivar("b", "c");
  check_ivar("int2", "@\"Int2\"");
  check_ivar("nested", 
    "{Nested=\"a\"f\"b\"f\"next\"@\"Int1\"\"innermost\"{Innermost=\"a\"C\"b\"C\"encl\"^{Nested}}}");

/* APPLE LOCAL begin objc2 */
/* APPLE LOCAL radar 4923914 */
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
  ivar = class_copyIvarList ((Class)OBJC_GETCLASS("Int2"), NULL);
#else
  ivar = ((Class)OBJC_GETCLASS("Int2"))->ivars->ivar_list;
#endif
/* APPLE LOCAL end objc2 */
  check_ivar("innermost", "^{Innermost=CC^{Nested}}");
  check_ivar("base", "@\"Int1\"");
  
  return 0;
}
