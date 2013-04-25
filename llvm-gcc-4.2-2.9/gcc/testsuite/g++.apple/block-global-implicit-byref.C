/* APPLE LOCAL file radar 5803005 */
/* Test that all globals are 'byref' as default. */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-do compile } */

int a;
static int s;
extern int i;

void foo() {
   extern int b;
   static int c;

   ^ {  a  = 1;  // byref OK
         b  = 2; // byref OK
         s  = 5; // byref OK
	 i = 12; // byref OK
         c = 3;  // implicit byref OK
     };
}
