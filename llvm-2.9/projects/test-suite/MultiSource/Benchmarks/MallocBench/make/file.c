/* Copyright (C) 1988, 1989, 1990, 1991 Free Software Foundation, Inc.
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
#include "dep.h"
#include "file.h"
#include "variable.h"
#include <errno.h>



/* Hash table of files the makefile knows how to make.  */

#ifndef	FILE_BUCKETS
#define FILE_BUCKETS	1007
#endif
static struct file *files[FILE_BUCKETS];

/* Number of files with the `intermediate' flag set.  */

unsigned int num_intermediates = 0;


/* Access the hash table of all file records.
   lookup_file  given a name, return the struct file * for that name,
           or nil if there is none.
   enter_file   similar, but create one if there is none.  */

struct file *
lookup_file (name)
     char *name;
{
  register struct file *f;
  register char *n;
  register unsigned int hashval;

  if (*name == '\0')
    abort ();

  while (name[0] == '.' && name[1] == '/' && name[2] != '\0')
    name += 2;

  hashval = 0;
  for (n = name; *n != '\0'; ++n)
    HASH (hashval, *n);
  hashval %= FILE_BUCKETS;

  for (f = files[hashval]; f != 0; f = f->next)
    if (streq (f->name, name))
      return f;
  return 0;
}

struct file *
enter_file (name)
     char *name;
{
  register struct file *f, *new;
  register char *n;
  register unsigned int hashval;

  if (*name == '\0')
    abort ();

  while (name[0] == '.' && name[1] == '/' && name[2] != '\0')
    name += 2;

  hashval = 0;
  for (n = name; *n != '\0'; ++n)
    HASH (hashval, *n);
  hashval %= FILE_BUCKETS;

  for (f = files[hashval]; f != 0; f = f->next)
    if (streq (f->name, name))
      break;

  if (f != 0 && !f->double_colon)
    return f;

  new = (struct file *) xmalloc (sizeof (struct file));
  bzero ((char *) new, sizeof (struct file));
  new->name = name;
  new->update_status = -1;

  if (f == 0)
    {
      /* This is a completely new file.  */
      new->next = files[hashval];
      files[hashval] = new;
    }
  else
    {
      /* There is already a double-colon entry for this file.  */
      while (f->prev != 0)
	f = f->prev;
      f->prev = new;
    }

  return new;
}

/* Rename FILE to NAME.  This is not as simple as resetting
   the `name' member, since it must be put in a new hash bucket,
   and possibly merged with an existing file called NAME.  */

void
rename_file (file, name)
     register struct file *file;
     char *name;
{
  char *oldname = file->name;
  register unsigned int oldhash, newhash;
  register char *n;
  register struct file *f;
  struct file *oldfile;

  /* Find the hash values of the old and new names.  */

  oldhash = 0;
  for (n = oldname; *n != '\0'; ++n)
    HASH (oldhash, *n);
  oldhash %= FILE_BUCKETS;

  newhash = 0;
  for (n = name; *n != '\0'; ++n)
    HASH (newhash, *n);
  newhash %= FILE_BUCKETS;

  /* Look for an existing file under the new name.  */

  for (oldfile = files[newhash]; oldfile != 0; oldfile = oldfile->next)
    if (streq (oldfile->name, name))
      break;

  if (newhash != oldhash || oldfile != 0)
    {
      /* Remove FILE from its hash bucket.  */

      struct file *lastf = 0;

      for (f = files[oldhash]; f != file; f = f->next)
	lastf = f;

      if (lastf == 0)
	files[oldhash] = f->next;
      else
	lastf->next = f->next;
    }

  /* Give FILE its new name.  */

  for (f = file; f != 0; f = f->prev)
    f->name = name;

  if (oldfile == 0)
    {
      /* There is no existing file with the new name.  */

      if (newhash != oldhash)
	{
	  /* Put FILE in its new hash bucket.  */
	  file->next = files[newhash];
	  files[newhash] = file;
	}
    }
  else
    {
      /* There is an existing file with the new name.
	 We must merge FILE into the existing file.  */

      register struct dep *d;

      if (file->cmds != 0)
	{
	  if (oldfile->cmds == 0)
	    oldfile->cmds = file->cmds;
	  else if (file->cmds != oldfile->cmds)
	    {
	      /* We have two sets of commands.  We will go with the
		 one given in the rule explicitly mentioning this name,
		 but give a message to let the user know what's going on.  */
	      makefile_error (file->cmds->filename, file->cmds->lineno,
			      "Commands were specified for file `%s' at %s:%u,",
			      oldname, oldfile->cmds->filename, oldfile->cmds->lineno);
	      makefile_error (file->cmds->filename, file->cmds->lineno,
			      "but `%s' is now considered the same file \
as `%s'.",
			      oldname, name);
	      makefile_error (file->cmds->filename, file->cmds->lineno,
			      "Commands for `%s' will be ignored \
in favor of those for `%s'.",
			      name, oldname);
	    }
	}

      /* Merge the dependencies of the two files.  */

      d = oldfile->deps;
      if (d == 0)
	oldfile->deps = file->deps;
      else
	{
	  while (d->next != 0)
	    d = d->next;
	  d->next = file->deps;
	  uniquize_deps (oldfile->deps);
	}

      merge_variable_set_lists (&oldfile->variables, file->variables);

      if (oldfile->double_colon && !file->double_colon)
	fatal ("can't rename single-colon `%s' to double-colon `%s'",
	       oldname, name);
      if (!oldfile->double_colon && file->double_colon)
	fatal ("can't rename double-colon `%s' to single-colon `%s'",
	       oldname, name);

      if (file->last_mtime > oldfile->last_mtime)
	/* %%% Kludge so -W wins on a file that gets vpathized.  */
	oldfile->last_mtime = file->last_mtime;

#define MERGE(field) oldfile->field |= file->field
      MERGE (precious);
      MERGE (tried_implicit);
      MERGE (updating);
      MERGE (updated);
      MERGE (is_target);
      MERGE (cmd_target);
      MERGE (phony);
#undef MERGE

      file->renamed = oldfile;
    }
}

/* Remove all nonprecious intermediate files.
   If SIG is nonzero, this was caused by a fatal signal,
   meaning that a different message will be printed, and
   the message will go to stderr rather than stdout.  */

void
remove_intermediates (sig)
     int sig;
{
  register int i;
  register struct file *f;
  char doneany;
  
  if (!sig && just_print_flag)
    return;

  doneany = 0;
  for (i = 0; i < FILE_BUCKETS; ++i)
    for (f = files[i]; f != 0; f = f->next)
      if (f->intermediate && (f->dontcare || !f->precious))
	{
	  int status;
	  if (just_print_flag)
	    status = 0;
	  else
	    {
	      status = unlink (f->name);
	      if (status < 0 && errno == ENOENT)
		continue;
	    }
	  if (!f->dontcare)
	    {
	      if (sig)
		error ("*** Deleting file `%s'", f->name);
	      else if (!silent_flag && !just_print_flag)
		{
		  if (!doneany)
		    {
		      fputs ("rm ", stdout);
		      doneany = 1;
		    }
		  putchar (' ');
		  fputs (f->name, stdout);
		  fflush (stdout);
		}
	      if (status < 0)
		perror_with_name ("unlink: ", f->name);
	    }
	}

  if (doneany && !sig)
    {
      putchar ('\n');
      fflush (stdout);
    }
}

/* For each dependency of each file, make the `struct dep' point
   at the appropriate `struct file' (which may have to be created).

   Also mark the files depended on by .PRECIOUS and .PHONY.  */

void
snap_deps ()
{
  register struct file *f, *f2;
  register struct dep *d;
  register int i;

  /* Enter each dependency name as a file.  */
  for (i = 0; i < FILE_BUCKETS; ++i)
    for (f = files[i]; f != 0; f = f->next)
      for (f2 = f; f2 != 0; f2 = f2->prev)
	for (d = f2->deps; d != 0; d = d->next)
	  if (d->name != 0)
	    {
	      d->file = lookup_file (d->name);
	      if (d->file == 0)
		d->file = enter_file (d->name);
	      else
		free (d->name);
	      d->name = 0;
	    }
  
  for (f = lookup_file (".PRECIOUS"); f != 0; f = f->prev)
    for (d = f->deps; d != 0; d = d->next)
      for (f2 = d->file; f2 != 0; f2 = f2->prev)
	f2->precious = 1;

  for (f = lookup_file (".PHONY"); f != 0; f = f->prev)
    for (d = f->deps; d != 0; d = d->next)
      for (f2 = d->file; f2 != 0; f2 = f2->prev)
	{
	  /* Mark this file as phony and nonexistent.  */
	  f2->phony = 1;
	  f2->last_mtime = (time_t) -1;
	}
}

/* Print the data base of files.  */

void
print_file_data_base ()
{
  register unsigned int i, nfiles, per_bucket;
  register struct file *file;
  register struct dep *d;

  puts ("\n# Files");

  per_bucket = nfiles = 0;
  for (i = 0; i < FILE_BUCKETS; ++i)
    {
      register unsigned int this_bucket = 0;

      for (file = files[i]; file != 0; file = file->next)
	{
	  register struct file *f;

	  ++this_bucket;

	  for (f = file; f != 0; f = f->prev)
	    {
	      putchar ('\n');
	      if (!f->is_target)
		puts ("# Not a target:");
	      printf ("%s:%s", f->name, f->double_colon ? ":" : "");
	      
	      for (d = f->deps; d != 0; d = d->next)
		printf (" %s", dep_name (d));
	      putchar ('\n');
	      
	      if (f->precious)
		puts ("#  Precious file (dependency of .PRECIOUS).");
	      if (f->phony)
		puts ("#  Phony target (dependency of .PHONY).");
	      if (f->cmd_target)
		puts ("#  Command-line target.");
	      if (f->dontcare)
		puts ("#  A default or MAKEFILES makefile.");
	      printf ("#  Implicit rule search has%s been done.\n",
		      f->tried_implicit ? "" : " not");
	      if (f->stem != 0)
		printf ("#  Implicit/static pattern stem: `%s'\n", f->stem);
	      if (f->intermediate)
		puts ("#  File is an intermediate dependency.");
	      if (f->also_make != 0)
		{
		  fputs ("#  Also makes:", stdout);
		  for (d = f->also_make; d != 0; d = d->next)
		    printf (" %s", dep_name (d));
		  putchar ('\n');
		}
	      if (f->last_mtime == (time_t) 0)
		puts ("#  Modification time never checked.");
	      else if (f->last_mtime == (time_t) -1)
		puts ("#  File does not exist.");
	      else
		printf ("#  Last modified %.24s (%ld)\n",
			ctime (&f->last_mtime), (long int) f->last_mtime);
	      printf ("#  File has%s been updated.\n",
		      f->updated ? "" : " not");
	      switch (f->command_state)
		{
		case cs_running:
		  puts ("#  Commands currently running (?!).");
		  break;
		case cs_deps_running:
		  puts ("#  Dependencies currently being made (?!).");
		  break;
		case cs_not_started:
		case cs_finished:
		  switch (f->update_status)
		    {
		    case -1:
		      break;
		    case 0:
		      puts ("#  Successfully updated.");
		      break;
		    case 1:
		      puts ("#  Failed to be updated.");
		      break;
		    default:
		      puts ("#  Invalid value in `update_status' member!");
		      fflush (stdout);
		      fflush (stderr);
		      abort ();
		    }
		  break;
		default:
		  puts ("#  Invalid value in `command_state' member!");
		  fflush (stdout);
		  fflush (stderr);
		  abort ();
		}

	      if (f->variables != 0)
		print_file_variables (file);

	      if (f->cmds != 0)
		print_commands (f->cmds);
	    }
	}

      nfiles += this_bucket;
      if (this_bucket > per_bucket)
	per_bucket = this_bucket;
    }

  if (nfiles == 0)
    puts ("\n# No files.");
  else
    {
      printf ("\n# %u files in %u hash buckets.\n", nfiles, FILE_BUCKETS);
#ifndef	NO_FLOAT
      printf ("# average %.1f files per bucket, max %u files in one bucket.\n",
	      ((double) FILE_BUCKETS) / ((double) nfiles) * 100.0, per_bucket);
#endif
    }
}
