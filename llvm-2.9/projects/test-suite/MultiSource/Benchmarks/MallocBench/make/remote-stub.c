/* Copyright (C) 1988, 1989 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
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


char *remote_description = 0;


/* Return nonzero if the next job should be done remotely.  */

int
start_remote_job_p ()
{
  return 0;
}

/* Start a remote job running the command in ARGV.
   It gets standard input from STDIN_FD.  On failure,
   return nonzero.  On success, return zero, and set
   *USED_STDIN to nonzero if it will actually use STDIN_FD,
   zero if not, set *ID_PTR to a unique identification, and
   set *IS_REMOTE to zero if the job is local, nonzero if it
   is remote (meaning *ID_PTR is a process ID).  */

int
start_remote_job (argv, stdin_fd, is_remote, id_ptr, used_stdin)
     char **argv;
     int stdin_fd;
     int *is_remote;
     int *id_ptr;
     int *used_stdin;
{
  return -1;
}

/* Get the status of a dead remote child.  Block waiting for one to die
   if BLOCK is nonzero.  Set *EXIT_CODE_PTR to the exit status, *SIGNAL_PTR
   to the termination signal or zero if it exited normally, and *COREDUMP_PTR
   nonzero if it dumped core.  Return the ID of the child that died,
   0 if we would have to block and !BLOCK, or < 0 if there were none.  */

int
remote_status (exit_code_ptr, signal_ptr, coredump_ptr, block)
     int *exit_code_ptr, *signal_ptr, *coredump_ptr;
     int block;
{
  return -1;
}

/* Block asynchronous notification of remote child death.
   If this notification is done by raising the child termination
   signal, do not block that signal.  */
void
block_remote_children ()
{
  return;
}

/* Restore asynchronous notification of remote child death.
   If this is done by raising the child termination signal,
   do not unblock that signal.  */
void
unblock_remote_children ()
{
  return;
}

/* Send signal SIG to child ID.  Return 0 if successful, -1 if not.  */
int
remote_kill (id, sig)
     int id;
     int sig;
{
  return -1;
}
