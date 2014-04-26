/* APPLE LOCAL file radar 4268581 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-static -m64  -Os -finline-limit=1500 --param inline-unit-growth=200" } */
typedef __SIZE_TYPE__ size_t;
int match (unsigned long long *name, unsigned long long *pat)
{
  unsigned long long c;
  c = *pat++;
  switch (c & 0xffffffffffULL) {
    case ((unsigned long long)(('*')|0x8000000000ULL)):
      do
        if (match(name, pat))
          return(1);
      while (*name++ != '\0');

      return(0);
    default:
      if (*name++ != c)
        return(0);
      break;
  }
  return(*name == '\0');
}

int glob ()
{
  unsigned long long patbuf[1024];
  return glob0 (patbuf);
}

int glob0()
{
  int err;
  if ((err = glob1 ()) != 0)
    return (err);
  return(0);
}

int glob1 ()
{
  unsigned long long pathbuf[1024];
  return (glob2 (pathbuf, pathbuf));
}

int glob2 (pathend, pattern)
  unsigned long long *pathend, *pattern;
{
  return (glob3 (pathend, pattern));
}

int glob3 (unsigned long long *pathend, unsigned long long *pattern)
{
  return !match (pathend, pattern);
}
