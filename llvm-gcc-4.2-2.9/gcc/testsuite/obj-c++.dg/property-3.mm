/* APPLE LOCAL file radar 4727191 */
/* Test that assigning local variable to a property does not ICE. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface Link 
  @property (copy) id test;
@end

int main() {
    id pid;
    Link *link;
    link.test = pid;
}
