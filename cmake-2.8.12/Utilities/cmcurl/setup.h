#ifndef __LIB_CURL_SETUP_H
#define __LIB_CURL_SETUP_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2007, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id$
 ***************************************************************************/

#ifdef HTTP_ONLY
#define CURL_DISABLE_TFTP
#define CURL_DISABLE_FTP
#define CURL_DISABLE_LDAP
#define CURL_DISABLE_TELNET
#define CURL_DISABLE_DICT
#define CURL_DISABLE_FILE
#endif /* HTTP_ONLY */

#if !defined(WIN32) && defined(__WIN32__)
/* Borland fix */
#define WIN32
#endif

#if !defined(WIN32) && defined(_WIN32)
/* VS2005 on x64 fix */
#define WIN32
#endif

#if defined(__clang__)
# pragma clang diagnostic ignored "-Wcast-align"
#endif

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else

/*
#ifdef _WIN32_WCE
#include "config-win32ce.h"
#else
#ifdef WIN32
#include "config-win32.h"
#endif
#endif
*/

#ifdef macintosh
#include "config-mac.h"
#endif

#ifdef AMIGA
#include "amigaos.h"
#endif

#ifdef TPF
#include "config-tpf.h" /* hand-modified TPF config.h */
/* change which select is used for libcurl */
#define select(a,b,c,d,e) tpf_select_libcurl(a,b,c,d,e)
#endif

#endif /* HAVE_CONFIG_H */

/*
 * Include header files for windows builds before redefining anything.
 * Use this preproessor block only to include or exclude windows.h,
 * winsock2.h, ws2tcpip.h or winsock.h. Any other windows thing belongs
 * to any other further and independant block.  Under Cygwin things work
 * just as under linux (e.g. <sys/socket.h>) and the winsock headers should
 * never be included when __CYGWIN__ is defined.  configure script takes
 * care of this, not defining HAVE_WINDOWS_H, HAVE_WINSOCK_H, HAVE_WINSOCK2_H,
 * neither HAVE_WS2TCPIP_H when __CYGWIN__ is defined.
 */

#ifdef HAVE_WINDOWS_H
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  ifdef HAVE_WINSOCK2_H
#    include <winsock2.h>
#    ifdef HAVE_WS2TCPIP_H
#       include <ws2tcpip.h>
#    endif
#  else
#    ifdef HAVE_WINSOCK_H
#      include <winsock.h>
#    endif
#  endif
#endif

/*
 * Define USE_WINSOCK to 2 if we have and use WINSOCK2 API, else
 * define USE_WINSOCK to 1 if we have and use WINSOCK  API, else
 * undefine USE_WINSOCK.
 */

#undef USE_WINSOCK

#ifdef HAVE_WINSOCK2_H
#  define USE_WINSOCK 2
#else
#  ifdef HAVE_WINSOCK_H
#    define USE_WINSOCK 1
#  endif
#endif


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#if !defined(__cplusplus) && !defined(__BEOS__) && !defined(__ECOS) && !defined(typedef_bool)
typedef unsigned char bool;
#define typedef_bool
#endif

#ifdef HAVE_LONGLONG
#define LONG_LONG long long
#define ENABLE_64BIT
#else
#ifdef _MSC_VER
#define LONG_LONG __int64
#define ENABLE_64BIT
#endif /* _MSC_VER */
#endif /* HAVE_LONGLONG */

#ifndef SIZEOF_CURL_OFF_T
/* If we don't know the size here, we assume a conservative size: 4. When
   building libcurl, the actual size of this variable should be define in the
   config*.h file. */
#define SIZEOF_CURL_OFF_T 4
#endif

/* We set up our internal preferred (CURL_)FORMAT_OFF_T here */
#if SIZEOF_CURL_OFF_T > 4
#define FORMAT_OFF_T "lld"
#else
#define FORMAT_OFF_T "ld"
#endif /* SIZEOF_CURL_OFF_T */

#ifndef _REENTRANT
/* Solaris needs _REENTRANT set for a few function prototypes and things to
   appear in the #include files. We need to #define it before all #include
   files. Unixware needs it to build proper reentrant code. Others may also
   need it. */
#define _REENTRANT
#endif

#include <stdio.h>
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#include <errno.h>

#ifdef __TANDEM /* for nsr-tandem-nsk systems */
#include <floss.h>
#endif

#ifndef STDC_HEADERS /* no standard C headers! */
#include <curl/stdcheaders.h>
#endif

/*
 * PellesC cludge section (yikes);
 *  - It has 'ssize_t', but it is in <unistd.h>. The way the headers
 *    on Win32 are included, forces me to include this header here.
 *  - sys_nerr, EINTR is missing in v4.0 or older.
 */
#ifdef __POCC__
  #include <sys/types.h>
  #include <unistd.h>
  #if (__POCC__ <= 400)
  #define sys_nerr EILSEQ  /* for strerror.c */
  #define EINTR    -1      /* for select.c */
  #endif
#endif

/*
 * Salford-C cludge section (mostly borrowed from wxWidgets).
 */
#ifdef __SALFORDC__
  #pragma suppress 353             /* Possible nested comments */
  #pragma suppress 593             /* Define not used */
  #pragma suppress 61              /* enum has no name */
  #pragma suppress 106             /* unnamed, unused parameter */
  #include <clib.h>
#endif

#if defined(CURLDEBUG) && defined(HAVE_ASSERT_H)
#define curlassert(x) assert(x)
#else
/* does nothing without CURLDEBUG defined */
#define curlassert(x)
#endif


/* To make large file support transparent even on Windows */
#if defined(WIN32) && (SIZEOF_CURL_OFF_T > 4)
#include <sys/stat.h>   /* must come first before we redefine stat() */
#include <io.h>
#define lseek(x,y,z) _lseeki64(x, y, z)
#define struct_stat struct _stati64
#define stat(file,st) _stati64(file,st)
#define fstat(fd,st) _fstati64(fd,st)
#else
#define struct_stat struct stat
#endif /* Win32 with large file support */


/* Below we define some functions. They should
   1. close a socket

   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
   */

#ifdef WIN32

#if !defined(__CYGWIN__)
#define sclose(x) closesocket(x)

#undef HAVE_ALARM
#else
     /* gcc-for-win is still good :) */
#define sclose(x) close(x)
#define HAVE_ALARM
#endif /* !GNU or mingw */

#define DIR_CHAR      "\\"
#define DOT_CHAR      "_"

#else /* WIN32 */

#ifdef MSDOS  /* Watt-32 */
#include <sys/ioctl.h>
#define sclose(x)         close_s(x)
#define select(n,r,w,x,t) select_s(n,r,w,x,t)
#define ioctl(x,y,z) ioctlsocket(x,y,(char *)(z))
#define IOCTL_3_ARGS
#include <tcp.h>
#ifdef word
#undef word
#endif

#else /* MSDOS */

#ifdef __VMS
#define IOCTL_3_ARGS 1
#endif

#ifdef __BEOS__
#define sclose(x) closesocket(x)
#else /* __BEOS__ */
#define sclose(x) close(x)
#endif /* __BEOS__ */

#define HAVE_ALARM

#endif /* MSDOS */

#ifdef _AMIGASF
#undef HAVE_ALARM
#undef sclose
#define sclose(x) CloseSocket(x)
#endif

#define DIR_CHAR      "/"
#ifndef DOT_CHAR
#define DOT_CHAR      "."
#endif

#ifdef MSDOS
#undef DOT_CHAR
#define DOT_CHAR      "_"
#endif

#ifndef fileno /* sunos 4 have this as a macro! */
int fileno( FILE *stream);
#endif

#endif /* WIN32 */

#if defined(WIN32) && !defined(__CYGWIN__) && !defined(USE_ARES) && \
    !defined(__LCC__)  /* lcc-win32 doesn't have _beginthreadex() */
#ifdef ENABLE_IPV6
#define USE_THREADING_GETADDRINFO
#else
#define USE_THREADING_GETHOSTBYNAME  /* Cygwin uses alarm() function */
#endif
#endif

/* "cl -ML" or "cl -MLd" implies a single-threaded runtime library where
   _beginthreadex() is not available */
#if (defined(_MSC_VER) && !defined(__POCC__)) && !defined(_MT) && !defined(USE_ARES)
#undef USE_THREADING_GETADDRINFO
#undef USE_THREADING_GETHOSTBYNAME
#define CURL_NO__BEGINTHREADEX
#endif

/*
 * msvc 6.0 does not have struct sockaddr_storage and
 * does not define IPPROTO_ESP in winsock2.h. But both
 * are available if PSDK is properly installed.
 */

#ifdef _MSC_VER
#if !defined(HAVE_WINSOCK2_H) || ((_MSC_VER < 1300) && !defined(IPPROTO_ESP))
#undef HAVE_STRUCT_SOCKADDR_STORAGE
#endif
#endif

#ifdef mpeix
#define IOCTL_3_ARGS
#endif

#ifdef NETWARE
#undef HAVE_ALARM
#endif

#if defined(HAVE_LIBIDN) && defined(HAVE_TLD_H)
/* The lib was present and the tld.h header (which is missing in libidn 0.3.X
   but we only work with libidn 0.4.1 or later) */
#define USE_LIBIDN
#endif

#ifndef SIZEOF_TIME_T
/* assume default size of time_t to be 32 bit */
#define SIZEOF_TIME_T 4
#endif

#define LIBIDN_REQUIRED_VERSION "0.4.1"

#ifdef __UCLIBC__
#define HAVE_INET_NTOA_R_2_ARGS 1
#endif

#if defined(USE_GNUTLS) || defined(USE_SSLEAY)
#define USE_SSL    /* Either OpenSSL || GnuTLS */
#endif

#if !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_NTLM)
#if defined(USE_SSLEAY) || defined(USE_WINDOWS_SSPI)
#define USE_NTLM
#endif
#endif

#ifdef CURLDEBUG
#define DEBUGF(x) x
#else
#define DEBUGF(x)
#endif

/* non-configure builds may define CURL_WANTS_CA_BUNDLE_ENV */
#if defined(CURL_WANTS_CA_BUNDLE_ENV) && !defined(CURL_CA_BUNDLE)
#define CURL_CA_BUNDLE getenv("CURL_CA_BUNDLE")
#endif

/*
 * Include macros and defines that should only be processed once.
 */

#ifndef __SETUP_ONCE_H
#include "setup_once.h"
#endif

#endif /* __LIB_CURL_SETUP_H */
