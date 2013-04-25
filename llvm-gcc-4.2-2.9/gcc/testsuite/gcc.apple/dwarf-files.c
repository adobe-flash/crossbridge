/* APPLE LOCAL file  */
/* Radar 5645155 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-c -g -fasm-blocks" } */
/* { dg-final { scan-assembler-times "dwarf-files.c" 2 } } */
asm(".globl _x\n"
    "_x:\n"
    "pushl      %ebp\n");
