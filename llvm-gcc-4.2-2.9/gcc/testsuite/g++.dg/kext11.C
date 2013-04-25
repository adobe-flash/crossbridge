/* APPLE LOCAL file kext ptmf casts --bowdidge */
/* In gcc-4.0 and beyond, kexts are not permitted to cast
   pointer-to-member- functions into pointer-to-functions.  
   These casts should be flagged as a hard error if
   -fapple-kext is an option for the compile.  -fpermissive shouldn't permit
   this, either.  This checks for OSMemberFunctionCast in the error message
   because the error advises developers to use that function to replace
   these casts.

   Radar 4025923, gcc-4.0 should ban kexts from doing ptmf to ptf
   conversions without OSMemberFunctionCast
*/
/* { dg-do compile { target powerpc*-apple-darwin* } } */
/* { dg-options "-S -static -fapple-kext -fpermissive -fno-exceptions" } */

typedef int(*INT_FUNC_INT)(int);
class Superclass {
public:
  /* In 3.3, this would get the foo function for the class of "this".  In 4.0, it
     gets the foo function for Superclass. */
  virtual int init(void) {_myFunc = (INT_FUNC_INT)&Superclass::foo;}; /* { dg-error "OSMemberFunctionCast" } */
  INT_FUNC_INT _myFunc;
  virtual int foo(int i) { return 2;};
};

class Subclass : public Superclass  { 
public:
  virtual int foo(int) { return 1;};
};

main(int argc, char **argv) {
  Superclass sup;
  Subclass sub;
  sup.init();
  sup.foo(1);
  sub.init();
  sub.foo(1);
}
  
