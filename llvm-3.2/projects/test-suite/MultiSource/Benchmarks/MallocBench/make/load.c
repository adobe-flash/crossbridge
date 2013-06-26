/* Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "make.h"
#include "commands.h"
#include "job.h"

#ifdef UMAX

#define LDAV_BASED

/* UMAX 4.2, which runs on the Encore Multimax multiprocessor, does not
   have a /dev/kmem.  Information about the workings of the running kernel
   can be gathered with inq_stats system calls.  */
#include <stdio.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#ifdef	UMAX_43
#include <machine/cpu.h>
#include <inq_stats/statistics.h>
#include <inq_stats/sysstats.h>
#include <inq_stats/cpustats.h>
#include <inq_stats/procstats.h>
#else	/* Not UMAX_43.  */
#include <sys/sysdefs.h>
#include <sys/statistics.h>
#include <sys/sysstats.h>
#include <sys/cpudefs.h>
#include <sys/cpustats.h>
#include <sys/procstats.h>
#endif

double
load_average ()
{
  static unsigned int cpus = 0, samples;
  struct proc_summary proc_sum_data;
  struct stat_descr proc_info;
  double load;
  register unsigned int i, j;

  if (cpus == 0)
    {
      register unsigned int c, i;
      struct cpu_config conf;
      struct stat_descr desc;

      desc.sd_next = 0;
      desc.sd_subsys = SUBSYS_CPU;
      desc.sd_type = CPUTYPE_CONFIG;
      desc.sd_addr = (char *) &conf;
      desc.sd_size = sizeof conf;

      if (inq_stats (1, &desc))
	return 0.0;

      c = 0;
      for (i = 0; i < conf.config_maxclass; ++i)
	{
	  struct class_stats stats;
	  bzero ((char *) &stats, sizeof stats);

	  desc.sd_type = CPUTYPE_CLASS;
	  desc.sd_objid = i;
	  desc.sd_addr = (char *) &stats;
	  desc.sd_size = sizeof stats;

	  if (inq_stats (1, &desc))
	    return 0.0;

	  c += stats.class_numcpus;
	}
      cpus = c;
      samples = cpus < 2 ? 3 : (2 * cpus / 3);
    }
  
  proc_info.sd_next = 0;
  proc_info.sd_subsys = SUBSYS_PROC;
  proc_info.sd_type = PROCTYPE_SUMMARY;
  proc_info.sd_addr = (char *) &proc_sum_data;
  proc_info.sd_size = sizeof (struct proc_summary);
  proc_info.sd_sizeused = 0;
  
  if (inq_stats (1, &proc_info) != 0)
    return 0.0;
  
  load = proc_sum_data.ps_nrunnable;
  j = 0;
  for (i = samples - 1; i > 0; --i)
    {
      load += proc_sum_data.ps_nrun[j];
      if (j++ == PS_NRUNSIZE)
	j = 0;
    }
  
  return (load / samples / cpus);
}

#else	/* Not UMAX.  */

#ifdef	apollo

/* Apollo code from lisch@mentorg.com (Ray Lischner).  */

#define	LDAV_BASED

/* Return the current load average as a double.

   This system call is not documented.  The load average is obtained as
   three long integers, for the load average over the past minute,
   five minutes, and fifteen minutes.  Each value is a scaled integer,
   with 16 bits of integer part and 16 bits of fraction part.

   I'm not sure which operating system first supported this system call,
   but I know that SR10.2 supports it.  */

double
load_average ()
{
  extern void proc1_$get_loadav ();
  unsigned long int loadav[3];
  proc1_$get_loadav(loadav);
  return loadav[0] / 65536.0;
}
 
#else /* Not apollo. */

#ifndef NO_LDAV

#define LDAV_BASED

#if	defined(hp300) && !defined(USG)
#define	LDAV_CVT	(((double) load) / 2048.0)
#endif

#if	defined(ultrix) && defined(vax)
#define	LDAV_TYPE	double
#define	LDAV_CVT	(load)
#endif

#ifndef	KERNEL_FILE_NAME
#define KERNEL_FILE_NAME "/vmunix"
#endif
#ifndef	LDAV_SYMBOL
#define LDAV_SYMBOL "_avenrun"
#endif
#ifndef LDAV_TYPE
#define LDAV_TYPE long int
#endif
#ifndef LDAV_CVT
#define LDAV_CVT ((double) load)
#endif

#include <nlist.h>
#ifdef	NLIST_NAME_UNION
#define	nl_name	n_un.n_name
#else
#define	nl_name	n_name
#endif

#ifdef	USG
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

/* Return the current load average as a double.  */

double
load_average ()
{
  extern int nlist ();
  LDAV_TYPE load;
  static int complained = 0;
  static int kmem = -1;
  static unsigned long int offset = 0;

  if (kmem < 0)
    {
      kmem = open ("/dev/kmem", O_RDONLY);
      if (kmem < 0)
	{
	  if (!complained)
	    perror_with_name ("open: ", "/dev/kmem");
	  goto lose;
	}
    }

  if (offset == 0)
    {
      struct nlist nl[2];

#ifdef	NLIST_NAME_ARRAY
      strcpy (nl[0].nl_name, LDAV_SYMBOL);
      strcpy (nl[1].nl_name, "");
#else	/* Not NLIST_NAME_ARRAY.  */
      nl[0].nl_name = LDAV_SYMBOL;
      nl[1].nl_name = 0;
#endif	/* NLIST_NAME_ARRAY.  */

      if (nlist (KERNEL_FILE_NAME, nl) < 0 || nl[0].n_type == 0)
	{
	  if (!complained)
	    perror_with_name ("nlist: ", KERNEL_FILE_NAME);
	  goto lose;
	}
      offset = nl[0].n_value;
    }

  if (lseek (kmem, offset, 0) == -1L)
    {
      if (!complained)
	perror_with_name ("lseek: ", "/dev/kmem");
      goto lose;
    }
  if (read (kmem, &load, sizeof load) < 0)
    {
      if (!complained)
	perror_with_name ("read: ", "/dev/kmem");
      goto lose;
    }

  if (complained)
    {
      error ("Load average limits will be enforced again.");
      complained = 0;
    }
  return LDAV_CVT;

 lose:;
  if (!complained)
    {
      error ("Load average limits will not be enforced.");
      complained = 1;
    }
  return 0.0;
}

#endif	/* Not NO_LDAV.  */

#endif	/* Apollo.  */
#endif	/* UMAX.  */


#ifdef LDAV_BASED

extern unsigned int job_slots_used;

extern unsigned int sleep ();

/* Don't return until a job should be started.  */

void
wait_to_start_job ()
{
  register unsigned int loops = 0;

  if (max_load_average < 0.0)
    return;

  while (job_slots_used > 0)
    {
      double load = load_average ();

      if (load < max_load_average)
	return;

      ++loops;
      if (loops == 5 || load > max_load_average * 2)
	{
	  /* If the load is still too high after five loops or it is very
	     high, just wait for a child to die before checking again.  */
	  loops = 0;
	  wait_for_children (1, 0);
	}
      else
	/* Don't check the load again immediately, because that will
	   just worsen the load.  Check it progressively more slowly.  */
	sleep (loops);
    }
}

#else	/* Not LDAV_BASED.  */

/* How else to do it?  */

void
wait_to_start_job ()
{
  return;
}
#endif	/* LDAV_BASED.  */
