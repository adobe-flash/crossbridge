/*
 * These values are modified forms of what you will find in the Linux getopt(3)
 * manual page.  Why I can't get these from unistd.h is beyond me.
 */
#if 1
extern int getopt (...);
extern char * optarg;
extern int optind, opterr, optopt;
#endif

#if 0
#include <unistd.h>
#include <stdlib.h>
#endif
