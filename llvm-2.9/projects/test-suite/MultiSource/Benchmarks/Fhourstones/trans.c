#include "stdlib.h"
#include "types.h"
#include "c4.h"

#define  NSAMELOCK	0x20000		/* number of position with same lock */
#define  STRIDERANGE	(TRANSIZE/PROBES-NSAMELOCK)

unsigned int *ht;			/* hash locks */
byte *he;			/* hash entries */
int stride;
int htindex;
unsigned int lock;
int64 posed, hits;		/* counts transtore calls */

extern int columns[];

void trans_init()
{
  play_init();
  ht = (unsigned int *)calloc(TRANSIZE, sizeof(unsigned int));
  he = (byte *)calloc(TRANSIZE, sizeof(byte));
  if (ht == 0 || he == 0) {
    printf("Failed to allocate %u bytes.\n",
      TRANSIZE * (sizeof(unsigned int) + sizeof(byte)));
    exit(0);
  }
}

void emptyTT()
{
  int i, h, work;

  for (i=0; i<TRANSIZE; i++)
    if ((work = (h = he[i]) & 31) < 31)           /* bytes are signed!!! */
      he[i] = (byte)(h - (work<16 ? work : 4));     /* work = work monus 4 */
  posed = hits = 0;
}

double hitRate()
{
  return posed != 0 ? (double)hits/(double)posed : 0.0;
}

void hash()
{
  unsigned int t1,t2;

  int64 htemp;

  t1 = (columns[1] << 7 | columns[2]) << 7 | columns[3];
  t2 = (columns[7] << 7 | columns[6]) << 7 | columns[5];
  
  htemp = t1 > t2 ? (int64)(t1 << 7 | columns[4]) << 21 | t2:
                    (int64)(t2 << 7 | columns[4]) << 21 | t1;
  lock = (int)(htemp >> 17);
  htindex = (int)(htemp % TRANSIZE);
  stride = NSAMELOCK + lock % STRIDERANGE;
}

int transpose()
{
  int i,x;

  hash();
  for (x=htindex, i=0; i < PROBES; i++) {
    if (ht[x] == lock)
      return he[x];
    if ((x += stride) >= TRANSIZE)
      x -= TRANSIZE;
  }
  return ABSENT;
}

void transput(int score, int work)
{
  int i,x;

  for (x=htindex, i=0; i < PROBES; i++) {
    if (work > (he[x] & 31)) {
      hits++;
      ht[x] = lock;
      he[x] = (byte)(score << 5 | work);
      return;
    }
    if ((x += stride) >= TRANSIZE)
      x -= TRANSIZE;
  }
}

void transrestore(int score, int work)
{
  int i,x;

  if (work > 31)
    work = 31;
  posed++;
  hash();
  for (x=htindex, i=0; i < PROBES; i++) {
    if (ht[x] == lock) {
      hits++;
      he[x] = (byte)(score << 5 | work);
      return;
    }
    if ((x += stride) >= TRANSIZE)
      x -= TRANSIZE;
  }
  transput(score, work);
}

void transtore(int score, int work)
{
  if (work > 31)
    work = 31;
  posed++;
  hash();
  transput(score, work);
}

void htstat()      /* some statistics on hash table performance */
{
  int total, i;
  int works[32];
  int typecnt[8];                /* bound type stats */

  for (i=0; i<32; i++)
    works[i] = 0;
  for (i=0; i<8; i++)
    typecnt[i] = 0;
  for (i=0; i<TRANSIZE; i++) {
    works[he[i] & 31]++;
    if ((he[i] & 31) != 0)
      typecnt[4 + (he[i] >> 5)]++;
  }
  for (total=i=0; i<8; i++)
    total += typecnt[i];
  if (total > 0) {
    printf("store rate = %.3f\n", hitRate());
    printf("- %5.3f  < %5.3f  = %5.3f  > %5.3f  + %5.3f\n",
      typecnt[4+LOSE]/(double)total, typecnt[4+DRAWLOSE]/(double)total,
      typecnt[4+DRAW]/(double)total, typecnt[4+DRAWWIN]/(double)total,
      typecnt[4+WIN]/(double)total);
  }
  for (i=0; i<32; i++) {
    printf("%7d%c", works[i], (i&7)==7 ? '\n':'\t');
  }
}
