/* choose any prime >= 1050011 (preferably 3 mod 4), e.g , one of
1050011, 2087483, 3124999, 6249979, 12499999, 24999983,
49999991, 99999971, 199999991, 399999959, 799999999
Memory use will be 5 * TRANSIZE bytes.
*/
#include <stdio.h>
#ifndef TRANSIZE
#define  TRANSIZE       1050011
#endif

#ifndef PROBES
#define  PROBES         8
#endif

#ifndef REPORTPLY
#define  REPORTPLY	8
#endif

#define UNK		-4
#define LOSE		-2
#define DRAWLOSE	-1
#define DRAW		0
#define DRAWWIN		1
#define WIN		2
#define EMPTY		0
#define BLACK		1
#define WHITE		2
#define EDGE		3

#define  ABSENT         -128
