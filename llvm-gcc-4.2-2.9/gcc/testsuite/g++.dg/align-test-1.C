/* APPLE LOCAL file Macintosh alignment */

/* { dg-do run } */
/* { dg-options "-Wno-long-long -Wno-invalid-offsetof" } */

/*
 * Macintosh compiler alignment test for C++.
 * Fred Forsman
 * Apple Computer, Inc.
 */
 
#include <stdio.h>
#include <stddef.h>
#include <string.h>

extern "C" void abort (void);

#define Q(x) #x, x

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;

static int bad_option = 0;
static int flag_verbose = 0;
static int nbr_failures = 0;

/* === classes === */

class C1 {
    static const int	f1 = 1;
    UINT8		f2;
};

class C2 {
    static int		f1;
    UINT8		f2;
};

class C3 {
  public:
    enum E1 {
    	f1 = 1
    };
  protected:
    UINT8		f2;
};

class C4 {
    UINT8		f1;
    static const int	f2 = 1;
};

class C5 {
    UINT8		f2;
    static int		f1;
};

class C6 {
    UINT8		f1;
    enum E1 {
    	f2 = 1
    };
};

class C7 {
    /* empty base class */
};

#ifndef __LP64__
#pragma options align=mac68k

class C8 {
    /* empty base class */
};

class C9: public C8 {
  public:
    UINT8		f1;
};

#pragma options align=reset
#endif /* n __LP64 __ */

/* What is offset of first field after an empty base class? */
class C10: public C7 {
  public:
    UINT8		f1; 
};

/* Check that we no longer try to put derived class bits in padding at end of base class. */
class C11 {
  public:
    UINT32		f1;
    UINT8		f2; 
};

class C12: public C11 {
  public:
    UINT8		f3; 
};

/* Check whether compiler will reorder members to take advantage of
   padding.  If the compiler did this (which it does not appear to
   do), f3 and f4 in C14 would be reordered to take advantage of the
   padding at the end of the base class. */
class C13 {
  public:
    UINT32		f1;
    UINT16		f2; 
};

class C14: public C13 {
  public:
    UINT32		f3; 
    UINT16		f4; 
};

/* Tests for double aligned base class */

class C15 {
  public:
    double		f1;
    long		f2; 
};

class C16: public C15 {
};

class C17: public C15 {
  public:
    long		f3; 
};

class C18: public C16 {
  public:
    char		f3; 
};

class C19: public C17 {
  public:
    char		f4; 
};

/* Tests for alignment in class with v-table pointer */

class C20 {
  public:
    double		f1;
    virtual void func1(void);
};

/* === vectors === */

#ifdef __APPLE_ALTIVEC__
class VC1 {
  public:
    vector signed short f1;
    UINT8		f2;
};

typedef struct VS1 {
    VC1		f1;
    UINT8	f2;
} VS1;

class VC2: public VC1 {
  public:
    UINT8		f1;
};

typedef struct VS2 {
    UINT8	f1;
    VC2		f2;
    UINT8	f3;
} VS2;

class VC3 {
  public:
    vector signed short f1;
    virtual void func1(void);
};

#endif

/* === bools === */

typedef struct B1 {
    bool	f1;
    UINT8	f2;
} B1;

typedef struct B2 {
    UINT8	f1;
    bool	f2;
} B2;


/* APPLE LOCAL begin ARM alignments */
/* APPLE LOCAL begin radar 4869885 */
static void check(const char * rec_name, int actual, int expected32, int expected64, 
		  int expected_ia32, int expected_arm, const char * comment)
/* APPLE LOCAL end radar 4869885 */
{
    int expected;
#ifdef __i386__
    expected = expected_ia32;
#elif defined (__arm__)
    expected = expected_arm;
#else
    expected = ((sizeof(char *) == 8) ? expected64 : expected32);
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
/* APPLE LOCAL end ARM alignments */

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

/* APPLE LOCAL begin ARM alignments */
int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)
        check_option(argv[i]);
    
    if (bad_option)
        return 1;

    check(Q(sizeof(C1)), 1, 1, 1, 1, "const as 1st field");
    check(Q(sizeof(C2)), 1, 1, 1, 1, "static as 1st field");
    check(Q(sizeof(C3)), 1, 1, 1, 1, "enum as 1st field");
    check(Q(sizeof(C4)), 1, 1, 1, 1, "const as 2nd field");
    check(Q(sizeof(C5)), 1, 1, 1, 1, "static as 2nd field");
    check(Q(sizeof(C6)), 1, 1, 1, 1, "enum as 2nd field");
    check(Q(sizeof(C7)), 1, 1, 1, 1, "empty class, power mode");
#ifndef __LP64__
    check(Q(sizeof(C8)), 2, 2, 2, 1, "empty class, mac68k mode");
    check(Q(sizeof(C9)), 2, 2, 2, 1, "class with empty base class and one char, mac68k");
    check(Q(offsetof(C9, f1)), 0, 0, 0, 0, "offset of 1st field after empty base class");
#endif
    check(Q(sizeof(C10)), 1, 1, 1, 1, "class based on an empty class, power mode");
    check(Q(sizeof(C11)), 8, 16, 8, 8, "class with long, char");
    check(Q(sizeof(C12)), 12, 24, 12, 12, "class with base class with long, char and its own char");
    check(Q(offsetof(C12, f3)), 8, 16, 8, 8, "offset of 1st field in class with a base class with a long, char");
    check(Q(sizeof(C13)), 8, 16, 8, 8, "class with long, short");
    check(Q(sizeof(C14)), 16, 32, 16, 16, "derived class with short, long");
    check(Q(offsetof(C14, f3)), 8, 16, 8, 8, "offset of 1st field after base class with padding");
    check(Q(offsetof(C14, f4)), 12, 24, 12, 12, "offset of 2nd field after base class with padding");

    check(Q(sizeof(C15)), 16, 16, 12, 12, "base class with double, long");
    check(Q(sizeof(C16)), 16, 16, 12, 12, "empty derived class with base with double, long");
    check(Q(sizeof(C17)), 24, 24, 16, 16, "derived class with base with double, long and its own long");
    check(Q(sizeof(C18)), 20, 24, 16, 16, "derived class based on empty derived class with base with double, long");
    check(Q(sizeof(C19)), 24, 32, 20, 20, "derived class based on derived class with base with double, long and its own long");
    check(Q(sizeof(C20)), 12, 16, 12, 12, "class with double and v-table ptr");
    check(Q(offsetof(C20, f1)), 4, 8, 4, 4, "offset of double 1st field in class with v-table ptr");

    /* Vector tests */
#ifdef __APPLE_ALTIVEC__
    check(Q(sizeof(VC1)), 32, 32, 32, 32, "class with vector as 1st field");
    check(Q(sizeof(VS1)), 48, 48, 48, 48, "struct with a class with a vector as 1st field");
    check(Q(sizeof(VC2)), 48, 48, 48, 48, "class with base class containing a vector");
    check(Q(offsetof(VC2, f1)), 32, 32, 32, 32, "offset of 1st field after base class with vector, char, and padding");
    check(Q(sizeof(VS2)), 80, 80, 80, 80, "struct with a char, class with a vector, char");
    check(Q(offsetof(VS2, f2)), 16, 16, 16, 16, "offset of class with a vector in a struct with char, class...");
    check(Q(offsetof(VS2, f3)), 64, 64, 64, 64, "offset of 2nd char in a struct with char, class, char");
    check(Q(sizeof(VC3)), 32, 32, 32, 32, "class with a vector and v-table ptr");
    check(Q(offsetof(VC3, f1)), 16, 16, 16, 16, "offset vector in class with a vector and v-table ptr");
#endif

    /* bool tests */
    check(Q(sizeof(bool)), 4, 1, 1, 1, "bool data type");
    check(Q(sizeof(B1)), 8, 2, 2, 2, "struct with bool, char");
    check(Q(sizeof(B2)), 8, 2, 2, 2, "struct with char, bool");

    if (nbr_failures > 0)
    	return 1;
    else
    	return 0;
}
/* APPLE LOCAL end ARM alignments */
