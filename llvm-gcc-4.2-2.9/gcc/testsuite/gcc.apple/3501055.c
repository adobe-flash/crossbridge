/* APPLE LOCAL file 3501055 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O2" } */
int printf(const char*, ...);
double atof(char *);
int main(int argc, char **argv) {
  double d = atof(argv[1]);
  int i = d;

  printf("%d\n", i);

  return 0;
}
/* { dg-final { scan-assembler-not "movsd" } } */
