/* APPLE LOCAL file radar 4829851 */
/* Test that objective-c++ compiler catches duplicate declarations in objective-c++'s 
   global namespace. */
@class One;
int One; 	/* { dg-error "redeclaration of Objective-C class" } */

@class Foo;

namespace Foo { int x; } /* { dg-error "redeclaration of Objective-C class" } */

@class X;

struct X {
    int X;
};
