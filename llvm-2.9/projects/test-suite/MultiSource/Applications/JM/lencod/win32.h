
/*!
 ************************************************************************
 *  \file
 *     win32.h
 *
 *  \brief
 *     win32 definitions for H.264 encoder.
 *
 *  \author
 *
 ************************************************************************
 */
#ifndef _WIN32_H_
#define _WIN32_H_

# include <fcntl.h>
# include <stdio.h>

#if defined(WIN32)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
# define strcasecmp _strcmpi

# define  snprintf _snprintf
# define  open     _open
# define  close    _close
# define  read     _read
# define  write    _write
# define  lseek    _lseeki64
# define  fsync    _commit
# define  tell     _tell
# define  TIMEB    _timeb
# define  ftime    _ftime
# define  OPENFLAGS_WRITE _O_WRONLY|_O_CREAT|_O_BINARY|_O_TRUNC
# define  OPEN_PERMISSIONS _S_IREAD | _S_IWRITE
# define  OPENFLAGS_READ  _O_RDONLY|_O_BINARY
# define  inline   _inline
#else
# include <unistd.h>
# include <strings.h>
# define  TIMEB    timeb
# define  OPENFLAGS_WRITE O_WRONLY|O_CREAT|O_TRUNC
# define  OPENFLAGS_READ  O_RDONLY
# define  OPEN_PERMISSIONS S_IRUSR | S_IWUSR

# if __STDC_VERSION__ >= 199901L
   /* "inline" is a keyword */
# else
#  define inline /* nothing */
# endif
#endif

#if defined(WIN32) && !defined(__GNUC__)
typedef __int64   int64;
# define FORMAT_OFF_T "I64d"
# ifndef INT64_MIN
#  define INT64_MIN        (-9223372036854775807i64 - 1i64)
# endif
#else
typedef long long int64;
# define FORMAT_OFF_T "lld"
# ifndef INT64_MIN
#  define INT64_MIN        (-9223372036854775807LL - 1LL)
# endif
#endif

#endif
