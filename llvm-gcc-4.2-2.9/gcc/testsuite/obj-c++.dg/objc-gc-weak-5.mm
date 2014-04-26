/* APPLE LOCAL file radar 4592503 */
/* Check that gcc issues warning when __weak attribute is used unwisely. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

struct S {
	__weak id  p; /* { dg-warning "__weak attribute cannot be specified on a field declaration" } */
};
union U {
	__weak id  p; /* { dg-warning "__weak attribute cannot be specified on a field declaration" } */
};
@interface Foo {
struct {
   __weak id i; /* { dg-warning "__weak attribute cannot be specified on a field declaration" } */
} IVAR;
union {
    __weak id u;  /* { dg-warning "__weak attribute cannot be specified on a field declaration" } */
} UVAR;
__weak id  arr[3]; // ok
__weak id  aok;   // ok
}
@end
__weak id global;	// ok

__weak id ggg = 0;	// ok

static id ppp;		// ok

extern __weak id ewid;  // ok

int main ()
{
  __weak id  local; /* { dg-warning "__weak attribute cannot be specified on a local object declaration" } */
  static __weak id  s;		// ok
  extern __weak id  e;		// ok
}

