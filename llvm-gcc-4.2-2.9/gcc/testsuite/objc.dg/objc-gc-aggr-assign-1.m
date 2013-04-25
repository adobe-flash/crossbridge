/* APPLE LOCAL file radar 3742561 */
/* Test that we generate void * objc_memmove_collectable(void *dst, const void *src, size_t size)
   API when struct has a 'strong' object pointer in a variety of situations. */
/* { dg-options "-fobjc-gc -mmacosx-version-min=10.5 -framework Cocoa" } */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#define objc_copyStruct X_objc_copyStruct
#define objc_memmove_collectable X_objc_memmove_collectable
#include <Cocoa/Cocoa.h>
#undef objc_memmove_collectable
#undef objc_copyStruct

static int count;

/* Only ppc32 API for property assignment makes the call to objc_copyStruct. */
static void 
objc_copyStruct (void *dst, const void * src, size_t size, bool arg, bool arg2)
{
  memcpy (dst, src, size);
  count++;
}

typedef struct S {
   int ii;
} SS;

struct type_s {
   SS may_recurse;
   id id_val;
};

@interface NamedObject : NSObject
{
  struct type_s type_s_ivar;
}
- (void) setSome : (struct type_s) arg;
- (struct type_s) getSome;
@property(assign) struct type_s aggre_prop;
@end

@implementation NamedObject 
- (void) setSome : (struct type_s) arg
  {
     type_s_ivar = arg;
  }
- (struct type_s) getSome 
  {
    return type_s_ivar;
  }
@synthesize aggre_prop = type_s_ivar;
@end

struct type_s some = {{1234}, (id)0};

struct type_s get(void)
{
  return some;
}

struct type_s GlobalVariable;


static void *
objc_memmove_collectable(void *dst, const void *src, size_t size)
{
  memcpy (dst, src, size);
  count++;
}

int main(void) {
   struct type_s local;
   struct type_s *p;

   NamedObject *object = [[NamedObject alloc] init];

   /* Assigning into a global */
   GlobalVariable = get(); 
   if (count != 1 || GlobalVariable.may_recurse.ii != 1234)
     abort ();

   /* Assigning into a local */
   local = GlobalVariable;
   if (count != 2 || local.may_recurse.ii != 1234)
     abort ();

   p = (struct type_s *) malloc (sizeof (struct type_s));
   /* Assigning thourgh a pointer */
   *p = local;
   if (count != 3 || p->may_recurse.ii != 1234)
     abort ();

   /* Assigning to an ivar */
   [object setSome:GlobalVariable];
   if (count != 4 || [object getSome].may_recurse.ii != 1234)
     abort ();

   local.may_recurse.ii = 6578;
   object.aggre_prop = local;
  if (count != 5 || object.aggre_prop.may_recurse.ii != 6578)
    abort ();

   return 0;
}

