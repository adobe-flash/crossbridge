/* APPLE LOCAL file radar 4947311, 5556192 */
/* Check implementation of protocol attributes. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile } */

__attribute ((unavailable))
@protocol FwProto;

Class <FwProto> cFw = 0; /* { dg-error "protocol 'FwProto' is unavailable" } */


__attribute ((deprecated)) @protocol MyProto1
@end

@protocol Proto2  <MyProto1>  /* { dg-warning "protocol 'MyProto1' is deprecated" } */
+method2;
@end


@interface MyClass1 <MyProto1> /* { dg-warning "protocol 'MyProto1' is deprecated" } */
{
  Class isa;
}
@end

@interface Derived : MyClass1 <MyProto1> /* { dg-warning "protocol 'MyProto1' is deprecated" } */
{
	id <MyProto1> ivar; /* { dg-warning "protocol 'MyProto1' is deprecated" } */
}
@end

@interface MyClass1 (Category) <MyProto1, Proto2> /* { dg-warning "protocol 'MyProto1' is deprecated" } */
@end



Class <MyProto1> clsP1 = 0; /* { dg-warning "protocol 'MyProto1' is deprecated" } */

int main ()
{
	MyClass1 <MyProto1> *p1; /* { dg-warning "protocol 'MyProto1' is deprecated" } */
}

