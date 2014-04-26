/* APPLE LOCAL file 5235474 5683689 */
/* Additional functions for the GCC driver on Darwin native.
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.
   Contributed by Apple Computer Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef CROSS_DIRECTORY_STRUCTURE
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "gcc.h"
#include <sys/sysctl.h>
#include "xregex.h"

#ifndef SWITCH_TAKES_ARG
#define SWITCH_TAKES_ARG(CHAR) DEFAULT_SWITCH_TAKES_ARG(CHAR)
#endif

#ifndef WORD_SWITCH_TAKES_ARG
#define WORD_SWITCH_TAKES_ARG(STR) DEFAULT_WORD_SWITCH_TAKES_ARG (STR)
#endif

/* This function is used when running on a Darwin system and using that
   system's headers and libraries.  Unless specified otherwise by
   command-line options or environment variables, this routine will
   set the appropriate version specification flag to a default value.
   The version flag used is based on VERS_TYPE, and is either:
   DARWIN_VERSION_MACOSX to use -mmacosx-version-min and
   DARWIN_VERSION_IPHONEOS to use -miphoneos-version-min.  */

void
darwin_default_min_version (int * argc_p, char *** argv_p,
			    enum darwin_version_type vers_type)
{
  const int argc = *argc_p;
  char ** const argv = *argv_p;
  int i;
  char osversion[32];
  size_t osversion_len = sizeof (osversion) - 1;
  static int osversion_name[2] = { CTL_KERN, KERN_OSRELEASE };
  char * version_p;
  char * version_pend;
  int major_vers;
  char minor_vers[6];
  static char new_flag[sizeof ("-mxxxxxx-version-min=99.99.99") + 6];

  /* If the command-line is empty, just return.  */
  if (argc <= 1)
    return;
  /* Don't do this if the user has specified -b or -V at the start
     of the command-line.  */
  if (argv[1][0] == '-'
      && (argv[1][1] == 'V' ||
	  ((argv[1][1] == 'b') && (NULL != strchr(argv[1] + 2,'-')))))
    return;
  
  /* Don't do this if the user specified -mmacosx-version-min=,
     -miphoneos-version-min, -mno-macosx-version-min, or
     -mno-iphoneos-version-min.  */
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      {
	const char * const p = argv[i];
	if (strncmp (p, "-mno-macosx-version-min", 23) == 0
	    || strncmp (p, "-mno-iphoneos-version-min", 25) == 0
	    || strncmp (p, "-mmacosx-version-min", 20) == 0
	    || strncmp (p, "-miphoneos-version-min", 22) == 0)
	  return;
	
	/* It doesn't count if it's an argument to a different switch.  */
	if (p[0] == '-'
	    && ((SWITCH_TAKES_ARG (p[1]) > (p[2] != 0))
		|| WORD_SWITCH_TAKES_ARG (p + 1)))
	  i++;
      }

  /* Retrieve the deployment target from the environment and insert
     it as a flag.  */
  {
    const char * macosx_deployment_target;
    const char * iphoneos_deployment_target;
    bool iphoneos_env_set, macosx_env_set;

    macosx_deployment_target = getenv ("MACOSX_DEPLOYMENT_TARGET");
    iphoneos_deployment_target = getenv ("IPHONEOS_DEPLOYMENT_TARGET");

    /* We choose to ignore an environment variable set to an empty
       string.  */
    macosx_env_set = macosx_deployment_target
		     && macosx_deployment_target[0];
    iphoneos_env_set = iphoneos_deployment_target
		       && iphoneos_deployment_target[0];

    if (macosx_env_set && iphoneos_env_set)
      {
	/* Conflicting DEPLOYMENT_TARGETs given.  Don't emit a warning
	   for now (see rdar://5819018) -- just choose based on
	   VERS_TYPE.  */
	if (vers_type == DARWIN_VERSION_IPHONEOS)
	  macosx_env_set = 0;
	else
	  iphoneos_env_set = 0;
      }

    if (macosx_env_set)
      {
	++*argc_p;
	*argv_p = xmalloc (sizeof (char *) * *argc_p);
	(*argv_p)[0] = argv[0];
	(*argv_p)[1] = concat ("-mmacosx-version-min=",
			       macosx_deployment_target, NULL);
	memcpy (*argv_p + 2, argv + 1, (argc - 1) * sizeof (char *));
	return;
      }

    if (iphoneos_env_set)
      {
	++*argc_p;
	*argv_p = xmalloc (sizeof (char *) * *argc_p);
	(*argv_p)[0] = argv[0];
	(*argv_p)[1] = concat ("-miphoneos-version-min=",
			       iphoneos_deployment_target, NULL);
	memcpy (*argv_p + 2, argv + 1, (argc - 1) * sizeof (char *));
	return;
      }
  }

  /* For iPhone OS, if no version number is specified, we default to
     3.0.  */
  if (vers_type == DARWIN_VERSION_IPHONEOS)
    {
      ++*argc_p;
      *argv_p = xmalloc (sizeof (char *) * *argc_p);
      (*argv_p)[0] = argv[0];
      (*argv_p)[1] = xstrdup ("-miphoneos-version-min=3.0");
      memcpy (*argv_p + 2, argv + 1, (argc - 1) * sizeof (char *));
      return;
    }

  gcc_assert (vers_type == DARWIN_VERSION_MACOSX);

  /* Determine the version of the running OS.  If we can't, warn user,
     and do nothing.  */
  if (sysctl (osversion_name, ARRAY_SIZE (osversion_name), osversion,
	      &osversion_len, NULL, 0) == -1)
    {
      fprintf (stderr, "sysctl for kern.osversion failed: %s\n",
	       xstrerror (errno));
      return;
    }

  /* Try to parse the first two parts of the OS version number.  Warn
     user and return if it doesn't make sense.  */
  if (! ISDIGIT (osversion[0]))
    goto parse_failed;
  major_vers = osversion[0] - '0';
  version_p = osversion + 1;
  if (ISDIGIT (*version_p))
    major_vers = major_vers * 10 + (*version_p++ - '0');
  if (major_vers > 4 + 9)
    goto parse_failed;
  if (*version_p++ != '.')
    goto parse_failed;
  version_pend = strchr(version_p, '.');
  if (!version_pend)
    goto parse_failed;
  if (! ISDIGIT (*version_p))
    goto parse_failed;
  strncpy(minor_vers, version_p, version_pend - version_p);
  minor_vers[version_pend - version_p] = '\0';
  
  /* The major kernel version number is 4 plus the second OS version
     component.  */
  if (major_vers - 4 <= 4)
    /* On 10.4 and earlier, the old linker is used which does not
       support three-component system versions.  */
    sprintf (new_flag, "-mmacosx-version-min=10.%d", major_vers - 4);
  else
    sprintf (new_flag, "-mmacosx-version-min=10.%d.%s", major_vers - 4,
	     minor_vers);

  /* Add the new flag.  */
  ++*argc_p;
  *argv_p = xmalloc (sizeof (char *) * *argc_p);
  (*argv_p)[0] = argv[0];
  (*argv_p)[1] = new_flag;
  memcpy (*argv_p + 2, argv + 1, (argc - 1) * sizeof (char *));
  return;
  
 parse_failed:
  fprintf (stderr, "couldn't understand kern.osversion `%.*s'\n",
	   (int) osversion_len, osversion);
  return;
}

#endif /* CROSS_DIRECTORY_STRUCTURE */
