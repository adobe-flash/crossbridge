/* APPLE LOCAL begin radar 4161346 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-faltivec -mmacosx-version-min=10.4" } */
/* { dg-final { scan-assembler-not "ppc7400" } } */

int main( int argc, char * argv[] )
{
    return 0;
}
/* APPLE LOCAL end radar 4161346 */
