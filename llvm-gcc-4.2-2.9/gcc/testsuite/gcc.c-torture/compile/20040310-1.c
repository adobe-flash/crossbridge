/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
void I_wacom ()
{
  char buffer[50], *p;
  int RequestData (char *cmd)
  {
    p = buffer;
    foo (buffer);
  }
  RequestData (0);
}
