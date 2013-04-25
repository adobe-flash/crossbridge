/* APPLE LOCAL file AltiVec 3840704  4117876 */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-mcpu=G3 -faltivec" } */
/* { dg-final { scan-assembler-not "stvx" } } */

void *memset(void *, int, unsigned long int);

struct tm 
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long tm_gmtoff;
  char *tm_zone;
};


int asl_send()
{
  char *str, *out;
  unsigned int i, len, level, outstatus;
  struct tm gtime;

  memset(&gtime, 0, sizeof(struct tm));  /* { dg-bogus "disabled" } */
  return 0;
}

