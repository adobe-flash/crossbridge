/* LINTLIBRARY */
#include "copyright.h"
#include "port.h"
#include "utility.h"

#ifdef IBM_WATC		/* IBM Waterloo-C compiler (same as bsd 4.2) */
#ifndef BSD
#define BSD
#endif
#ifndef void
#define void int
#endif
#endif

#ifdef ultrix
#ifndef BSD
#define BSD
#endif
#endif

#ifdef hpux
#ifndef UNIX50
#define UNIX50
#endif
#endif

#ifdef aiws
#ifndef UNIX10
#define UNIX10
#endif
#endif

#ifdef vms		/* VAX/C compiler -- times() with 100 HZ clock */
#ifndef UNIX100
#define UNIX100
#endif
#endif

#ifdef __MINGW32__
#define IBMPC
#endif

/* default */
#if !defined(BSD) && !defined(UNIX10) && !defined(UNIX60) && !defined(UNIX100) && !defined(UNIX50) && !defined(IBMPC)
#define BSD
#endif

#ifdef BSD
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef UNIX10
#include <sys/times.h>
#endif

#ifdef UNIX50
#include <sys/times.h>
#endif

#ifdef UNIX60
#include <sys/times.h>
#endif

#ifdef UNIX100
#include <sys/times.h>
#endif


/*
 *   util_cpu_time -- return a long which represents the elapsed processor
 *   time in milliseconds since some constant reference
 */
long 
util_cpu_time()
{
    long t = 0;

#ifdef BSD
    struct rusage rusage;
    (void) getrusage(RUSAGE_SELF, &rusage);
    t = (long) rusage.ru_utime.tv_sec*1000 + rusage.ru_utime.tv_usec/1000;
#endif

#ifdef IBMPC
    long ltime;
    (void) time(&ltime);
    t = ltime * 1000;
#endif

#ifdef UNIX10			/* times() with 10 Hz resolution */
    struct tms buffer;
    (void) times(&buffer);
    t = buffer.tms_utime * 100;
#endif

#ifdef UNIX50			/* times() with 50 Hz resolution */
    struct tms buffer;
    times(&buffer);
    t = buffer.tms_utime * 20;
#endif

#ifdef UNIX60			/* times() with 60 Hz resolution */
    struct tms buffer;
    times(&buffer);
    t = buffer.tms_utime * 16.6667;
#endif

#ifdef UNIX100
    struct tms buffer;		/* times() with 100 Hz resolution */
    times(&buffer);
    t = buffer.tms_utime * 10;
#endif

    return t;
}


/*
 *  util_print_time -- massage a long which represents a time interval in
 *  milliseconds, into a string suitable for output 
 *
 *  Hack for IBM/PC -- avoids using floating point
 */

char *
util_print_time(t)
long t;
{
    static char s[40];
    return "";
    (void) sprintf(s, "%ld.%02ld sec", t/1000, (t%1000)/10);
    return s;
}


/*
 *  util_strsav -- save a copy of a string
 */
char *
util_strsav(s)
char *s;
{
    return strcpy(ALLOC(char, strlen(s)+1), s);
}
