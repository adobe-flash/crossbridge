/* APPLE LOCAL file */
/* Test constant cfstring and writabe strings interaction.
   Radar 3978580.  */
/* Developed by Devang Patel <dpatel@apple.com>.  */

/* { dg-do assemble { target *-*-darwin* } } */
/* { dg-options "-fconstant-cfstrings -fwritable-strings" } */

typedef const struct __CFString *CFStringRef;

#ifdef __CONSTANT_CFSTRINGS__
#define CFSTR(STR)  ((CFStringRef) __builtin___CFStringMakeConstantString (STR))
#else
#error __CONSTANT_CFSTRINGS__ not defined
#endif

extern void  bar (const void **);

static void foo()
{
                CFStringRef keys[] =
                {       
                        CFSTR("blah1"),
                        CFSTR("blah2") 
                };      
                int count = sizeof(keys) / sizeof(keys[0]);

                bar ((const void **)keys);
}

