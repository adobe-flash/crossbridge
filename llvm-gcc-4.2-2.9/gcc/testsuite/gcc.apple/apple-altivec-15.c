/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

union agg_def {
  vector unsigned int v ;
  unsigned int i;
};

extern void abort();

int my_vec_all_eq(union agg_def a, union agg_def b);
int compare(int d1, int d2);

int main() {
  union agg_def a, b, d1, d2;

  d1.i = 0;
  d2.i = 1;
  a.v = (vector unsigned int) (2,3,4,5);
  b.v = (vector unsigned int) (2,3,4,5);

  d1.i = my_vec_all_eq(a, b);
  d2.i = vec_all_eq(a.v, b.v);
  
  if (compare(d1.i, d2.i)) 
    return 0;
  else 
    abort();
}

int my_vec_all_eq(union agg_def a, union agg_def b) {
  return vec_all_eq(a.v, b.v);
}

int compare(int d1, int d2) {
  return (d1 == d2);
}

