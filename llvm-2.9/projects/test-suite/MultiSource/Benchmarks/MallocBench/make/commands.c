/* Command processing for GNU Make.
Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
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
#include "dep.h"
#include "commands.h"
#include "file.h"
#include "variable.h"
#include "job.h"

extern int remote_kill ();

#if	!defined(POSIX) && !defined(__GNU_LIBRARY__)
extern int getpid ();
#endif

/* Set FILE's automatic variables up.  */

static void
set_file_variables (file)
     register struct file *file;
{
  register char *p;
  char *at, *percent, *star, *less;

#define	DEFINE_VARIABLE(name, len, value) \
  (void) define_variable_for_file (name, len, value, o_automatic, 0, file)

#ifndef	NO_ARCHIVES
  /* If the target is an archive member `lib(member)',
     then $@ is `lib' and $% is `member'.  */

  if (ar_name (file->name))
    {
      p = index (file->name, '(');
      at = savestring (file->name, p - file->name);
      ++p;
      percent = savestring (p, strlen (p) - 1);
    }
  else
#endif	/* NO_ARCHIVES.  */
    {
      at = savestring (file->name, strlen (file->name));
      percent = "";
    }

  DEFINE_VARIABLE ("@", 1, at);
  DEFINE_VARIABLE ("%", 1, percent);

#define	LASTSLASH(s)	rindex ((s), '/')
#define	FILEONLY(s)	(p != 0 ? p + 1 : (s))
#define	DIRONLY(s)	(p == 0 ? "./" : p == (s) ? "/" \
			 : savestring ((s), (p - (s)) + 1))

  /* $* is the stem from an implicit or static pattern rule.  */
  if (file->stem == 0)
    {
      /* In Unix make, $* is set to the target name with
	 any suffix in the .SUFFIXES list stripped off for
	 explicit rules.  We store this in the `stem' member.  */
      register struct dep *d;
      for (d = enter_file (".SUFFIXES")->deps; d != 0; d = d->next)
	{
	  unsigned int len = strlen (file->name);
	  unsigned int slen = strlen (dep_name (d));
	  if (len > slen && streq (dep_name (d), file->name + len - slen))
	    {
	      file->stem = savestring (file->name, len - slen);
	      break;
	    }
	}
      if (d == 0)
	file->stem = "";
    }
  star = file->stem;

  DEFINE_VARIABLE ("*", 1, star);

  /* $< is the first dependency.  */
  less = file->deps != 0 ? dep_name (file->deps) : "";

  if (file->cmds == default_file->cmds)
    /* This file got its commands from .DEFAULT.
       In this case $< is the same as $@.  */
    less = at;

  DEFINE_VARIABLE ("<", 1, less);

  /* Set up the D and F versions.  */
  p = LASTSLASH (at);
  DEFINE_VARIABLE ("@D", 2, DIRONLY (at));
  DEFINE_VARIABLE ("@F", 2, FILEONLY (at));
  p = LASTSLASH (star);
  DEFINE_VARIABLE ("*D", 2, DIRONLY (star));
  DEFINE_VARIABLE ("*F", 2, FILEONLY (star));
  p = LASTSLASH (less);
  DEFINE_VARIABLE ("<D", 2, DIRONLY (less));
  DEFINE_VARIABLE ("<F", 2, FILEONLY (less));
  p = LASTSLASH (percent);
  DEFINE_VARIABLE ("%D", 2, DIRONLY (percent));
  DEFINE_VARIABLE ("%F", 2, FILEONLY (percent));

  /* Compute the values for $^ and $? and their F and D versions.  */

  {
    register unsigned int caret_len, qmark_len;
    char *caret_value, *caretD_value, *caretF_value;
    register char *cp, *cDp, *cFp;
    char *qmark_value, *qmarkD_value, *qmarkF_value;
    register char *qp, *qDp, *qFp;
    register struct dep *d;
    unsigned int len;

    caret_len = qmark_len = 0;
    for (d = file->deps; d != 0; d = d->next)
      {
	register unsigned int i = strlen (dep_name (d)) + 1;
	caret_len += i;
	if (d->changed)
	  qmark_len += i;
      }

    len = caret_len == 0 ? 1 : caret_len;
    cp = caret_value = (char *) xmalloc (len);
    cDp = caretD_value = (char *) xmalloc (len);
    cFp = caretF_value = (char *) xmalloc (len);
    len = qmark_len == 0 ? 1 : qmark_len;
    qp = qmark_value = (char *) xmalloc (len);
    qDp = qmarkD_value = (char *) xmalloc (len);
    qFp = qmarkF_value = (char *) xmalloc (len);

    for (d = file->deps; d != 0; d = d->next)
      {
	char *c, *cD, *cF;
	unsigned int Dlen, Flen;

	c = dep_name (d);
	len = strlen (c);
	bcopy (c, cp, len);
	cp += len;
	*cp++ = ' ';

	p = LASTSLASH (c);
	if (p == 0)
	  {
	    cF = c;
	    Flen = len;
	    cD = "./";
	    Dlen = 2;
	  }
	else if (p == c)
	  {
	    cD = c;
	    Dlen = 1;
	    cF = c + 1;
	    Flen = len - 1;
	  }
	else
	  {
	    cF = p + 1;
	    Flen = len - (p + 1 - c);
	    cD = c;
	    Dlen = p - c;
	  }
	bcopy (cD, cDp, Dlen);
	cDp += Dlen;
	*cDp++ = ' ';
	bcopy (cF, cFp, Flen);
	cFp += Flen;
	*cFp++ = ' ';

	if (d->changed)
	  {
	    bcopy (c, qp, len);
	    qp += len;
	    *qp++ = ' ';
	    bcopy (cD, qDp, Dlen);
	    qDp += Dlen;
	    *qDp++ = ' ';
	    bcopy (cF, qFp, Flen);
	    qFp += Flen;
	    *qFp++ = ' ';
	  }
      }

    /* Kill the last spaces and define the variables.  */

    cp[cp > caret_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("^", 1, caret_value);
    cDp[cDp > caretD_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("^D", 2, caretD_value);
    cFp[cFp > caretF_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("^F", 2, caretF_value);

    qp[qp > qmark_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("?", 1, qmark_value);
    qDp[qDp > qmarkD_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("?D", 2, qmarkD_value);
    qFp[qFp > qmarkF_value ? -1 : 0] = '\0';
    DEFINE_VARIABLE ("?F", 2, qmarkF_value);
  }

#undef	LASTSLASH
#undef	FILEONLY
#undef	DIRONLY

#undef	DEFINE_VARIABLE
}

/* Chop CMDS up into individual command lines if necessary.  */

void
chop_commands (cmds)
     register struct commands *cmds;
{
  if (cmds != 0 && cmds->command_lines == 0)
    {
      /* Chop CMDS->commands up into lines in CMDS->command_lines.
	 Also set the corresponding CMDS->lines_recurse elements,
	 and the CMDS->any_recurse flag.  */
      register char *p;
      unsigned int nlines, idx;
      char **lines;

      nlines = 5;
      lines = (char **) xmalloc (5 * sizeof (char *));
      idx = 0;
      p = cmds->commands;
      while (*p != '\0')
	{
	  char *end = p;
	find_end:;
	  end = index (end, '\n');
	  if (end == 0)
	    end = p + strlen (p);
	  else if (end > p && end[-1] == '\\')
	    {
	      int backslash = 1;
	      register char *b;
	      for (b = end - 2; b >= p && *b == '\\'; --b)
		backslash = !backslash;
	      if (backslash)
		{
		  ++end;
		  goto find_end;
		}
	    }

	  if (idx == nlines)
	    {
	      nlines += 2;
	      lines = (char **) xrealloc ((char *) lines,
					  nlines * sizeof (char *));
	    }
	  lines[idx++] = savestring (p, end - p);
	  p = end;
	  if (*p != '\0')
	    ++p;
	}

      if (idx != nlines)
	{
	  nlines = idx;
	  lines = (char **) xrealloc ((char *) lines,
				      nlines * sizeof (char *));
	}

      cmds->ncommand_lines = nlines;
      cmds->command_lines = lines;

      cmds->any_recurse = 0;
      cmds->lines_recurse = (char *) xmalloc (nlines);
      for (idx = 0; idx < nlines; ++idx)
	{
	  unsigned int len;
	  int recursive;
	  p = lines[idx];
	  len = strlen (p);
	  recursive = (sindex (p, len, "$(MAKE)", 7) != 0
		       || sindex (p, len, "${MAKE}", 7) != 0);
	  cmds->lines_recurse[idx] = recursive;
	  cmds->any_recurse |= recursive;
	}
    }
}

/* Execute the commands to remake FILE.  If they are currently executing,
   return or have already finished executing, just return.  Otherwise,
   fork off a child process to run the first command line in the sequence.  */

void
execute_file_commands (file)
     struct file *file;
{
  register char *p;

  /* Don't go through all the preparations if
     the commands are nothing but whitespace.  */

  for (p = file->cmds->commands; *p != '\0'; ++p)
    if (!isspace (*p) && *p != '-' && *p != '@')
      break;
  if (*p == '\0')
    {
      file->update_status = 0;
      notice_finished_file (file);
      return;
    }

  /* First set the automatic variables according to this file.  */

  initialize_file_variables (file);

  set_file_variables (file);

  /* Start the commands running.  */
  new_job (file);
}

#define	PROPAGATED_SIGNAL_MASK \
  (sigmask (SIGTERM) | sigmask (SIGINT) | sigmask (SIGHUP) | sigmask (SIGQUIT))

/* Handle fatal signals.  */

int
fatal_error_signal (sig)
     int sig;
{
  signal (sig, SIG_DFL);
#if !defined(USG) && !defined(__CYGWIN__)
  (void) sigsetmask (0);
#endif

  /* A termination signal won't be sent to the entire
     process group, but it means we want to kill the children.  */

  if (sig == SIGTERM)
    {
      register struct child *c;
      push_signals_blocked_p (1);
      for (c = children; c != 0; c = c->next)
	if (!c->remote)
	  (void) kill (c->pid, SIGTERM);
      pop_signals_blocked_p ();
    }

  /* If we got a signal that means the user
     wanted to kill make, remove pending targets.  */

  if (PROPAGATED_SIGNAL_MASK & sigmask (sig))
    {
      register struct child *c;
      push_signals_blocked_p (1);

      /* Remote children won't automatically get signals sent
	 to the process group, so we must send them.  */
      for (c = children; c != 0; c = c->next)
	if (c->remote)
	  (void) remote_kill (c->pid, sig);

      for (c = children; c != 0; c = c->next)
	delete_child_targets (c);

      pop_signals_blocked_p ();

      /* Clean up the children.  We don't just use the call below because
	 we don't want to print the "Waiting for children" message.  */
      wait_for_children (0, 0);
    }
  else
    /* Wait for our children to die.  */
    wait_for_children (0, 1);

  /* Delete any non-precious intermediate files that were made.  */

  remove_intermediates (1);

  if (sig == SIGQUIT)
    /* We don't want to send ourselves SIGQUIT, because it will
       cause a core dump.  Just exit instead.  */
    exit (1);

  /* Signal the same code; this time it will really be fatal.  */
  if (kill (getpid (), sig) < 0)
    /* It shouldn't return, but if it does, die anyway.  */
    pfatal_with_name ("kill");

  return 0;
}

/* Delete all non-precious targets of CHILD unless they were already deleted.
   Set the flag in CHILD to say they've been deleted.  */

void
delete_child_targets (child)
     struct child *child;
{
  struct stat st;
  struct dep *d;

  if (child->deleted)
    return;

  /* Delete the file unless it's precious.  */
  if (!child->file->precious
      && stat (child->file->name, &st) == 0
      && S_ISREG (st.st_mode)
      && (time_t) st.st_mtime != child->file->last_mtime)
    {
      error ("*** Deleting file `%s'", child->file->name);
      if (unlink (child->file->name) < 0)
	perror_with_name ("unlink: ", child->file->name);
    }

  /* Also remove any non-precious targets listed
     in the `also_make' member.  */
  for (d = child->file->also_make; d != 0; d = d->next)
    if (!d->file->precious)
      if (stat (d->file->name, &st) == 0
	  && S_ISREG (st.st_mode)
	  && (time_t) st.st_mtime != d->file->last_mtime)
	{
	  error ("*** [%s] Deleting file `%s'", child->file->name,
		 d->file->name);
	  if (unlink (d->file->name) < 0)
	    perror_with_name ("unlink: ", d->file->name);
	}

  child->deleted = 1;
}

/* Print out the commands in CMDS.  */

void
print_commands (cmds)
     register struct commands *cmds;
{
  register char *s;

  fputs ("#  commands to execute", stdout);

  if (cmds->filename == 0)
    puts (" (built-in):");
  else
    printf (" (from `%s', line %u):\n", cmds->filename, cmds->lineno);

  s = cmds->commands;
  while (*s != '\0')
    {
      char *end;

      while (isspace (*s))
	++s;

      end = index (s, '\n');
      if (end == 0)
	end = s + strlen (s);

      printf ("\t%.*s\n", end - s, s);

      s = end;
    }
}
