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

/* This is POSIX.2, but most systems using -DPOSIX probably don't have it.  */
#if defined(__GNU_LIBRARY__) || defined(__CYGWIN__) || defined(__FreeBSD__) || \
    defined(__OpenBSD__) || defined(__APPLE__) || \
    (defined(__sun__) && defined(__svr4__))
#include <glob.h>
#else
#include "glob/glob.h"
#endif

#include <pwd.h>
struct passwd *getpwnam ();


static void read_makefile ();
static unsigned int readline (), do_define ();
static int conditional_line ();
static void record_files ();
static char *find_semicolon ();


/* A `struct linebuffer' is a structure which holds a line of text.
   `readline' reads a line from a stream into a linebuffer
   and works regardless of the length of the line.  */

struct linebuffer
  {
    /* Note:  This is the number of bytes malloc'ed for `buffer'
       It does not indicate `buffer's real length.
       Instead, a null char indicates end-of-string.  */
    unsigned int size;
    char *buffer;
  };

#define initbuffer(lb) (lb)->buffer = (char *) xmalloc ((lb)->size = 200)
#define freebuffer(lb) free ((lb)->buffer)


/* A `struct conditionals' contains the information describing
   all the active conditionals in a makefile.

   The global variable `conditionals' contains the conditionals
   information for the current makefile.  It is initialized from
   the static structure `toplevel_conditionals' and is later changed
   to new structures for included makefiles.  */

struct conditionals
  {
    unsigned int if_cmds;
    unsigned int max_ignoring;
    char *ignoring;
  };

static struct conditionals toplevel_conditionals;
static struct conditionals *conditionals = &toplevel_conditionals;
  

/* Default directories to search for include files in  */

static char *default_include_directories[] =
  {
    "/usr/gnu/include",
    "/usr/local/include",
    "/usr/include",
    0
  };

/* List of directories to search for include files in  */

static char **include_directories;

/* Maximum length of an element of the above.  */

static unsigned int max_incl_len;

/* The filename and pointer to line number of the
   makefile currently being read in.  */

char *reading_filename;
unsigned int *reading_lineno_ptr;

/* The chain of makefiles read by read_makefile.  */

static struct dep *read_makefiles = 0;

/* Read in all the makefiles and return the chain of their names.  */

struct dep *
read_all_makefiles (makefiles)
     char **makefiles;
{
  unsigned int num_makefiles = 0;

  if (debug_flag)
    puts ("Reading makefiles...");

  /* If there's a non-null variable MAKEFILES, its value is a list of
     files to read first thing.  But don't let it prevent reading the
     default makefiles and don't let the default goal come from there.  */

  {
    char *value = allocated_variable_expand ("$(MAKEFILES)");
    char *name, *p;
    unsigned int length;

    /* Set NAME to the start of next token and LENGTH to its length.
       MAKEFILES is updated for finding remaining tokens.  */
    p = value;
    while ((name = find_next_token (&p, &length)) != 0)
      read_makefile (name, 1);

    free (value);
  }

  /* Read makefiles specified with -f switches.  */

  if (makefiles != 0)
    while (*makefiles != 0)
      {
	struct dep *tail = read_makefiles;
	register struct dep *d;

	read_makefile (*makefiles, 0);

	/* Find the right element of read_makefiles.  */
	d = read_makefiles;
	while (d->next != tail)
	  d = d->next;

	/* Use the storage read_filename allocates.  */
	free (*makefiles);
	*makefiles = dep_name (d);
	++num_makefiles;
	++makefiles;
      }

  /* If there were no -f switches, try the default names.  */

  if (num_makefiles == 0)
    {
      static char *default_makefiles[] =
	{ "GNUmakefile", "makefile", "Makefile", 0 };
      register char **p = default_makefiles;
      while (*p != 0 && !file_exists_p (*p))
	++p;

      if (*p != 0)
	read_makefile (*p, 0);
      else
	{
	  /* No default makefile was found.  Add the default makefiles to the
	     `read_makefiles' chain so they will be updated if possible.  */
	  struct dep *tail = read_makefiles;
	  for (p = default_makefiles; *p != 0; ++p)
	    {
	      struct dep *d = (struct dep *) xmalloc (sizeof (struct dep));
	      d->name = 0;
	      d->file = enter_file (*p);
	      d->file->dontcare = 1;
	      /* Setting the `changed' member to 1 will make failure to
		 update or find this makefile as if it had come from the
		 MAKEFILES variable: we don't care, so we won't die.  */
	      d->changed = 1;
	      if (tail == 0)
		read_makefiles = d;
	      else
		tail->next = d;
	      tail = d;
	    }
	  if (tail != 0)
	    tail->next = 0;
	}
    }

  return read_makefiles;
}

/* Read file FILENAME as a makefile and add its contents to the data base.

   TYPE indicates what flavor of makefile this is: 0 => a default or -f
   makefile (the basis for comparison); 1 => from the MAKEFILES variable:
   cannot determine the default goal, is searched for in the search path,
   and it's not an error if it doesn't exist; 2 => an included makefile:
   is searched for in the search path.

   FILENAME is added to the `read_makefiles' chain.  */

static void
read_makefile (filename, type)
     char *filename;
     int type;
{
  static char *collapsed = 0;
  static unsigned int collapsed_length = 0;
  register FILE *infile;
  struct linebuffer lb;
  unsigned int commands_len = 200;
  char *commands = (char *) xmalloc (200);
  unsigned int commands_idx = 0;
  unsigned int commands_started;
  register char *p;
  char *p2;
  int ignoring = 0;

  struct nameseq *filenames = 0;
  struct dep *deps;
  unsigned int lineno = 1;
  unsigned int nlines = 0;
  int two_colon;
  char *pattern = 0, *pattern_percent;

#define record_waiting_files()						      \
  do									      \
    { 									      \
      record_files (filenames, pattern, pattern_percent, deps,		      \
		    commands_started, commands, commands_idx,		      \
		    two_colon, filename, lineno, type != 1);		      \
      filenames = 0;							      \
      commands_idx = 0;							      \
      pattern = 0;							      \
    } while (0)

#ifdef	lint	/* Suppress `used before set' messages.  */
  two_colon = 0;
#endif

  /* First, get a stream to read.  */

  infile = fopen (filename, "r");

  /* If the makefile wasn't found and it's either a makefile from
     the `MAKEFILES' variable (type 1) or an included makefile (type 2),
     search the included makefile search path for this makefile.  */

  if (infile == 0 && (type == 1 || type == 2) && *filename != '/')
    {
      register unsigned int i;
      for (i = 0; include_directories[i] != 0; ++i)
	{
	  char *name = concat (include_directories[i], "/", filename);
	  infile = fopen (name, "r");
	  if (infile != 0)
	    {
	      filename = name;
	      break;
	    }
	  else
	    free (name);
	}
    }

  /* Add FILENAME to the chain of read makefiles.  */
  deps = (struct dep *) xmalloc (sizeof (struct dep));
  deps->next = read_makefiles;
  read_makefiles = deps;
  deps->name = 0;
  deps->file = lookup_file (filename);
  if (deps->file == 0)
    {
      deps->file = enter_file (savestring (filename, strlen (filename)));
      if (type == 1)
	deps->file->dontcare = 1;
    }
  filename = deps->file->name;
  deps->file->precious = 1;
  deps->changed = type;
  deps = 0;

  /* If the makefile can't be found at all,
     either ignore it or give up entirely.  */

  if (infile == 0)
    {
      if (type != 1)
	perror_with_name ("fopen: ", filename);
      return;
    }

  reading_filename = filename;
  reading_lineno_ptr = &lineno;

  /* Loop over lines in the file.
     The strategy is to accumulate target names in FILENAMES,
     dependencies in DEPS and commands in COMMANDS.
     These are used to define a rule
     when the start of the next rule (or eof) is encountered.  */

  initbuffer (&lb);

  while (!feof (infile))
    {
      lineno += nlines;
      nlines = readline (&lb, infile, filename);

      /* Collapse continuation lines.  */
      collapse_continuations (lb.buffer);

      if (collapsed_length < lb.size)
	{
	  collapsed_length = lb.size;
	  if (collapsed != 0)
	    free (collapsed);
	  collapsed = (char *) xmalloc (collapsed_length);
	}
      strcpy (collapsed, lb.buffer);
      remove_comments (collapsed);

      p = collapsed;
      while (isspace (*p))
	++p;
      if (*p == '\0' && lb.buffer[0] != '\t')
	continue;

#define	word1eq(s, l) 	((p[l] == '\0' || isblank (p[l])) && \
			 !strncmp (s, p, l))
      if (word1eq ("ifdef", 5) || word1eq ("ifndef", 6)
	  || word1eq ("ifeq", 4) || word1eq ("ifneq", 5)
	  || word1eq ("else", 4) || word1eq ("endif", 5))
	{
	  int i = conditional_line (p, filename, lineno);
	  if (i >= 0)
	    {
	      ignoring = i;
	      continue;
	    }
	  else
	    makefile_fatal (filename, lineno,
			    "invalid syntax in conditional");
	}
      if (ignoring)
	continue;
      else if (lb.buffer[0] == '\t')
	{
	  /* This line is a shell command.  */
	  unsigned int len;

	  if (filenames == 0)
	    makefile_fatal (filename, lineno,
			    "commands commence before first target");

	  /* Add this command line to end of the line being accumulated.  */
	  p = lb.buffer;
	  if (commands_idx == 0)
	    commands_started = lineno;
	  len = strlen (p);
	  if (len + 1 + commands_idx > commands_len)
	    {
	      commands_len = (len + 1 + commands_idx) * 2;
	      commands = (char *) xrealloc (commands, commands_len);
	    }
	  bcopy (p, &commands[commands_idx], len);
	  commands_idx += len;
	  commands[commands_idx++] = '\n';
	}
      else if (word1eq ("define", 6))
	{
	  p2 = next_token (p + 6);
	  p = end_of_token (p2);
	  lineno = do_define (p2, p - p2, o_file, lineno, infile, filename);
	  continue;
	}
      else if (word1eq ("endef", 5))
	makefile_fatal (filename, lineno, "extraneous `endef'");
      else if (word1eq ("override", 8))
	{
	  p2 = next_token (p + 8);
	  if (p2 == 0)
	    makefile_error (filename, lineno, "empty `override' directive");
	  if (!strncmp (p2, "define", 6))
	    {
	      unsigned int len;
	      p2 = end_of_token (p2);
	      p = find_next_token (&p2, &len);
	      lineno = do_define (p, len, o_override,
				  lineno, infile, filename);
	    }
	  else if (!try_variable_definition (p2, o_override))
	    makefile_error (filename, lineno,
			    "Empty `override' directive");
	  continue;
	}
      else if (word1eq ("include", 7))
	{
	  /* We have found an `include' line specifying a nested
	     makefile to be read at this point.  */
	  struct conditionals *save = conditionals;
	  struct conditionals new_conditionals;
	  p = allocated_variable_expand (next_token (p + 8));
	  if (*p == '\0')
	    {
	      makefile_error (filename, lineno, "no filename for `include'");
	      continue;
	    }
	  p2 = end_of_token (p);
	  if (*p2 != '\0')
	    {
	      *p2++ = '\0';
	      if (*next_token (p2) != '\0')
		makefile_error (filename, lineno,
				"extraneous text after `include'");
	    }
	  bzero ((char *) &new_conditionals, sizeof new_conditionals);
	  conditionals = &new_conditionals;
	  /* Record the rules that are waiting so they will determine
	     the default goal before those in the included makefile.  */
	  record_waiting_files ();
	  read_makefile (p, 2);
	  free (p);
	  conditionals = save;
	  reading_filename = filename;
	  reading_lineno_ptr = &lineno;
	  continue;
	}
      else if (word1eq ("vpath", 5))
	{
	  char *pattern;
	  unsigned int len;
	  p2 = variable_expand (p + 5);
	  p = find_next_token (&p2, &len);
	  if (p != 0)
	    {
	      pattern = savestring (p, len);
	      p = find_next_token (&p2, &len);
	      if (p != 0)
		{
		  p = savestring (p, len);
		  if (find_next_token (&p2, (unsigned int *) 0) != 0)
		    makefile_error (filename, lineno,
				    "extraneous text after `vpath' directive");
		}
	      /* No searchpath means remove all previous
		 selective VPATH's with the same pattern.  */
	    }
	  else
	    /* No pattern means remove all previous selective VPATH's.  */
	    pattern = 0;
	  construct_vpath_list (pattern, p);
	  if (pattern != 0)
	    free (pattern);
	  if (p != 0)
	    free (p);
	  continue;
	}
#undef	word1eq
      else if (try_variable_definition (p, o_file))
	continue;
      else
	{
	  /* This line describes some target files.  */

	  char *cmdleft;

	  /* Record the previous rule.  */

	  record_waiting_files ();

	  /* Look for a semicolon in the unexpanded line.  */
	  cmdleft = find_semicolon (lb.buffer);
	  if (cmdleft != 0)
	    /* Found one.  Cut the line short there before expanding it.  */
	    *cmdleft = '\0';

	  /* Expand variable and function references before doing anything
	     else so that special characters can be inside variables.  */
	  p = variable_expand (lb.buffer);

	  if (cmdleft == 0)
	    /* Look for a semicolon in the expanded line.  */
	    cmdleft = find_semicolon (p);

	  if (cmdleft != 0)
	    /* Cut the line short at the semicolon.  */
	    *cmdleft = '\0';

	  /* Remove comments from the line.  */
	  remove_comments (p);

	  p2 = next_token (p);
	  if (*p2 == '\0')
	    /* This line contained a variable reference that
	       expanded to nothing but whitespace.  */
	    continue;
	  else if (*p2 == ':')
	    makefile_fatal (filename, lineno, "missing target name");

	  filenames = multi_glob (parse_file_seq (&p2, ':',
						  sizeof (struct nameseq)),
				  sizeof (struct nameseq));
	  if (*p2++ == '\0')
	    makefile_fatal (filename, lineno, "missing separator");
	  /* Is this a one-colon or two-colon entry?  */
	  two_colon = *p2 == ':';
	  if (two_colon)
	    p2++;

	  /* Is this a static pattern rule: `target: %targ: %dep; ...'?  */
	  p = index (p2, ':');
	  while (p != 0 && p[-1] == '\\')
	    {
	      register char *q = &p[-1];
	      register int backslash = 0;
	      while (*q-- == '\\')
		backslash = !backslash;
	      if (backslash)
		p = index (p + 1, ':');
	      else
		break;
	    }
	  if (p != 0)
	    {
	      struct nameseq *target;
	      target = parse_file_seq (&p2, ':', sizeof (struct nameseq));
	      ++p2;
	      if (target == 0)
		makefile_fatal (filename, lineno, "missing target pattern");
	      else if (target->next != 0)
		makefile_fatal (filename, lineno, "multiple target patterns");
	      pattern = target->name;
	      pattern_percent = find_percent (pattern);
	      if (pattern_percent == 0)
		makefile_fatal (filename, lineno,
				"target pattern contains no `%%'");
	    }
	  else
	    pattern = 0;

	  /* Parse the dependencies.  */
	  deps = (struct dep *)
	    multi_glob (parse_file_seq (&p2, '\0', sizeof (struct dep)),
			sizeof (struct dep));

	  commands_idx = 0;
	  if (cmdleft != 0)
	    {
	      /* Semicolon means rest of line is a command */
	      unsigned int len = strlen (cmdleft + 1);

	      commands_started = lineno;

	      /* Add this command line to the buffer.  */
	      if (len + 2 > commands_len)
		{
		  commands_len = (len + 2) * 2;
		  commands = (char *) xrealloc (commands, commands_len);
		}
	      bcopy (cmdleft + 1, commands, len);
	      commands_idx += len;
	      commands[commands_idx++] = '\n';
	    }
	}
    }

  if (ignoring)
    makefile_fatal (filename, lineno, "missing `endif'");

  /* At eof, record the last rule.  */
  record_waiting_files ();

  freebuffer (&lb);
  free ((char *) commands);
  fclose (infile);

  reading_filename = 0;
  reading_lineno_ptr = 0;
}

/* Execute a `define' directive.
   The first line has already been read, and NAME is the name of
   the variable to be defined.  The following lines remain to be read.
   LINENO, INFILE and FILENAME refer to the makefile being read.
   The value returned is LINENO, updated for lines read here.  */

static unsigned int
do_define (name, namelen, origin, lineno, infile, filename)
     char *name;
     unsigned int namelen;
     enum variable_origin origin;
     unsigned int lineno;
     FILE *infile;
     char *filename;
{
  struct linebuffer lb;
  unsigned int nlines = 0;
  unsigned int length = 100;
  char *definition = (char *) xmalloc (100);
  register unsigned int idx = 0;
  register char *p;

  initbuffer (&lb);
  while (!feof (infile))
    {
      lineno += nlines;
      nlines = readline (&lb, infile, filename);
      p = next_token (lb.buffer);

      if ((p[5] == '\0' || isblank (p[5])) && !strncmp (p, "endef", 5))
	{
	  p += 5;
	  collapse_continuations (p);
	  remove_comments (p);
	  if (*next_token (p) != '\0')
	    makefile_error (filename, lineno,
			    "Extraneous text after `endef' directive");
	  /* Define the variable.  */
	  if (idx == 0)
	    definition[0] = '\0';
	  else
	    definition[idx - 1] = '\0';
	  (void) define_variable (name, namelen, definition, origin, 1);
	  free (definition);
	  return lineno;
	}
      else
	{
	  unsigned int len = strlen (p);

	  /* Increase the buffer size if necessary.  */
	  if (idx + len + 1 > length)
	    {
	      length = (idx + len) * 2;
	      definition = (char *) xrealloc (definition, length + 1);
	    }

	  bcopy (p, &definition[idx], len);
	  idx += len;
	  /* Separate lines with a newline.  */
	  definition[idx++] = '\n';
	}
    }

  /* No `endef'!!  */
  makefile_fatal (filename, lineno, "missing `endef', unterminated `define'");
  return 0;
}

/* Interpret conditional commands "ifdef", "ifndef", "ifeq",
   "ifneq", "else" and "endif".
   LINE is the input line, with the command as its first word.

   FILENAME and LINENO are the filename and line number in the
   current makefile.  They are used for error messages.

   Value is -1 if the line is invalid,
   0 if following text should be interpreted,
   1 if following text should be ignored.  */

static int
conditional_line (line, filename, lineno)
     char *line;
     char *filename;
     unsigned int lineno;
{
  int notdef;
  char *cmdname;
  register unsigned int i;

  if (*line == 'i')
    {
      /* It's an "if..." command.  */
      notdef = line[2] == 'n';
      if (notdef)
	{
	  cmdname = line[3] == 'd' ? "ifndef" : "ifneq";
	  line += cmdname[3] == 'd' ? 7 : 6;
	}
      else
	{
	  cmdname = line[2] == 'd' ? "ifdef" : "ifeq";
	  line += cmdname[2] == 'd' ? 6 : 5;
	}
    }
  else
    {
      /* It's an "else" or "endif" command.  */
      notdef = line[1] == 'n';
      cmdname = notdef ? "endif" : "else";
      line += notdef ? 5 : 4;
    }

  line = next_token (line);

  if (*cmdname == 'e')
    {
      if (*line != '\0')
	makefile_error (filename, lineno,
			"Extraneous text after `%s' directive",
			cmdname);
      /* "Else" or "endif".  */
      if (conditionals->if_cmds == 0)
	makefile_fatal (filename, lineno, "extraneous `%s'", cmdname);
      /* NOTDEF indicates an `endif' command.  */
      if (notdef)
	--conditionals->if_cmds;
      else
	conditionals->ignoring[conditionals->if_cmds - 1]
	  = !conditionals->ignoring[conditionals->if_cmds - 1];
      for (i = 0; i < conditionals->if_cmds; ++i)
	if (conditionals->ignoring[i])
	  return 1;
      return 0;
    }

  if (conditionals->max_ignoring == 0)
    {
      conditionals->max_ignoring = 5;
      conditionals->ignoring = (char *) xmalloc (conditionals->max_ignoring);
    }

  ++conditionals->if_cmds;
  if (conditionals->if_cmds > conditionals->max_ignoring)
    {
      conditionals->max_ignoring += 5;
      conditionals->ignoring = (char *)
	xrealloc (conditionals->ignoring, conditionals->max_ignoring);
    }

  if (conditionals->if_cmds > 1 &&
      conditionals->ignoring[conditionals->if_cmds - 2])
    /* We are already ignoring, so just push a level
       to match the next "else" or "endif", and keep ignoring.  */
    conditionals->ignoring[conditionals->if_cmds - 1] = 1;
  else if (cmdname[notdef ? 3 : 2] == 'd')
    {
      /* "Ifdef" or "ifndef".  */
      struct variable *v;
      register char *p = end_of_token (line);
      i = p - line;
      p = next_token (p);
      if (*p != '\0')
	return -1;
      v = lookup_variable (line, i);
      conditionals->ignoring[conditionals->if_cmds - 1]
	= (v != 0 && *v->value != '\0') == notdef;
    }
  else
    {
      /* "Ifeq" or "ifneq".  */
      char *s1, *s2;
      unsigned int len;
      char termin = *line == '(' ? ',' : *line;

      if (termin != ',' && termin != '"' && termin != '\'')
	return -1;

      s1 = ++line;
      /* Find the end of the first string.  */
      if (termin == ',')
	{
	  register int count = 0;
	  for (; *line != '\0'; ++line)
	    if (*line == '(')
	      ++count;
	    else if (*line == ')')
	      --count;
	    else if (*line == ',' && count <= 0)
	      break;
	}
      else
	while (*line != '\0' && *line != termin)
	  ++line;

      if (*line == '\0')
	return -1;

      *line++ = '\0';

      s2 = variable_expand (s1);
      /* We must allocate a new copy of the expanded string because
	 variable_expand re-uses the same buffer.  */
      len = strlen (s2);
      s1 = (char *) alloca (len + 1);
      bcopy (s2, s1, len + 1);

      if (termin != ',')
	/* Find the start of the second string.  */
	line = next_token (line);

      termin = termin == ',' ? ')' : *line;
      if (termin != ')' && termin != '"' && termin != '\'')
	return -1;

      /* Find the end of the second string.  */
      if (termin == ')')
	{
	  register int count = 0;
	  s2 = next_token (line);
	  for (line = s2; *line != '\0'; ++line)
	    {
	      if (*line == '(')
		++count;
	      else if (*line == ')')
		if (count <= 0)
		  break;
		else
		  --count;
	    }
	}
      else
	{
	  ++line;
	  s2 = line;
	  while (*line != '\0' && *line != termin)
	    ++line;
	}

      if (*line == '\0')
	return -1;

      *line = '\0';
      line = next_token (++line);
      if (*line != '\0')
	makefile_error (filename, lineno,
			"Extraneous text after `%s' directive",
			cmdname);

      s2 = variable_expand (s2);
      conditionals->ignoring[conditionals->if_cmds - 1]
	= streq (s1, s2) == notdef;
    }

  /* Search through the stack to see if we're ignoring.  */
  for (i = 0; i < conditionals->if_cmds; ++i)
    if (conditionals->ignoring[i])
      return 1;
  return 0;
}

/* Remove duplicate dependencies in CHAIN.  */

void
uniquize_deps (chain)
     struct dep *chain;
{
  register struct dep *d;

  /* Make sure that no dependencies are repeated.  This does not
     really matter for the purpose of updating targets, but it
     might make some names be listed twice for $^ and $?.  */

  for (d = chain; d != 0; d = d->next)
    {
      struct dep *last, *next;

      last = d;
      next = d->next;
      while (next != 0)
	if (streq (dep_name (d), dep_name (next)))
	  {
	    struct dep *n = next->next;
	    last->next = n;
	    if (next->name != 0 && next->name != d->name)
	      free (next->name);
	    if (next != d)
	      free ((char *) next);
	    next = n;
	  }
	else
	  {
	    last = next;
	    next = next->next;
	  }
    }
}

/* Record a description line for files FILENAMES,
   with dependencies DEPS, commands to execute described
   by COMMANDS and COMMANDS_IDX, coming from FILENAME:COMMANDS_STARTED.
   TWO_COLON is nonzero if a double colon was used.
   If not nil, PATTERN is the `%' pattern to make this
   a static pattern rule, and PATTERN_PERCENT is a pointer
   to the `%' within it.

   The links of FILENAMES are freed, and so are any names in it
   that are not incorporated into other data structures.  */

static void
record_files (filenames, pattern, pattern_percent, deps, commands_started,
	      commands, commands_idx, two_colon, filename, lineno, set_default)
     struct nameseq *filenames;
     char *pattern, *pattern_percent;
     struct dep *deps;
     unsigned int commands_started;
     char *commands;
     unsigned int commands_idx;
     int two_colon;
     char *filename;
     unsigned int lineno;
     int set_default;
{
  struct nameseq *nextf;
  int implicit = 0;
  unsigned int max_targets, target_idx;
  char **targets = 0, **target_percents = 0;
  struct commands *cmds;

  if (commands_idx > 0)
    {
      cmds = (struct commands *) xmalloc (sizeof (struct commands));
      cmds->filename = filename;
      cmds->lineno = commands_started;
      cmds->commands = savestring (commands, commands_idx);
      cmds->command_lines = 0;
    }
  else
    cmds = 0;

  for (; filenames != 0; filenames = nextf)
    {
      register char *name = filenames->name;
      register struct file *f;
      register struct dep *d;
      struct dep *this;
      char *implicit_percent;

      nextf = filenames->next;
      free ((char *) filenames);

      implicit_percent = find_percent (name);
      implicit |= implicit_percent != 0;

      if (implicit && pattern != 0)
	makefile_fatal (filename, lineno,
			"mixed implicit and static pattern rules");

      if (implicit && implicit_percent == 0)
	makefile_fatal (filename, lineno, "mixed implicit and normal rules");

      if (implicit)
	{
	  if (targets == 0)
	    {
	      max_targets = 5;
	      targets = (char **) xmalloc (5 * sizeof (char *));
	      target_percents = (char **) xmalloc (5 * sizeof (char *));
	      target_idx = 0;
	    }
	  else if (target_idx == max_targets - 1)
	    {
	      max_targets += 5;
	      targets = (char **) xrealloc ((char *) targets,
					    max_targets * sizeof (char *));
	      target_percents
		= (char **) xrealloc ((char *) target_percents,
				      max_targets * sizeof (char *));
	    }
	  targets[target_idx] = name;
	  target_percents[target_idx] = implicit_percent;
	  ++target_idx;
	  continue;
	}

      /* If there are multiple filenames, copy the chain DEPS
	 for all but the last one.  It is not safe for the same deps
	 to go in more than one place in the data base.  */
      this = nextf != 0 ? copy_dep_chain (deps) : deps;

      if (pattern != 0)
	/* If this is an extended static rule:
	   `targets: target%pattern: dep%pattern; cmds',
	   translate each dependency pattern into a plain filename
	   using the target pattern and this target's name.  */
	if (!pattern_matches (pattern, pattern_percent, name))
	  {
	    /* Give a warning if the rule is meaningless.  */
	    makefile_error (filename, lineno,
			    "target `%s' doesn't match the target pattern");
	    this = 0;
	  }
	else
	  {
	    /* We use patsubst_expand to do the work of translating
	       the target pattern, the target's name and the dependencies'
	       patterns into plain dependency names.  */
	    char *buffer = variable_expand ("");

	    for (d = this; d != 0; d = d->next)
	      {
		char *o;
		char *percent = find_percent (d->name);
		if (percent == 0)
		  continue;
		o = patsubst_expand (buffer, name, pattern, d->name,
				     pattern_percent, percent);
		free (d->name);
		d->name = savestring (buffer, o - buffer);
	      }
	  }
      
      if (!two_colon)
	{
	  /* Single-colon.  Combine these dependencies
	     with others in file's existing record, if any.  */
	  f = enter_file (name);

	  if (f->double_colon)
	    makefile_fatal (filename, lineno,
			    "target file `%s' has both : and :: entries",
			    f->name);

	  /* If CMDS == F->CMDS, this target was listed in this rule
	     more than once.  Just give a warning since this is harmless.  */
	  if (cmds != 0 && cmds == f->cmds)
	    makefile_error
	      (filename, lineno,
	       "target `%s' given more than once in the same rule.",
	       f->name);

	  /* Check for two single-colon entries both with commands.
	     Check is_target so that we don't lose on files such as .c.o
	     whose commands were preinitialized.  */
	  else if (cmds != 0 && f->cmds != 0 && f->is_target)
	    {
	      makefile_error (cmds->filename, cmds->lineno,
			      "warning: overriding commands for target `%s'",
			      f->name);
	      makefile_error (f->cmds->filename, f->cmds->lineno,
			      "warning: ignoring old commands for target `%s'",
			      f->name);
	    }

	  f->is_target = 1;

	  /* Defining .DEFAULT with no deps or cmds clears it.  */
	  if (f == default_file && this == 0 && cmds == 0)
	    f->cmds = 0;
	  if (cmds != 0)
	    f->cmds = cmds;
	  /* Defining .SUFFIXES with no dependencies
	     clears out the list of suffixes.  */
	  if (f == suffix_file && this == 0)
	    f->deps = 0;
	  else if (f->deps != 0)
	    {
	      d = f->deps;
	      while (d->next != 0)
		d = d->next;
	      d->next = this;
	    }
	  else
	    f->deps = this;

	  uniquize_deps (f->deps);

	  /* If this is a static pattern rule, set the file's stem to
	     the part of its name that matched the `%' in the pattern,
	     so you can use $* in the commands.  */
	  if (pattern != 0)
	    {
	      static char *percent = "%";
	      char *buffer = variable_expand ("");
	      char *o = patsubst_expand (buffer, name, pattern, percent,
					 pattern_percent, percent);
	      f->stem = savestring (buffer, o - buffer);
	    }
	}
      else
	{
	  /* Double-colon.  Make a new record
	     even if the file already has one.  */
	  f = lookup_file (name);
	  /* Check for both : and :: rules.  Check is_target so
	     we don't lose on default suffix rules or makefiles.  */
	  if (f != 0 && f->is_target && !f->double_colon)
	    makefile_fatal (filename, lineno,
			    "target file `%s' has both : and :: entries",
			    f->name);
	  f = enter_file (name);
	  /* If there was an existing entry and it was a
	     double-colon entry, enter_file will have returned a
	     new one, making it the prev pointer of the old one.  */
	  f->is_target = 1;
	  f->double_colon = 1;
	  f->deps = this;
	  f->cmds = cmds;
	}

      /* Free name if not needed further.  */
      if (f != 0 && name != f->name
	  && !(f->name == name + 2 && name[0] == '.' && name[1] == '/'))
	{
	  free (name);
	  name = f->name;
	}

      /* See if this is first target seen whose name does
	 not start with a `.', unless it contains a slash.  */
      if (default_goal_file == 0 && set_default
	  && (*name != '.' || index (name, '/') != 0))
	{
	  int reject = 0;

	  /* If this file is a suffix, don't
	     let it be the default goal file.  */

	  for (d = suffix_file->deps; d != 0; d = d->next)
	    {
	      register struct dep *d2;
	      if (*dep_name (d) != '.' && streq (name, dep_name (d)))
		{
		  reject = 1;
		  break;
		}
	      for (d2 = suffix_file->deps; d2 != 0; d2 = d2->next)
		{
		  register unsigned int len = strlen (dep_name (d2));
		  if (strncmp (name, dep_name (d2), len))
		    continue;
		  if (streq (name + len, dep_name (d)))
		    {
		      reject = 1;
		      break;
		    }
		}
	      if (reject)
		break;
	    }

	  if (!reject)
	    default_goal_file = f;
	}
    }

  if (implicit)
    {
      targets[target_idx] = 0;
      target_percents[target_idx] = 0;
      create_pattern_rule (targets, target_percents, two_colon, deps, cmds, 1);
      free ((char *) target_percents);
    }
}

/* Search STRING for an unquoted ; that is not after an unquoted #.  */

static char *
find_semicolon (string)
     char *string;
{
  char *found, *p;

  found = index (string, ';');
  while (found != 0 && found[-1] == '\\')
    {
      register char *q = &found[-1];
      register int backslash = 0;
      while (*q-- == '\\')
	backslash = !backslash;
      if (backslash)
	found = index (found + 1, ';');
      else
	break;
    }
  if (found == 0)
    return 0;

  /* Look for a comment character (#) before the ; we found.  */
  p = lindex (string, found, '#');
  while (p != 0 && p[-1] == '\\')
    {
      register char *q = &p[-1];
      register int backslash = 0;
      while (*q-- == '\\')
	backslash = !backslash;
      if (backslash)
	p = lindex (p + 1, found, '#');
      else
	break;
    }
  if (p == 0)
    return found;
  return 0;
}

/* Search PATTERN for an unquoted %.  Backslashes quote % and backslash.
   Quoting backslashes are removed from PATTERN by compacting it into
   itself.  Returns a pointer to the first unquoted % if there is one,
   or nil if there are none.  */

char *
find_percent (pattern)
     char *pattern;
{
  unsigned int pattern_len = strlen (pattern);
  register char *p = pattern;

  while ((p = index (p, '%')) != 0)
    if (p > pattern && p[-1] == '\\')
      {
	/* Search for more backslashes.  */
	register int i = -2;
	while (&p[i] >= pattern && p[i] == '\\')
	  --i;
	++i;
	/* The number of backslashes is now -I.
	   Copy P over itself to swallow half of them.  */
	bcopy (&p[i / 2], &p[i], (pattern_len - (p - pattern)) - (i / 2) + 1);
	p += i / 2;
	if (i % 2 == 0)
	  /* All the backslashes quoted each other; the % was unquoted.  */
	  return p;
	else
	  /* The % was quoted by a backslash.  Look for another.  */
	  ++p;
      }
    else
      /* No backslash in sight.  */
      return p;

  /* Never hit a %.  */
  return 0;
}

/* Parse a string into a sequence of filenames represented as a
   chain of struct nameseq's in reverse order and return that chain.

   The string is passed as STRINGP, the address of a string pointer.
   The string pointer is updated to point at the first character
   not parsed, which either is a null char or equals STOPCHAR.

   SIZE is how big to construct chain elements.
   This is useful if we want them actually to be other structures
   that have room for additional info.  */

struct nameseq *
parse_file_seq (stringp, stopchar, size)
     char **stringp;
     char stopchar;
     unsigned int size;
{
  register struct nameseq *new = 0;
  register struct nameseq *new1;
  register char *p = *stringp;
  char *q;
  char *name;
  register int c;

  while (1)
    {
      /* Skip whitespace; see if any more names are left.  */
      p = next_token (p);
      if (*p == '\0')
	break;
      if (*p == stopchar)
	break;
      /* Yes, find end of next name.  */
      q = p;
      while (1)
	{
	  c = *p++;
	  if (c == '\0')
	    break;
	  else if (c == '\\' &&
	           (*p == '\\' || isblank (*p) || *p == stopchar))
	    ++p;
	  else if (isblank (c) || c == stopchar)
	    break;
	}
      p--;

      /* Extract the filename just found, and skip it.  */
      name = savestring (q, p - q);

      /* Add it to the front of the chain.  */
      new1 = (struct nameseq *) xmalloc (size);
      new1->name = name;
      new1->next = new;
      new = new1;
    }

  *stringp = p;
  return new;
}

/* Read a line of text from STREAM into LINEBUFFER.
   Combine continuation lines into one line.
   Return the number of actual lines read (> 1 if hacked continuation lines).
 */

static unsigned int
readline (linebuffer, stream, filename)
     struct linebuffer *linebuffer;
     FILE *stream;
     char *filename;
{
  char *buffer = linebuffer->buffer;
  register char *p = linebuffer->buffer;
  register char *end = p + linebuffer->size;
  register int len, lastlen = 0;
  register char *p2;
  register unsigned int nlines = 0;
  register int backslash;

  *p = '\0';

  while (1)
    {
      if (fgets (p, end - p, stream) == 0)
	if (feof (stream))
	  return nlines;
	else
	  pfatal_with_name (filename);

      len = strlen (p);
      if (len == 0 || (p += len)[-1] != '\n')
	{
	  /* Probably ran out of buffer space.  */
	  register unsigned int p_off = p - buffer;
	  linebuffer->size *= 2;
	  buffer = (char *) xrealloc (buffer, linebuffer->size);
	  p = buffer + p_off;
	  end = buffer + linebuffer->size;
	  linebuffer->buffer = buffer;
	  *p = '\0';
	  lastlen = len;
	  continue;
	}

      ++nlines;

      if (len == 1 && p > buffer)
	/* P is pointing at a newline and it's the beginning of
	   the buffer returned by the last fgets call.  However,
	   it is not necessarily the beginning of a line if P is
	   pointing past the beginning of the holding buffer.
	   If the buffer was just enlarged (right before the newline),
	   we must account for that, so we pretend that the two lines
	   were one line.  */
	len += lastlen;
      lastlen = len;
      backslash = 0;
      for (p2 = p - 2; --len > 0; --p2)
	{
	  if (*p2 == '\\')
	    backslash = !backslash;
	  else
	    break;
	}
      
      if (!backslash)
	{
	  p[-1] = '\0';
	  return nlines;
	}

      if (end - p <= 1)
	{
	  /* Enlarge the buffer.  */
	  register unsigned int p_off = p - buffer;
	  linebuffer->size *= 2;
	  buffer = (char *) xrealloc (buffer, linebuffer->size);
	  p = buffer + p_off;
	  end = buffer + linebuffer->size;
	  linebuffer->buffer = buffer;
	}
    }
}

/* Construct the list of include directories
   from the arguments and the default list.  */

void
construct_include_path (arg_dirs)
     char **arg_dirs;
{
  register unsigned int i;
  struct stat stbuf;

  /* Table to hold the dirs.  */

  register unsigned int defsize = (sizeof (default_include_directories)
				   / sizeof (default_include_directories[0]));
  register unsigned int max = 5;
  register char **dirs = (char **) xmalloc ((5 + defsize) * sizeof (char *));
  register unsigned int idx = 0;

  /* First consider any dirs specified with -I switches.
     Ignore dirs that don't exist.  */

  if (arg_dirs != 0)
    while (*arg_dirs != 0)
      {
	char *dir = *arg_dirs++;
	if (stat (dir, &stbuf) == 0 && S_ISDIR (stbuf.st_mode))
	  {
	    if (idx == max - 1)
	      {
		max += 5;
		dirs = (char **)
		  xrealloc ((char *) dirs, (max + defsize) * sizeof (char *));
	      }
	    dirs[idx++] = dir;
	  }
      }

  /* Now add at the end the standard default dirs.  */

  for (i = 0; default_include_directories[i] != 0; ++i)
    if (stat (default_include_directories[i], &stbuf) == 0
	&& S_ISDIR (stbuf.st_mode))
      dirs[idx++] = default_include_directories[i];

  dirs[idx] = 0;

  /* Now compute the maximum length of any name in it.  */

  max_incl_len = 0;
  for (i = 0; i < idx; ++i)
    {
      unsigned int len = strlen (dirs[i]);
      /* If dir name is written with a trailing slash, discard it.  */
      if (dirs[i][len - 1] == '/')
	/* We can't just clobber a null in because it may have come from
	   a literal string and literal strings may not be writable.  */
	dirs[i] = savestring (dirs[i], len - 1);
      if (len > max_incl_len)
	max_incl_len = len;
    }

  include_directories = dirs;
}

/* Given a chain of struct nameseq's describing a sequence of filenames,
   in reverse of the intended order, return a new chain describing the
   result of globbing the filenames.  The new chain is in forward order.
   The links of the old chain are freed or used in the new chain.
   Likewise for the names in the old chain.

   SIZE is how big to construct chain elements.
   This is useful if we want them actually to be other structures
   that have room for additional info.  */

struct nameseq *
multi_glob (chain, size)
     struct nameseq *chain;
     unsigned int size;
{
  register struct nameseq *new = 0;
  register struct nameseq *old;
  struct nameseq *nexto;

  for (old = chain; old != 0; old = nexto)
    {
      glob_t gl;

      nexto = old->next;

      if (old->name[0] == '~')
	{
	  if (old->name[1] == '/' || old->name[1] == '\0')
	    {
	      extern char *getenv ();
	      char *home_dir = allocated_variable_expand ("$(HOME)");
	      int is_variable = home_dir[0] != '\0';
	      if (!is_variable)
		{
		  free (home_dir);
		  home_dir = getenv ("HOME");
		}
	      if (home_dir == 0 || home_dir[0] == '\0')
		{
		  extern char *getlogin ();
		  char *name = getlogin ();
		  home_dir = 0;
		  if (name != 0)
		    {
		      struct passwd *p = getpwnam (name);
		      if (p != 0)
			home_dir = p->pw_dir;
		    }
		}
	      if (home_dir != 0)
		{
		  char *new = concat (home_dir, "", old->name + 1);
		  if (is_variable)
		    free (home_dir);
		  free (old->name);
		  old->name = new;
		}
	    }
	  else
	    {
	      struct passwd *pwent;
	      char *userend = index (old->name + 1, '/');
	      if (userend != 0)
		*userend = '\0';
	      pwent = getpwnam (old->name + 1);
	      if (pwent != 0)
		{
		  if (userend == 0)
		    {
		      free (old->name);
		      old->name = savestring (pwent->pw_dir,
					      strlen (pwent->pw_dir));
		    }
		  else
		    {
		      char *new = concat (pwent->pw_dir, "/", userend + 1);
		      free (old->name);
		      old->name = new;
		    }
		}
	      else if (userend != 0)
		*userend = '/';
	    }
	}

      switch (glob (old->name, GLOB_NOCHECK, NULL, &gl))
	{
	case 0:			/* Success.  */
	  {
	    register int i;
	    for (i = 0; i < gl.gl_pathc; ++i)
	      {
		struct nameseq *elt = (struct nameseq *) xmalloc (size);
		elt->name = savestring (gl.gl_pathv[i],
					strlen (gl.gl_pathv[i]));
		elt->next = new;
		new = elt;
	      }
	    globfree (&gl);
	    break;
	  }

	case GLOB_NOSPACE:
	  fatal ("virtual memory exhausted");
	  break;

	default:
	  old->next = new;
	  new = old;
	}
    }

  return new;
}
