/* PR debug/7241 */
/* { dg-do compile } */
/* { dg-options "-O2 -gdwarf-2 -dA" } */
/* APPLE LOCAL begin radar 4874405  */
/* { dg-skip-if "Unmatchable assembly" { mmix-*-* } { "*" } { "" } } */
/* { dg-final { scan-assembler-not "0x5\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } }*/
/* { dg-final { scan-assembler "0x5\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"int\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } }*/
/* { dg-final { scan-assembler "0x6\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } }*/
/* { dg-final { scan-assembler "0x6\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"signed char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } }*/
/* { dg-final { scan-assembler "0x8\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding\[ \t\n\]+\.ascii \"unsigned char\\\\0\"\[ \t\]+\[#@;!/|\+\[\ \t\]+DW_AT_name" { target *-*-darwin* } } }*/
/* { dg-final { scan-assembler "0x\[68\]\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding" { target { ! { *-*-darwin* } } } } } */
/* { dg-final { scan-assembler-not "0x\[57\]\[ \t\]+\[#@;!/|\]+\[ \t\]+DW_AT_encoding" { target { ! { *-*-darwin* } } } } } */
/* APPLE LOCAL end radar 4874405  */

char a;
const char b;
signed char c;
volatile signed char d;
unsigned char e;
volatile const unsigned char f;
