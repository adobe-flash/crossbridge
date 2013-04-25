/* APPLE LOCAL file radar 4646337 */
/* align-test-5*.c are all the same code but with different options.  */

/* { dg-do run { target *-*-darwin* } } */

/*
 * GCC alignment test for command line options for setting alignment modes.
 * Bill Wendling
 * Apple Computer, Inc.
 * Copyright (C) 2006
 * Last modified 2006-8-10
 */
 
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#if defined(__x86_64__) || defined(__ppc64__)
/* 64-bit platforms say that the structure will be 16 bytes */
#define SIZE 16
#else
/* Others claim 12 bytes */
#define SIZE 12
#endif

static int bad_option = 0;
static int flag_verbose = 0;
static int nbr_failures = 0;

typedef struct S0 {
  unsigned char f1;
  double f2;
} S0;

static void check (const char* rec_name, int actual, int expected,
		   const char* comment)
{
  if (flag_verbose || actual != expected)
    {
      printf ("%-20s = %2d (%2d) ", rec_name, actual, expected);

      if (actual != expected)
	{
	  printf ("*** FAIL");
	  ++nbr_failures;
        }
      else if (flag_verbose)
	printf ("    PASS");

      printf (": %s\n", comment);
    }
}

static void check_option (char* option)
{
  if (*option == '-')
    {
      if (strcmp (option, "-v") == 0)
	flag_verbose = 1;
      else
	{
	  fprintf (stderr, "*** unrecognized option '%s'.\n", option);
	  bad_option = 1;
	}
    }
  else
    {
      fprintf (stderr, "*** unrecognized option '%s'.\n", option);
      bad_option = 1;
    }
}

int main (int argc, char* argv[])
{
  int i;
    
  for (i = 1; i < argc; i++)
    check_option (argv[i]);
    
  if (bad_option)
    return 1;

#define M(x) "sizeof ("#x")", sizeof (x)

  check(M(S0), SIZE, "struct with 1 char, 1 double");

  return nbr_failures;
}
