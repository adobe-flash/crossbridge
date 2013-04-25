/* Test diagnostics for options used on their own without
   -Wformat.  -Wformat-nonliteral.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* APPLE LOCAL default to Wformat-security 5764921 */
/* { dg-options "-Wno-format -Wformat-nonliteral" } */

/* { dg-warning "warning: -Wformat-nonliteral ignored without -Wformat" "ignored" { target *-*-* } 0 } */
