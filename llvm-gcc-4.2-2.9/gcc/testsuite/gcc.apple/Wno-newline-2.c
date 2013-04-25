/* Test no newline at eof warning.  */
/* { dg-do compile } */
/* { dg-options "-Wnewline-eof" } */
int main() { return 0; } /* { dg-warning "no newline" } */