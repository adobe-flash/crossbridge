/* APPLE LOCAL file AltiVec */
/* Vectors in template functions.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec -Wall" } */

template<int I>
void FooBar()
{
    vector unsigned char va, vb, vc;
    vc = (vector unsigned char)vec_mergeh((vector unsigned short)va, (vector unsigned short)vb);
}

int main (int argc, char * const argv[]) 
{
    FooBar<0>();
    return 0;
}
