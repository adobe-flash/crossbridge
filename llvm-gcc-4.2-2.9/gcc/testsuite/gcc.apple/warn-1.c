/* APPLE LOCAL file 6218859 */
/* { dg-options "-O2 -Werror=uninitialized" } */

int main(int argc, char *argv[]) {
  int a;

  return a;	/* { dg-error "is used uninitialized in this function" } */
}
