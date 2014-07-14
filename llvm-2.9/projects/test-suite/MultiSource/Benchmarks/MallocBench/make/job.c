/* Job execution and handling for GNU Make.
Copyright (C) 1988-1991 Free Software Foundation, Inc.
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
#include "file.h"
#include "variable.h"
#include <errno.h>
#include <signal.h>

/* Default path to search for executables.  */
static char default_path[] = ":/bin:/usr/bin";

/* Default shell to use.  */
char default_shell[] = "/bin/sh";

#if	defined(POSIX) || defined(__GNU_LIBRARY__) || defined(__FreeBSD__) || \
    defined(__OpenBSD__) || defined(__APPLE__)
#include <limits.h>
#include <unistd.h>
#define	GET_NGROUPS_MAX	sysconf (_SC_NGROUPS_MAX)
#else	/* Not POSIX.  */
#ifndef	USG
#include <sys/param.h>
#define	NGROUPS_MAX	NGROUPS
#endif	/* Not USG.  */
#endif	/* POSIX.  */

#ifdef	POSIX
#include <sys/wait.h>

#define	WAIT_NOHANG(status)	waitpid(-1, (status), WNOHANG)

#else	/* Not POSIX.  */

#if	defined(HAVE_SYS_WAIT) || !defined(USG)
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifndef	wait3
extern int wait3 ();
#endif
#define	WAIT_NOHANG(status) \
  wait3((union wait *) (status), WNOHANG, (struct rusage *) 0)

#if	!defined (wait) && !defined (POSIX)
extern int wait ();
#endif
#endif	/* HAVE_SYS_WAIT || !USG */
#endif	/* POSIX.  */

#if	defined(WTERMSIG) || (defined(USG) && !defined(HAVE_SYS_WAIT))
#define	WAIT_T int

#ifndef	WTERMSIG
#define WTERMSIG(x) ((x) & 0x7f)
#endif
#ifndef	WCOREDUMP
#define WCOREDUMP(x) ((x) & 0x80)
#endif
#ifndef	WEXITSTATUS
#define WEXITSTATUS(x) (((x) >> 8) & 0xff)
#endif
#ifndef	WIFSIGNALED
#define WIFSIGNALED(x) (WTERMSIG (x) != 0)
#endif
#ifndef	WIFEXITED
#define WIFEXITED(x) (WTERMSIG (x) == 0)
#endif

#else	/* WTERMSIG not defined and have <sys/wait.h> or not USG.  */

#define WAIT_T union wait
#define WTERMSIG(x)	((x).w_termsig)
#define WCOREDUMP(x)	((x).w_coredump)
#define WEXITSTATUS(x)	((x).w_retcode)
#ifndef	WIFSIGNALED
#define	WIFSIGNALED(x)	(WTERMSIG(x) != 0)
#endif
#ifndef	WIFEXITED
#define	WIFEXITED(x)	(WTERMSIG(x) == 0)
#endif

#endif	/* WTERMSIG defined or USG and don't have <sys/wait.h>.  */


#if	defined(__GNU_LIBRARY__) || defined(POSIX) || defined(__CYGWIN__) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)

#include <sys/types.h>
#define	GID_T	gid_t

#else	/* Not GNU C library.  */

#define	GID_T	int

extern int dup2 ();
extern int execve ();
extern void _exit ();
extern int geteuid (), getegid ();
extern int setgid (), getgid ();
#endif	/* GNU C library.  */

#ifndef USG
extern int getdtablesize ();
#else
#include <sys/param.h>
#define getdtablesize() NOFILE
#endif

extern void wait_to_start_job ();
extern int start_remote_job_p ();
extern int start_remote_job (), remote_status ();

#if	(defined(USG) && !defined(HAVE_SIGLIST)) || defined(DGUX) || defined(__CYGWIN__)
static char *sys_siglist[NSIG];
void init_siglist ();
#else	/* Not (USG and HAVE_SIGLIST), or DGUX.  */
extern const char * const sys_siglist[];
#endif	/* USG and not HAVE_SIGLIST, or DGUX.  */

int child_handler ();
static void free_child (), start_job ();

/* Chain of all children.  */

struct child *children = 0;

/* Number of children currently running.  */

unsigned int job_slots_used = 0;

/* Nonzero if the `good' standard input is in use.  */

static int good_stdin_used = 0;

/* Write an error message describing the exit status given in
   EXIT_CODE, EXIT_SIG, and COREDUMP, for the target TARGET_NAME.
   Append "(ignored)" if IGNORED is nonzero.  */

static void
child_error (target_name, exit_code, exit_sig, coredump, ignored)
     char *target_name;
     int exit_code, exit_sig, coredump;
     int ignored;
{
  char *ignore_string = ignored ? " (ignored)" : "";

  if (exit_sig == 0)
    error ("*** [%s] Error %d%s", target_name, exit_code, ignore_string);
  else
    {
      char *coredump_string = coredump ? " (core dumped)" : "";
      if (exit_sig > 0 && exit_sig < NSIG)
	error ("*** [%s] %s%s",
	       target_name, sys_siglist[exit_sig], coredump_string);
      else
	error ("*** [%s] Signal %d%s", target_name, exit_sig, coredump_string);
    }
}

extern void block_remote_children (), unblock_remote_children ();

extern int fatal_signal_mask;

#if defined(USG) || defined(__CYGWIN__)

/* Set nonzero in the interval when it's possible that we may see a dead
   child that's not in the `children' chain.  */
static int unknown_children_possible = 0;

#endif


/* Block the child termination signal and fatal signals.  */

static void
block_signals ()
{
#if defined(USG) || defined(__CYGWIN__)

  /* Tell child_handler that it might see children that aren't yet
     in the `children' chain.  */
  unknown_children_possible = 1;

  /* Ignoring SIGCLD makes wait always return -1.
     Using the default action does the right thing.  */
  (void) SIGNAL (SIGCLD, SIG_DFL);

#else	/* Not USG.  */

  /* Block the signals.  */
  (void) sigblock (fatal_signal_mask | sigmask (SIGCHLD));

#endif

  block_remote_children ();
}

/* Unblock the child termination signal and fatal signals.  */
static void
unblock_signals ()
{
#if defined(USG) || defined(__CYGWIN__)

  (void) SIGNAL (SIGCLD, child_handler);

  /* It should no longer be possible for children not in the chain to die.  */
  unknown_children_possible = 0;

#else	/* Not USG.  */

  /* Unblock the signals.  */
  (void) sigsetmask (sigblock (0) & ~(fatal_signal_mask | sigmask (SIGCHLD)));

#endif

  unblock_remote_children ();
}

static char *signals_blocked_p_stack = 0;
static unsigned int signals_blocked_p_max;
static unsigned int signals_blocked_p_depth;

/* Make signals blocked in FLAG is nonzero, unblocked if FLAG is zero.
   Push this setting on the signals_blocked_p_stack, so it can be
   popped off by pop_signals_blocked_p.  */

void
push_signals_blocked_p (flag)
     int flag;
{
  int blocked;

  if (signals_blocked_p_stack == 0)
    {
      signals_blocked_p_max = 8;
      signals_blocked_p_stack = (char *) xmalloc (8);
      signals_blocked_p_depth = 1;
      signals_blocked_p_stack[0] = flag;

      blocked = 0;
    }
  else
    {
      if (signals_blocked_p_depth == signals_blocked_p_max)
	{
	  signals_blocked_p_max += 8;
	  signals_blocked_p_stack
	    = (char *) xrealloc(signals_blocked_p_stack,
				signals_blocked_p_max);
	}

      blocked = (signals_blocked_p_depth > 0
		 && signals_blocked_p_stack[signals_blocked_p_depth - 1]);

      signals_blocked_p_stack[++signals_blocked_p_depth - 1] = flag;
    }

  if (blocked && !flag)
    unblock_signals ();
  else if (flag && !blocked)
    block_signals ();
}

/* Pop the signals_blocked_p setting from the stack
   and block or unblock signals as appropriate.  */

void
pop_signals_blocked_p ()
{
  int blocked, block;

  blocked = (signals_blocked_p_depth > 0
	     && signals_blocked_p_stack[signals_blocked_p_depth-- - 1]);

  block = (signals_blocked_p_depth > 0
	   && signals_blocked_p_stack[signals_blocked_p_depth - 1]);

  if (block && !blocked)
    block_signals ();
  else if (blocked && !block)
    unblock_signals ();
}

extern int shell_function_pid, shell_function_completed;

/* Handle a child-termination signal (SIGCHLD, or SIGCLD for USG),
   storing the returned status and the new command state (`cs_finished')
   in the `file' member of the `struct child' for the dead child,
   and removing the child from the chain.

   If we were called as a signal handler, SIG should be SIGCHLD
   (SIGCLD for USG).  If instead it is zero, we were called explicitly
   and should block waiting for running children.
   If SIG is < 0, - SIG is the maximum number of children to bury (record
   status of and remove from the chain).  */

int
child_handler (sig)
     int sig;
{
  WAIT_T status;
  unsigned int dead_children = 0;

  if (sig > 0)
    block_signals ();

  while (1)
    {
      int remote = 0;
      register int pid;
      int exit_code, exit_sig, coredump;
      register struct child *lastc, *c;
      int child_failed;

      /* First, check for remote children.  */
      pid = remote_status (&exit_code, &exit_sig, &coredump, 0);
      if (pid < 0)
	{
	  /* No remote children.  Check for local children.  */

#ifdef	WAIT_NOHANG
	  if (sig > 0)
	    pid = WAIT_NOHANG (&status);
	  else
	    pid = wait (&status);
#else	/* USG and don't HAVE_SYS_WAIT.  */
	  /* System V cannot do non-blocking waits, so we have two
	     choices if called as a signal handler: handle only one
	     child (there may be more if the signal was blocked),
	     or block waiting for more.  The latter option makes
	     parallelism useless, so we must choose the former.  */
	  pid = wait (&status);
#endif	/* HAVE_SYS_WAIT or not USG.  */

	  if (pid <= 0)
	    /* No local children.  */
	    break;
	  else
	    {
	      /* Chop the status word up.  */
	      exit_code = WEXITSTATUS (status);
	      exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
	      coredump = WCOREDUMP (status);
	    }
	}
      else
	/* We got a remote child.  */
	remote = 1;

      /* Check if this is the child of the `shell' function.  */
      if (!remote && pid == shell_function_pid)
	{
	  /* It is.  Leave an indicator for the `shell' function.  */
	  if (exit_sig == 0 && exit_code == 127)
	    shell_function_completed = -1;
	  else
	    shell_function_completed = 1;

	  /* Check if we have reached our quota of children.  */
	  ++dead_children;
	  if (sig < 0 && dead_children == -sig)
	    break;
#if	defined(USG) && !defined(HAVE_SYS_WAIT)
	  else if (sig > 0)
	    break;
#endif
	  else
	    continue;
	}

      child_failed = exit_sig != 0 || exit_code != 0;

      /* Search for a child matching the deceased one.  */
      lastc = 0;
      for (c = children; c != 0; lastc = c, c = c->next)
	if (c->remote == remote && c->pid == pid)
	  break;

      if (c == 0)
	{
	  /* An unknown child died.  */
#ifdef	USG
	  if (!unknown_children_possible)
	    {
#endif
	      char buf[100];
	      sprintf (buf, "Unknown%s job %d", remote ? " remote" : "", pid);
	      if (child_failed)
		child_error (buf, exit_code, exit_sig, coredump,
			     ignore_errors_flag);
	      else
		error ("%s finished.", buf);
#ifdef	USG
	    }
#endif
	}
      else
	{
	  /* If this child had the good stdin, say it is now free.  */
	  if (c->good_stdin)
	    good_stdin_used = 0;

	  if (child_failed && !c->noerror && !ignore_errors_flag)
	    {
	      /* The commands failed.  Write an error message,
		 delete non-precious targets, and abort.  */
	      child_error (c->file->name, exit_code, exit_sig, coredump, 0);
	      c->file->update_status = 1;
	      if (exit_sig != 0)
		delete_child_targets (c);
	    }
	  else
	    {
	      if (child_failed)
		{
		  /* The commands failed, but we don't care.  */
		  child_error (c->file->name,
			       exit_code, exit_sig, coredump, 1);
		  child_failed = 0;
		}

	      /* If there are more commands to run, try to start them.  */
	      start_job (c);

	      switch (c->file->command_state)
		{
		case cs_running:
		  /* Successfully started.  Loop to reap more children.  */
		  continue;

		case cs_finished:
		  if (c->file->update_status != 0)
		    {
		      /* We failed to start the commands.  */
		      delete_child_targets (c);
		    }
		  break;

		default:
		  error ("internal error: `%s' command_state \
%d in child_handler", c->file->name);
		  abort ();
		  break;
		}
	    }

	  /* Set the state flag to say the commands have finished.  */
	  notice_finished_file (c->file);

	  /* Remove the child from the chain and free it.  */
	  if (lastc == 0)
	    children = c->next;
	  else
	    lastc->next = c->next;
	  free_child (c);

	  /* There is now another slot open.  */
	  --job_slots_used;

	  /* If the job failed, and the -k flag was not given, die.  */
	  if (child_failed && !keep_going_flag)
	    die (1);

	  /* See if we have reached our quota for blocking.  */
	  ++dead_children;
	  if (sig < 0 && dead_children == -sig)
	    break;
#if	defined(USG) && !defined(HAVE_SYS_WAIT)
	  else if (sig > 0)
	    break;
#endif
	}
    }

#ifdef	USG
  if (sig > 0)
    (void) SIGNAL (sig, child_handler);
#endif

  if (sig > 0)
    unblock_signals ();

  return 0;
}


/* Wait for N children, blocking if necessary.
   If N is zero, wait until we run out of children.
   If ERR is nonzero and we have any children to wait for,
   print a message on stderr.  */

void
wait_for_children (n, err)
     unsigned int n;
     int err;
{
  push_signals_blocked_p (1);

  if (err && (children != 0 || shell_function_pid != 0))
    {
      fflush (stdout);
      error ("*** Waiting for unfinished jobs....");
    }

  /* Call child_handler to do the work.  */
  (void) child_handler (- (int) n);

  pop_signals_blocked_p ();
}

/* Free the storage allocated for CHILD.  */

static void
free_child (child)
     register struct child *child;
{
  if (child->command_lines != 0)
    {
      register unsigned int i;
      for (i = 0; i < child->file->cmds->ncommand_lines; ++i)
	free (child->command_lines[i]);
      free ((char *) child->command_lines);
    }

  if (child->environment != 0)
    {
      register char **ep = child->environment;
      while (*ep != 0)
	free (*ep++);
      free ((char *) child->environment);
    }

  free ((char *) child);
}

/* Start a job to run the commands specified in CHILD.
   CHILD is updated to reflect the commands and ID of the child process.  */

static void
start_job (child)
     register struct child *child;
{
  static int bad_stdin = -1;
  register char *p;
  char noprint = 0, recursive;
  char **argv;

  if (child->command_ptr == 0 || *child->command_ptr == '\0')
    {
      /* There are no more lines in the expansion of this line.  */
      if (child->command_line == child->file->cmds->ncommand_lines)
	{
	  /* There are no more lines to be expanded.  */
	  child->command_ptr = 0;
	  child->file->command_state = cs_finished;
	  child->file->update_status = 0;
	  return;
	}
      else
	{
	  /* Get the next line to run, and set RECURSIVE
	     if the unexpanded line contains $(MAKE).  */
	  child->command_ptr = child->command_lines[child->command_line];
	  recursive = child->file->cmds->lines_recurse[child->command_line];
	  ++child->command_line;
	}
    }
  else
    /* Still executing the last line we started.  */
    recursive = child->file->cmds->lines_recurse[child->command_line - 1];

  /* Find the end of this line.  Backslash-newlines don't mean the end.  */

  p = child->command_ptr;
  child->noerror = 0;
  while (*p != '\0')
    {
      if (*p == '@')
	noprint = 1;
      else if (*p == '-')
	child->noerror = 1;
      else if (*p == '+')
	recursive = 1;
      else if (!isblank (*p))
	break;
      ++p;
    }

  /* If -q was given, just say that updating `failed'.  */
  if (question_flag && !recursive)
    goto error;

  /* There may be some preceding whitespace left if there
     was nothing but a backslash on the first line.  */
  p = next_token (p);

  /* Figure out an argument list from this command line.  */

  {
    char *end;
    argv = construct_command_argv (p, &end, child->file);
    if (end == NULL)
      child->command_ptr = NULL;
    else
      {
	*end++ = '\0';
	child->command_ptr = end;
      }
  }

  /* Print out the command.  */

  if (just_print_flag || (!noprint && !silent_flag))
    puts (p);

  if (argv == 0)
    {
      /* This line has no commands.  Go to the next.  */
      start_job (child);
      return;
    }

  /* If -n was given, recurse to get the next line in the sequence.  */

  if (just_print_flag && !recursive)
    {
      free (argv[0]);
      free ((char *) argv);
      start_job (child);
      return;
    }

  /* Flush the output streams so they won't have things written twice.  */

  fflush (stdout);
  fflush (stderr);
  
  /* Set up a bad standard input that reads from a broken pipe.  */

  if (bad_stdin == -1)
    {
      /* Make a file descriptor that is the read end of a broken pipe.
	 This will be used for some children's standard inputs.  */
      int pd[2];
      if (pipe (pd) == 0)
	{
	  /* Close the write side.  */
	  (void) close (pd[1]);
	  /* Save the read side.  */
	  bad_stdin = pd[0];
	}
    }

  /* Decide whether to give this child the `good' standard input
     (one that points to the terminal or whatever), or the `bad' one
     that points to the read side of a broken pipe.  */

  child->good_stdin = !good_stdin_used;
  if (child->good_stdin)
    good_stdin_used = 1;

  child->deleted = 0;

  /* Set up the environment for the child.  */
  if (child->environment == 0)
    child->environment = target_environment (child->file);

  if (start_remote_job_p ())
    {
      int is_remote, id, used_stdin;
      if (start_remote_job (argv, child->good_stdin ? 0 : bad_stdin,
			    &is_remote, &id, &used_stdin))
	goto error;
      else
	{
	  if (child->good_stdin && !used_stdin)
	    {
	      child->good_stdin = 0;
	      good_stdin_used = 0;
	    }
	  child->remote = is_remote;
	  child->pid = id;
	}
    }
  else
    {
      if (child->command_line - 1 == 0)
	{
	  /* Wait for the load to be low enough if this
	     is the first command in the sequence.  */
	  make_access ();
	  wait_to_start_job ();
	  user_access ();
	}

      /* Fork the child process.  */

      child->remote = 0;
      child->pid = vfork ();
      if (child->pid == 0)
	/* We are the child side.  */
	child_execute_job (child->good_stdin ? 0 : bad_stdin, 1,
			   argv, child->environment);
      else if (child->pid < 0)
	{
	  /* Fork failed!  */
	  perror_with_name (VFORK_NAME, "");
	  goto error;
	}
    }

  /* We are the parent side.  Set the state to
     say the commands are running and return.  */

  child->file->command_state = cs_running;

  /* Free the storage used by the child's argument list.  */

  free (argv[0]);
  free ((char *) argv);

  return;

 error:;
  child->file->update_status = 1;
  child->file->command_state = cs_finished;
}


/* Create a `struct child' for FILE and start its commands running.  */

void
new_job (file)
     register struct file *file;
{
  register struct commands *cmds = file->cmds;
  register struct child *c;
  char **lines;
  register unsigned int i;

  /* Chop the commands up into lines if they aren't already.  */
  chop_commands (cmds);

  if (job_slots > 0)
    /* Wait for a job slot to be freed up.  */
    while (job_slots_used == job_slots)
      wait_for_children (1, 0);

  /* Expand the command lines and store the results in LINES.  */
  lines = (char **) xmalloc (cmds->ncommand_lines * sizeof (char *));
  for (i = 0; i < cmds->ncommand_lines; ++i)
    lines[i] = allocated_variable_expand_for_file (cmds->command_lines[i],
						   file);

  /* Start the command sequence, record it in a new
     `struct child', and add that to the chain.  */

  push_signals_blocked_p (1);

  c = (struct child *) xmalloc (sizeof (struct child));
  c->file = file;
  c->command_lines = lines;
  c->command_line = 0;
  c->command_ptr = 0;
  c->environment = 0;
  start_job (c);
  switch (file->command_state)
    {
    case cs_running:
      c->next = children;
      children = c;
      /* One more job slot is in use.  */
      ++job_slots_used;
      break;

    case cs_finished:
      free_child (c);
      notice_finished_file (file);
      break;

    default:
      error ("internal error: `%s' command_state == %d in new_job",
	     file->name, (int) file->command_state);
      abort ();
      break;
    }

  pop_signals_blocked_p ();

  if (job_slots == 1 && file->command_state == cs_running)
    {
      /* Since there is only one job slot, make things run linearly.
	 Wait for the child to finish, setting the state to `cs_finished'.  */
      while (file->command_state != cs_finished)
	wait_for_children (1, 0);
    }
}

/* Replace the current process with one executing the command in ARGV.
   STDIN_FD and STDOUT_FD are used as the process's stdin and stdout; ENVP is
   the environment of the new program.  This function does not return.  */

void
child_execute_job (stdin_fd, stdout_fd, argv, envp)
     int stdin_fd, stdout_fd;
     char **argv, **envp;
{
  if (stdin_fd != 0)
    (void) dup2 (stdin_fd, 0);
  if (stdout_fd != 1)
    (void) dup2 (stdout_fd, 1);

  /* Free up file descriptors.  */
  {
    register int d;
    int max = getdtablesize ();
    for (d = 3; d < max; ++d)
      (void) close (d);
  }

  /* Don't block signals for the new process.  */
  unblock_signals ();

  /* Run the command.  */
  exec_command (argv, envp);
}

/* Search PATH for FILE.
   If successful, store the full pathname in PROGRAM and return 1.
   If not sucessful, return zero.  */

static int
search_path (file, path, program)
     char *file, *path, *program;
{
  if (path == 0 || path[0] == '\0')
    path = default_path;

  if (index (file, '/') != 0)
    {
      strcpy (program, file);
      return 1;
    }
  else
    {
      unsigned int len;

#if	!defined (USG) || defined (POSIX)
#ifndef	POSIX
      extern int getgroups ();
#endif
      static int ngroups = -1;
#ifdef	NGROUPS_MAX
      static GID_T groups[NGROUPS_MAX];
#define	ngroups_max	NGROUPS_MAX
#else
      static GID_T *groups = 0;
      static int ngroups_max;
      if (groups == 0)
	{
	  ngroups_max = GET_NGROUPS_MAX;
	  groups = (GID_T *) malloc (ngroups_max * sizeof (GID_T));
	}
#endif
      if (groups != 0 && ngroups == -1)
	ngroups = getgroups (ngroups_max, groups);
#endif	/* POSIX or not USG.  */

      len = strlen (file) + 1;
      do
	{
	  struct stat st;
	  int perm;
	  char *p;

	  p = index (path, ':');
	  if (p == 0)
	    p = path + strlen (path);

	  if (p == path)
	    bcopy (file, program, len);
	  else
	    {
	      bcopy (path, program, p - path);
	      program[p - path] = '/';
	      bcopy (file, program + (p - path) + 1, len);
	    }

	  if (stat (program, &st) == 0
	      && S_ISREG (st.st_mode))
	    {
	      if (st.st_uid == geteuid ())
		perm = (st.st_mode & 0100);
	      else if (st.st_gid == getegid ())
		perm = (st.st_mode & 0010);
	      else
		{
#ifndef	USG
		  register int i;
		  for (i = 0; i < ngroups; ++i)
		    if (groups[i] == st.st_gid)
		      break;
		  if (i < ngroups)
		    perm = (st.st_mode & 0010);
		  else
#endif	/* Not USG.  */
		    perm = (st.st_mode & 0001);
		}

	      if (perm != 0)
		return 1;
	    }

	  path = p + 1;
	} while (*path != '\0');
    }

  return 0;
}

/* Replace the current process with one running the command in ARGV,
   with environment ENVP.  This function does not return.  */

void
exec_command (argv, envp)
     char **argv, **envp;
{
  char *shell, *path;
  PATH_VAR (program);
  register char **ep;

  shell = path = 0;
  for (ep = envp; *ep != 0; ++ep)
    {
      if (shell == 0 && !strncmp(*ep, "SHELL=", 6))
	shell = &(*ep)[6];
      else if (path == 0 && !strncmp(*ep, "PATH=", 5))
	path = &(*ep)[5];
      else if (path != 0 && shell != 0)
	break;
    }

  /* Be the user, permanently.  */
  child_access ();

  if (!search_path (argv[0], path, program))
    error ("%s: Command not found", argv[0]);
  else
    {
      /* Run the program.  */
      execve (program, argv, envp);

      if (errno == ENOEXEC)
	{
	  PATH_VAR (shell_program);
	  char *shell_path;
	  if (shell == 0)
	    shell_path = default_shell;
	  else
	    {
	      if (search_path (shell, path, shell_program))
		shell_path = shell_program;
	      else
		{
		  shell_path = 0;
		  error ("%s: Shell program not found", shell);
		}
	    }

	  if (shell_path != 0)
	    {
	      char **new_argv;
	      int argc;

	      argc = 1;
	      while (argv[argc] != 0)
		++argc;

	      new_argv = (char **) alloca ((1 + argc + 1) * sizeof (char *));
	      new_argv[0] = shell_path;
	      new_argv[1] = program;
	      while (argc > 0)
		{
		  new_argv[1 + argc] = argv[argc];
		  --argc;
		}

	      execve (shell_path, new_argv, envp);
	      perror_with_name ("execve: ", shell_path);
	    }
	}
      else
	perror_with_name ("execve: ", program);
    }

  _exit (127);
}

/* Figure out the argument list necessary to run LINE as a command.
   Try to avoid using a shell.  This routine handles only ' quoting.
   Starting quotes may be escaped with a backslash.  If any of the
   characters in sh_chars[] is seen, or any of the builtin commands
   listed in sh_cmds[] is the first word of a line, the shell is used.

   If RESTP is not NULL, *RESTP is set to point to the first newline in LINE.
   If *RESTP is NULL, newlines will be ignored.

   SHELL is the shell to use, or nil to use the default shell.
   IFS is the value of $IFS, or nil (meaning the default).  */

static char **
construct_command_argv_internal (line, restp, shell, ifs)
     char *line, **restp;
     char *shell, *ifs;
{
  static char sh_chars[] = "#;\"*?[]&|<>(){}=$`";
  static char *sh_cmds[] = { "cd", "eval", "exec", "exit", "login",
			     "logout", "set", "umask", "wait", "while", "for",
			     "case", "if", ":", ".", "break", "continue",
			     "export", "read", "readonly", "shift", "times",
			     "trap", "switch", 0 };
  register int i;
  register char *p;
  register char *ap;
  char *end;
  int instring;
  char **new_argv = 0;

  /* See if it is safe to parse commands internally.  */
  if (shell != 0 && strcmp (shell, default_shell))
    goto slow;

  if (ifs != 0)
    for (ap = ifs; *ap != '\0'; ++ap)
      if (*ap != ' ' && *ap != '\t' && *ap != '\n')
	goto slow;

  i = strlen (line) + 1;

  /* More than 1 arg per character is impossible.  */
  new_argv = (char **) xmalloc (i * sizeof (char *));

  /* All the args can fit in a buffer as big as LINE is.   */
  ap = new_argv[0] = (char *) xmalloc (i);
  end = ap + i;

  if (restp != NULL)
    *restp = NULL;

  /* I is how many complete arguments have been found.  */
  i = 0;
  instring = 0;
  for (p = line; *p != '\0'; ++p)
    {
      if (ap > end)
	abort ();

      if (instring)
	{
	  /* Inside a string, just copy any char except a closing quote.  */
	  if (*p == '\'')
	    instring = 0;
	  else
	    *ap++ = *p;
	}
      else if (index (sh_chars, *p) != 0)
	/* Not inside a string, but it's a special char.  */
	goto slow;
      else
	/* Not a special char.  */
	switch (*p)
	  {
	  case '\\':
	    /* Backslash-newline combinations are eaten.  */
	    if (p[1] != '\0' && p[1] != '\n')
	      /* Copy and skip the following char.  */
	      *ap++ = *++p;
	    break;

	  case '\'':
	    instring = 1;
	    break;

	  case '\n':
	    if (restp != NULL)
	      {
		/* End of the command line.  */
		*restp = p;
		goto end_of_line;
	      }
	    else
	      /* Newlines are not special.  */
	      *ap++ = '\n';
	    break;

	  case ' ':
	  case '\t':
	    /* We have the end of an argument.
	       Terminate the text of the argument.  */
	    *ap++ = '\0';
	    new_argv[++i] = ap;
	    /* If this argument is the command name,
	       see if it is a built-in shell command.
	       If so, have the shell handle it.  */
	    if (i == 1)
	      {
		register int j;
		for (j = 0; sh_cmds[j] != 0; ++j)
		  if (streq (sh_cmds[j], new_argv[0]))
		    goto slow;
	      }

	    /* Ignore multiple whitespace chars.  */
	    p = next_token (p);
	    /* Next iteration should examine the first nonwhite char.  */
	    --p;
	    break;

	  default:
	    *ap++ = *p;
	    break;
	  }
    }
 end_of_line:

  if (instring)
    /* Let the shell deal with an unterminated quote.  */
    goto slow;

  /* Terminate the last argument and the argument list.  */

  *ap = '\0';
  if (new_argv[i][0] != '\0')
    ++i;
  new_argv[i] = 0;

  if (new_argv[0] == 0)
    /* Line was empty.  */
    return 0;
  else
    return new_argv;

 slow:;
  /* We must use the shell.  */

  if (new_argv != 0)
    {
      /* Free the old argument list we were working on.  */
      free (new_argv[0]);
      free (new_argv);
    }

  if (shell == 0 || !strcmp (shell, default_shell))
    {
      /* The shell is the default, or we're in a recursive call to construct
	 the argument list for the real shell.  Construct a simple argument
	 list using the default shell.  */
      new_argv = (char **) xmalloc (4 * sizeof (char *));
      new_argv[0] = savestring (default_shell, sizeof (default_shell) - 1);
      new_argv[1] = "-c";
      new_argv[2] = savestring (line, strlen (line));
      new_argv[3] = 0;
    }
  else
    {
      /* SHELL may be a multi-word command.  Construct a command line
	 "SHELL -c LINE", with all special chars in LINE escaped.
	 Then recurse, expanding this command line to get the final
	 argument list.  */

      unsigned int shell_len = strlen (shell);
      static char minus_c[] = " -c ";
      unsigned int line_len = strlen (line);
      
      char *new_line = (char *) alloca (shell_len + (sizeof (minus_c) - 1)
					+ (line_len * 2) + 1);

      ap = new_line;
      bcopy (shell, ap, shell_len);
      ap += shell_len;
      bcopy (minus_c, ap, sizeof (minus_c) - 1);
      ap += sizeof (minus_c) - 1;
      for (p = line; *p != '\0'; ++p)
	{
	  if (*p == '\\' || *p == '\''
	      || isspace (*p)
	      || index (sh_chars, *p) != 0)
	    *ap++ = '\\';
	  *ap++ = *p;
	}
      *ap = '\0';

      new_argv = construct_command_argv_internal (new_line, restp,
						  (char *) 0, (char *) 0);
    }

  return new_argv;
}

/* Figure out the argument list necessary to run LINE as a command.
   Try to avoid using a shell.  This routine handles only ' quoting.
   Starting quotes may be escaped with a backslash.  If any of the
   characters in sh_chars[] is seen, or any of the builtin commands
   listed in sh_cmds[] is the first word of a line, the shell is used.

   If RESTP is not NULL, *RESTP is set to point to the first newline in LINE.
   If *RESTP is NULL, newlines will be ignored.

   FILE is the target whose commands these are.  It is used for
   variable expansion for $(SHELL) and $(IFS).  */

char **
construct_command_argv (line, restp, file)
     char *line, **restp;
     struct file *file;
{
  char *shell = allocated_variable_expand_for_file ("$(SHELL)", file);
  char *ifs = allocated_variable_expand_for_file ("$(IFS)", file);
  char **argv;

  argv = construct_command_argv_internal (line, restp, shell, ifs);

  free (shell);
  free (ifs);

  return argv;
}

#if	(defined(USG) && !defined(HAVE_SIGLIST)) || defined(DGUX) || defined(__CYGWIN__)
/* Initialize sys_siglist.  */

void
init_siglist ()
{
  char buf[100];
  register unsigned int i;

  for (i = 0; i < NSIG; ++i)
    switch (i)
      {
      default:
	sprintf (buf, "Signal %u", i);
	sys_siglist[i] = savestring (buf, strlen (buf));
	break;
      case SIGHUP:
	sys_siglist[i] = "Hangup";
	break;
      case SIGINT:
	sys_siglist[i] = "Interrupt";
	break;
      case SIGQUIT:
	sys_siglist[i] = "Quit";
	break;
      case SIGILL:
	sys_siglist[i] = "Illegal Instruction";
	break;
      case SIGTRAP:
	sys_siglist[i] = "Trace Trap";
	break;
#ifdef SIGIOT
      case SIGIOT:
	sys_siglist[i] = "IOT Trap";
	break;
#endif
#ifdef	SIGEMT
      case SIGEMT:
	sys_siglist[i] = "EMT Trap";
	break;
#endif
#ifdef	SIGDANGER
      case SIGDANGER:
	sys_siglist[i] = "Danger signal";
	break;
#endif
      case SIGFPE:
	sys_siglist[i] = "Floating Point Exception";
	break;
      case SIGKILL:
	sys_siglist[i] = "Killed";
	break;
      case SIGBUS:
	sys_siglist[i] = "Bus Error";
	break;
      case SIGSEGV:
	sys_siglist[i] = "Segmentation fault";
	break;
      case SIGSYS:
	sys_siglist[i] = "Bad Argument to System Call";
	break;
      case SIGPIPE:
	sys_siglist[i] = "Broken Pipe";
	break;
      case SIGALRM:
	sys_siglist[i] = "Alarm Clock";
	break;
      case SIGTERM:
	sys_siglist[i] = "Terminated";
	break;
#if	!defined (SIGIO) || SIGUSR1 != SIGIO
      case SIGUSR1:
	sys_siglist[i] = "User-defined signal 1";
	break;
#endif
#if	!defined (SIGURG) || SIGUSR2 != SIGURG
      case SIGUSR2:
	sys_siglist[i] = "User-defined signal 2";
	break;
#endif
#ifdef	SIGCLD
      case SIGCLD:
#endif
#if	defined(SIGCHLD) && !defined(SIGCLD)
      case SIGCHLD:
#endif
	sys_siglist[i] = "Child Process Exited";
	break;
#ifdef	SIGPWR
      case SIGPWR:
	sys_siglist[i] = "Power Failure";
	break;
#endif
#ifdef	SIGVTALRM
      case SIGVTALRM:
	sys_siglist[i] = "Virtual Timer Alarm";
	break;
#endif
#ifdef	SIGPROF
      case SIGPROF:
	sys_siglist[i] = "Profiling Alarm Clock";
	break;
#endif
#ifdef	SIGIO
      case SIGIO:
	sys_siglist[i] = "I/O Possible";
	break;
#endif
#ifdef	SIGWINDOW
      case SIGWINDOW:
	sys_siglist[i] = "Window System Signal";
	break;
#endif
#ifdef	SIGSTOP
      case SIGSTOP:
	sys_siglist[i] = "Stopped (signal)";
	break;
#endif
#ifdef	SIGTSTP
      case SIGTSTP:
	sys_siglist[i] = "Stopped";
	break;
#endif
#ifdef	SIGCONT
      case SIGCONT:
	sys_siglist[i] = "Continued";
	break;
#endif
#ifdef	SIGTTIN
      case SIGTTIN:
	sys_siglist[i] = "Stopped (tty input)";
	break;
#endif
#ifdef	SIGTTOU
      case SIGTTOU:
	sys_siglist[i] = "Stopped (tty output)";
	break;
#endif
#ifdef	SIGURG
      case SIGURG:
	sys_siglist[i] = "Urgent Condition on Socket";
	break;
#endif
#ifdef	SIGXCPU
      case SIGXCPU:
	sys_siglist[i] = "CPU Limit Exceeded";
	break;
#endif
#ifdef	SIGXFSZ
      case SIGXFSZ:
	sys_siglist[i] = "File Size Limit Exceeded";
	break;
#endif
      }
}
#endif	/* USG and not HAVE_SIGLIST.  */

#if	defined(USG) && !defined(USGr3) && !defined(HAVE_DUP2)
int
dup2 (old, new)
     int old, new;
{
  int fd;

  (void) close (new);
  fd = dup (old);
  if (fd != new)
    {
      (void) close (fd);
      errno = EMFILE;
      return -1;
    }

  return fd;
}
#endif	/* USG and not USGr3 and not HAVE_DUP2.  */
