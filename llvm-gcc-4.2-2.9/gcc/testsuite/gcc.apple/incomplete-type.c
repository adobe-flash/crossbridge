/* APPLE LOCAL file radar 4745307 */
/* { dg-do compile } */

struct A X[(927 - 37) / sizeof (struct A)]; /* { dg-error "" } */
