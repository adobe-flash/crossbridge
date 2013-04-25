/* APPLE LOCAL file -Wfour-char-constants */
/* Explicitly enable the warning.  */

/* { dg-do compile } */
/* { dg-options "-Wfour-char-constants" } */

int glob1 = 'a';
int glob2 = 'ab';	/* { dg-warning "multi-character character constant" } */
int glob3 = 'abc';	/* { dg-warning "multi-character character constant" } */
int glob4 = 'abcd';     /* { dg-warning "multi-character character constant" } */
int glob5 = 'abcde';	/* { dg-warning "character constant too long" } */

