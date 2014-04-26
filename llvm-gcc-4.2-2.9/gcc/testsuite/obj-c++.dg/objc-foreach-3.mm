/* APPLE LOCAL file radar 4538105 */
/* The new foreach generates a spurious warning if the key is not used, but 
   the loop is instead being utilized for a side effect. */
/* { dg-options "-mmacosx-version-min=10.5 -Wall" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-Wall" { target arm*-*-darwin* } } */
/* { dg-do compile } */

typedef struct objc_class *Class;
typedef struct objc_object {
 Class isa;
} *id;


@interface MyList 
@end

@implementation MyList
- (unsigned int)countByEnumeratingWithState:
  (struct __objcFastEnumerationState *)state objects:
  (id *)items count:(unsigned int)stackcount
{
        return 0;
}
@end

@interface MyList (BasicTest)
- (void)compilerTestAgainst;
@end

@implementation MyList (BasicTest)
- (void)compilerTestAgainst {

	int i=0;
        for (id elem in self) 
	   ++i;
}
@end
