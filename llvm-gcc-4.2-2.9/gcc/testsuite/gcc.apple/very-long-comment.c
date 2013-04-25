/*
This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment. This is a really long comment.
 */
#warning test warning /* { dg-warning "test warning" } */
#include <stdio.h>

int main()
{
  printf("This is line %d\n", __LINE__); 
  return 0;
}

/* { dg-options "-Wall" } */
/* { dg-do compile } */
