/* APPLE LOCAL file 4641942 */
/* { dg-do compile } */
/* { dg-options "-fstrict-aliasing -Wstrict-aliasing" } */

typedef struct rec {
   unsigned long hi;
   unsigned long lo;
} rec;

typedef struct data
{
 unsigned long long s;
 unsigned long long t;
} data;

void foo (void)
{
  data* eng;
  unsigned long long next = eng->t + eng->s;
  rec m = *((rec*)&next); /* { dg-warning "dereferencing type-punned pointer will break strict-aliasing rules" } */
}
