/* APPLE LOCAL file radar 4603883 */
/* { dg-do compile } */
/* { dg-require-effective-target ilp32 } */
double param[10][10][5000000]; /* { dg-error "size of array 'param' is too large" } */
void foo ()
{
  char *result, lines[26843000][100]; /* { dg-error "size of array 'lines' is too large" } */
  int i, j, n, m, nchains, nparams;
  for (i = 0; i < nchains; i++) {
    for (n = 0; n < j; n++) {
      for (m = 0; m < nparams; m++) {
        param[i][m][n] = atof (result);
      }
    }
  }
}
