#include "pdefs.h"

static precisionType pzeroConst = {
#ifndef BWGC
   (short) 1,		/* refcount (read/write!) */
#endif
   (posit) 1,		/* size */
   (posit) 1,		/* digitcount */
   (boolean) 0,		/* sign */
   { (digit) 0 }	/* value */
};

static precisionType poneConst = {
#ifndef BWGC
   (short) 1,		/* refcount (read/write!) */
#endif
   (posit) 1,		/* size */
   (posit) 1,		/* digitcount */
   (boolean) 0,		/* sign */
   { (digit) 1 }	/* value */
};

static precisionType ptwoConst = {
#ifndef BWGC
   (short) 1,		/* refcount (read/write!) */
#endif
   (posit) 1,		/* size */
   (posit) 1,		/* digitcount */
   (boolean) 0,		/* sign */
   { (digit) 2 }	/* value */
};

static precisionType p_oneConst = { 
#ifndef BWGC
   (short) 1,		/* refcount (read/write!) */ 
#endif
   (posit) 1,		/* size */
   (posit) 1,		/* digitcount */
   (boolean) 1,		/* sign */
   { (digit) 1 }	/* value */
};

precision pzero    = &pzeroConst;		/* zero */
precision pone     = &poneConst;		/* one */
precision ptwo     = &ptwoConst;		/* two */
precision p_one    = &p_oneConst;		/* negative one */
