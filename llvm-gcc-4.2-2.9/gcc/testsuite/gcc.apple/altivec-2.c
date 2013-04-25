/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

/* Check whether AltiVec allows for 'bool'
   and 'pixel' to be #defined to mean other things.  */

extern void abort (void);
#define CHECK_IF(E) if(!(E)) abort()

#define bool char
#define pixel unsigned char

int main(void) {
  bool x1;
  pixel x2;

  CHECK_IF(sizeof(x1) == 1);
  CHECK_IF(sizeof(x2) == 1);
  return 0;
}

