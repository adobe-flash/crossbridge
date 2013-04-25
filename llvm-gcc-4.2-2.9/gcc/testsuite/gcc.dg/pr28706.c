/* PR c/28706 */
/* { dg-do compile } */
/* { dg-options "--combine" } */
/* { dg-additional-sources "pr28706.c" } */
/* APPLE LOCAL 4881841 */
/* { dg-skip-if "" { *-*-* } { "-ObjC" } { "" } } */

struct A
{
  int i;
} __attribute__((aligned (sizeof (long int))));

extern void foo (struct A *);
extern void foo (struct A *);
