/* APPLE LOCAL file radar 4491608 */
/* { dg-do compile } */

@interface SomeClass
+ method:(int)foo;
@end

int main(void) {
    [SomeClass method:3, 4];	/* { dg-error "too many arguments to function \\'method:\\'" } */
    return 0;
}
