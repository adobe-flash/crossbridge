/* APPLE LOCAL file 6258941 */
/* { dg-do compile { target powerpc*-apple-darwin* } } */
/* { dg-options { -mtune=G4 } } */
extern void malloc_printf(const char *format, ...);
void auto_fatal(const char *format, ...) {
  static char buffer[512];
  malloc_printf("%s", buffer);
  __builtin_trap();
}
