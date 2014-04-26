/* APPLE LOCAL file preprocess .s files */

/* Regression test - in assembly language, # may have some significance
   other than 'stringize macro argument' and therefore must be preserved
   in the output, and should not be warned about.  */

/* { dg-do preprocess } */

#define foo() mov r0, #5  /* { dg-bogus "not followed" "spurious warning" } */

entry:
	foo()

/* Check we don't EOF on an unknown directive.  */
#unknown directive
#error a later diagnostic	/* { dg-error "diagnostic" } */
