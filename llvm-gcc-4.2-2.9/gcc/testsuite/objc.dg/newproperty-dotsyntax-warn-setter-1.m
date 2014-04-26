/* APPLE LOCAL file radar 4838528 */
/* Warn when dot-syntax calls a 'setter' and setter's return type is not
   'void' */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Subclass 
{
    int nonVoidSetter;
}
@end

@implementation Subclass
- (int)nonVoidSetter {
    return nonVoidSetter;
}

- (int)setNonVoidSetter:(int)arg {
    nonVoidSetter = arg;
    return 0;
}
@end

int main (void) {
    Subclass *x;
    x.nonVoidSetter = 10; /* { dg-warning "type of setter 'setNonVoidSetter:' must be 'void'" } */
    return 0;
}

