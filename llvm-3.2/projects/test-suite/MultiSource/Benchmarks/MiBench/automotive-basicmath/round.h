/* +++Date last modified: 05-Jul-1997 */

/*
** rounding macros by Dave Knapp, Thad Smith, Jon Strayer, & Bob Stout
*/

#ifndef ROUND__H
#define ROUND__H

#include <math.h>

#if defined(__cplusplus) && __cplusplus

/*
** Safe C++ inline versions
*/

/* round to integer */

inline int iround(double x)
{
      return (int)floor(x + 0.5);
}

/* round number n to d decimal points */

inline double fround(double n, unsigned d)
{
      return floor(n * pow(10., d) + .5) / pow(10., d);
}

#else

/*
** NOTE: These C macro versions are unsafe since arguments are referenced
**       more than once.
**
**       Avoid using these with expression arguments to be safe.
*/

/*
** round to integer
*/

#define iround(x) floor((x) + 0.5)

/*
** round number n to d decimal points
*/

#define fround(n,d) (floor((n)*pow(10.,(d))+.5)/pow(10.,(d)))

#endif

#endif /* ROUND__H */
