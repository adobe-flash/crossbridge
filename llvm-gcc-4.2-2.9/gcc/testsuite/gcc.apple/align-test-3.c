/* APPLE LOCAL file Macintosh alignment */

/* { dg-do run { target { powerpc*-*-darwin* i?86*-*-darwin* } } } */
/* { dg-options "-Wno-long-long" } */

/*
 * GCC alignment test for alignment problems due to interactions 
 * between FSF and Macintosh alignment modes.
 * Fred Forsman
 * Apple Computer, Inc.
 */

#ifdef __LP64__
int main()
{
  return 0;
}
#else /* 32-bit */
 
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

/* === alignment modes === */

#pragma options align=power
#pragma options align=mac68k
#pragma pack(1)

typedef struct S0 {
    UINT32	f1;
    UINT8	f2;
} S0;

#pragma options align=reset

/* We should be back in mac68k alignment, if #pragma option align=reset
   can rest a #pragma pack(n).  So check for mac68k alignment.  */

typedef struct S1 {
    UINT32	f1;
    UINT8	f2;
} S1;

#if GCC3
#pragma options align=power
#pragma options align=mac68k
//#pragma pack(push, 1)
//#pragma pack(pop)
#pragma pack(1)
#pragma pack()

/* We should be back in mac68k alignment, if #pragma pack()
   can reset a #pragma pack(n).  So check for mac68k alignment.  */

typedef struct S2 {
    UINT32	f1;
    UINT8	f2;
} S2;
#endif /* GCC3 */

static void check(char * rec_name, int actual, int expected_ppc32, int expected_ia32, char * comment)
{
    int expected;
#ifdef __i386__
    expected = expected_ia32;
#else
    expected = expected_ppc32;
#endif
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

    check(Q(sizeof(S0)), 5, 5, "struct with 1 long, 1 char; pack(1) mode");
    check(Q(sizeof(S1)), 6, 6, "struct with 1 long, 1 char; should be mac68k mode");
#if GCC3
    check(Q(sizeof(S2)), 6, 6, "struct with 1 long, 1 char; should be mac68k mode");
#endif

    if (nbr_failures > 0)
    	return 1;
    else
    	return 0;
}

#endif /* 32-bit */
