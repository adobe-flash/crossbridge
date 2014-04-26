/* APPLE LOCAL file 4241538 */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec" } */
/* Contributed by Devang Patel  <dpatel@apple.com>  */

int main( int argc, char **argv )
{
    float array[ 4 ];
    vector unsigned char vuc1, vuc2;
    vector unsigned char vuc = vec_sld (vuc1, vuc2); /* { dg-error "too few arguments to function" } */
    vector float two = vec_ctf( vec_splat_u32(2)); /* { dg-error "too few arguments to function" } */
    vector unsigned int vi = vec_splat_u32(); /* { dg-error "too few arguments to function" } */
    return 0;
}


