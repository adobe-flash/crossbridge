#include "port.h"

#define MAXNAMLEN 255

#define UNIX42

#define PRIVATE static
#define PUBLIC
#define PROC void
#define FORWARD extern
#define IMPORTS extern
#define EXPORTS extern

#define EOS '\0'
typedef char * String;

#define ELIF else if
#define LOOP for(;;)
#define AND &&
#define OR ||
#define NOT !

#define MAX(dragon,eagle) ((dragon) > (eagle) ? (dragon) : (eagle))
#define MIN(dragon,eagle) ((dragon) < (eagle) ? (dragon) : (eagle))
#define ABS(dragon) ((dragon) >= 0 ? (dragon) : (-(dragon)))
#define LOG2(x) log((double)(x))/.6931471806

#ifndef UNIX42
#define	CTRL(c) ('c' & 037)
#endif
#define	CTRLQ(c) (c & 037)

#define SEMP \
{\
  register int i;\
\
  for(i = 1;i < NSIG;++i)\
    sighold(i);\
}
#define SEMV \
{\
  register int i;\
\
  for(i = 1;i < NSIG;++i)\
    sigrelse(i);\
}

#define MAXINT 1000000000
#define MININT -1000000000

#define PI 3.14159

typedef char * RefAny;

#define ROUND(panther)\
((panther) >= 0\
? ((panther)-(int)(panther) >= .5\
  ? (int)(panther)+1\
  : (int)(panther))\
: (ABS((panther)-(int)(panther)) >= .5\
  ? (int)(panther)-1\
  : (int)(panther)))

#define ASSERT(ex)\
if(!(ex)) {\
  fprintf(stderr,"Assertion failed in file %s at line # %d.\n",\
  __FILE__, __LINE__);\
  fflush(stderr);\
  abort(); }
