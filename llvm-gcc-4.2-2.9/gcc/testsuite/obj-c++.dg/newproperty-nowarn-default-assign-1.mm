/* APPLE LOCAL file radar 5168496 */
/* Check that no warning must be issued when property type does not require use of 
   memory management; i.e. no 'retain' or 'copy' attribute is needed.
*/
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@interface A {
};
@property id *foo;
@property id **foo2;
@property Class foo3;
@end
