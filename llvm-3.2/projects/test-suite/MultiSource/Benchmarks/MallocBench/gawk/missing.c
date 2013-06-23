#ifdef MSDOS
#define BCOPY_MISSING
#define STRCASE_MISSING
#define BLKSIZE_MISSING
#define SPRINTF_INT
#define RANDOM_MISSING
#define GETOPT_MISSING
#endif

#ifdef DUP2_MISSING
#include "missing.d/dup2.c"
#endif /* DUP2_MISSING */

#ifdef GCVT_MISSING
#include "missing.d/gcvt.c"
#endif /* GCVT_MISSING */

#ifdef GETOPT_MISSING
#include "missing.d/getopt.c"
#endif	/* GETOPT_MISSING */

#ifdef MEMCMP_MISSING
#include "missing.d/memcmp.c"
#endif	/* MEMCMP_MISSING */

#ifdef MEMCPY_MISSING
#include "missing.d/memcpy.c"
#endif	/* MEMCPY_MISSING */

#ifdef MEMSET_MISSING
#include "missing.d/memset.c"
#endif	/* MEMSET_MISSING */

#ifdef RANDOM_MISSING
#include "missing.d/random.c"
#endif	/* RANDOM_MISSING */

#ifdef STRCASE_MISSING
#include "missing.d/strcase.c"
#endif	/* STRCASE_MISSING */

#ifdef STRCHR_MISSING
#include "missing.d/strchr.c"
#endif	/* STRCHR_MISSING */

#ifdef STRERROR_MISSING
#include "missing.d/strerror.c"
#endif	/* STRERROR_MISSING */

#ifdef STRTOD_MISSING
#include "missing.d/strtod.c"
#endif	/* STRTOD_MISSING */

#ifdef TMPNAM_MISSING
#include "missing.d/tmpnam.c"
#endif	/* TMPNAM_MISSING */

#if defined(VPRINTF_MISSING) && defined(BSDSTDIO)
#include "missing.d/vprintf.c"
#endif	/* VPRINTF_MISSING && BSDSTDIO */
