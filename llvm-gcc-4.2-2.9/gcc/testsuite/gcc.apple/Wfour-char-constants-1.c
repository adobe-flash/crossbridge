/* APPLE LOCAL file -Wfour-char-constants */
/* We warn by default on Darwin, so no specific option needed.  */

/* { dg-do compile { target "*-*-darwin*" } } */
/* { dg-options "" } */

int glob1 = 'a';
int glob2 = 'ab';	/* { dg-warning "multi-character character constant" } */
int glob3 = 'abc';	/* { dg-warning "multi-character character constant" } */
int glob4 = 'abcd';     /* say nothing */
int glob5 = 'abcde';	/* { dg-warning "character constant too long" } */

