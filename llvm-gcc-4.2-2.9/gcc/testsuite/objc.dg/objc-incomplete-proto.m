/* APPLE LOCAL file radar 4398221 */
/* Issue warning if protocol used in @interface declaration is not defined. */
/* { dg-do compile } */

@protocol MyProto;

@interface MyClass <MyProto>   /* { dg-warning "no definition of protocol \\'MyProto\\' " } */
@end

@interface MyClass (Category) <MyProto>	 /* { dg-warning "no definition of protocol \\'MyProto\\' " } */
@end
