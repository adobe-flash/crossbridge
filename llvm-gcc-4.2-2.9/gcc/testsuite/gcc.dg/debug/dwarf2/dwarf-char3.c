/* PR debug/7241 */
/* { dg-do compile } */
/* { dg-options "-O2 -gdwarf-2 -dA" } */
/* { dg-skip-if "Unmatchable assembly" { mmix-*-* } { "*" } { "" } } */
/* APPLE LOCAL begin radar 4874405  */
/* { dg-final { scan-assembler-not "0x5\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } } */
/* { dg-final { scan-assembler "0x5\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"int\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } } */
/* { dg-final { scan-assembler "0x6\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } } */
/* { dg-final { scan-assembler "0x\[68\]\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding" { target { ! { *-*-darwin* } } } } } */
/* { dg-final { scan-assembler-not "0x\[57\]\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding" { target { ! { *-*-darwin* } } } } } */
/* APPLE LOCAL end radar 4874405  */

const char *p = "abc";
