/* Copyright (C) 1988-1991 Free Software Foundation, Inc.
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
#include "dep.h"
#include "file.h"

#if !defined (USG) && !defined (POSIX)
#ifndef	sgi
#include <sys/file.h>
#endif
#else
#include <fcntl.h>
#endif


#if	!defined(__GNU_LIBRARY__) && !defined(POSIX)
extern int fstat ();
extern time_t time ();
#endif

extern int try_implicit_rule ();


/* Incremented when a file has been remade.  */
static unsigned int files_remade = 0;

static int update_file (), update_file_1 (), check_dep ();
static void remake_file ();
static time_t name_mtime (), library_file_mtime ();
extern time_t f_mtime ();

/* Remake all the goals in the `struct dep' chain GOALS.  Return -1 if nothing
   was done, 0 if all goals were updated successfully, or 1 if a goal failed.
   If MAKEFILES is nonzero, these goals are makefiles, so -t, -q, and -n should
   be disabled for them unless they were also command-line targets, and we
   should only make one goal at a time and return as soon as one goal whose
   `changed' member is nonzero is successfully made.  */

int
update_goal_chain (goals, makefiles)
     register struct dep *goals;
     int makefiles;
{
  int t = touch_flag, q = question_flag, n = just_print_flag;
  unsigned int j = job_slots;
  int status = -1;

#define	MTIME(file) (makefiles ? file_mtime_no_search (file) \
		     : file_mtime (file))

  /* Duplicate the chain so we can remove things from it.  */

  goals = copy_dep_chain (goals);

  if (makefiles)
    /* Only run one job at a time.  */
    job_slots = 1;

  /* Update all the goals until they are all finished.  */

  while (goals != 0)
    {
      register struct dep *g, *lastgoal;

      /* Wait for a child to die.  */

      wait_for_children (1, 0);

      lastgoal = 0;
      g = goals;
      while (g != 0)
	{
	  int x;
	  time_t mtime = MTIME (g->file);
	  check_renamed (g->file);

	  if (makefiles)
	    {
	      if (g->file->cmd_target)
		{
		  touch_flag = t;
		  question_flag = q;
		  just_print_flag = n;
		}
	      else
		touch_flag = question_flag = just_print_flag = 0;
	    }

	  x = update_file (g->file, makefiles ? 1 : 0);
	  check_renamed (g->file);
	  if (x != 0 || g->file->updated)
	    {
	      int stop = 0;
	      /* If STATUS was not already 1, set it to 1 if
		 updating failed, or to 0 if updating succeeded.
		 Leave STATUS as it is if no updating was done.  */
	      if (status < 1)
		{
		  if (g->file->update_status != 0)
		    {
		      /* Updating failed.  */
		      status = 1;
		      stop = !keep_going_flag && !makefiles;
		    }
		  else if (MTIME (g->file) != mtime)
		    {
		      /* Updating was done.
			 If this is a makefile and just_print_flag or
			 question_flag is set (meaning -n or -q was given
			 and this file was specified as a command-line target),
			 don't change STATUS.  If STATUS is changed, we will
			 get re-exec'd, and fall into an infinite loop.  */
		      if (!makefiles || (!just_print_flag && !question_flag))
			status = 0;
		      if (makefiles && g->file->dontcare)
			/* This is a default makefile.  Stop remaking.  */
			stop = 1;
		    }
		}

	      g->file = g->file->prev;
	      if (stop || g->file == 0)
		{
		  /* This goal is finished.  Remove it from the chain.  */
		  if (lastgoal == 0)
		    goals = g->next;
		  else
		    lastgoal->next = g->next;

		  /* Free the storage.  */
		  free ((char *) g);

		  g = lastgoal == 0 ? goals : lastgoal->next;
		}

	      if (stop)
		break;
	    }
	  else
	    {
	      lastgoal = g;
	      g = g->next;
	    }
	}
    }

  if (makefiles)
    {
      touch_flag = t;
      question_flag = q;
      just_print_flag = n;
      job_slots = j;
    }

  return status;
}

/* If FILE is not up to date, execute the commands for it.
   Return 0 if successful, 1 if unsuccessful;
   but with some flag settings, just call `exit' if unsuccessful.

   DEPTH is the depth in recursions of this function.
   We increment it during the consideration of our dependencies,
   then decrement it again after finding out whether this file
   is out of date.

   If there are multiple double-colon entries for FILE,
   each is considered in turn.  */

static int
update_file (file, depth)
     struct file *file;
     unsigned int depth;
{
  register int status = 0;
  register struct file *f;
  unsigned int ofiles_remade = files_remade;
  int commands_finished = 0;

  for (f = file; f != 0; f = f->prev)
    {
      register struct dep *d;
      char not_started = f->command_state == cs_not_started;

      status |= update_file_1 (f, depth);
      check_renamed (f);

      for (d = f->also_make; d != 0; d = d->next)
	{
	  d->file->command_state = f->command_state;
	  d->file->update_status = f->update_status;
	  d->file->updated = f->updated;
	}

      if (status != 0 && !keep_going_flag)
	return status;

      commands_finished |= not_started && f->command_state == cs_finished;
    }

  /* For a top level target, if we have found nothing whatever to do for it,
     print a message saying nothing needs doing.  */

  if (status == 0 && files_remade == ofiles_remade
      && commands_finished && depth == 0 && !silent_flag && !question_flag)
    {
      if (file->phony || file->cmds == 0)
	message ("Nothing to be done for `%s'.", file->name);
      else
	message ("`%s' is up to date.", file->name);
      fflush (stdout);
    }

  return status;
}

/* Consider a single `struct file' and update it as appropriate.  */

static int
update_file_1 (file, depth)
     struct file *file;
     unsigned int depth;
{
  register time_t this_mtime;
  int noexist, must_make, deps_changed;
  int dep_status = 0;
  register struct dep *d, *lastd;
  char running = 0;

  DEBUGPR ("Considering target file `%s'.\n");

  if (file->updated)
    {
      if (file->update_status > 0)
	{
	  DEBUGPR ("Recently tried and failed to update file `%s'.\n");
	  return file->update_status;
	}

      DEBUGPR ("File `%s' was considered already.\n");
      return 0;
    }

  switch (file->command_state)
    {
    case cs_not_started:
    case cs_deps_running:
      break;
    case cs_running:
      DEBUGPR ("Still updating file `%s'.\n");
      return 0;
    case cs_finished:
      DEBUGPR ("Finished updating file `%s'.\n");
      return file->update_status;
    default:
      abort ();
    }

  ++depth;

  /* Notice recursive update of the same file.  */
  file->updating = 1;

  /* Looking at the file's modtime beforehand allows the possibility
     that its name may be changed by a VPATH search, and thus it may
     not need an implicit rule.  If this were not done, the file
     might get implicit commands that apply to its initial name, only
     to have that name replaced with another found by VPATH search.  */

  this_mtime = file_mtime (file);
  check_renamed (file);
  noexist = this_mtime == (time_t) -1;
  if (noexist)
    DEBUGPR ("File `%s' does not exist.\n");

  must_make = noexist;

  /* If file was specified as a target with no commands,
     come up with some default commands.  */

  if (!file->phony && file->cmds == 0 && !file->tried_implicit)
    {
      if (try_implicit_rule (file, depth))
	DEBUGPR ("Found an implicit rule for `%s'.\n");
      else
	{
	  DEBUGPR ("No implicit rule found for `%s'.\n");
	  if (default_file != 0 && default_file->cmds != 0)
	    {
	      DEBUGPR ("Using default commands for `%s'.\n");
	      file->cmds = default_file->cmds;
	    }
	}
      file->tried_implicit = 1;
    }

  /* Update all non-intermediate files we depend on, if necessary,
     and see whether any of them is more recent than this file.  */

  lastd = 0;
  d = file->deps;
  while (d != 0)
    {
      time_t mtime;

      mtime = file_mtime (d->file);
      check_renamed (d->file);

      if (d->file->updating)
	{
	  error ("Circular %s <- %s dependency dropped.",
		 file->name, d->file->name);
	  if (lastd == 0)
	    {
	      file->deps = d->next;
	      free ((char *) d);
	      d = file->deps;
	    }
	  else
	    {
	      lastd->next = d->next;
	      free ((char *) d);
	      d = lastd->next;
	    }
	  continue;
	}

      d->file->parent = file;
      dep_status |= check_dep (d->file, depth, this_mtime, &must_make);
      check_renamed (d->file);

      {
	register struct file *f = d->file;
	do
	  {
	    running |= (f->command_state == cs_running
			|| f->command_state == cs_deps_running);
	    f = f->prev;
	  }
	while (f != 0);
      }

      if (dep_status != 0 && !keep_going_flag)
	break;

      if (!running)
	d->changed = file_mtime (d->file) != mtime;

      lastd = d;
      d = d->next;
    }

  /* Now we know whether this target needs updating.
     If it does, update all the intermediate files we depend on.  */

  if (must_make)
    {
      for (d = file->deps; d != 0; d = d->next)
	if (d->file->intermediate)
	  {
	    time_t mtime = file_mtime (d->file);
	    check_renamed (d->file);
	    d->file->parent = file;
	    dep_status |= update_file (d->file, depth);
	    check_renamed (d->file);

	    {
	      register struct file *f = d->file;
	      do
		{
		  running |= (f->command_state == cs_running
			      || f->command_state == cs_deps_running);
		  f = f->prev;
		}
	      while (f != 0);
	    }

	    if (dep_status != 0 && !keep_going_flag)
	      break;

	    if (!running)
	      d->changed = ((file->phony && file->cmds != 0)
			    || file_mtime (d->file) != mtime);
	  }
    }

  file->updating = 0;

  DEBUGPR ("Finished dependencies of target file `%s'.\n");

  if (running)
    {
      file->command_state = cs_deps_running;
      --depth;
      DEBUGPR ("The dependencies of `%s' are being made.\n");
      return 0;
    }

  /* If any dependency failed, give up now.  */

  if (dep_status != 0)
    {
      file->command_state = cs_finished;
      file->update_status = dep_status;
      file->updated = 1;

      depth--;

      DEBUGPR ("Giving up on target file `%s'.\n");

      if (depth == 0 && keep_going_flag
	  && !just_print_flag && !question_flag)
	error ("Target `%s' not remade because of errors.", file->name);

      return dep_status;
    }

  file->command_state = cs_not_started;

  /* Now record which dependencies are more
     recent than this file, so we can define $?.  */

  deps_changed = 0;
  for (d = file->deps; d != 0; d = d->next)
    {
      time_t d_mtime = file_mtime (d->file);
      check_renamed (d->file);

#if 1	/* %%% In version 4, remove this code completely to
	   implement not remaking deps if their deps are newer
	   than their parents.  */
      if (d_mtime == (time_t) -1 && !d->file->intermediate)
	/* We must remake if this dep does not
	   exist and is not intermediate.  */
	must_make = 1;
#endif

      /* Set DEPS_CHANGED if this dep actually changed.  */
      deps_changed |= d->changed;

      /* Set D->changed if either this dep actually changed,
	 or its dependent, FILE, is older or does not exist.  */
      d->changed |= noexist || d_mtime > this_mtime;

      if (debug_flag && !noexist)
	{
	  print_spaces (depth);
	  if (d_mtime == (time_t) -1)
	    printf ("Dependency `%s' does not exist.\n", dep_name (d));
	  else
	    printf ("Dependency `%s' is %s than dependent `%s'.\n",
		    dep_name (d), d->changed ? "newer" : "older", file->name);
	  fflush (stdout);
	}
    }

  /* Here depth returns to the value it had when we were called.  */
  depth--;

  if (file->double_colon && file->deps == 0)
    {
      must_make = 1;
      DEBUGPR ("Target `%s' is double-colon and has no dependencies.\n");
    }
  else if (file->is_target && !deps_changed && file->cmds == 0)
    {
      must_make = 0;
      DEBUGPR ("No commands for `%s' and no dependencies actually changed.\n");
    }

  if (!must_make)
    {
      DEBUGPR ("No need to remake target `%s'.\n");
      file->command_state = cs_finished;
      file->update_status = 0;
      file->updated = 1;
      return 0;
    }

  DEBUGPR ("Must remake target `%s'.\n");

  /* Now, take appropriate actions to remake the file.  */
  remake_file (file);

  if (file->command_state != cs_finished)
    {
      DEBUGPR ("Commands of `%s' are being run.\n");
      return 0;
    }

  switch (file->update_status)
    {
    case 1:
      DEBUGPR ("Failed to remake target file `%s'.\n");
      break;
    case 0:
      DEBUGPR ("Successfully remade target file `%s'.\n");
      break;
    case -1:
      error ("internal error: `%s' update_status is -1 at cs_finished!",
	     file->name);
      abort ();
    default:
      error ("internal error: `%s' update_status invalid!", file->name);
      abort ();
    }

  file->updated = 1;
  return file->update_status;
}

/* Set FILE's `updated' flag and re-check its mtime and the mtime's
   of all files listed in its `also_make' member.  */

void
notice_finished_file (file)
     register struct file *file;
{
  struct dep *d;

  file->command_state = cs_finished;
  file->updated = 1;

  ++files_remade;

  if (!file->phony)
    {
      if (just_print_flag || question_flag
	  || (file->is_target && file->cmds == 0))
	file->last_mtime = time ((time_t *) 0);
      else
	file->last_mtime = 0;
    }

  for (d = file->also_make; d != 0; d = d->next)
    {
      d->file->command_state = cs_finished;
      d->file->updated = 1;
      d->file->update_status = file->update_status;
      if (just_print_flag || question_flag)
	d->file->last_mtime = file->last_mtime;
      else
	d->file->last_mtime = 0;
    }
}

/* Check whether another file (whose mtime is THIS_MTIME)
   needs updating on account of a dependency which is file FILE.
   If it does, store 1 in *MUST_MAKE_PTR.
   In the process, update any non-intermediate files
   that FILE depends on (including FILE itself).
   Return nonzero if any updating failed.  */

static int
check_dep (file, depth, this_mtime, must_make_ptr)
     struct file *file;
     unsigned int depth;
     time_t this_mtime;
     int *must_make_ptr;
{
  register struct dep *d;
  int dep_status = 0;

  ++depth;
  file->updating = 1;

  if (!file->intermediate)
    /* If this is a non-intermediate file, update it and record
       whether it is newer than THIS_MTIME.  */
    {
      time_t mtime;
      dep_status = update_file (file, depth);
      check_renamed (file);
      mtime = file_mtime (file);
      check_renamed (file);
      if (mtime == (time_t) -1 || mtime > this_mtime)
	*must_make_ptr = 1;
    }
  else
    {
      /* FILE is an intermediate file.
	 Update all non-intermediate files we depend on, if necessary,
	 and see whether any of them is more recent than the file
	 on whose behalf we are checking.  */
      register struct dep *lastd;
      lastd = 0;
      d = file->deps;
      while (d != 0)
	{
	  if (d->file->updating)
	    {
	      error ("Circular %s <- %s dependency dropped.",
		     file->name, d->file->name);
	      if (lastd == 0)
		{
		  file->deps = d->next;
		  free ((char *) d);
		  d = file->deps;
		}
	      else
		{
		  lastd->next = d->next;
		  free ((char *) d);
		  d = lastd->next;
		}
	      continue;
	    }

	  d->file->parent = file;
	  dep_status |= check_dep (d->file, depth, this_mtime, must_make_ptr);
	  check_renamed (d->file);
	  if (dep_status != 0 && !keep_going_flag)
	    break;

	  lastd = d;
	  d = d->next;
	}
    }

  file->updating = 0;
  return dep_status;
}

/* Touch FILE.  Return zero if successful, nonzero if not.  */

#define TOUCH_ERROR(call) return (perror_with_name (call, file->name), 1)

static int
touch_file (file)
     register struct file *file;
{
  if (!silent_flag)
    {
      printf ("touch %s\n", file->name);
      fflush (stdout);
    }

#ifndef	NO_ARCHIVES
  if (ar_name (file->name))
    return ar_touch (file->name);
  else
#endif
    {
      int fd = open (file->name, O_RDWR | O_CREAT, 0666);

      if (fd < 0)
	TOUCH_ERROR ("touch: open: ");
      else
	{
	  struct stat statbuf;
	  char buf;

	  if (fstat (fd, &statbuf) < 0)
	    TOUCH_ERROR ("touch: fstat: ");
	  /* Rewrite character 0 same as it already is.  */
	  if (read (fd, &buf, 1) < 0)
	    TOUCH_ERROR ("touch: read: ");
	  if (lseek (fd, 0L, 0) < 0L)
	    TOUCH_ERROR ("touch: lseek: ");
	  if (write (fd, &buf, 1) < 0)
	    TOUCH_ERROR ("touch: write: ");
	  /* If file length was 0, we just
	     changed it, so change it back.  */
	  if (statbuf.st_size == 0)
	    {
	      (void) close (fd);
	      fd = open (file->name, O_RDWR | O_TRUNC, 0666);
	      if (fd < 0)
		TOUCH_ERROR ("touch: open: ");
	    }
	  (void) close (fd);
	}
    }

  return 0;
}

/* Having checked and updated the dependencies of FILE,
   do whatever is appropriate to remake FILE itself.
   Return the status from executing FILE's commands.  */

static void
remake_file (file)
     struct file *file;
{
  if (file->cmds == 0)
    {
      if (file->phony)
	/* Phony target.  Pretend it succeeded.  */
	file->update_status = 0;
      else if (file->is_target)
	/* This is a nonexistent target file we cannot make.
	   Pretend it was successfully remade.  */
	file->update_status = 0;
      else
	{
	  /* This is a dependency file we cannot remake.  Fail.  */
	  static char noway[] = "*** No way to make target";
	  if (keep_going_flag || file->dontcare)
	    {
	      if (!file->dontcare)
		error ("%s `%s'.", noway, file->name);
 	      file->update_status = 1;
	    }
	  else
	    fatal ("%s `%s'", noway, file->name);
	}
    }
  else
    {
      chop_commands (file->cmds);

      if (touch_flag && !file->cmds->any_recurse)
	{
	  if (file->phony)
	    file->update_status = 0;
	  else
	    /* Should set file's modification date and do nothing else.  */
	    file->update_status = touch_file (file);
	}
      else
	{
	  execute_file_commands (file);
	  return;
	}
    }

  notice_finished_file (file);
}

/* Return the mtime of a file, given a `struct file'.
   Caches the time in the struct file to avoid excess stat calls.  If the file
   is not found, and SEARCH is nonzero, VPATH searching and replacement is
   done.  If that fails, a library (-lLIBNAME) is tried but the library's
   actual name (/lib/libLIBNAME.a, etc.) is not substituted into FILE.  */

time_t
f_mtime (file, search)
     register struct file *file;
     int search;
{
  register time_t mtime;

  /* File's mtime is not known; must get it from the system.  */

#ifndef	NO_ARCHIVES
  if (ar_name (file->name))
    {
      /* This file is an archive-member reference.  */

      char *arname, *memname;
      struct file *arfile;
      int arname_used = 0;

      /* Find the archive's name.  */
      ar_parse_name (file->name, &arname, &memname);

      /* Find the modification time of the archive itself.
	 Also allow for its name to be changed via VPATH search.  */
      arfile = lookup_file (arname);
      if (arfile == 0)
	{
	  arfile = enter_file (arname);
	  arname_used = 1;
	}
      mtime = f_mtime (arfile, search);
      check_renamed (arfile);
      if (search && strcmp (arfile->name, arname))
	{
	  /* The archive's name has changed.
	     Change the archive-member reference accordingly.  */

	  unsigned int arlen, memlen;

	  if (!arname_used)
	    {
	      free (arname);
	      arname_used = 1;
	    }

	  arname = arfile->name;
	  arlen = strlen (arname);
	  memlen = strlen (memname);

	  free (file->name);

	  file->name = (char *) xmalloc (arlen + 1 + memlen + 2);
	  bcopy (arname, file->name, arlen);
	  file->name[arlen] = '(';
	  bcopy (memname, file->name + arlen + 1, memlen);
	  file->name[arlen + 1 + memlen] = ')';
	  file->name[arlen + 1 + memlen + 1] = '\0';
	}

      if (!arname_used)
	free (arname);
      free (memname);

      if (mtime == (time_t) -1)
	/* The archive doesn't exist, so it's members don't exist either.  */
	return (time_t) -1;

      mtime = ar_member_date (file->name);
    }
  else
#endif
    {
      mtime = name_mtime (file->name);

      if (mtime == (time_t) -1 && search)
	{
	  /* If name_mtime failed, search VPATH.  */
	  char *name = file->name;
	  if (vpath_search (&name))
	    {
	      rename_file (file, name);
	      check_renamed (file);
	      return file_mtime (file);
	    }
	  else
	    /* Last resort, is it a library (-lxxx)?  */
	    if (name[0] == '-' && name[1] == 'l')
	      mtime = library_file_mtime (&name[2]);
	}
    }

  /* Store the mtime into all the entries for this file.  */

  while (file != 0)
    {
      file->last_mtime = mtime;
      file = file->prev;
    }

  return mtime;
}


/* Return the mtime of the file or archive-member reference NAME.  */

static time_t
name_mtime (name)
     register char *name;
{
  struct stat st;

  if (stat (name, &st) < 0)
    return (time_t) -1;

  return (time_t) st.st_mtime;
}


/* Return the mtime of a library file specified as -lLIBNAME,
   searching for a suitable library file in the system library directories
   and the VPATH directories.  */

static time_t
library_file_mtime (lib)
     char *lib;
{
  time_t mtime;
  char *name;

  name = concat ("/usr/lib/lib", lib, ".a");
  mtime = name_mtime (name);
  if (mtime == (time_t) -1)
    mtime = name_mtime (name + 4);
  if (mtime == (time_t) -1)
    {
      char *local = concat ("/usr/local/lib/lib", lib,  ".a");
      mtime = name_mtime (local);
      free (local);
    }
  if (mtime == (time_t) -1)
    mtime = name_mtime (name + 9);
  if (mtime == (time_t) -1)
    {
      char *newname = name + 9;
      if (vpath_search (&newname))
	{
	  mtime = name_mtime (newname);
	  free (newname);
	}
    }

  free (name);

  return mtime;
}
