#define global 
#include "defs.h"

typedef struct {
  bodyptr pskip;		/* body to skip in force evaluation */
  vector pos0;			/* point at which to evaluate field */
  real phi0;			/* computed potential at pos0       */
  vector acc0;			/* computed acceleration at pos0    */
} hgstruct, *hgsptr;

/*
 * WALKSUB: recursive routine to do hackwalk operation.
 * p: pointer into body-tree 
 * dsq: size of box squared 
 */
extern bool subdivp(nodeptr p, real dsq, real tolsq, hgstruct hg);
hgstruct walksub(nodeptr p, real dsq, real tolsq, hgstruct hg, int level);
hgstruct gravsub(nodeptr p, hgstruct hg);

hgstruct walksub(nodeptr p, real dsq, real tolsq, hgstruct hg, int level)
{
  register int k, i;
  register nodeptr r;
  nodeptr tmp[NSUB];


  if (subdivp(p, dsq, tolsq, hg)) {           /* should p be opened?    */
    for (k = 0; k < NSUB; k++) {              /* loop over the subcells */
      r = Subp((cellptr) p)[k]; /* <-- 6.7% load penalty */
      if (r != NULL)                  /* does this one exist?   */
	hg = walksub(r, dsq / 4.0, tolsq, hg, level+1);
    }
  }
  else if (p != (nodeptr) hg.pskip)   {         /* should p be included?  */
    hg = gravsub(p, hg);                           /* then use interaction   */
  }


  return hg;
}




