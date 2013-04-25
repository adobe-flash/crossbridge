/* APPLE LOCAL file driver driver */

/* Darwin driver program that handles -arch commands and invokes
   appropriate compiler driver.
   Copyright (C) 2004, 2005 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <mach-o/arch.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <regex.h>
#include "libiberty.h"
#include "filenames.h"
#include "stdbool.h"
/* Hack!.
   Pay the price for including darwin.h.  */
typedef int tree;
typedef int rtx;
#define GTY(x) /* nothing */
#define USED_FOR_TARGET 1
/* Include darwin.h for SWITCH_TAKES_ARG and
   WORD_SWIATCH_TAKES_ARG. */

#include "darwin.h"

/* Include gcc.h for DEFAULT_SWITCH_TAKES_ARG and
   DEFAULT_WORD_SWIATCH_TAKES_ARG. */

#include "gcc.h"

/* This program name.  */
const char *progname;

/* driver prefix.  */
const char *driver_exec_prefix;

/* driver prefix length.  */
int prefix_len;

/* current working directory.  */
char *curr_dir;

/* Use if -o flag is absent.  */
const char *final_output = "a.out";

/* Variabless to track presence and/or absence of important command
   line options.  */
int compile_only_request = 0;
int asm_output_request = 0;
int dash_capital_m_seen = 0;
int preprocessed_output_request = 0;
int ima_is_used = 0;
int dash_dynamiclib_seen = 0;
int verbose_flag = 0;
int save_temps_seen = 0;
int dash_m32_seen = 0;
int dash_m64_seen = 0;

/* Support at the max 10 arch. at a time. This is historical limit.  */
#define MAX_ARCHES 10

/* Name of user supplied architectures.  */
const char *arches[MAX_ARCHES];

/* -arch counter.  */
static int num_arches;

/* Input filenames.  */
struct input_filename
{
  const char *name;
  int index;
  struct input_filename *next;
};
struct input_filename *in_files;
struct input_filename *last_infile;

static int num_infiles;

/* User specified output file name.  */
const char *output_filename = NULL;

/* Output file names used for arch specific driver invocation. These
   are input file names for 'lipo'.  */
const char **out_files;
static int num_outfiles;

/* Architecture names used by config.guess does not match the names
   used by NXGet... Use this hand coded mapping to connect them.  */
struct arch_config_guess_map
{
  const char *arch_name;
  const char *config_string;
};

struct arch_config_guess_map arch_config_map [] =
{
  {"i386", "i686"},
  {"ppc", "powerpc"},
  {"ppc64", "powerpc"},
  {"x86_64", "i686"},
  {"arm", "arm"},
  {"armv4t", "arm"},
  {"armv5", "arm"},
  {"xscale", "arm"},
  {"armv6", "arm"},
  {"armv7", "arm"},
  {NULL, NULL}
};

/* List of interpreted command line flags. Supply this to gcc driver.  */
const char **new_argv;
int new_argc;

/* For each of the options in new_argv, specifies an architecture to
   which the option applies (or if NULL, the option applies to all).  */
const char **arch_conditional;

/* Argument list for 'lipo'.  */
const char **lipo_argv;

/* Info about the sub process. Need one subprocess for each arch plus
   additional one for 'lipo'.  */
struct command
{
  const char *prog;
  const char **argv;
  int pid;
} commands[MAX_ARCHES+1];

/* total number of argc.  */
static int total_argc;

static int greatest_status = 0;
static int signal_count = 0;

#ifndef SWITCH_TAKES_ARG
#define SWITCH_TAKES_ARG(CHAR) DEFAULT_SWITCH_TAKES_ARG(CHAR)
#endif

#ifndef WORD_SWITCH_TAKES_ARG
#define WORD_SWITCH_TAKES_ARG(STR) DEFAULT_WORD_SWITCH_TAKES_ARG (STR)
#endif


/* Local function prototypes.  */
static const char * get_arch_name (const char *);
static char * get_driver_name (const char *);
static void delete_out_files (void);
static char * strip_path_and_suffix (const char *, const char *);
static void initialize (void);
static void final_cleanup (void);
static int do_wait (int, const char *);
static void do_lipo (int, const char *);
static void do_compile (const char **, int);
static void do_compile_separately (void);
static void do_lipo_separately (void);
static int filter_args_for_arch (const char **, int, const char **,
				 const char *);
static int add_arch_options (int, const char **, int);
static int remove_arch_options (const char**, int);
static void add_arch (const char *);
static const char *resolve_symlink (const char *, char *, int, int);
static const char *resolve_path_to_executable (const char *filename);
static int get_prog_name_len (const char *prog);

/* Find arch name for the given input string. If input name is NULL then local
   arch name is used.  */

static const char *
get_arch_name (const char *name)
{
  NXArchInfo * a_info;
  const NXArchInfo * all_info;
  cpu_type_t cputype;
  struct arch_config_guess_map *map;
  const char *aname;

  if (name) {
    /* Find config name based on arch name.  */
    aname = NULL;
    map = arch_config_map;
    while (map->arch_name) {
      if (!strcmp (map->arch_name, name))
	return name;
      else map++;
    }
    a_info = (NXArchInfo *) NXGetArchInfoFromName (name);
  } else {
    a_info = (NXArchInfo *) NXGetLocalArchInfo();
    if (a_info) {
      if (dash_m32_seen) {
        /* If -m32 is seen then do not change cpu type.  */
      } else if (dash_m64_seen) {
        /* If -m64 is seen then enable CPU_ARCH_ABI64.  */
	a_info->cputype |= CPU_ARCH_ABI64;
      } else if (sizeof (long) == 8)
	/* On x86, by default (name is NULL here) enable 64 bit code.  */
	a_info->cputype |= CPU_ARCH_ABI64;
    }
  }

  if (!a_info)
    fatal ("Invalid arch name : %s", name);

  all_info = NXGetAllArchInfos();

  if (!all_info)
    fatal ("Unable to get architecture information");

  /* Find first arch. that matches cputype.  */
  cputype = a_info->cputype;

  while (all_info->name)
    {
      if (all_info->cputype == cputype)
	break;
      else
	all_info++;
    }

  return all_info->name;
}

/* Find driver name based on input arch name.  */

static char *
get_driver_name (const char *arch_name)
{
  char *driver_name;
  const char *config_name;
  int len;
  int index;
  struct arch_config_guess_map *map;

  /* find config name based on arch name.  */
  config_name = NULL;
  map = arch_config_map;
  while (map->arch_name)
    {
      if (!strcmp (map->arch_name, arch_name))
	{
	  config_name = map->config_string;
	  break;
	}
      else map++;
    }

  if (!config_name)
    fatal ("Unable to guess config name for arch %s", arch_name);

  len = strlen (config_name) + strlen (PDN) + prefix_len + 1;
  driver_name = (char *) malloc (sizeof (char) * len);
  driver_name[0] = '\0';

  if (driver_exec_prefix)
    strcpy (driver_name, driver_exec_prefix);
  strcat (driver_name, config_name);
  strcat (driver_name, PDN);

  return driver_name;
}

/* Delete out_files.  */

static void
delete_out_files (void)
{
  const char *temp;
  struct stat st;
  int i = 0;

  for (i = 0, temp = out_files[i];
       temp && i < total_argc * MAX_ARCHES;
       temp = out_files[++i])
    if (stat (temp, &st) >= 0 && S_ISREG (st.st_mode))
      unlink (temp);

}

/* Put fatal error message on stderr and exit.  */

void
fatal (const char *msgid, ...)
{
  va_list ap;

  va_start (ap, msgid);

  fprintf (stderr, "%s: ", progname);
  vfprintf (stderr, msgid, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  delete_out_files ();
  exit (1);
}

/* Print error message and exit.  */

static void
pfatal_pexecute (const char *errmsg_fmt, const char *errmsg_arg)
{
  if (errmsg_arg)
    {
      int save_errno = errno;

      /* Space for trailing '\0' is in %s.  */
      char *msg = (char *) malloc (strlen (errmsg_fmt) + strlen (errmsg_arg));
      sprintf (msg, errmsg_fmt, errmsg_arg);
      errmsg_fmt = msg;

      errno = save_errno;
    }

  fprintf (stderr,"%s: %s: %s\n", progname, errmsg_fmt, xstrerror (errno));
  delete_out_files ();
  exit (1);
}

#ifdef DEBUG
static void
debug_command_line (const char **debug_argv, int debug_argc)
{
  int i;

  fprintf (stderr,"%s: debug_command_line\n", progname);
  fprintf (stderr,"%s: arg count = %d\n", progname, debug_argc);

  for (i = 0; debug_argv[i]; i++)
    fprintf (stderr,"%s: arg [%d] %s\n", progname, i, debug_argv[i]);
}
#endif

/* Strip directory name from the input file name and replace file name
   suffix with new.  */

static char *
strip_path_and_suffix (const char *full_name, const char *new_suffix)
{
  char *name;
  char *p;

  if (!full_name || !new_suffix)
    return NULL;

  /* Strip path name.  */
  p = (char *)full_name + strlen (full_name);
  while (p != full_name && !IS_DIR_SEPARATOR (p[-1]))
    --p;

  /* Now 'p' is a file name with suffix.  */
  name = (char *) malloc (strlen (p) + 1 + strlen (new_suffix));

  strcpy (name, p);

  p = name + strlen (name);
  while (p != name && *p != '.')
    --p;

  /* If did not reach at the beginning of name then '.' is found.
     Replace '.' with NULL.  */
  if (p != name)
    *p = '\0';

  strcat (name, new_suffix);
  return name;
}

/* Initialization */

static void
initialize (void)
{

  int i;

  /* Let's count, how many additional arguments driver driver will supply
     to compiler driver:

     Each "-arch" "<blah>" is replaced by approriate "-mcpu=<blah>".
     That leaves one additional arg space available.

     Note that only one -m* is supplied to each compiler driver. Which
     means, extra "-arch" "<blah>" are removed from the original command
     line. But lets not count how many additional slots are available.

     Driver driver may need to specify temp. output file name, say
     "-o" "foobar". That needs two extra argments.

     Sometimes linker wants one additional "-Wl,-arch_multiple".

     Sometimes linker wants to see "-final_output" "outputname".

     In the end, we may need five extra arguments, plus one extra
     space for the NULL terminator.  */

  new_argv = (const char **) malloc ((total_argc + 6) * sizeof (const char *));
  if (!new_argv)
    abort ();

  arch_conditional = (const char **) malloc ((total_argc + 6) * sizeof (const char *));
  if (!arch_conditional)
    abort ();

  for (i = 0; i < total_argc + 6; i++)
    arch_conditional[i] = NULL;

  /* First slot, new_argv[0] is reserved for the driver name.  */
  new_argc = 1;

  /* For each -arch, three arguments are needed.
     For example, "-arch" "ppc" "file".  Additional slots are for
     "lipo" "-create" "-o" "outputfilename" and the NULL. */
  lipo_argv = (const char **) malloc ((total_argc * 3 + 5) * sizeof (const char *));
  if (!lipo_argv)
    abort ();

  /* Need separate out_files for each arch, max is MAX_ARCHES.
     Need separate out_files for each input file.  */

  out_files = (const char **) malloc ((total_argc * MAX_ARCHES) * sizeof (const char *));
  if (!out_files)
    abort ();

  num_arches = 0;
  num_infiles = 0;

  in_files = NULL;
  last_infile = NULL;

  for (i = 0; i < (MAX_ARCHES + 1); i++)
    {
      commands[i].prog = NULL;
      commands[i].argv = NULL;
      commands[i].pid = 0;
    }
}

/* Cleanup.  */

static void
final_cleanup (void)
{
  int i;
  struct input_filename *next;
  delete_out_files ();
  free (new_argv);
  free (lipo_argv);
  free (out_files);

  for (i = 0, next = in_files;
       i < num_infiles && next;
       i++)
    {
      next = in_files->next;
      free (in_files);
      in_files = next;
    }
}

/* Wait for the process pid and return appropriate code.  */

static int
do_wait (int pid, const char *prog)
{
  int status = 0;
  int ret = 0;

  pid = pwait (pid, &status, 0);

  if (WIFSIGNALED (status))
    {
      if (!signal_count &&
	  WEXITSTATUS (status) > greatest_status)
	greatest_status = WEXITSTATUS (status);
      ret = -1;
    }
  else if (WIFEXITED (status)
	   && WEXITSTATUS (status) >= 1)
    {
      if (WEXITSTATUS (status) > greatest_status)
	greatest_status = WEXITSTATUS (status);
      signal_count++;
      ret = -1;
    }
  return ret;
}

/* Invoke 'lipo' and combine and all output files.  */

static void
do_lipo (int start_outfile_index, const char *out_file)
{
  int i, j, pid;
  char *errmsg_fmt, *errmsg_arg;

  /* Populate lipo arguments.  */
  lipo_argv[0] = "lipo";
  lipo_argv[1] = "-create";
  lipo_argv[2] = "-o";
  lipo_argv[3] = out_file;

  /* Already 4 lipo arguments are set.  Now add all lipo inputs.  */
  j = 4;
  for (i = 0; i < num_arches; i++)
    lipo_argv[j++] = out_files[start_outfile_index + i];

  /* Add the NULL at the end.  */
  lipo_argv[j++] = NULL;

#ifdef DEBUG
  debug_command_line (lipo_argv, j);
#endif

  if (verbose_flag)
    {
      for (i = 0; lipo_argv[i]; i++)
	fprintf (stderr, "%s ", lipo_argv[i]);
      fprintf (stderr, "\n");
    }
  pid = pexecute (lipo_argv[0], (char *const *)lipo_argv, progname, NULL, &errmsg_fmt,
		  &errmsg_arg, PEXECUTE_SEARCH | PEXECUTE_ONE);

  if (pid == -1)
    pfatal_pexecute (errmsg_fmt, errmsg_arg);

  do_wait (pid, lipo_argv[0]);
}

/* Invoke compiler for all architectures.  */

static void
do_compile (const char **current_argv, int current_argc)
{
  char *errmsg_fmt, *errmsg_arg;
  int index = 0;

  int dash_o_index = current_argc;
  int of_index = current_argc + 1;
  int argc_count = current_argc + 2;

  const char **arch_specific_argv;

  int arch_specific_argc;

  while (index < num_arches)
    {
      int additional_arch_options = 0;

      current_argv[0] = get_driver_name (get_arch_name (arches[index]));

      /* setup output file.  */
      out_files[num_outfiles] = make_temp_file (".out");
      current_argv[dash_o_index] = "-o";
      current_argv[of_index] = out_files [num_outfiles];
      num_outfiles++;

      /* Add arch option as the last option. Do not add any other option
	 before removing this option.  */
      additional_arch_options = add_arch_options (index, current_argv, argc_count);
      argc_count += additional_arch_options;

      current_argv[argc_count] = NULL;

      arch_specific_argv =
	(const char **) malloc ((argc_count + 1) * sizeof (const char *));

      arch_specific_argc = filter_args_for_arch (current_argv,
						 argc_count,
						 arch_specific_argv,
						 get_arch_name (arches[index]));

      commands[index].prog = arch_specific_argv[0];
      commands[index].argv = arch_specific_argv;

#ifdef DEBUG
      debug_command_line (arch_specific_argv, arch_specific_argc);
#endif
      commands[index].pid = pexecute (arch_specific_argv[0],
				      (char *const *)arch_specific_argv,
				      progname, NULL,
				      &errmsg_fmt,
				      &errmsg_arg,
				      PEXECUTE_SEARCH | PEXECUTE_ONE);

      if (commands[index].pid == -1)
	pfatal_pexecute (errmsg_fmt, errmsg_arg);

      do_wait (commands[index].pid, commands[index].prog);
      fflush (stdout);

      /* Remove the last arch option added in the current_argv list.  */
      if (additional_arch_options)
	argc_count -= remove_arch_options (current_argv, argc_count);
      index++;
      free (arch_specific_argv);
    }
}

/* Invoke compiler for each input file separately.
   Construct command line for each invocation with one input file.  */

static void
do_compile_separately (void)
{
  const char **new_new_argv;
  int i, new_new_argc;
  struct input_filename *current_ifn;

  if (num_infiles == 1 || ima_is_used)
    abort ();

  /* Total number of arguments in separate compiler invocation is :
     total number of original arguments - total no input files + one input
     file + "-o" + output file + arch specific options + NULL .  */
  new_new_argv = (const char **) malloc ((new_argc - num_infiles + 5) * sizeof (const char *));
  if (!new_new_argv)
    abort ();

  for (current_ifn = in_files; current_ifn && current_ifn->name;
       current_ifn = current_ifn->next)
    {
      struct input_filename *ifn = in_files;
      int go_back = 0;
      new_new_argc = 1;
      bool ifn_found = false;

      for (i = 1; i < new_argc; i++)
	{
	  if (ifn && ifn->name && !strcmp (new_argv[i], ifn->name))
	    {
	      /* This argument is one of the input file.  */

 	      if (!strcmp (new_argv[i], current_ifn->name))
		{
		  if (ifn_found)
		    fatal ("file %s specified more than once on the command line", current_ifn->name);

		  /* If it is current input file name then add it in the new
		     list.  */
		  new_new_argv[new_new_argc] = new_argv[i];
		  arch_conditional[new_new_argc] = arch_conditional[i];
		  new_new_argc++;
		  ifn_found = true;
		}
	      /* This input file can  not appear in
		 again on the command line so next time look for next input
		 file.  */
	      ifn = ifn->next;
	    }
	  else
	    {
	      /* This argument is not a input file name. Add it into new
		 list.  */
	      new_new_argv[new_new_argc] = new_argv[i];
	      arch_conditional[new_new_argc] = arch_conditional[i];
	      new_new_argc++;
	    }
	}

      /* OK now we have only one input file and all other arguments.  */
      do_compile (new_new_argv, new_new_argc);
    }
}

/* Invoke 'lipo' on set of output files and create multile FAT binaries.  */

static void
do_lipo_separately (void)
{
  int ifn_index;
  struct input_filename *ifn;
  for (ifn_index = 0, ifn = in_files;
       ifn_index < num_infiles && ifn && ifn->name;
       ifn_index++, ifn = ifn->next)
    do_lipo (ifn_index * num_arches,
	     strip_path_and_suffix (ifn->name, ".o"));
}

/* Remove all options which are architecture-specific and are not for the
   current architecture (arch).  */
static int
filter_args_for_arch (const char **orig_argv, int orig_argc,
		      const char **new_argv, const char *arch)
{
  int new_argc = 0;
  int i;

  for (i = 0; i < orig_argc; i++)
    if (arch_conditional[i] == NULL
	|| *arch_conditional[i] == '\0'
	|| ! strcmp (arch_conditional[i], arch))
      new_argv[new_argc++] = orig_argv[i];

  new_argv[new_argc] = NULL;

  return new_argc; 
}

/* Replace -arch <blah> options with appropriate "-mcpu=<blah>" OR
   "-march=<blah>".  INDEX is the index in arches[] table.  We cannot
   return more than 1 as do_compile_separately only allocated one
   extra slot for us.  */

static int
add_arch_options (int index, const char **current_argv, int arch_index)
{
  int count;

  /* We are adding 1 argument for selected arches.  */
  count = 1;

#ifdef DEBUG
  fprintf (stderr, "%s: add_arch_options: %s\n", progname, arches[index]);
#endif

  if (!strcmp (arches[index], "ppc601"))
    current_argv[arch_index] = "-mcpu=601";
  else if (!strcmp (arches[index], "ppc603"))
    current_argv[arch_index] = "-mcpu=603";
  else if (!strcmp (arches[index], "ppc604"))
    current_argv[arch_index] = "-mcpu=604";
  else if (!strcmp (arches[index], "ppc604e"))
    current_argv[arch_index] = "-mcpu=604e";
  else if (!strcmp (arches[index], "ppc750"))
    current_argv[arch_index] = "-mcpu=750";
  else if (!strcmp (arches[index], "ppc7400"))
    current_argv[arch_index] = "-mcpu=7400";
  else if (!strcmp (arches[index], "ppc7450"))
    current_argv[arch_index] = "-mcpu=7450";
  else if (!strcmp (arches[index], "ppc970"))
    current_argv[arch_index] = "-mcpu=970";
  else if (!strcmp (arches[index], "ppc64"))
    current_argv[arch_index] = "-m64";
  else if (!strcmp (arches[index], "i486"))
    current_argv[arch_index] = "-march=i486";
  else if (!strcmp (arches[index], "i586"))
    current_argv[arch_index] = "-march=i586";
  else if (!strcmp (arches[index], "i686"))
    current_argv[arch_index] = "-march=i686";
  else if (!strcmp (arches[index], "pentium"))
    current_argv[arch_index] = "-march=pentium";
  else if (!strcmp (arches[index], "pentium2"))
    current_argv[arch_index] = "-march=pentium2";
  else if (!strcmp (arches[index], "pentpro"))
    current_argv[arch_index] = "-march=pentiumpro";
  else if (!strcmp (arches[index], "pentIIm3"))
    current_argv[arch_index] = "-march=pentium2";
  else if (!strcmp (arches[index], "x86_64"))
    current_argv[arch_index] = "-m64";
  else if (!strcmp (arches[index], "arm"))
    current_argv[arch_index] = "-march=armv4t";
  else if (!strcmp (arches[index], "armv4t"))
    current_argv[arch_index] = "-march=armv4t";
  else if (!strcmp (arches[index], "armv5"))
    current_argv[arch_index] = "-march=armv5tej";
  else if (!strcmp (arches[index], "xscale"))
    current_argv[arch_index] = "-march=xscale";
  else if (!strcmp (arches[index], "armv6"))
    current_argv[arch_index] = "-march=armv6k";
  else if (!strcmp (arches[index], "armv7"))
    current_argv[arch_index] = "-march=armv7a";
  else
    count = 0;

  return count;
}

/* Remove the last option, which is arch option, added by
   add_arch_options.  Return how count of arguments removed.  */
static int
remove_arch_options (const char **current_argv, int arch_index)
{
#ifdef DEBUG
  fprintf (stderr, "%s: Removing argument no %d\n", progname, arch_index);
#endif

  current_argv[arch_index] = '\0';

#ifdef DEBUG
      debug_command_line (current_argv, arch_index);
#endif

  return 1;
}

/* Add new arch request.  */
void
add_arch (const char *new_arch)
{
  int i;

  /* User can say cc -arch ppc -arch ppc foo.c
     Do not invoke ppc compiler twice in this case.  */
  for (i = 0; i < num_arches; i++)
    {
      if (!strcmp (arches[i], new_arch))
	return;
    }

  arches[num_arches] = new_arch;
  num_arches++;
}

/* Rewrite the command line as requested in the QA_OVERRIDE_GCC3_OPTIONS
   environment variable -- used for testing the compiler, working around bugs
   in the Apple build environment, etc.

   The override string is made up of a set of space-separated clauses.  The
   first letter of each clause describes what's to be done:
   +string       Add string as a new argument (at the end of the command line).
                 Multi-word command lines can be added with +x +y
   s/x/y/        substitute x for y in the command line. X must be an entire
                 argument, and can be a regular expression as accepted by the
                 POSIX regexp code.  Y will be substituted as a single argument,
                 and will not have regexp replacements added in.
   xoption       Removes argument matching option
   Xoption       Removes argument matching option and following word
   Ox            Removes any optimization flags in command line and replaces
                 with -Ox.


   Here's some examples:
     O2
     s/precomp-trustfile=foo//
     +-fexplore-antartica
     +-fast
     s/-fsetvalue=* //
     x-fwritable-strings
     s/-O[0-2]/-Osize/
     x-v
     X-o +-o +foo.o

   Option substitutions are processed from left to right; matches and changes
   are cumulative.  An error in processing one element (such as trying to
   remove an element and successor when the match is at the end) cause the
   particular change to stop, but additional changes in the environment
   variable to be applied.

   Key details:
   * we always want to be able to adjust optimization levels for testing
   * adding options is a common task
   * substitution and deletion are less common.

   If the first character of the environment variable is #, changes are
   silent.  If not, diagnostics are written to stderr explaining what
   changes are being performed.

*/

char **arg_array;
int arg_array_size=0;
int arg_count = 0;
int confirm_changes = 1;
const int ARG_ARRAY_INCREMENT_SIZE = 8;
#define FALSE 0

/* Routines for the argument array.  The argument array routines are
   responsible for allocation and deallocation of all objects in the
   array */

void read_args (int argc, char **argv)
{
  int i;

  arg_array_size = argc+10;
  arg_count = argc;
  arg_array = (char**) malloc(sizeof(char*)*arg_array_size);

  for (i=0;i<argc;i++) {
    arg_array[i] = malloc (strlen (argv[i])+1);
    strcpy (arg_array[i], argv[i]);
  }
}

/* Insert the argument before pos. */
void insert_arg(int pos, char *arg_to_insert)
{
  int i;
  char *newArg = malloc (strlen (arg_to_insert)+1);
  strcpy(newArg, arg_to_insert);

  if (arg_count == arg_array_size) {
    /* expand array */
    arg_array_size = arg_count + ARG_ARRAY_INCREMENT_SIZE;
    arg_array = (char**) realloc (arg_array, arg_array_size);
  }

  for (i = arg_count; i > pos; i--) {
    arg_array[i+1] = arg_array[i];
  }

  arg_array[pos] = newArg;
  arg_count++;

  if (confirm_changes)
    fprintf(stderr,"### Adding argument %s at position %d\n",arg_to_insert, pos);
}


void replace_arg (char *str, int pos) {
  char *newArg = malloc(strlen(str)+1);
  strcpy(newArg,str);

  if (confirm_changes)
    fprintf (stderr,"### Replacing %s with %s\n",arg_array[pos], str);

  free (arg_array[pos]);
  arg_array[pos] = newArg;
}

void append_arg (char *str)
{
  char *new_arg = malloc (strlen (str)+1);
  strcpy (new_arg, str);
  if (confirm_changes)
    fprintf(stderr,"### Adding argument %s at end\n", str);

  if (arg_count == arg_array_size) {
    /* expand array */
    arg_array_size = arg_count + ARG_ARRAY_INCREMENT_SIZE;
    arg_array = (char**) realloc (arg_array, arg_array_size);
  }

  arg_array[arg_count++] = new_arg;
}

void delete_arg(int pos) {
  int i;

  if (confirm_changes)
    fprintf(stderr,"### Deleting argument %s\n",arg_array[pos]);

  free (arg_array[pos]);

  for (i=pos; i < arg_count; i++)
    arg_array[i] = arg_array[i+1];

  arg_count--;
}

/* Changing optimization levels is a common testing pattern --
   we've got a special option that searches for and replaces anything
   beginning with -O */
void replace_optimization_level (char *new_level) {
  int i;
  int optionFound = 0;
  char *new_opt = malloc(strlen(new_opt)+3);
  sprintf(new_opt, "-O%s",new_level);


  for (i=0;i<arg_count;i++) {
    if (strncmp(arg_array[i],"-O",2) == 0) {
      replace_arg (new_opt, i);
      optionFound = 1;
      break;
    }
  }

  if (optionFound == 0)
    /* No optimization level?  Add it! */
    append_arg (new_opt);

  free (new_opt);
}

/* Returns a NULL terminated string holding whatever was in the original
   string at that point.  This must be freed by the caller. */

char *arg_string(char *str, int begin, int len) {
  char *new_str = malloc(len+1);
  strncpy(new_str,&str[begin],len);
  new_str[len] = '\0';
  return new_str;
}

/* Given a search-and-replace string of the form
   s/x/y/

   do search and replace on the arg list.  Make sure to check that the
   string is sane -- that it has all the proper slashes that are necessary.
   The search string can be a regular expression, but the replace string
   must be a literal; the search must also be for a full argument, not for
   a chain of arguments.  The result will be treated as a single argument.

   Return true if success, false if bad failure.
*/

bool search_and_replace (char *str) {
  regex_t regexp_search_struct;
  int searchLen;
  int replaceLen;
  int i;
  int err;

  char *searchStr;
  char *replaceStr;
  char *replacedStr;
  const int  ERRSIZ = 512;
  char errbuf[ERRSIZ];


  if (str[0] != '/') {
    return false;
  }

  searchLen = strcspn (str + 1, "/\0");

  if (str[1 + searchLen] != '/')
    return false;

  replaceLen = strcspn(str+1+searchLen+1, "/\0");

  if (str[1 + searchLen + 1 +replaceLen] != '/')
    return false;

  searchStr = arg_string(str, 1, searchLen);
  replaceStr = arg_string (str, 1 + searchLen + 1, replaceLen);

  if ((err = regcomp(&regexp_search_struct, searchStr, REG_EXTENDED)) != 0) {
    regerror(err, &regexp_search_struct, errbuf, ERRSIZ);
    fprintf(stderr,"%s",errbuf);
    return false;
  }

  for (i=0;i<arg_count;i++) {
    regmatch_t matches[5];
    if (regexec (&regexp_search_struct, arg_array[i],
		 5, matches, 0) == 0) {
      if ((matches[0].rm_eo - matches[0].rm_so) == strlen (arg_array[i])) {
	/* Success! Change the string. */
	replace_arg(replaceStr,i);
	break;
      }
    }
  }

  regfree (&regexp_search_struct);
  free (searchStr);
  free (replaceStr);

  return true;
}


/* Given a string, return the argument number where the first match occurs. */
int find_arg (char *str) {
  int i;
  int matchIndex = -1;

  for (i=0;i<arg_count;i++) {
    if (strcmp(arg_array[i],str) == 0) {
      matchIndex = i;
      break;
    }
  }

  return matchIndex;
}

void rewrite_command_line (char *override_options_line, int *argc, char ***argv){
  int line_pos = 0;

  read_args (*argc, *argv);

  if (override_options_line[0] == '#')
    {
      confirm_changes = 0;
      line_pos++;
    }


  if (confirm_changes)
    fprintf (stderr, "### QA_OVERRIDE_GCC3_OPTIONS: %s\n",
	     override_options_line);

  /* Loop through all commands in the file */

  while (override_options_line[line_pos] != '\0')
    {
      char first_char;
      char *searchStr;
      char *arg;
      int search_index;
      int arg_len;

      /* Any spaces in between options don't count. */
      if (override_options_line[line_pos] == ' ')
	{
	  line_pos++;
	  continue;
	}

      /* The first non-space character is the command. */
      first_char = override_options_line[line_pos];
      line_pos++;
      arg_len = strcspn(override_options_line+line_pos, " ");

      switch (first_char) {
      case '+':
	/* Add an argument to the end of the arg list */
	arg = arg_string (override_options_line,
			  line_pos,
			  arg_len);
	append_arg (arg);
	free (arg);
	break;

      case 'x':
	/* Delete a matching argument */
	searchStr = arg_string(override_options_line, line_pos, arg_len);
	if ((search_index = find_arg(searchStr)) != -1) {
	  delete_arg(search_index);
	}
	free (searchStr);
	break;

      case 'X':
	/* Delete a matching argument and the argument following. */
	searchStr = arg_string(override_options_line, line_pos, arg_len);
	if ((search_index = find_arg(searchStr)) != -1) {
	  if (search_index >= arg_count -1) {
	    if (confirm_changes)
	      fprintf(stderr,"Not enough arguments to do X\n");
	  } else {
	    delete_arg(search_index); /* Delete the matching argument */
	    delete_arg(search_index); /* Delete the following argument */
	  }
	}
	free (searchStr);
	break;

      case 'O':
	/* Change the optimization level to the specified value, and
	   remove any optimization arguments.   This is a separate command
	   because we often want is to substitute our favorite
	   optimization level for whatever the project normally wants.
	   As we probably care about this a lot (for things like
	   testing file sizes at different optimization levels) we
	   make a special rewrite clause. */
	arg = arg_string (override_options_line, line_pos, arg_len);
	replace_optimization_level(arg);
	free (arg);
	break;
      case 's':
	/* Search for the regexp passed in, and replace a matching argument
	   with the provided replacement string */
	searchStr = arg_string (override_options_line, line_pos, arg_len);
	search_and_replace (searchStr);
	free (searchStr);
	break;

      default:
	fprintf(stderr,"### QA_OVERRIDE_GCC3_OPTIONS: invalid string (pos %d)\n",
		line_pos);
	break;
      }
      line_pos += arg_len;
    }
  *argc = arg_count;
  *argv = arg_array;
}

/* Given a path to a file, potentially containing a directory name, return the
   number of characters at the end of the path that make up the filename part of
   the path. */

static int
get_prog_name_len (const char *prog)
{
  int result = 0;
  const char *progend = prog + strlen(prog);
  const char *progname = progend;
  while (progname != prog && !IS_DIR_SEPARATOR (progname[-1]))
    --progname;
  return progend-progname;
}

/* Return true iff the path is an executable file and not a directory.  */

static bool
is_x_file (const char *path)
{
  struct stat st;
  if (access (path, X_OK))
    return false;
  if (stat (path, &st) == -1)
    return false;
  if (S_ISDIR (st.st_mode))
    return false;
  return true;
}

/* Given a FILENAME of an executable (for example "gcc") search the PATH
   environment variable to find out which directory it is in and return a fully
   qualified path to the executable.
 */

static const char *
resolve_path_to_executable (const char *filename)
{
  char path_buffer[2*PATH_MAX+1];
  char *PATH = getenv ("PATH");
  if (PATH == 0) return filename;  /* PATH not set */

  do {
    unsigned prefix_size;
    struct stat st;
    char *colon = strchr (PATH, ':');

    /* If we didn't find a :, use the whole last chunk. */
    prefix_size = colon ? colon-PATH : strlen (PATH);

    /* Form the full path. */
    memcpy (path_buffer, PATH, prefix_size);
    path_buffer[prefix_size] = '/';
    strcpy (path_buffer+prefix_size+1, filename);

    /* Check to see if this file is executable, if so, return it. */
    if (is_x_file (path_buffer))
      return strdup (path_buffer);
    PATH = colon ? colon+1 : PATH+prefix_size;
  } while (PATH[0]);

  return filename;
}

/* If prog is a symlink, we want to rewrite prog to an absolute location,
   symlink_buffer contains the destination of the symlink.  Glue these pieces
   together to form an absolute path. */

static const char *
resolve_symlink (const char *prog, char *symlink_buffer,
		 int argv_0_len, int prog_len)
{
  /* If the link isn't to an absolute path, prefix it with the argv[0]
    directory. */
  if (!IS_ABSOLUTE_PATH (symlink_buffer))
  {
    int prefix_len = argv_0_len - prog_len;
    memmove (symlink_buffer+prefix_len, symlink_buffer,
             PATH_MAX-prefix_len+1);
    memcpy (symlink_buffer, prog, prefix_len);
  }
  return strdup(symlink_buffer);
}

/* Main entry point. This is gcc driver driver!
   Interpret -arch flag from the list of input arguments. Invoke appropriate
   compiler driver. 'lipo' the results if more than one -arch is supplied.  */
int
main (int argc, const char **argv)
{
  size_t i;
  int l, pid, argv_0_len, prog_len;
  char *errmsg_fmt, *errmsg_arg;
  char *override_option_str = NULL;
  char path_buffer[2*PATH_MAX+1];
  int linklen;

  total_argc = argc;
  prog_len = 0;

  argv_0_len = strlen (argv[0]);

  /* Get the progname, required by pexecute () and program location.  */
  prog_len = get_prog_name_len (argv[0]);

  /* If argv[0] is all program name (no slashes), search the PATH environment
     variable to get the fully resolved path to the executable. */
  if (prog_len == argv_0_len)
    {
#ifdef DEBUG
      progname = argv[0] + argv_0_len - prog_len;
      fprintf (stderr,"%s: before PATH resolution, full progname = %s\n",
               argv[0]+argv_0_len-prog_len, argv[0]);
#endif
      argv[0] = resolve_path_to_executable (argv[0]);
      prog_len = get_prog_name_len (argv[0]);
      argv_0_len = strlen(argv[0]);
    }

  /* If argv[0] is a symbolic link, use the directory of the pointed-to file
     to find compiler components. */
  /* LLVM LOCAL: loop to follow multiple levels of links */
  while ((linklen = readlink (argv[0], path_buffer, PATH_MAX)) != -1)
    {
      /* readlink succeeds if argv[0] is a symlink.  path_buffer now contains
	 the file referenced. */
      path_buffer[linklen] = '\0';
#ifdef DEBUG
      progname = argv[0] + argv_0_len - prog_len;
      fprintf (stderr, "%s: before symlink, full prog = %s target = %s\n",
	       progname, argv[0], path_buffer);
#endif
      argv[0] = resolve_symlink(argv[0], path_buffer, argv_0_len, prog_len);
      argv_0_len = strlen(argv[0]);

      /* Get the progname, required by pexecute () and program location.  */
      prog_len = get_prog_name_len (argv[0]);

#ifdef DEBUG
      progname = argv[0] + argv_0_len - prog_len;
      printf("%s: ARGV[0] after symlink = %s\n", progname, argv[0]);
#endif
    }

  progname = argv[0] + argv_0_len - prog_len;

  /* Setup driver prefix.  */
  prefix_len = argv_0_len - prog_len;
  curr_dir = (char *) malloc (sizeof (char) * (prefix_len + 1));
  strncpy (curr_dir, argv[0], prefix_len);
  curr_dir[prefix_len] = '\0';
  driver_exec_prefix = (argv[0], "/usr/bin", curr_dir);

#ifdef DEBUG
  fprintf (stderr,"%s: full progname = %s\n", progname, argv[0]);
  fprintf (stderr,"%s: progname = %s\n", progname, progname);
  fprintf (stderr,"%s: driver_exec_prefix = %s\n", progname, driver_exec_prefix);
#endif

  /* Before we get too far, rewrite the command line with any requested overrides */
  if ((override_option_str = getenv ("QA_OVERRIDE_GCC3_OPTIONS")) != NULL)
    rewrite_command_line(override_option_str, &argc, (char***)&argv);



  initialize ();

  /* Process arguments. Take appropriate actions when
     -arch, -c, -S, -E, -o is encountered. Find input file name.  */
  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-arch"))
	{
	  if (i + 1 >= argc)
	    abort ();

	  add_arch (argv[i+1]);
	  i++;
	}
      else if (!strcmp (argv[i], "-c"))
	{
	  new_argv[new_argc++] = argv[i];
	  compile_only_request = 1;
	}
      else if (!strcmp (argv[i], "-S"))
	{
	  new_argv[new_argc++] = argv[i];
	  asm_output_request = 1;
	}
      else if (!strcmp (argv[i], "-E"))
	{
	  new_argv[new_argc++] = argv[i];
	  preprocessed_output_request = 1;
	}
      else if (!strcmp (argv[i], "-MD") || !strcmp (argv[i], "-MMD"))
	{
	  new_argv[new_argc++] = argv[i];
	  dash_capital_m_seen = 1;
	}
      else if (!strcmp (argv[i], "-m32"))
	{
	  new_argv[new_argc++] = argv[i];
	  dash_m32_seen = 1;
	}
      else if (!strcmp (argv[i], "-m64"))
	{
	  new_argv[new_argc++] = argv[i];
	  dash_m64_seen = 1;
	}
      else if (!strcmp (argv[i], "-dynamiclib"))
	{
	  new_argv[new_argc++] = argv[i];
	  dash_dynamiclib_seen = 1;
        }
      else if (!strcmp (argv[i], "-v"))
	{
	  new_argv[new_argc++] = argv[i];
	  verbose_flag = 1;
	}
      else if (!strcmp (argv[i], "-o"))
	{
	  if (i + 1 >= argc)
	    fatal ("argument to '-o' is missing");

	  output_filename = argv[i+1];
	  i++;
	}
      else if ((! strcmp (argv[i], "-pass-exit-codes"))
	       || (! strcmp (argv[i], "-print-search-dirs"))
	       || (! strcmp (argv[i], "-print-libgcc-file-name"))
	       || (! strncmp (argv[i], "-print-file-name=", 17))
	       || (! strncmp (argv[i], "-print-prog-name=", 17))
	       || (! strcmp (argv[i], "-print-multi-lib"))
	       || (! strcmp (argv[i], "-print-multi-directory"))
	       || (! strcmp (argv[i], "-print-multi-os-directory"))
	       || (! strcmp (argv[i], "-ftarget-help"))
	       || (! strcmp (argv[i], "-fhelp"))
	       || (! strcmp (argv[i], "+e"))
	       || (! strncmp (argv[i], "-Wa,",4))
	       || (! strncmp (argv[i], "-Wp,",4))
	       || (! strncmp (argv[i], "-Wl,",4))
	       || (! strncmp (argv[i], "-l", 2))
	       || (! strncmp (argv[i], "-weak-l", 7))
	       || (! strncmp (argv[i], "-specs=", 7))
	       || (! strcmp (argv[i], "-ObjC"))
	       || (! strcmp (argv[i], "-fobjC"))
	       || (! strcmp (argv[i], "-ObjC++"))
	       || (! strcmp (argv[i], "-time"))
	       || (! strcmp (argv[i], "-###"))
	       || (! strcmp (argv[i], "-fconstant-cfstrings"))
	       || (! strcmp (argv[i], "-fno-constant-cfstrings"))
	       || (! strcmp (argv[i], "-static-libgcc"))
	       || (! strcmp (argv[i], "-shared-libgcc"))
	       || (! strcmp (argv[i], "-pipe"))
	       )
	{
	  new_argv[new_argc++] = argv[i];
	}
      else if (! strcmp (argv[i], "-save-temps")
	       || ! strcmp (argv[i], "--save-temps"))
	{
	  new_argv[new_argc++] = argv[i];
	  save_temps_seen = 1;
	}
      else if ((! strcmp (argv[i], "-Xlinker"))
	       || (! strcmp (argv[i], "-Xassembler"))
	       || (! strcmp (argv[i], "-Xpreprocessor"))
	       || (! strcmp (argv[i], "-l"))
	       || (! strcmp (argv[i], "-weak_library"))
	       || (! strcmp (argv[i], "-weak_framework"))
	       || (! strcmp (argv[i], "-specs"))
	       || (! strcmp (argv[i], "-framework"))
	       )
	{
	  new_argv[new_argc++] = argv[i];
	  i++;
	  new_argv[new_argc++] = argv[i];
	}
      else if (! strncmp (argv[i], "-Xarch_", 7))
	{
	  arch_conditional[new_argc] = get_arch_name (argv[i] + 7);
	  i++;
	  new_argv[new_argc++] = argv[i];
	}
      else if (argv[i][0] == '-' && argv[i][1] != 0)
	{
	  const char *p = &argv[i][1];
	  int c = *p;

	  /* First copy this flag itself.  */
	  new_argv[new_argc++] = argv[i];

	  if (argv[i][1] == 'M')
	    dash_capital_m_seen = 1;

	  /* Now copy this flag's arguments, if any, appropriately.  */
	  if ((SWITCH_TAKES_ARG (c) > (p[1] != 0))
	      || WORD_SWITCH_TAKES_ARG (p))
	    {
	      int j = 0;
	      int n_args = WORD_SWITCH_TAKES_ARG (p);
	      if (n_args == 0)
		{
		  /* Count only the option arguments in separate argv elements.  */
		  n_args = SWITCH_TAKES_ARG (c) - (p[1] != 0);
		}
	      if (i + n_args >= argc)
		fatal ("argument to `-%s' is missing", p);


	      while ( j < n_args)
		{
		  i++;
		  new_argv[new_argc++] = argv[i];
		  j++;
		}
	    }

	}
      else
	{
	  struct input_filename *ifn;
	  new_argv[new_argc++] = argv[i];
	  ifn = (struct input_filename *) malloc (sizeof (struct input_filename));
	  ifn->name = argv[i];
	  ifn->index = i;
	  ifn->next = NULL;
	  num_infiles++;

	  if (last_infile)
	      last_infile->next = ifn;
	  else
	    in_files = ifn;

	  last_infile = ifn;
	}
    }

#if 0
  if (num_infiles == 0)
    fatal ("no input files");
#endif

  if (num_arches == 0)
    add_arch(get_arch_name(NULL));

  if (num_arches > 1)
    {
      if (preprocessed_output_request
	  || save_temps_seen
	  || asm_output_request
	  || dash_capital_m_seen)
	fatal ("-E, -S, -save-temps and -M options are not allowed with multiple -arch flags");
    }
  /* If -arch is not present OR Only one -arch <blah> is specified.
     Invoke appropriate compiler driver.  FAT build is not required in this
     case.  */

  if (num_arches == 1)
    {
      int arch_specific_argc;
      const char **arch_specific_argv;

      /* Find compiler driver based on -arch <foo> and add approriate
	 -m* argument.  */
      new_argv[0] = get_driver_name (get_arch_name (arches[0]));
      new_argc = new_argc + add_arch_options (0, new_argv, new_argc);

#ifdef DEBUG
      printf ("%s: invoking single driver name = %s\n", progname, new_argv[0]);
#endif

      /* Re insert output file name.  */
      if (output_filename)
	{
	  new_argv[new_argc++] = "-o";
	  new_argv[new_argc++] = output_filename;
	}

      /* Add the NULL.  */
      new_argv[new_argc] = NULL;

      arch_specific_argv =
	(const char **) malloc ((new_argc + 1) * sizeof (const char *));
      arch_specific_argc = filter_args_for_arch (new_argv,
						 new_argc,
						 arch_specific_argv,
						 get_arch_name (arches[0]));

#ifdef DEBUG
      debug_command_line (arch_specific_argv, arch_specific_argc);
#endif

      pid = pexecute (arch_specific_argv[0], (char *const *)arch_specific_argv,
		      progname, NULL, &errmsg_fmt, &errmsg_arg,
		      PEXECUTE_SEARCH | PEXECUTE_ONE);

      if (pid == -1)
	pfatal_pexecute (errmsg_fmt, errmsg_arg);

      do_wait (pid, arch_specific_argv[0]);
    }
  else
    {
      /* Handle multiple -arch <blah>.  */

      /* If more than one input files are supplied but only one output filename
	 is present then IMA will be used.  */
      if (num_infiles > 1 && !compile_only_request)
	ima_is_used = 1;

      /* The compiler and linker both want to know if we have multiple archs.
         The compiler for debug info emission and the linker for augmenting
         error and warning messages.  */
	    new_argv[new_argc++] = "-arch_multiple";


      /* If only one input file is specified OR IMA is used then expected output
	 is one FAT binary.  */
      if (num_infiles == 1 || ima_is_used)
	{
	  const char *out_file;

	     /* Create output file name based on
	     input filename, if required.  */
	  if (compile_only_request && !output_filename && num_infiles == 1)
	    out_file = strip_path_and_suffix (in_files->name, ".o");
	  else
	    out_file = (output_filename ? output_filename : final_output);


	  /* Linker wants to know name of output file using one extra arg.  */
	  if (!compile_only_request)
	    {
	      char *oname = (char *)(output_filename ? output_filename : final_output);
	      char *n =  malloc (sizeof (char) * (strlen (oname) + 5));
	      strcpy (n, "-Wl,");
	      strcat (n, oname);
	      new_argv[new_argc++] = "-Wl,-final_output";
	      new_argv[new_argc++] = n;
	    }

	  /* Compile file(s) for each arch and lipo 'em together.  */
	  do_compile (new_argv, new_argc);

	  /* Make FAT binary by combining individual output files for each
	     architecture, using 'lipo'.  */
	  do_lipo (0, out_file);
	}
      else
	{
	  /* Multiple input files are present and IMA is not used.
	     Which means need to generate multiple FAT files.  */
	  do_compile_separately ();
	  do_lipo_separately ();
	}
    }

  final_cleanup ();
  free (curr_dir);
  return greatest_status;
}
