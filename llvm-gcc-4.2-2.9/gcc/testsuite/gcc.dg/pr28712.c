/* PR c/28712 */
/* { dg-do compile } */
/* { dg-options "--combine" } */
/* { dg-additional-sources "pr28712.c pr28712.c" } */
/* APPLE LOCAL 4881841 */
/* { dg-skip-if "" { *-*-* } { "-ObjC" } { "" } } */

struct A;

extern struct A *a;

struct A { } __attribute__((packed));

struct B __attribute__((aligned (sizeof (int))));

extern struct B *b;

struct B { int i; } __attribute__((packed));
