/* APPLE LOCAL file */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-maltivec -pedantic -faltivec" } */

int main()
    {
    typedef        unsigned char  UC;
    typedef vector unsigned char  VUC;

    const UC   kBar  = 7;
    VUC        vBar1 = (VUC)(kBar);
    VUC        vBar2 =      {kBar};
    VUC        vBar3 = (VUC){kBar};
    return 0;
    }
