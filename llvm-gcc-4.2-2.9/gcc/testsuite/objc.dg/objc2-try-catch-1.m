/* APPLE LOCAL file radar 4951324 */
/* Test that in new objc2 eh, existing declaration of objc_ehtype_vtable
   and OBJC_EHTYPE_id will be used. */
/* { dg-options "-mmacosx-version-min=10.5 -m64 -fobjc-zerocost-exceptions" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-zerocost-exceptions" { target arm*-*-darwin* } } */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"
#include <stdio.h>
#include <setjmp.h>

int objc_ehtype_vtable;
int OBJC_EHTYPE_id;

@interface Frob: Object
@end

@implementation Frob: Object
@end

static int exc_control = 0;

int proc() {
  if(exc_control) {
    printf ("Throwing (%d)... ", exc_control);
    @throw [Frob new];
  }
  return 1;
}

int foo()
{
        @try {
                return proc();
        }
        @catch (Frob* ex) {
		if(exc_control > 1) {
		  printf("Rethrowing (%d)... ", exc_control);
		  @throw;
		}
		return 0;
        }
	@finally {
		printf("In @finally block (%d)... ", exc_control);
	}
}
