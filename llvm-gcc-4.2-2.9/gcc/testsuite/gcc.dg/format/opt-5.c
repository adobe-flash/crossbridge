/* Test diagnostics for options used on their own without
   -Wformat.  -Wformat-zero-length.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-Wno-format -Wformat-zero-length" } */

/* { dg-warning "warning: -Wformat-zero-length ignored without -Wformat" "ignored" { target *-*-* } 0 } */
