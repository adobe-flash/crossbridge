/* Test diagnostics for options used on their own without
   -Wformat.  -Wformat-security.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-Wno-format -Wformat-security" } */

/* { dg-warning "warning: -Wformat-security ignored without -Wformat" "ignored" { target *-*-* } 0 } */
