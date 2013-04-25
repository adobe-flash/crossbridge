/* APPLE LOCAL file radar 4531086 */
/* Test for warning on usage of objc2 features on older os's. */
/* { dg-options "-mmacosx-version-min=10.4 -fobjc-abi-version=2 -fobjc-new-property -std=c99" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <objc/objc.h>
#include <objc/Object.h>

@interface Bar : Object
{
  int iVar;
}
@property (assign, setter = MySetter:) int FooBar;  /* { dg-warning "Mac OS X version 10.5 or later is needed for use of property" } */
- (void) MySetter : (int) value;
@end

@implementation Bar
@synthesize FooBar = iVar;
- (void) MySetter : (int) value { iVar = value; }

@end

@interface MyList 
@end

@implementation MyList
- (unsigned int)countByEnumeratingWithState:(struct __objcFastEnumerationState *)state objects:(id *)items count:(unsigned int)stackcount
{
        return 0;
}
- (void)addObject:object {
}

@end

@interface MyList (BasicTest)
- (void)compilerTestAgainst;
@end
void BEGIN();
void INFORLOOP();
void END();
@implementation MyList (BasicTest)
- (void)compilerTestAgainst {

        BEGIN();
        for (id elem in (self))  /* { dg-warning "Mac OS X version 10.5 or later is needed for use of foreach-collection-statement" } */
          if (elem)
            INFORLOOP();

        END();
}
@end


int main(int argc, char *argv[]) {
    Bar *f;
    f.FooBar = 1;

    f.FooBar += 3;

    f.FooBar -= 4;
    return f.FooBar;
}
/* { dg-warning "Mac OS X version 10.5 or later is needed for use of the new objc abi" "" { target *-*-* } 0 } */
