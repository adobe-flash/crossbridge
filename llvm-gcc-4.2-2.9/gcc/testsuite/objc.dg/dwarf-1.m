/* { dg-options "-gdwarf-2 -dA" } */
/* LLVM LOCAL allow for asciz instead of ascii */
/* { dg-final { scan-assembler "\"id(.0)?\".*DW_AT_name" } } */
/* { dg-skip-if "No Dwarf" { { *-*-aix* hppa*-*-hpux* *-*-solaris2.[56]* } && { ! hppa*64*-*-* } } { "*" } { "" } } */
@interface foo
  id x;
@end
