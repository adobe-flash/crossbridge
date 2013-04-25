/* APPLE LOCAL file 7901648 */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++ -fblocks" } */


@interface Foo {
    int x;
    int y;
}
@end

void fee(int);

@implementation Foo
+ (int)method {
    ^{ x = 1;  /* { dg-error "instance variable \\'x\\' accessed in class method" } */
	       /* { dg-error "was not declared" "" { target *-*-* } 16 } */
       x =  
          y+1; /* { dg-error "instance variable \\'y\\' accessed in class method" } */
	       /* { dg-error "was not declared" "" { target *-*-* } 19 } */
       fee(x); 
       return x; }; 
}
@end

