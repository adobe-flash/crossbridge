/* Darwin support needed only by C/C++ frontends.
   Copyright (C) 2001, 2003, 2004, 2005  Free Software Foundation, Inc.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "cpplib.h"
#include "tree.h"
#include "c-pragma.h"
#include "c-tree.h"
#include "c-incpath.h"
#include "c-common.h"
#include "toplev.h"
#include "flags.h"
#include "tm_p.h"
#include "cppdefault.h"
#include "prefix.h"
/* APPLE LOCAL include options.h */
#include "options.h"
/* APPLE LOCAL begin optimization pragmas 3124235/3420242 */
#include "flags.h"
#include "opts.h"
#include "varray.h"
/* APPLE LOCAL end optimization pragmas 3124235/3420242 */

/* Pragmas.  */

#define BAD(gmsgid) do { warning (OPT_Wpragmas, gmsgid); return; } while (0)
#define BAD2(msgid, arg) do { warning (OPT_Wpragmas, msgid, arg); return; } while (0)

static bool using_frameworks = false;

/* Maintain a small stack of alignments.  This is similar to pragma
   pack's stack, but simpler.  */

/* APPLE LOCAL begin Macintosh alignment 2001-12-17 --ff */
static void push_field_alignment (int, int, int);
/* APPLE LOCAL end Macintosh alignment 2001-12-17 --ff */
static void pop_field_alignment (void);
static const char *find_subframework_file (const char *, const char *);
/* APPLE LOCAL begin iframework for 4.3 4094959 */
/* Remove add_system_framework_path */
/* APPLE LOCAL end iframework for 4.3 4094959 */
static const char *find_subframework_header (cpp_reader *pfile, const char *header,
					     cpp_dir **dirp);

/* APPLE LOCAL begin Macintosh alignment 2002-1-22 --ff */
/* There are four alignment modes supported on the Apple Macintosh
   platform: power, mac68k, natural, and packed.  These modes are
   identified as follows:
     if maximum_field_alignment != 0
       mode = packed
     else if OPTION_ALIGN_NATURAL
       mode = natural
     else if OPTION_ALIGN_MAC68K
       mode
     else
       mode = power
   These modes are saved on the alignment stack by saving the values
   of maximum_field_alignment, OPTION_ALIGN_MAC68K, and
   OPTION_ALIGN_NATURAL.  */
typedef struct align_stack
{
  int alignment;
  unsigned long mac68k;
  unsigned long natural;
  struct align_stack * prev;
} align_stack;
/* APPLE LOCAL end Macintosh alignment 2002-1-22 --ff */

static struct align_stack * field_align_stack = NULL;

/* APPLE LOCAL begin Macintosh alignment 2001-12-17 --ff */
/* APPLE LOCAL begin radar 4679943 */
/* natural_alignment == 0 means "off"
   natural_alignment == 1 means "on"
   natural_alignment == 2 means "unchanged"  */
/* APPLE LOCAL end radar 4679943 */

static void
push_field_alignment (int bit_alignment,
		      int mac68k_alignment, int natural_alignment)
{
  align_stack *entry = XNEW (align_stack);

  entry->alignment = maximum_field_alignment;
  entry->mac68k = OPTION_ALIGN_MAC68K;
  entry->natural = OPTION_ALIGN_NATURAL;
  entry->prev = field_align_stack;
  field_align_stack = entry;

  maximum_field_alignment = bit_alignment;
  if (mac68k_alignment)
    darwin_alignment_flags |= OPTION_MASK_ALIGN_MAC68K;
  else
    darwin_alignment_flags &= ~OPTION_MASK_ALIGN_MAC68K;

  /* APPLE LOCAL begin radar 4679943 */
  if (natural_alignment == 1)
    darwin_alignment_flags |= OPTION_MASK_ALIGN_NATURAL;
  else if (natural_alignment == 0)
    darwin_alignment_flags &= ~OPTION_MASK_ALIGN_NATURAL;
  /* APPLE LOCAL end radar 4679943 */
}
/* APPLE LOCAL end Macintosh alignment 2001-12-17 --ff */

static void
pop_field_alignment (void)
{
  if (field_align_stack)
    {
      align_stack *entry = field_align_stack;

      maximum_field_alignment = entry->alignment;
/* APPLE LOCAL begin Macintosh alignment 2001-12-17 --ff */
      if (entry->mac68k)
	darwin_alignment_flags |= OPTION_MASK_ALIGN_MAC68K;
      else
	darwin_alignment_flags &= ~OPTION_MASK_ALIGN_MAC68K;
      if (entry->natural)
	darwin_alignment_flags |= OPTION_MASK_ALIGN_NATURAL;
      else
	darwin_alignment_flags &= ~OPTION_MASK_ALIGN_NATURAL;
/* APPLE LOCAL end Macintosh alignment 2001-12-17 --ff */
      field_align_stack = entry->prev;
      free (entry);
    }
  else
    error ("too many #pragma options align=reset");
}

/* Handlers for Darwin-specific pragmas.  */

void
darwin_pragma_ignore (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  /* Do nothing.  */
}

/* APPLE LOCAL begin pragma fenv */
/* #pragma GCC fenv
   This is kept in <fenv.h>.  The point is to allow trapping
   math to default to off.  According to C99, any program
   that requires trapping math must include <fenv.h>, so
   we enable trapping math when that gets included.  */

void
darwin_pragma_fenv (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  flag_trapping_math = 1;
}
/* APPLE LOCAL end pragma fenv */

/* #pragma options align={mac68k|power|reset} */

void
darwin_pragma_options (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  const char *arg;
  tree t, x;

  if (pragma_lex (&t) != CPP_NAME)
    BAD ("malformed '#pragma options', ignoring");
  arg = IDENTIFIER_POINTER (t);
  if (strcmp (arg, "align"))
    BAD ("malformed '#pragma options', ignoring");
  if (pragma_lex (&t) != CPP_EQ)
    BAD ("malformed '#pragma options', ignoring");
  if (pragma_lex (&t) != CPP_NAME)
    BAD ("malformed '#pragma options', ignoring");

  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of '#pragma options'");

  arg = IDENTIFIER_POINTER (t);
/* APPLE LOCAL begin Macintosh alignment 2002-1-22 --ff */
  if (!strcmp (arg, "mac68k"))
    {
      if (POINTER_SIZE == 64)
	warning (OPT_Wpragmas, "mac68k alignment pragma is deprecated for 64-bit Darwin");
      push_field_alignment (0, 1, 0);
    }
  else if (!strcmp (arg, "native"))	/* equivalent to power on PowerPC */
    push_field_alignment (0, 0, 0);
  else if (!strcmp (arg, "natural"))
    push_field_alignment (0, 0, 1);
  else if (!strcmp (arg, "packed"))
    push_field_alignment (8, 0, 0);
  else if (!strcmp (arg, "power"))
    push_field_alignment (0, 0, 0);
  else if (!strcmp (arg, "reset"))
    pop_field_alignment ();
  else
    BAD ("malformed '#pragma options align={mac68k|power|natural|reset}', ignoring");
}
/* APPLE LOCAL end Macintosh alignment 2002-1-22 --ff */

/* APPLE LOCAL begin Macintosh alignment 2002-1-22 --ff */
/* #pragma pack ()
   #pragma pack (N)
   #pragma pack (pop[,id])
   #pragma pack (push[,id],N)

   We have a problem handling the semantics of these directives since,
   to play well with the Macintosh alignment directives, we want the
   usual pack(N) form to do a push of the previous alignment state.
   Do we want pack() to do another push or a pop?  */

void
darwin_pragma_pack (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  tree x, id = 0;
  int align = -1;
  enum cpp_ttype token;
  enum { set, push, pop } action;

  if (pragma_lex (&x) != CPP_OPEN_PAREN)
    BAD ("missing '(' after '#pragma pack' - ignored");

  token = pragma_lex (&x);
  if (token == CPP_CLOSE_PAREN)
    {
      action = pop;
      align = 0;
    }
  else if (token == CPP_NUMBER)
    {
      if (TREE_CODE (x) != INTEGER_CST)
	BAD ("invalid constant in %<#pragma pack%> - ignored");
      align = TREE_INT_CST_LOW (x);
      action = push;
      if (pragma_lex (&x) != CPP_CLOSE_PAREN)
	BAD ("malformed '#pragma pack' - ignored");
    }
  else if (token == CPP_NAME)
    {
#define GCC_BAD_ACTION do { if (action == push) \
	  BAD ("malformed '#pragma pack(push[, id], <n>)' - ignored"); \
	else \
	  BAD ("malformed '#pragma pack(pop[, id])' - ignored"); \
	} while (0)

      const char *op = IDENTIFIER_POINTER (x);
      if (!strcmp (op, "push"))
	action = push;
      else if (!strcmp (op, "pop"))
	action = pop;
      else
	BAD2 ("unknown action '%s' for '#pragma pack' - ignored", op);

      while ((token = pragma_lex (&x)) == CPP_COMMA)
        {
          token = pragma_lex (&x);
          if (token == CPP_NAME && id == 0)
            {
              id = x;
            }
          else if (token == CPP_NUMBER && action == push && align == -1)
            {
              if (TREE_CODE (x) != INTEGER_CST)
                BAD ("invalid constant in %<#pragma pack%> - ignored");
              align = TREE_INT_CST_LOW (x);
              if (align == -1)
                action = set;
            }
          else
            GCC_BAD_ACTION;
        }

      if (token != CPP_CLOSE_PAREN)
	GCC_BAD_ACTION;
#undef GCC_BAD_ACTION
    }
else
  BAD ("malformed '#pragma pack' - ignored");

  if (pragma_lex (&x) != CPP_EOF)
    warning (OPT_Wpragmas, "junk at end of '#pragma pack'");

  if (action != pop)
    {
      switch (align)
	{
	  case 0:
	  case 1:
	  case 2:
	  case 4:
	  case 8:
	  case 16:
	    align *= BITS_PER_UNIT;
	    break;
	  case -1:
	    if (action == push)
              {
		align = maximum_field_alignment;
		break;
	      }
	  default:
	    BAD2 ("alignment must be a small power of two, not %d", align);
	}
    }

  switch (action)
    {
    case pop:   pop_field_alignment ();		      break;
    /* APPLE LOCAL begin radar 4679943 */
    case push:  push_field_alignment (align, 0, 2);   break;
    /* APPLE LOCAL end radar 4679943 */
    case set:                                         break;
    }
}
/* APPLE LOCAL end Macintosh alignment 2002-1-22 --ff */

/* #pragma unused ([var {, var}*]) */

void
darwin_pragma_unused (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  tree decl, x;
  int tok;

  /* APPLE LOCAL 5979888 */
  if ((tok=pragma_lex (&x)) != CPP_OPEN_PAREN)
    BAD ("missing '(' after '#pragma unused', ignoring");

  /* APPLE LOCAL 5979888 */
  while (tok != CPP_EOF && tok != CPP_CLOSE_PAREN)
    {
      tok = pragma_lex (&decl);
      if (tok == CPP_NAME && decl)
	{
	  tree local = lookup_name (decl);
	  if (local && (TREE_CODE (local) == PARM_DECL
			|| TREE_CODE (local) == VAR_DECL))
	    TREE_USED (local) = 1;
	  tok = pragma_lex (&x);
	  if (tok != CPP_COMMA)
	    break;
	}
    }

  if (tok != CPP_CLOSE_PAREN)
    BAD ("missing ')' after '#pragma unused', ignoring");

  if (pragma_lex (&x) != CPP_EOF)
    BAD ("junk at end of '#pragma unused'");
}

/* Parse the ms_struct pragma.  */
void
darwin_pragma_ms_struct (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  const char *arg;
  tree t;

  if (pragma_lex (&t) != CPP_NAME)
    BAD ("malformed '#pragma ms_struct', ignoring");
  arg = IDENTIFIER_POINTER (t);

  if (!strcmp (arg, "on"))
    darwin_ms_struct = true;
  else if (!strcmp (arg, "off") || !strcmp (arg, "reset"))
    darwin_ms_struct = false;
  else
    BAD ("malformed '#pragma ms_struct {on|off|reset}', ignoring");

  if (pragma_lex (&t) != CPP_EOF)
    BAD ("junk at end of '#pragma ms_struct'");
}

/* APPLE LOCAL begin pragma reverse_bitfields */
/* Handle the reverse_bitfields pragma.  */

void
darwin_pragma_reverse_bitfields (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  const char* arg;
  tree t;

  if (pragma_lex (&t) != CPP_NAME)
    BAD ("malformed '#pragma reverse_bitfields', ignoring");
  arg = IDENTIFIER_POINTER (t);

  if (!strcmp (arg, "on"))
    darwin_reverse_bitfields = true;
  else if (!strcmp (arg, "off") || !strcmp (arg, "reset"))
    darwin_reverse_bitfields = false;
  else
    BAD ("malformed '#pragma reverse_bitfields {on|off|reset}', ignoring");
  if (pragma_lex (&t) != CPP_EOF)
    BAD ("junk at end of '#pragma reverse_bitfields'");
}
/* APPLE LOCAL end pragma reverse_bitfields */

/* APPLE LOCAL begin optimization pragmas 3124235/3420242 */
/* LLVM LOCAL begin disable optimization pragmas */
void
darwin_pragma_opt_level  (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  if (warn_unknown_pragmas > in_system_header)
    warning (OPT_Wunknown_pragmas, "ignoring #pragma optimization_level");  
  return;
}

void
darwin_pragma_opt_size  (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  if (warn_unknown_pragmas > in_system_header)
    warning (OPT_Wunknown_pragmas, "ignoring #pragma optimize_for_size");
  return;
}
/* LLVM LOCAL end disable optimization pragmas */
/* APPLE LOCAL end optimization pragmas 3124235/3420242 */

static struct {
  size_t len;
  const char *name;
  cpp_dir* dir;
} *frameworks_in_use;
static int num_frameworks = 0;
static int max_frameworks = 0;


/* Remember which frameworks have been seen, so that we can ensure
   that all uses of that framework come from the same framework.  DIR
   is the place where the named framework NAME, which is of length
   LEN, was found.  We copy the directory name from NAME, as it will be
   freed by others.  */

static void
add_framework (const char *name, size_t len, cpp_dir *dir)
{
  char *dir_name;
  int i;
  for (i = 0; i < num_frameworks; ++i)
    {
      if (len == frameworks_in_use[i].len
	  && strncmp (name, frameworks_in_use[i].name, len) == 0)
	{
	  return;
	}
    }
  if (i >= max_frameworks)
    {
      max_frameworks = i*2;
      max_frameworks += i == 0;
      frameworks_in_use = xrealloc (frameworks_in_use,
				    max_frameworks*sizeof(*frameworks_in_use));
    }
  dir_name = XNEWVEC (char, len + 1);
  memcpy (dir_name, name, len);
  dir_name[len] = '\0';
  frameworks_in_use[num_frameworks].name = dir_name;
  frameworks_in_use[num_frameworks].len = len;
  frameworks_in_use[num_frameworks].dir = dir;
  ++num_frameworks;
}

/* Recall if we have seen the named framework NAME, before, and where
   we saw it.  NAME is LEN bytes long.  The return value is the place
   where it was seen before.  */

static struct cpp_dir*
find_framework (const char *name, size_t len)
{
  int i;
  for (i = 0; i < num_frameworks; ++i)
    {
      if (len == frameworks_in_use[i].len
	  && strncmp (name, frameworks_in_use[i].name, len) == 0)
	{
	  return frameworks_in_use[i].dir;
	}
    }
  return 0;
}

/* There are two directories in a framework that contain header files,
   Headers and PrivateHeaders.  We search Headers first as it is more
   common to upgrade a header from PrivateHeaders to Headers and when
   that is done, the old one might hang around and be out of data,
   causing grief.  */

struct framework_header {const char * dirName; int dirNameLen; };
static struct framework_header framework_header_dirs[] = {
  { "Headers", 7 },
  { "PrivateHeaders", 14 },
  { NULL, 0 }
};

/* Returns a pointer to a malloced string that contains the real pathname
   to the file, given the base name and the name.  */

static char *
framework_construct_pathname (const char *fname, cpp_dir *dir)
{
  char *buf;
  size_t fname_len, frname_len;
  cpp_dir *fast_dir;
  char *frname;
  struct stat st;
  int i;

  /* Framework names must have a / in them.  */
  buf = strchr (fname, '/');
  if (buf)
    fname_len = buf - fname;
  else
    return 0;

  fast_dir = find_framework (fname, fname_len);

  /* Framework includes must all come from one framework.  */
  if (fast_dir && dir != fast_dir)
    return 0;

  frname = XNEWVEC (char, strlen (fname) + dir->len + 2
		    + strlen(".framework/") + strlen("PrivateHeaders"));
  strncpy (&frname[0], dir->name, dir->len);
  frname_len = dir->len;
  if (frname_len && frname[frname_len-1] != '/')
    frname[frname_len++] = '/';
  strncpy (&frname[frname_len], fname, fname_len);
  frname_len += fname_len;
  strncpy (&frname[frname_len], ".framework/", strlen (".framework/"));
  frname_len += strlen (".framework/");

  if (fast_dir == 0)
    {
      frname[frname_len-1] = 0;
      if (stat (frname, &st) == 0)
	{
	  /* As soon as we find the first instance of the framework,
	     we stop and never use any later instance of that
	     framework.  */
	  add_framework (fname, fname_len, dir);
	}
      else
	{
	  /* If we can't find the parent directory, no point looking
	     further.  */
	  free (frname);
	  return 0;
	}
      frname[frname_len-1] = '/';
    }

  /* Append framework_header_dirs and header file name */
  for (i = 0; framework_header_dirs[i].dirName; i++)
    {
      strncpy (&frname[frname_len],
	       framework_header_dirs[i].dirName,
	       framework_header_dirs[i].dirNameLen);
      strcpy (&frname[frname_len + framework_header_dirs[i].dirNameLen],
	      &fname[fname_len]);

      if (stat (frname, &st) == 0)
	return frname;
    }

  free (frname);
  return 0;
}

/* Search for FNAME in sub-frameworks.  pname is the context that we
   wish to search in.  Return the path the file was found at,
   otherwise return 0.  */

static const char*
find_subframework_file (const char *fname, const char *pname)
{
  char *sfrname;
  const char *dot_framework = ".framework/";
  char *bufptr;
  int sfrname_len, i, fname_len;
  struct cpp_dir *fast_dir;
  static struct cpp_dir subframe_dir;
  struct stat st;

  bufptr = strchr (fname, '/');

  /* Subframework files must have / in the name.  */
  if (bufptr == 0)
    return 0;

  fname_len = bufptr - fname;
  fast_dir = find_framework (fname, fname_len);

  /* Sub framework header filename includes parent framework name and
     header name in the "CarbonCore/OSUtils.h" form. If it does not
     include slash it is not a sub framework include.  */
  bufptr = strstr (pname, dot_framework);

  /* If the parent header is not of any framework, then this header
     cannot be part of any subframework.  */
  if (!bufptr)
    return 0;

  /* Now translate. For example,                  +- bufptr
     fname = CarbonCore/OSUtils.h                 |
     pname = /System/Library/Frameworks/Foundation.framework/Headers/Foundation.h
     into
     sfrname = /System/Library/Frameworks/Foundation.framework/Frameworks/CarbonCore.framework/Headers/OSUtils.h */

  sfrname = XNEWVEC (char, strlen (pname) + strlen (fname) + 2 +
			      strlen ("Frameworks/") + strlen (".framework/")
			      + strlen ("PrivateHeaders"));

  bufptr += strlen (dot_framework);

  sfrname_len = bufptr - pname;

  strncpy (&sfrname[0], pname, sfrname_len);

  strncpy (&sfrname[sfrname_len], "Frameworks/", strlen ("Frameworks/"));
  sfrname_len += strlen("Frameworks/");

  strncpy (&sfrname[sfrname_len], fname, fname_len);
  sfrname_len += fname_len;

  strncpy (&sfrname[sfrname_len], ".framework/", strlen (".framework/"));
  sfrname_len += strlen (".framework/");

  /* Append framework_header_dirs and header file name */
  for (i = 0; framework_header_dirs[i].dirName; i++)
    {
      strncpy (&sfrname[sfrname_len],
	       framework_header_dirs[i].dirName,
	       framework_header_dirs[i].dirNameLen);
      strcpy (&sfrname[sfrname_len + framework_header_dirs[i].dirNameLen],
	      &fname[fname_len]);

      if (stat (sfrname, &st) == 0)
	{
	  if (fast_dir != &subframe_dir)
	    {
	      if (fast_dir)
		warning (0, "subframework include %s conflicts with framework include",
			 fname);
	      else
		add_framework (fname, fname_len, &subframe_dir);
	    }

	  return sfrname;
	}
    }
  free (sfrname);

  return 0;
}

/* Add PATH to the system includes. PATH must be malloc-ed and
   NUL-terminated.  System framework paths are C++ aware.  */

static void
add_system_framework_path (char *path)
{
  int cxx_aware = 1;
  cpp_dir *p;

  p = XNEW (cpp_dir);
  p->next = NULL;
  p->name = path;
  p->sysp = 1 + !cxx_aware;
  p->construct = framework_construct_pathname;
  using_frameworks = 1;

  add_cpp_dir_path (p, SYSTEM);
}

/* Add PATH to the bracket includes. PATH must be malloc-ed and
   NUL-terminated.  */

void
add_framework_path (char *path)
{
  cpp_dir *p;

  p = XNEW (cpp_dir);
  p->next = NULL;
  p->name = path;
  p->sysp = 0;
  p->construct = framework_construct_pathname;
  using_frameworks = 1;

  add_cpp_dir_path (p, BRACKET);
}

static const char *framework_defaults [] =
  {
    "/System/Library/Frameworks",
    "/Library/Frameworks",
  };

/* Register the GNU objective-C runtime include path if STDINC.  */

void
darwin_register_objc_includes (const char *sysroot, const char *iprefix,
			       int stdinc)
{
  const char *fname;
  size_t len;
  /* We do not do anything if we do not want the standard includes. */
  if (!stdinc)
    return;

  fname = GCC_INCLUDE_DIR "-gnu-runtime";

  /* Register the GNU OBJC runtime include path if we are compiling  OBJC
    with GNU-runtime.  */

  if (c_dialect_objc () && !flag_next_runtime)
    {
      char *str;
      /* See if our directory starts with the standard prefix.
	 "Translate" them, i.e. replace /usr/local/lib/gcc... with
	 IPREFIX and search them first.  */
      if (iprefix && (len = cpp_GCC_INCLUDE_DIR_len) != 0 && !sysroot
	  && !strncmp (fname, cpp_GCC_INCLUDE_DIR, len))
	{
	  str = concat (iprefix, fname + len, NULL);
          /* FIXME: wrap the headers for C++awareness.  */
	  add_path (str, SYSTEM, /*c++aware=*/false, false);
	}

      /* Should this directory start with the sysroot?  */
      if (sysroot)
	str = concat (sysroot, fname, NULL);
      else
	str = update_path (fname, "");

      add_path (str, SYSTEM, /*c++aware=*/false, false);
    }
}


/* Register all the system framework paths if STDINC is true and setup
   the missing_header callback for subframework searching if any
   frameworks had been registered.  */

void
darwin_register_frameworks (const char *sysroot,
			    const char *iprefix ATTRIBUTE_UNUSED, int stdinc)
{
  if (stdinc)
    {
      size_t i;

      /* Setup default search path for frameworks.  */
      for (i=0; i<sizeof (framework_defaults)/sizeof(const char *); ++i)
	{
	  char *str;
	  if (sysroot)
	    str = concat (sysroot, xstrdup (framework_defaults [i]), NULL);
	  else
	    str = xstrdup (framework_defaults[i]);
	  /* System Framework headers are cxx aware.  */
	  add_system_framework_path (str);
	}
    }

  if (using_frameworks)
    cpp_get_callbacks (parse_in)->missing_header = find_subframework_header;
}

/* Search for HEADER in context dependent way.  The return value is
   the malloced name of a header to try and open, if any, or NULL
   otherwise.  This is called after normal header lookup processing
   fails to find a header.  We search each file in the include stack,
   using FUNC, starting from the most deeply nested include and
   finishing with the main input file.  We stop searching when FUNC
   returns nonzero.  */

static const char*
find_subframework_header (cpp_reader *pfile, const char *header, cpp_dir **dirp)
{
  const char *fname = header;
  struct cpp_buffer *b;
  const char *n;

  for (b = cpp_get_buffer (pfile);
       b && cpp_get_file (b) && cpp_get_path (cpp_get_file (b));
       b = cpp_get_prev (b))
    {
      n = find_subframework_file (fname, cpp_get_path (cpp_get_file (b)));
      if (n)
	{
	  /* Logically, the place where we found the subframework is
	     the place where we found the Framework that contains the
	     subframework.  This is useful for tracking wether or not
	     we are in a system header.  */
	  *dirp = cpp_get_dir (cpp_get_file (b));
	  return n;
	}
    }

  return 0;
}

/* Return the value of darwin_macosx_version_min suitable for the
   __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ macro,
   so '10.4.2' becomes 1042.
   Print a warning if the version number is not known.  */
static const char *
/* APPLE LOCAL ARM 5683689 */
macosx_version_as_macro (void)
{
  static char result[] = "1000";

  if (strncmp (darwin_macosx_version_min, "10.", 3) != 0)
    goto fail;
  if (! ISDIGIT (darwin_macosx_version_min[3]))
    goto fail;
  result[2] = darwin_macosx_version_min[3];
  if (darwin_macosx_version_min[4] != '\0')
    {
      if (darwin_macosx_version_min[4] != '.')
	goto fail;
      if (! ISDIGIT (darwin_macosx_version_min[5]))
	goto fail;
      if (darwin_macosx_version_min[6] != '\0')
	goto fail;
      result[3] = darwin_macosx_version_min[5];
    }
  else
    result[3] = '0';

  return result;

 fail:
  error ("Unknown value %qs of -mmacosx-version-min",
	 darwin_macosx_version_min);
  return "1000";
}

/* APPLE LOCAL begin ARM 5683689 */
/* Return the value of darwin_iphoneos_version_min suitable for the
   __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ macro.  Unlike the
   __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ macros, minor version
   numbers are left-zero-padded.  e.g., '1.2.3' becomes 10203.
   The last/third version number (patch level?) is optional, and
   defaults to '00' if not specified.  In the case of a parse error,
   print a warning and return 10200.  */
static const char *
iphoneos_version_as_macro (void)
{
  static char result[sizeof ("99.99.99") + 1];
  const char *src_ptr = darwin_iphoneos_version_min;
  char *result_ptr = &result[0];

  if (! darwin_iphoneos_version_min)
    goto fail;

  if (! ISDIGIT (*src_ptr))
    goto fail;

  /* Copy over the major version number.  */
  *result_ptr++ = *src_ptr++;

  if (ISDIGIT (*src_ptr))
    *result_ptr++ = *src_ptr++;

  if (*src_ptr != '.')
    goto fail;

  src_ptr++;

  /* Start parsing the minor version number.  */
  if (! ISDIGIT (*src_ptr))
    goto fail;

  /* Zero-pad a single-digit value, or copy a two-digit value.  */
  *result_ptr++ = ISDIGIT (*(src_ptr + 1)) ? *src_ptr++ : '0';
  *result_ptr++ = *src_ptr++;

  /* Parse the third version number (patch level?)  */
  if (*src_ptr == '\0')
    {
      /* Not present -- default to zeroes.  */
      *result_ptr++ = '0';
      *result_ptr++ = '0';
    }
  else if (*src_ptr == '.')
    {
      src_ptr++;

      if (! ISDIGIT (*src_ptr))
	goto fail;

      /* Zero-pad a single-digit value, or copy a two-digit value.  */
      *result_ptr++ = ISDIGIT (*(src_ptr + 1)) ? *src_ptr++ : '0';
      *result_ptr++ = *src_ptr++;
    }
  else
    goto fail;

  /* Verify and copy the terminating NULL.  */
  if (*src_ptr != '\0')
    goto fail;
 
  *result_ptr++ = '\0'; 
  return result;
  
 fail:
  error ("Unknown value %qs of -miphoneos-version-min",
	 darwin_iphoneos_version_min);
  return "10200";
}
/* APPLE LOCAL end ARM 5683689 */

/* Define additional CPP flags for Darwin.   */

#define builtin_define(TXT) cpp_define (pfile, TXT)

void
darwin_cpp_builtins (cpp_reader *pfile)
{
  builtin_define ("__MACH__");
  builtin_define ("__APPLE__");

  /* APPLE LOCAL Apple version */
  /* Don't define __APPLE_CC__ here.  */

  /* APPLE LOCAL begin ARM 5683689 */
  if (darwin_macosx_version_min)
    builtin_define_with_value ("__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__",
			       macosx_version_as_macro(), false);
  else
    builtin_define_with_value ("__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__",
			       iphoneos_version_as_macro(), false);
  /* APPLE LOCAL end ARM 5683689 */

  /* APPLE LOCAL begin constant cfstrings */
  if (darwin_constant_cfstrings)
    builtin_define ("__CONSTANT_CFSTRINGS__");
  /* APPLE LOCAL end constant cfstrings */
  /* APPLE LOCAL begin pascal strings */
  if (darwin_pascal_strings)
    {
      builtin_define ("__PASCAL_STRINGS__");
    }
  /* APPLE LOCAL end pascal strings */
  /* APPLE LOCAL begin ObjC GC */
  /* APPLE LOCAL radar 5914395 */
  if (flag_objc_gc || flag_objc_gc_only)
    {
      builtin_define ("__strong=__attribute__((objc_gc(strong)))");
      builtin_define ("__weak=__attribute__((objc_gc(weak)))");
      builtin_define ("__OBJC_GC__");
    }
  else
    {
      builtin_define ("__strong=");
      /* APPLE LOCAL radar 5847976 */
      builtin_define ("__weak=__attribute__((objc_gc(weak)))");
    }
  /* APPLE LOCAL end ObjC GC */
  /* APPLE LOCAL begin radar 5932809 - copyable byref blocks */
  if (flag_blocks) {
    builtin_define ("__block=__attribute__((__blocks__(byref)))");
  }
  /* APPLE LOCAL radar 6230656 */
  /* code removed */
  /* APPLE LOCAL end radar 5932809 - copyable byref blocks */

  /* APPLE LOCAL begin C* warnings to easy porting to new abi */
  if (flag_objc_abi == 2)
    builtin_define ("__OBJC2__");
  /* APPLE LOCAL end C* warnings to easy porting to new abi */
  /* APPLE LOCAL begin radar 5072864 */
  if (flag_objc_zerocost_exceptions)
    builtin_define ("OBJC_ZEROCOST_EXCEPTIONS");
  /* APPLE LOCAL radar 4899595 */
  builtin_define ("OBJC_NEW_PROPERTIES");
  /* APPLE LOCAL end radar 5072864 */
/* APPLE LOCAL begin confused diff */
}
/* APPLE LOCAL end confused diff */
/* APPLE LOCAL begin iframework for 4.3 4094959 */
bool
darwin_handle_c_option (size_t code, const char *arg, int value ATTRIBUTE_UNUSED)
{
  switch (code)
    {
    default:
      /* Options with a flag are otherwise assumed to be handled.  */
      if (cl_options[code].flag_var)
	break;

      /* Unrecognized options that we said we'd handle turn into
	 errors if not listed here if they don't have a flag.  */
      return false;

    case OPT_iframework:
      add_system_framework_path (xstrdup (arg));
      break;
    }
  return true;
}
/* APPLE LOCAL end iframework for 4.3 4094959 */

/* APPLE LOCAL begin radar 4985544 - radar 5096648 - radar 5195402 */
/* Check that TYPE is CFStringRef type. */
bool
objc_check_cfstringref_type (tree type)
{
   tree CFStringRef_decl = lookup_name (get_identifier ("CFStringRef"));
   if (!CFStringRef_decl || TREE_CODE (CFStringRef_decl) != TYPE_DECL)
     return false;
   return type == TREE_TYPE (CFStringRef_decl);
}

/* This routine checks that FORMAT_NUM'th argument ARGUMENT has the 'CFStringRef' type. */
bool
objc_check_format_cfstring (tree argument,
                            unsigned HOST_WIDE_INT format_num,
                            bool *no_add_attrs)
{
  unsigned HOST_WIDE_INT i;
  /* APPLE LOCAL begin 6212507 */
  if (format_num < 1)
    {
      error ("argument number of CFString format cannot be less than one");
      return false;
    }
  /* APPLE LOCAL end 6212507 */
  for (i = 1; i != format_num; i++)
    {
      if (argument == 0)
        break;
       argument = TREE_CHAIN (argument);
    }

  /* LLVM LOCAL begin 7020016 */
  if (argument == NULL_TREE)
    {
      error ("argument number of CFString format too large");
      *no_add_attrs = true;
      return false;
    }
  /* LLVM LOCAL end 7020016 */
  if (!objc_check_cfstringref_type (TREE_VALUE (argument)))
    {
      error ("format CFString argument not an 'CFStringRef' type");
      *no_add_attrs = true;
      return false;
    }
  return true;
}
/* APPLE LOCAL end radar 4985544 - radar 5096648 - radar 5195402 */

/* APPLE LOCAL begin radar 2996215 - 6068877 */
/* wrapper to call libcpp's conversion routine. */
bool
cvt_utf8_utf16 (const unsigned char *inbuf, size_t length, 
		     unsigned char **uniCharBuf, size_t *numUniChars)
{
  return cpp_utf8_utf16 (parse_in, inbuf, length, uniCharBuf, numUniChars);
}
/* This routine declares static char __utf16_string [numUniChars] in __TEXT,__ustring
   section and initializes it with uniCharBuf[numUniChars] characters.
*/ 
tree
create_init_utf16_var (const unsigned char *inbuf, size_t length, size_t *numUniChars)
{
  size_t l;
  tree decl, type, init;
  tree initlist = NULL_TREE;
  tree attribute; 
  const char *section_name = "__TEXT,__ustring";
  int len = strlen (section_name);
  unsigned char *uniCharBuf;
  static int num;
  const char *name_prefix = "__utf16_string_";
  char *name;
  int embedNull = 0;

  if (!cvt_utf8_utf16 (inbuf, length, &uniCharBuf, numUniChars))
    return NULL_TREE;

  /* LLVM LOCAL begin 7589850. */
  /* ustring with embedded null should go into __const. It should not be forced
     into "__TEXT,__ustring" section. */
  for (l = 0; l < length; l++) {
    if (!inbuf[l]) {
      embedNull = 1;
      break;
    }
  }
  /* LLVM LOCAL end 7589850. */

  for (l = 0; l < *numUniChars; l++)
    initlist = tree_cons (NULL_TREE, build_int_cst (char_type_node, uniCharBuf[l]), initlist);
  /* LLVM LOCAL utf16 has two trailing nulls 7095855 */
  type = build_array_type (char_type_node,
                           build_index_type (build_int_cst (NULL_TREE, *numUniChars + 1)));
  name = (char *)alloca (strlen (name_prefix) + 10);
  sprintf (name, "%s%d", name_prefix, ++num);
  decl = build_decl (VAR_DECL, get_identifier (name), type);
  TREE_STATIC (decl) = 1;
  DECL_INITIAL (decl) = error_mark_node;  /* A real initializer is coming... */
  DECL_IGNORED_P (decl) = 1;
  DECL_ARTIFICIAL (decl) = 1;
  DECL_CONTEXT (decl) = NULL_TREE;
  /* LLVM LOCAL begin 7115749 this object is constant. */
  TREE_CONSTANT (decl) = 1;
  TREE_READONLY (decl) = 1;
  /* LLVM LOCAL end */

  /* LLVM LOCAL 7589850 */
  if (!embedNull) {
    attribute = tree_cons (NULL_TREE, build_string (len, section_name), NULL_TREE);
    attribute = tree_cons (get_identifier ("section"), attribute, NULL_TREE);
    decl_attributes (&decl, attribute, 0);
  }
  attribute = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, 2), NULL_TREE);
  attribute = tree_cons (get_identifier ("aligned"), attribute, NULL_TREE);
  decl_attributes (&decl, attribute, 0);
  init = build_constructor_from_list (type, nreverse (initlist));
  TREE_CONSTANT (init) = 1;
  TREE_STATIC (init) = 1;
  TREE_READONLY (init) = 1;
  if (c_dialect_cxx ())
    TREE_TYPE (init) = NULL_TREE;
  finish_decl (decl, init, NULL_TREE);
  /* Ensure that the variable actually gets output.  */
  mark_decl_referenced (decl);
  /* Mark the decl to avoid "defined but not used" warning.  */
  TREE_USED (decl) = 1;
  return decl;
}
/* APPLE LOCAL end radar 2996215 - 6068877 */
