/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */


extern "C" void abort(void);

void CMP (vector unsigned char v, vector unsigned char v1)
{
    union {
	vector unsigned char vv;
	unsigned char s[16];
    } r, r1;
    r.vv = v;
    r1.vv = v1;

    for (int i=0; i < 16; i++)
       if (r.s[i] != r1.s[i])
	 abort();
    return;
}

#define TEST_FUNCTION_TEMPLATE

#ifdef TEST_FUNCTION_TEMPLATE
template <int I>
#endif
void vectorTest()
{
  typedef vector unsigned char VUC;

  // Multiple initializers with expressions
  const unsigned char kFoo = 0;
  enum              { kBar = 1 };
  VUC v1 = {kFoo,kBar,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  VUC v3 = (VUC)(kFoo,kBar,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
  CMP (v1, v3);

  VUC v2 = {kBar*kFoo,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  VUC v4 = (VUC)(kBar*kFoo,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
  CMP (v2, v4);

  // Single initializers
  VUC v5 = {42};
  VUC v7 = (VUC)(42);
  CMP (v5, v7);

  VUC v6 = {40+2};
  VUC v8 = (VUC)(40+2);
  CMP (v6, v8);

  VUC v9 = {I+5};
  VUC v11 = (VUC)(I+5);
  CMP (v9, v11);
}


int main (int argc, char * const argv[]) 
{
#ifdef TEST_FUNCTION_TEMPLATE
	vectorTest<0>();
#else
	vectorTest();
#endif
    return 0;
}
