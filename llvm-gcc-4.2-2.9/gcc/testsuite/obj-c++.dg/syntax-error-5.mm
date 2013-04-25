/* APPLE LOCAL begin radar 4261146 */
/* { dg-do compile } */

typedef struct S { int i; } NSDictionary;

@interface A 
{
}
@end

@interface B : A
{
    NSDictionary * _userInfo;
@end				/* { dg-error "at end of input"  } */
/* APPLE LOCAL end radar 4261146 */
