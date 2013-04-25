/* APPLE LOCAL file Macintosh alignment */

/* { dg-do run { target { powerpc*-*-darwin* i?86*-*-darwin* } } } */
/* { dg-options "-Wno-long-long" } */

/*
 * GCC alignment test for bit-fields.
 * This came up initially as an alignment problem in the kernel.
 * Fred Forsman
 * Apple Computer, Inc.
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

/*
 * The following defined determines whether we should compare against
 * the values produced by GCC 2.95 or against the values I expect given
 * my understanding of the alignment rules.
 */
#define USE_GCC2_VALUES 1

#define NAME2(name,mode) mode##_##name
#define NAME(name,mode) NAME2(name,mode)

#define STR(s) #s
#define SIZEOF3(name) STR(sizeof(name)), sizeof(name)
#define SIZEOF2(name,mode) SIZEOF3(mode##_##name)
#define SIZEOF(name,mode) SIZEOF2(name,mode)

#define OFFSETOF3(name,field) STR(offsetof(name,field)), offsetof(name,field)
#define OFFSETOF2(name,mode,field) OFFSETOF3(mode##_##name,field)
#define OFFSETOF(name,mode,field) OFFSETOF2(name,mode,field)

typedef unsigned char UINT8;
typedef unsigned long UINT32;

static int bad_option = 0;
static int flag_verbose = 0;
static int nbr_failures = 0;

/* === mac68k alignment problem in kernel === */

typedef unsigned PEF_UBits32, ByteCount;
typedef short SInt16;

/* === power === */
#pragma options align=power
#define MODE power
#include "align-test-4.h"

#ifndef __LP64__
/* === mac68k === */
#pragma options align=mac68k
#undef MODE
#define MODE mac68k
#include "align-test-4.h"
#endif

/* === pack(2) === */
#pragma pack(2)
#undef MODE
#define MODE pack2
#include "align-test-4.h"


static void check(char * rec_name, int actual, int expected32, int expected64,
		  int expected_ia32, char * comment)
{
    int expected;
#ifdef __i386__
    expected = expected_ia32;
#else
    expected = ((sizeof(char *) == 8) ? expected64 : expected32);
#endif
    if (flag_verbose || (actual != expected)) {
        printf("%-30s = %2d (%2d) ", rec_name, actual, expected);
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

#undef MODE
#define MODE power

#if USE_GCC2_VALUES
    check(SIZEOF(LoaderExport, MODE), 12, 12, 12, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, 8, "offset of sectionNumber");
    check(SIZEOF(S1, MODE), 8, 8, 8, "bitfields & char");
    check(SIZEOF(S2, MODE), 8, 8, 8, "int & char");
    check(SIZEOF(S3, MODE), 12, 12, 12, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 8, 8, 8, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 8, 8, 8, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 7, 7, 7, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 4, 4, 4, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 3, 3, 3, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 4, 4, 4, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 2, 2, 2, "offset of 2nd char");
#else
    check(SIZEOF(LoaderExport, MODE), 12, 12, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, "offset of sectionNumber");
    check(SIZEOF(S1, MODE), 8, 8, "bitfields & char");
    check(SIZEOF(S2, MODE), 8, 8, "int & char");
    check(SIZEOF(S3, MODE), 12, 12, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 8, 8, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 12, 12, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 8, 8, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 12, 12, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 8, 8, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 12, 12, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 8, 8, "offset of 2nd char");
#endif

#ifndef __LP64__
#undef MODE
#define MODE mac68k

#if USE_GCC2_VALUES
    check(SIZEOF(LoaderExport, MODE), 10, 10, 10, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, 8, "offset of sectionNumber");
#if 1
    // GCC 2 is wrong on the following.
    check(SIZEOF(S1, MODE), 6, 6, 6, "bitfields & char");
#else
    check(SIZEOF(S1, MODE), 8, 8, "bitfields & char");
#endif
    check(SIZEOF(S2, MODE), 6, 6, 6, "int & char");
    check(SIZEOF(S3, MODE), 6, 6, 6, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 5, 5, 5, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 6, 6, 6, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 5, 5, 5, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 4, 4, 4, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 3, 3, 3, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 4, 4, 4, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 2, 2, 2, "offset of 2nd char");
#else
    check(SIZEOF(LoaderExport, MODE), 10, 10, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, "offset of sectionNumber");
    check(SIZEOF(S1, MODE), 6, 6, "bitfields & char");
    check(SIZEOF(S2, MODE), 6, 6, "int & char");
    check(SIZEOF(S3, MODE), 8, 8, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 6, 6, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 8, 8, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 6, 6, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 6, 6, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 4, 4, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 4, 4, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 2, 2, "offset of 2nd char");
#endif
#endif /* n __LP64__ */

#undef MODE
#define MODE pack2

#if USE_GCC2_VALUES
    check(SIZEOF(LoaderExport, MODE), 10, 10, 10, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, 8, "offset of sectionNumber");
    /* GCC2 used to have this as '8', but it should really be 6.  */
    check(SIZEOF(S1, MODE), 6, 6, 6, "bitfields & char");
    check(SIZEOF(S2, MODE), 6, 6, 6, "int & char");
    check(SIZEOF(S3, MODE), 6, 6, 6, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 5, 5, 5, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 6, 6, 6, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 5, 5, 5, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 4, 4, 4, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 3, 3, 3, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 4, 4, 4, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 2, 2, 2, "offset of 2nd char");
#else
    check(SIZEOF(LoaderExport, MODE), 10, 10, "kernel struct");
    check(OFFSETOF(LoaderExport, MODE, offset), 4, 4, "offset of offset");
    check(OFFSETOF(LoaderExport, MODE, sectionNumber), 8, 8, "offset of sectionNumber");
    check(SIZEOF(S1, MODE), 6, 6, "bitfields & char");
    check(SIZEOF(S2, MODE), 6, 6, "int & char");
    check(SIZEOF(S3, MODE), 8, 8, "char, bitfields(32), char");
    check(OFFSETOF(S3, MODE, f3), 6, 6, "offset of 2nd char");
    check(SIZEOF(S4, MODE), 8, 8, "char, bitfields(32), char");
    check(OFFSETOF(S4, MODE, f3), 6, 6, "offset of 2nd char");
    check(SIZEOF(S5, MODE), 6, 6, "char, bitfields(16), char");
    check(OFFSETOF(S5, MODE, f3), 4, 4, "offset of 2nd char");
    check(SIZEOF(S6, MODE), 4, 4, "char, bitfields(8), char");
    check(OFFSETOF(S6, MODE, f3), 2, 2, "offset of 2nd char");
#endif

    if (nbr_failures > 0)
    	return 1;
    else
    	return 0;
}
