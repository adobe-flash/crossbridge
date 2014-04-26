/* Set up combined include path chain for the preprocessor.
   Copyright (C) 1986, 1987, 1989, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   Broken out of cppinit.c and cppfiles.c and rewritten Mar 2003.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "machmode.h"
#include "target.h"
#include "tm.h"
#include "cpplib.h"
#include "prefix.h"
#include "intl.h"
#include "c-incpath.h"
#include "cppdefault.h"
/* APPLE LOCAL headermaps 3871393 */ 
#include "errors.h"
/* LLVM LOCAL begin - sysroot */
#ifdef ENABLE_LLVM
#include "target-def.h"
#endif
/* LLVM LOCAL end - sysroot */

/* Windows does not natively support inodes, and neither does MSDOS.
   Cygwin's emulation can generate non-unique inodes, so don't use it.
   VMS has non-numeric inodes.  */
#ifdef VMS
# define INO_T_EQ(A, B) (!memcmp (&(A), &(B), sizeof (A)))
# define INO_T_COPY(DEST, SRC) memcpy(&(DEST), &(SRC), sizeof (SRC))
#else
# if (defined _WIN32 && !defined (_UWIN)) || defined __MSDOS__
#  define INO_T_EQ(A, B) 0
# else
#  define INO_T_EQ(A, B) ((A) == (B))
# endif
# define INO_T_COPY(DEST, SRC) (DEST) = (SRC)
#endif

static const char dir_separator_str[] = { DIR_SEPARATOR, 0 };

static void add_env_var_paths (const char *, int);
static void add_standard_paths (const char *, const char *, const char *, int);
static void free_path (struct cpp_dir *, int);
static void merge_include_chains (cpp_reader *, int);
static struct cpp_dir *remove_duplicates (cpp_reader *, struct cpp_dir *,
					   struct cpp_dir *,
					   struct cpp_dir *, int);

/* Include chains heads and tails.  */
static struct cpp_dir *heads[4];
static struct cpp_dir *tails[4];
static bool quote_ignores_source_dir;
enum { REASON_QUIET = 0, REASON_NOENT, REASON_DUP, REASON_DUP_SYS };

/* Free an element of the include chain, possibly giving a reason.  */
static void
free_path (struct cpp_dir *path, int reason)
{
  switch (reason)
    {
    case REASON_DUP:
    case REASON_DUP_SYS:
      fprintf (stderr, _("ignoring duplicate directory \"%s\"\n"), path->name);
      if (reason == REASON_DUP_SYS)
	/* APPLE LOCAL default to Wformat-security 5764921 */
	fprintf (stderr, "%s",
 _("  as it is a non-system directory that duplicates a system directory\n"));
      break;

    case REASON_NOENT:
      fprintf (stderr, _("ignoring nonexistent directory \"%s\"\n"),
	       path->name);
      break;

    case REASON_QUIET:
    default:
      break;
    }

  free (path->name);
  free (path);
}

/* Read ENV_VAR for a PATH_SEPARATOR-separated list of file names; and
   append all the names to the search path CHAIN.  */
static void
add_env_var_paths (const char *env_var, int chain)
{
  char *p, *q, *path;

  GET_ENVIRONMENT (q, env_var);

  if (!q)
    return;

  for (p = q; *q; p = q + 1)
    {
      q = p;
      while (*q != 0 && *q != PATH_SEPARATOR)
	q++;

      if (p == q)
	path = xstrdup (".");
      else
	{
	  path = XNEWVEC (char, q - p + 1);
	  memcpy (path, p, q - p);
	  path[q - p] = '\0';
	}

      add_path (path, chain, chain == SYSTEM, false);
    }
}

/* LLVM LOCAL begin sysroot */
#ifdef ENABLE_LLVM
char *default_build_sysroot_path(const char *sysroot, const char *path);
char *
default_build_sysroot_path(const char *sysroot, const char *path) {
  return concat (sysroot, path, NULL);
}
#endif
/* LLVM LOCAL end sysroot */

/* Append the standard include chain defined in cppdefault.c.  */
static void
add_standard_paths (const char *sysroot, const char *iprefix,
		    const char *imultilib, int cxx_stdinc)
{
  const struct default_include *p;
  size_t len;

  if (iprefix && (len = cpp_GCC_INCLUDE_DIR_len) != 0)
    {
      /* Look for directories that start with the standard prefix.
	 "Translate" them, i.e. replace /usr/local/lib/gcc... with
	 IPREFIX and search them first.  */
      for (p = cpp_include_defaults; p->fname; p++)
	{
	  if (!p->cplusplus || cxx_stdinc)
	    {
	      /* Should we be translating sysrooted dirs too?  Assume
		 that iprefix and sysroot are mutually exclusive, for
		 now.  */
	      if (sysroot && p->add_sysroot)
		continue;
	      if (!strncmp (p->fname, cpp_GCC_INCLUDE_DIR, len))
		{
		  char *str = concat (iprefix, p->fname + len, NULL);
		  if (p->multilib && imultilib)
		    str = concat (str, dir_separator_str, imultilib, NULL);
		  add_path (str, SYSTEM, p->cxx_aware, false);
		}
	    }
	}
    }

  for (p = cpp_include_defaults; p->fname; p++)
    {
      if (!p->cplusplus || cxx_stdinc)
	{
	  char *str;

	  /* Should this directory start with the sysroot?  */
	  if (sysroot && p->add_sysroot)
            /* LLVM LOCAL begin sysroot */
#ifdef ENABLE_LLVM
	    str = TARGET_BUILD_SYSROOT_PATH(sysroot, p->fname);
#else
	    str = concat (sysroot, p->fname, NULL);
#endif
            /* LLVM LOCAL end sysroot */
	  else
	    str = update_path (p->fname, p->component);

	  if (p->multilib && imultilib)
	    str = concat (str, dir_separator_str, imultilib, NULL);

	  add_path (str, SYSTEM, p->cxx_aware, false);
	}
    }
}


/* APPLE LOCAL begin headermaps 3871393 */ 
#include <stdint.h>

/* Private function that hashes the contents of the null-terminated
   string in a case-insensitive way.  For use by headermaps only.  */

static inline 
uint32_t hmap_hash_string (const char *str)
{
  const char *sp;
  unsigned hash_code = 0;

  for (sp = str; *sp; sp++)
    hash_code += TOLOWER (*(const unsigned char *)sp) * 13;
  return hash_code;
}

/* Private function that case-insensitively compares the null-
   terminated string STR1 with the null-terminated string STR2.
   Returns 1 if equal, 0 if not.  For use by headermaps only.  */

static inline unsigned 
hmap_compare_strings (const char *str1, const char *str2)
{
  const char *s1p;
  const char *s2p;

  for (s1p = str1, s2p = str2; *s1p && *s2p; s1p++, s2p++)
    {
      if (TOLOWER (*s1p) != TOLOWER (*s2p))
	return 0;
    }
  return (*s1p == '\0' && *s2p == '\0');
}

/* Attempts to load a headermap from the file at path FILEPATH, and
   returns a pointer to the loaded data.  Returns NULL if for any
   reason the headermap cannot be loaded.  This function is silent if
   the given file does not appear to be a headermap; this is useful
   for checking whether or not it in fact is.  */

static struct hmap_header_map *
hmap_load_header_map (const char *filepath)
{
  struct hmap_header_map *headermap = NULL;

  if (filepath != NULL && filepath[0] != '\0')
    {
      FILE *        f;
      struct stat   f_info;

      /* Try to open the file for reading. */
      f = fopen (filepath, "rb");
      if (f != NULL)
	{
	  /* If it is a regular file and if it is large enough to be a header-
	     map, see if it really is one. */
	  if (fstat (fileno (f), &f_info) == 0 && S_ISREG(f_info.st_mode)
            /* LLVM LOCAL begin */
              && (unsigned) f_info.st_size >= sizeof(struct hmap_header_map))
            /* LLVM LOCAL end */
	    {
	      unsigned   headermap_size = f_info.st_size;

	      headermap = (struct hmap_header_map *) xmalloc (headermap_size);
	      if (fread (headermap, 1, headermap_size, f) != headermap_size)
		{
		  /* For some reason we could not read the entire file. */
		  fprintf (stderr,
		    "warning: could not read \"%s\" (%s)\n",
		    filepath, xstrerror (errno));
		  free (headermap);
		  headermap = NULL;
		}
	      if (headermap->version == 1 && headermap->_reserved == 0)
		{
		  /* It might be a headermap. */
		  if (headermap->magic == HMAP_OPPOSITE_ENDIANNESS_MAGIC)
		    {
		      /* It's a headermap, but has the wrong endianness. */
		      fprintf (stderr,
			"warning: headermap \"%s\" has incompatible endianness\n",
			filepath);
		      free (headermap);
		      headermap = NULL;
		    }
		  else if (headermap->magic != HMAP_SAME_ENDIANNESS_MAGIC)
		    {
		      /* It's not a headermap after all. */
		      free (headermap);
		      headermap = NULL;
		    }
		}
	      else
		{
		   /* It's not a headermap after all. */
		   free (headermap);
		   headermap = NULL;
		}
	    }
	  fclose (f);
        }
    }
  return headermap;
}

/* Returns a pointer to a malloced string that contains the real pathname
   to the file, given the DIR that represents the headermap and given the
   name to be looked up.  This function looks in the headermap associated
   with DIR, if any.  If there is no headermap associated with DIR, or if
   it does not contain FILENAME, this function returns NULL.  The CONSTRUCT
   field of a cpp_dir that represents a headermap points to this function. */

static char *
hmap_construct_pathname (const char *filename, cpp_dir *dir)
{
  if (dir->header_map)
    {
      struct hmap_header_map *headermap;
      const char *strings;
      struct hmap_bucket *buckets;
      uint32_t bucket_mask;
      uint32_t i;
      uint32_t key_offset;

      headermap = (struct hmap_header_map *)dir->header_map;
      strings = ((const char *)headermap) + headermap->strings_offset;
      buckets = headermap->buckets;
      bucket_mask = headermap->capacity-1;
      i = hmap_hash_string (filename) & bucket_mask;
      while ((key_offset = buckets[i].key) != HMAP_NOT_A_KEY)
	{
	  if (hmap_compare_strings (filename, strings+key_offset))
	    {
	      char *result_path;
	      unsigned prefix_length;
	      unsigned suffix_length;

	      prefix_length = strlen(strings + buckets[i].value.prefix);
	      suffix_length = strlen(strings + buckets[i].value.suffix);
	      result_path = xmalloc(prefix_length + suffix_length + 1);
	      memcpy(result_path,
		     strings + buckets[i].value.prefix,
		     prefix_length);
	      memcpy(result_path + prefix_length,
		     strings + buckets[i].value.suffix,
		     suffix_length);
	      result_path[prefix_length + suffix_length] = '\0';
	      return result_path;
	    }
	  i = (i + 1) & bucket_mask;
	}
      /* didn't find header in header map -- return NULL.  */
      return NULL;
    }
  else
    return NULL;
}
/* APPLE LOCAL end headermaps 3871393 */ 


/* For each duplicate path in chain HEAD, keep just the first one.
   Remove each path in chain HEAD that also exists in chain SYSTEM.
   Set the NEXT pointer of the last path in the resulting chain to
   JOIN, unless it duplicates JOIN in which case the last path is
   removed.  Return the head of the resulting chain.  Any of HEAD,
   JOIN and SYSTEM can be NULL.  */

static struct cpp_dir *
remove_duplicates (cpp_reader *pfile, struct cpp_dir *head,
		   struct cpp_dir *system, struct cpp_dir *join,
		   int verbose)
{
  struct cpp_dir **pcur, *tmp, *cur;
  struct stat st;

  for (pcur = &head; *pcur; )
    {
      int reason = REASON_QUIET;

      cur = *pcur;

      if (stat (cur->name, &st))
	{
	  /* Dirs that don't exist are silently ignored, unless verbose.  */
	  if (errno != ENOENT)
	    cpp_errno (pfile, CPP_DL_ERROR, cur->name);
	  else
	    {
	      /* If -Wmissing-include-dirs is given, warn.  */
	      cpp_options *opts = cpp_get_options (pfile);
	      if (opts->warn_missing_include_dirs && cur->user_supplied_p)
		cpp_errno (pfile, CPP_DL_WARNING, cur->name);
	      reason = REASON_NOENT;
	    }
	}
      else if (!S_ISDIR (st.st_mode))
	/* APPLE LOCAL begin headermaps 3871393 */
	{
	  /* Only check for headermap if this is a regular file and if there
	     is no path-constructor function in CUR. */
	  if (S_ISREG (st.st_mode) && !cur->construct)
	    {
	      /* Try to load the file as a headermap.  We will get back NULL
	         if this fails, and there won't be any warnings/errors emitted
	         unless the load function is fairly sure it's dealing with a
	         headermap file that is malformed.  */
	      struct hmap_header_map *header_map;

	      header_map = hmap_load_header_map (cur->name);
	      if (header_map)
		{
		  /* Successfully loaded a headermap.  Store a pointer to it
		     and set up the construct function pointer in cur.  */
		  cur->header_map = header_map;
		  cur->construct = hmap_construct_pathname;
		  pcur = &cur->next;
		  continue;
		}
	    }

	    /* If we fall through to here, it's some other kind of file.  */
	    cpp_error_with_line (pfile, CPP_DL_ERROR, 0, 0,
				 "%s: not a directory", cur->name);
	}
        /* APPLE LOCAL end headermaps 3871393 */
      else
	{
	  INO_T_COPY (cur->ino, st.st_ino);
	  cur->dev  = st.st_dev;

	  /* Remove this one if it is in the system chain.  */
	  reason = REASON_DUP_SYS;
	  for (tmp = system; tmp; tmp = tmp->next)
	   if (INO_T_EQ (tmp->ino, cur->ino) && tmp->dev == cur->dev
	       && cur->construct == tmp->construct)
	      break;

	  if (!tmp)
	    {
	      /* Duplicate of something earlier in the same chain?  */
	      reason = REASON_DUP;
	      for (tmp = head; tmp != cur; tmp = tmp->next)
	       if (INO_T_EQ (cur->ino, tmp->ino) && cur->dev == tmp->dev
		   && cur->construct == tmp->construct)
		  break;

	      if (tmp == cur
		  /* Last in the chain and duplicate of JOIN?  */
		  && !(cur->next == NULL && join
		       && INO_T_EQ (cur->ino, join->ino)
		      && cur->dev == join->dev
		      && cur->construct == join->construct))
		{
		  /* Unique, so keep this directory.  */
		  pcur = &cur->next;
		  continue;
		}
	    }
	}

      /* Remove this entry from the chain.  */
      *pcur = cur->next;
      free_path (cur, verbose ? reason: REASON_QUIET);
    }

  *pcur = join;
  return head;
}

/* Merge the four include chains together in the order quote, bracket,
   system, after.  Remove duplicate dirs (as determined by
   INO_T_EQ()).

   We can't just merge the lists and then uniquify them because then
   we may lose directories from the <> search path that should be
   there; consider -iquote foo -iquote bar -Ifoo -Iquux.  It is
   however safe to treat -iquote bar -iquote foo -Ifoo -Iquux as if
   written -iquote bar -Ifoo -Iquux.  */

static void
merge_include_chains (cpp_reader *pfile, int verbose)
{
  /* Join the SYSTEM and AFTER chains.  Remove duplicates in the
     resulting SYSTEM chain.  */
  if (heads[SYSTEM])
    tails[SYSTEM]->next = heads[AFTER];
  else
    heads[SYSTEM] = heads[AFTER];
  heads[SYSTEM] = remove_duplicates (pfile, heads[SYSTEM], 0, 0, verbose);

  /* Remove duplicates from BRACKET that are in itself or SYSTEM, and
     join it to SYSTEM.  */
  heads[BRACKET] = remove_duplicates (pfile, heads[BRACKET], heads[SYSTEM],
				      heads[SYSTEM], verbose);

  /* Remove duplicates from QUOTE that are in itself or SYSTEM, and
     join it to BRACKET.  */
  heads[QUOTE] = remove_duplicates (pfile, heads[QUOTE], heads[SYSTEM],
				    heads[BRACKET], verbose);

  /* If verbose, print the list of dirs to search.  */
  if (verbose)
    {
      struct cpp_dir *p;

      /* APPLE LOCAL default to Wformat-security 5764921 */
      fprintf (stderr, "%s", _("#include \"...\" search starts here:\n"));
      for (p = heads[QUOTE];; p = p->next)
	{
	  if (p == heads[BRACKET])
	    /* APPLE LOCAL default to Wformat-security 5764921 */
	    fprintf (stderr, "%s", _("#include <...> search starts here:\n"));
	  if (!p)
	    break;
	  /* APPLE LOCAL begin 5033355 */
	  if (p->construct == 0)
	    fprintf (stderr, " %s\n", p->name);
	  else if (p->construct == hmap_construct_pathname)
	    fprintf (stderr, " %s (headermap)\n", p->name);
	  else
	    fprintf (stderr, " %s (framework directory)\n", p->name);
	  /* APPLE LOCAL end 5033355 */
	}
      /* APPLE LOCAL default to Wformat-security 5764921 */
      fprintf (stderr, "%s", _("End of search list.\n"));
    }
}

/* Use given -I paths for #include "..." but not #include <...>, and
   don't search the directory of the present file for #include "...".
   (Note that -I. -I- is not the same as the default setup; -I. uses
   the compiler's working dir.)  */
void
split_quote_chain (void)
{
  heads[QUOTE] = heads[BRACKET];
  tails[QUOTE] = tails[BRACKET];
  heads[BRACKET] = NULL;
  tails[BRACKET] = NULL;
  /* This is NOT redundant.  */
  quote_ignores_source_dir = true;
}

/* Add P to the chain specified by CHAIN.  */

void
add_cpp_dir_path (cpp_dir *p, int chain)
{
  if (tails[chain])
    tails[chain]->next = p;
  else
    heads[chain] = p;
  tails[chain] = p;
}

/* Add PATH to the include chain CHAIN. PATH must be malloc-ed and
   NUL-terminated.  */
void
add_path (char *path, int chain, int cxx_aware, bool user_supplied_p)
{
  cpp_dir *p;

#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  /* Convert all backslashes to slashes.  The native CRT stat()
     function does not recognize a directory that ends in a backslash
     (unless it is a drive root dir, such "c:\").  Forward slashes,
     trailing or otherwise, cause no problems for stat().  */
  char* c;
  for (c = path; *c; c++)
    if (*c == '\\') *c = '/';
#endif

  p = XNEW (cpp_dir);
  p->next = NULL;
  p->name = path;
  if (chain == SYSTEM || chain == AFTER)
    p->sysp = 1 + !cxx_aware;
  else
    p->sysp = 0;
  p->construct = 0;
  p->user_supplied_p = user_supplied_p;

  add_cpp_dir_path (p, chain);
}

/* Exported function to handle include chain merging, duplicate
   removal, and registration with cpplib.  */
void
register_include_chains (cpp_reader *pfile, const char *sysroot,
			 const char *iprefix, const char *imultilib,
			 int stdinc, int cxx_stdinc, int verbose)
{
  static const char *const lang_env_vars[] =
    { "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH",
      "OBJC_INCLUDE_PATH", "OBJCPLUS_INCLUDE_PATH" };
  cpp_options *cpp_opts = cpp_get_options (pfile);
  size_t idx = (cpp_opts->objc ? 2: 0);

  if (cpp_opts->cplusplus)
    idx++;
  else
    cxx_stdinc = false;

  /* CPATH and language-dependent environment variables may add to the
     include chain.  */
  add_env_var_paths ("CPATH", BRACKET);
  add_env_var_paths (lang_env_vars[idx], SYSTEM);

  target_c_incpath.extra_pre_includes (sysroot, iprefix, stdinc);

  /* Finally chain on the standard directories.  */
  if (stdinc)
    add_standard_paths (sysroot, iprefix, imultilib, cxx_stdinc);

  target_c_incpath.extra_includes (sysroot, iprefix, stdinc);

  merge_include_chains (pfile, verbose);

  cpp_set_include_chains (pfile, heads[QUOTE], heads[BRACKET],
			  quote_ignores_source_dir);
}
#if !(defined TARGET_EXTRA_INCLUDES) || !(defined TARGET_EXTRA_PRE_INCLUDES)
static void hook_void_charptr_charptr_int (const char *sysroot ATTRIBUTE_UNUSED,
					   const char *iprefix ATTRIBUTE_UNUSED,
					   int stdinc ATTRIBUTE_UNUSED)
{
}
#endif

#ifndef TARGET_EXTRA_INCLUDES
#define TARGET_EXTRA_INCLUDES hook_void_charptr_charptr_int
#endif
#ifndef TARGET_EXTRA_PRE_INCLUDES
#define TARGET_EXTRA_PRE_INCLUDES hook_void_charptr_charptr_int
#endif

struct target_c_incpath_s target_c_incpath = { TARGET_EXTRA_PRE_INCLUDES, TARGET_EXTRA_INCLUDES };

