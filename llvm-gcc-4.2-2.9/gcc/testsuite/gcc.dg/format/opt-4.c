/* Test diagnostics for options used on their own without
   -Wformat.  -Wformat-y2k.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-Wno-format -Wformat-y2k" } */

/* { dg-warning "warning: -Wformat-y2k ignored without -Wformat" "ignored" { target *-*-* } 0 } */
