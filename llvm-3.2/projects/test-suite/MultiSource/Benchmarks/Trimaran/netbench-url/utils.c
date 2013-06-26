/*
 * utils.c - Some utility functions common to several applications
 *
 * This file is a part of the NetBench suite by Gokhan Memik
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The suite comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * You are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 */

#include <stdio.h>

#ifdef __GNUC__
void _fatal(char *file, char *func, int line, char *fmt, ...)
#else
void fatal(char *fmt, ...)
#endif
{
  fprintf(stderr, "fatal: %s", fmt);
#ifdef __GNUC__
  fprintf(stderr, " [%s:%s, line %d]", func, file, line);
#endif
  fprintf(stderr, "\n");
  exit(1);
}

