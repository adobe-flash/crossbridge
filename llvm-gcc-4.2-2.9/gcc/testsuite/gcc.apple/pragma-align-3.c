/* APPLE LOCAL file 4679943 */
/* Prove that HANDLE_SYSTEMV_PRAGMA alignment handling works somewhat. */

/* { dg-do run { target *-*-darwin* } } */

extern void abort (void);

struct {
  char one;
  long two;
} defaultalign;

#if defined(__x86_64__) || defined (__ppc64__)
#pragma pack(8)
#else
#pragma pack(4)
#endif
struct {
  char one;
  long two;
} sixteen;

#pragma pack(1)
struct {
  char one;
  long two;
} two;

#pragma pack(2)
struct {
  char one;
  long two;
} three;

#pragma pack()
/* Acts like a "#pragma pack(pop)" call on Darwin */
struct {
  char one;
  long two;
} resetalign;

int main ()
{
  if (sizeof (sixteen) < sizeof (defaultalign))
    abort();
  if (sizeof (two) >= sizeof (defaultalign))
    abort();
  if (sizeof (three) <= sizeof (two))
    abort();
  if (sizeof (resetalign) != sizeof (two))
    abort();
  return 0;
}
