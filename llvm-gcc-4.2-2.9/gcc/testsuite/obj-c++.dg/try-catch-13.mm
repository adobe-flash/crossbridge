/* APPLE LOCAL begin radar 4204796 */
/* Ensure that variables declared volatile by the user (as opposed to
   synthesized by the EH-volatization machinery) _do_ trigger 
   "discards qualifiers from target pointer type" warnings.  */

/* { dg-options "-fobjc-exceptions -fnext-runtime" } */
/* { dg-do compile } */

@interface TestMyTests
- (void) testSpoon;
@end

extern void some_func (int *);

@implementation TestMyTests
- (void) testSpoon {
  volatile int i = 5;
  int q = 99;

  do {
    @try {
      typeof(i) j = 6;
      typeof(q) k = 66;
      some_func (&j);
/* { dg-error "invalid conversion" "" { target *-*-* } 24 } */ 
/* { dg-error "initializing argument" "" { target *-*-* } 24 } */ 
      some_func (&k);
    }
    @catch (id exc) {
      @throw;
    }
  } while(0);

  do {
    @try {
      typeof(i) j = 7;
      typeof(q) k = 77;
      some_func (&k);
      some_func (&j);
/* { dg-error "invalid conversion" "" { target *-*-* } 39 } */ 
/* { dg-error "initializing argument" "" { target *-*-* } 39 } */ 
    }
    @catch (id exc) {
      @throw;
    }
  } while(0);

  do {
    @try {
      typeof(q) k = 88;
      typeof(i) j = 8;
      some_func (&j); 
/* { dg-error "invalid conversion" "" { target *-*-* } 52 } */ 
/* { dg-error "initializing argument" "" { target *-*-* } 52 } */ 
      some_func (&k);
    }
    @catch (id exc) {
      @throw;
    }
  } while(0);
      
}
@end
/* APPLE LOCAL end radar 4204796 */
