/* APPLE LOCAL file Macintosh alignment */
/* align-test-5*.c are all the same code but with different options.  */

/* { dg-do run { target "i?86-*-darwin*" } } */
/* { dg-options "-malign-natural -DSIZE=12" } */

/* This is for Intel only. */
/* { dg-require-effective-target ilp32 } */

/*
 * GCC alignment test for command line options for setting alignment modes.
 * Fred Forsman
 * Apple Computer, Inc.
 * (C) 2000-2002.
 * Last modified 2002-2-18.
 *
 */

 /* Check whether we are testing GCC 3 or later.  */
#ifdef __GNUC__
#if __GNUC__ >= 3
#define GCC3 1
#else
#define GCC3 0
#endif
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define Q(x) #x, x

typedef unsigned char UINT8;
typedef unsigned long UINT32;

static int bad_option = 0;
static int flag_verbose = 0;
static int nbr_failures = 0;

typedef struct S0 {
    UINT8	f1;
    double	f2;
} S0;

static void check(char * rec_name, int actual, int expected, char * comment)
{
    if (flag_verbose || (actual != expected)) {
        printf("%-20s = %2d (%2d) ", rec_name, actual, expected);
        if (actual != expected) {
            printf("*** FAIL");
            nbr_failures++;
        } else
            printf("    PASS");
        printf(": %s\n", comment);
    }
}

static void check_option(char *option)
{
    if (*option == '-') {
        if (strcmp(option, "-v") == 0)
            flag_verbose = 1;
        else {
            fprintf(stderr, "*** unrecognized option '%s'.\n", option);
            bad_option = 1;
        }
    } else {
        fprintf(stderr, "*** unrecognized option '%s'.\n", option);
        bad_option = 1;
    }
}

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)
        check_option(argv[i]);

    if (bad_option)
        return 1;

    check(Q(sizeof(S0)), SIZE, "struct with 1 char, 1 double");

    if (nbr_failures > 0)
    	return 1;
    else
    	return 0;
}
