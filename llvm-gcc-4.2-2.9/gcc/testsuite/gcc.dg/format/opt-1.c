/* Test diagnostics for options used on their own without
   -Wformat.  -Wformat-extra-args.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-Wno-format -Wformat-extra-args" } */

/* { dg-warning "warning: -Wformat-extra-args ignored without -Wformat" "ignored" { target *-*-* } 0 } */
