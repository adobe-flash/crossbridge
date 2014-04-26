/* { dg-do compile } */ 
/* { dg-options "-fasm-blocks" } */

int i = 1st;	/* { dg-error "invalid suffix.* on integer constant" } */
