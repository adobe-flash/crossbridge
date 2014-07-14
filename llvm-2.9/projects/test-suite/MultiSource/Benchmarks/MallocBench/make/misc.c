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
#include "dep.h"
#include <errno.h>


/* Compare strings *S1 and *S2.
   Return negative if the first is less, positive if it is greater,
   zero if they are equal.  */

int
alpha_compare (s1, s2)
     char **s1, **s2;
{
  if (**s1 != **s2)
    return **s1 - **s2;
  return strcmp (*s1, *s2);
}

/* Discard each backslash-newline combination from LINE.
   Backslash-backslash-newline combinations become backslash-newlines.
   This is done by copying the text at LINE into itself.  */

void
collapse_continuations (line)
     char *line;
{
  register char *in, *out, *p;
  register int backslash;
  register unsigned int bs_write;

  in = index (line, '\n');
  if (in == 0)
    return;

  out = in;
  if (out > line)
    while (out[-1] == '\\')
      --out;

  while (*in != '\0')
    {
      /* BS_WRITE gets the number of quoted backslashes at
	 the end just before IN, and BACKSLASH gets nonzero
	 if the next character is quoted.  */
      backslash = 0;
      bs_write = 0;
      for (p = in - 1; p >= line && *p == '\\'; --p)
	{
	  if (backslash)
	    ++bs_write;
	  backslash = !backslash;

	  /* It should be impossible to go back this far without exiting,
	     but if we do, we can't get the right answer.  */
	  if (in == out - 1)
	    abort ();
	}

      /* Output the appropriate number of backslashes.  */
      while (bs_write-- > 0)
	*out++ = '\\';

      /* Skip the newline.  */
      ++in;

      /* If the newline is quoted, discard following whitespace
	 and any preceding whitespace; leave just one space.  */
      if (backslash)
	{
	  in = next_token (in);
	  while (out > line && isblank (out[-1]))
	    --out;
	  *out++ = ' ';
	}
      else
	/* If the newline isn't quoted, put it in the output.  */
	*out++ = '\n';

      /* Now copy the following line to the output.
	 Stop when we find backslashes followed by a newline.  */
      while (*in != '\0')
	if (*in == '\\')
	  {
	    p = in + 1;
	    while (*p == '\\')
	      ++p;
	    if (*p == '\n')
	      {
		in = p;
		break;
	      }
	    while (in < p)
	      *out++ = *in++;
	  }
	else
	  *out++ = *in++;
    }

  *out = '\0';
}


/* Remove comments from LINE.
   This is done by copying the text at LINE onto itself.  */

void
remove_comments (line)
     char *line;
{
  register char *p, *p2;
  register int backslash;
  register unsigned int bs_write;

  while (1)
    {
      p = index (line, '#');
      if (p == 0)
	break;

      backslash = 0;
      bs_write = 0;
      for (p2 = p - 1; p2 > line && *p2 == '\\'; --p2)
	{
	  if (backslash)
	    ++bs_write;
	  backslash = !backslash;
	}

      if (!backslash)
	{
	  /* Cut off the line at the #.  */
	  *p = '\0';
	  break;
	}

      /* strcpy better copy left to right.  */
      line = p;
      strcpy (p2 + 1 + bs_write, line);
    }
}

/* Print N spaces (used by DEBUGPR for target-depth).  */

void
print_spaces (n)
     register unsigned int n;
{
  while (n-- > 0)
    putchar (' ');
}


/* Return a newly-allocated string whose contents
   concatenate those of s1, s2, s3.  */

char *
concat (s1, s2, s3)
     register char *s1, *s2, *s3;
{
  register unsigned int len1, len2, len3;
  register char *result;

  len1 = *s1 != '\0' ? strlen (s1) : 0;
  len2 = *s2 != '\0' ? strlen (s2) : 0;
  len3 = *s3 != '\0' ? strlen (s3) : 0;

  result = (char *) xmalloc (len1 + len2 + len3 + 1);

  if (*s1 != '\0')
    bcopy (s1, result, len1);
  if (*s2 != '\0')
    bcopy (s2, result + len1, len2);
  if (*s3 != '\0')
    bcopy (s3, result + len1 + len2, len3);
  *(result + len1 + len2 + len3) = '\0';

  return result;
}

/* Print a message on stdout.  */

void
message (s1, s2, s3, s4, s5, s6)
     char *s1, *s2, *s3, *s4, *s5, *s6;
{
  if (makelevel == 0)
    printf ("%s: ", program);
  else
    printf ("%s[%u]: ", program, makelevel);
  printf (s1, s2, s3, s4, s5, s6);
  putchar ('\n');
  fflush (stdout);
}

/* Print an error message and exit.  */

/* VARARGS1 */
void
fatal (s1, s2, s3, s4, s5, s6)
     char *s1, *s2, *s3, *s4, *s5, *s6;
{
  if (makelevel == 0)
    fprintf (stderr, "%s: ", program);
  else
    fprintf (stderr, "%s[%u]: ", program, makelevel);
  fprintf (stderr, s1, s2, s3, s4, s5, s6);
  fputs (".  Stop.\n", stderr);

  die (1);
}

/* Print error message.  `s1' is printf control string, `s2' is arg for it. */

/* VARARGS1 */

void
error (s1, s2, s3, s4, s5, s6)
     char *s1, *s2, *s3, *s4, *s5, *s6;
{
  if (makelevel == 0)
    fprintf (stderr, "%s: ", program);
  else
    fprintf (stderr, "%s[%u]: ", program, makelevel);
  fprintf (stderr, s1, s2, s3, s4, s5, s6);
  putc ('\n', stderr);
  fflush (stderr);
}

void
makefile_error (file, lineno, s1, s2, s3, s4, s5, s6)
     char *file;
     unsigned int lineno;
     char *s1, *s2, *s3, *s4, *s5, *s6;
{
  fprintf (stderr, "%s:%u: ", file, lineno);
  fprintf (stderr, s1, s2, s3, s4, s5, s6);
  putc ('\n', stderr);
  fflush (stderr);
}

void
makefile_fatal (file, lineno, s1, s2, s3, s4, s5, s6)
     char *file;
     unsigned int lineno;
     char *s1, *s2, *s3, *s4, *s5, *s6;
{
  fprintf (stderr, "%s:%u: ", file, lineno);
  fprintf (stderr, s1, s2, s3, s4, s5, s6);
  fputs (".  Stop.\n", stderr);

  die (1);
}

/* Print an error message from errno.  */

void
perror_with_name (str, name)
     char *str, *name;
{
  if (errno < sys_nerr)
    error ("%s%s: %s", str, name, sys_errlist[errno]);
  else
    error ("%s%s: Unknown error %d", str, name, errno);
}

/* Print an error message from errno and exit.  */

void
pfatal_with_name (name)
     char *name;
{
  if (errno < sys_nerr)
    fatal ("%s: %s", name, sys_errlist[errno]);
  else
    fatal ("%s: Unknown error %d", name, errno);

  /* NOTREACHED */
}

/* Like malloc but get fatal error if memory is exhausted.  */

#undef xmalloc
#undef xrealloc

char *
xmalloc (size)
     unsigned int size;
{
  char *result = malloc (size);
  if (result == 0)
    fatal ("virtual memory exhausted");
  return result;
}


char *
xrealloc (ptr, size)
     char *ptr;
     unsigned int size;
{
  char *result = realloc (ptr, size);
  if (result == 0)
    fatal ("virtual memory exhausted");
  return result;
}

char *
savestring (str, length)
     char *str;
     unsigned int length;
{
  register char *out = (char *) xmalloc (length + 1);
  if (length > 0)
    bcopy (str, out, length);
  out[length] = '\0';
  return out;
}

/* Search string BIG (length BLEN) for an occurrence of
   string SMALL (length SLEN).  Return a pointer to the
   beginning of the first occurrence, or return nil if none found.  */

char *
sindex (big, blen, small, slen)
     char *big;
     unsigned int blen;
     char *small;
     unsigned int slen;
{
  register unsigned int b;

  if (blen < 1)
    blen = strlen (big);
  if (slen < 1)
    slen = strlen (small);

  for (b = 0; b < blen; ++b)
    if (big[b] == *small && !strncmp (&big[b + 1], small + 1, slen - 1))
      return (&big[b]);

  return 0;
}

/* Limited INDEX:
   Search through the string STRING, which ends at LIMIT, for the character C.
   Returns a pointer to the first occurrence, or nil if none is found.
   Like INDEX except that the string searched ends where specified
   instead of at the first null.  */

char *
lindex (s, limit, c)
     register char *s, *limit;
     int c;
{
  while (s < limit)
    if (*s++ == c)
      return s - 1;

  return 0;
}

/* Return the address of the first whitespace or null in the string S.  */

char *
end_of_token (s)
     char *s;
{
  register char *p = s;
  register int backslash = 0;

  while (*p != '\0' && (backslash || !isblank (*p)))
    {
      if (*p++ == '\\')
	{
	  backslash = !backslash;
	  while (*p == '\\')
	    {
	      backslash = !backslash;
	      ++p;
	    }
	}
      else
	backslash = 0;
    }

  return p;
}

/* Return the address of the first nonwhitespace or null in the string S.  */

char *
next_token (s)
     char *s;
{
  register char *p = s;

  while (isblank (*p))
    ++p;
  return p;
}

/* Find the next token in PTR; return the address of it, and store the
   length of the token into *LENGTHPTR if LENGTHPTR is not nil.  */

char *
find_next_token (ptr, lengthptr)
     char **ptr;
     unsigned int *lengthptr;
{
  char *p = next_token (*ptr);
  char *end;

  if (*p == '\0')
    return 0;

  *ptr = end = end_of_token (p);
  if (lengthptr != 0)
    *lengthptr = end - p;
  return p;
}

/* Copy a chain of `struct dep', making a new chain
   with the same contents as the old one.  */

struct dep *
copy_dep_chain (d)
     register struct dep *d;
{
  register struct dep *c;
  struct dep *firstnew = 0;
  struct dep *lastnew;

  while (d != 0)
    {
      c = (struct dep *) xmalloc (sizeof (struct dep));
      bcopy ((char *) d, (char *) c, sizeof (struct dep));
      if (c->name != 0)
	c->name = savestring (c->name, strlen (c->name));
      c->next = 0;
      if (firstnew == 0)
	firstnew = lastnew = c;
      else
	lastnew = lastnew->next = c;

      d = d->next;
    }

  return firstnew;
}

#ifdef	iAPX286
/* The losing compiler on this machine can't handle this macro.  */

char *
dep_name (dep)
     struct dep *dep;
{
  return dep->name == 0 ? dep->file->name : dep->name;
}
#endif

#if	!defined(POSIX) && !defined(__GNU_LIBRARY__) && !defined(__CYGWIN__) && \
    !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__APPLE__)
extern int getuid (), getgid (), geteuid (), getegid ();
#ifdef	USG
extern int setuid (), setgid ();
#else
extern int setreuid (), setregid ();
#endif	/* USG.  */
#endif	/* Not POSIX and not GNU C library.  */

/* Keep track of the user and group IDs for user- and make- access.  */
static int user_uid = -1, user_gid = -1, make_uid = -1, make_gid = -1;
#define	access_inited	(user_uid != -1)
static enum { make, user } current_access;

static void
init_access ()
{
  user_uid = getuid ();
  user_gid = getgid ();

  make_uid = geteuid ();
  make_gid = getegid ();

  /* Do these ever fail?  */
  if (user_uid == -1 || user_gid == -1 || make_uid == -1 || make_gid == -1)
    pfatal_with_name ("get{e}[gu]id");

  current_access = make;
}

/* Give the process appropriate permissions for access to
   user data (i.e., to stat files, or to spawn a child process).  */
void
user_access ()
{
  if (!access_inited)
    init_access ();

  if (current_access == user)
    return;

  /* We are in "make access" mode.  This means that the effective user and
     group IDs are those of make (if it was installed setuid or setgid).
     We now want to set the effective user and group IDs to the real IDs,
     which are the IDs of the process that exec'd make.  */

#if	defined (USG) || defined (POSIX)
  /* System V has only the setuid/setgid calls to set user/group IDs.
     There is an effective ID, which can be set by setuid/setgid.
     It can be set (unless you are root) only to either what it already is
     (returned by geteuid/getegid, now in make_uid/make_gid),
     the real ID (return by getuid/getgid, now in user_uid/user_gid),
     or the saved set ID (what the effective ID was before this set-ID
     executable (make) was exec'd).  */
  if (setuid (user_uid) < 0)
    pfatal_with_name ("setuid");
  if (setgid (user_gid) < 0)
    pfatal_with_name ("setgid");
#else
  /* In 4BSD, the setreuid/setregid calls set both the real and effective IDs.
     They may be set to themselves or each other.  So you have two alternatives
     at any one time.  If you use setuid/setgid, the effective will be set to
     the real, leaving only one alternative.  Using setreuid/setregid, however,
     you can toggle between your two alternatives by swapping the values in a
     single setreuid or setregid call.  */
  if (setreuid (make_uid, user_uid) < 0)
    pfatal_with_name ("setreuid");
  if (setregid (make_gid, user_gid) < 0)
    pfatal_with_name ("setregid");
#endif

  current_access = user;
}

/* Give the process appropriate permissions for access to
   make data (i.e., the load average).  */
void
make_access ()
{
  if (!access_inited)
    init_access ();

  if (current_access == make)
    return;

  /* See comments in user_access, above.  */

#if	defined (USG) || defined (POSIX)
  if (setuid (make_uid) < 0)
    pfatal_with_name ("setuid");
  if (setgid (make_gid) < 0)
    pfatal_with_name ("setgid");
#else
  if (setreuid (user_uid, make_uid) < 0)
    pfatal_with_name ("setreuid");
  if (setregid (user_gid, make_gid) < 0)
    pfatal_with_name ("setregid");
#endif

  current_access = make;
}

/* Give the process appropriate permissions for a child process.
   This is like user_access, but you can't get back to make_access.  */
void
child_access ()
{
  /* Set both the real and effective UID and GID to the user's.
     They cannot be changed back to make's.  */

  if (setuid (user_uid) < 0)
    pfatal_with_name ("setuid");
  if (setgid (user_gid) < 0)
    pfatal_with_name ("setgid");
}
