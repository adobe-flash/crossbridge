/* APPLE LOCAL file AltiVec */
/* Additional AltiVec PIM argument type combinations.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec -Wall" } */

vector signed int
LoadUnalignedSI( signed int *p)
{
    vector signed int vload1 = vec_ld( 0, (vector signed int *)p);
    if ( ((int)p) * 0xF){
        vector signed int vload2 = vec_ld( 16, (vector signed int *)p);
        vector unsigned char vperm = vec_ldl( 0, p);

        vload1 = vec_perm(vload1, vload2, vperm);
    }
    return vload1;
}

inline vector float Reciprocal( vector float v )
{

    vector float estimate = vec_re( v );

    return vec_madd( vec_nmsub( estimate, v, (vector float) (1.0) ), estimate, estimate );
}

void foo(void) {
    vector bool char vbc1 = (vector bool char)(255);
    vector pixel vp1, vp2;
    vector float vf1, vf2;
    vector unsigned char vuc1;
    vector unsigned short vus1, vus2;
    vector bool short vbs1, vbs2;
    vector signed short vss1;
    vector signed int vsi1, vsi2;
    vbc1 = vec_splat (vbc1, 9);
    vp2 = vec_splat (vp1, 7);
    vf1 = vec_splat (vf2, 31);
    vuc1 = vec_pack (vus1, vus2);
    vbc1 = vec_pack (vbs1, vbs2);
    vss1 = vec_pack (vsi1, vsi2);
}
