/* APPLE LOCAL file 7901648 */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC -fblocks" } */


@interface Foo {
    int x;
    int y;
}
@end

void fee(int);

@implementation Foo
+ (int)method {
    ^{ x = 1;  /* { dg-error "instance variable \\'x\\' accessed in class method" } */
       x =  /* { dg-error "instance variable \\'x\\' accessed in class method" } */ 
          y+1; /* { dg-error "instance variable \\'y\\' accessed in class method" } */
       fee(x); /* { dg-error "instance variable \\'x\\' accessed in class method" } */
       return x; }; /* { dg-error "instance variable \\'x\\' accessed in class method" } */
}
@end

