/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-g" } */

@protocol MyProtocol
- (void) method;
@end

@interface MyClass
@end

int main()
{
  id <MyProtocol> obj_p =  ((void *)0);
  return 0;
}
