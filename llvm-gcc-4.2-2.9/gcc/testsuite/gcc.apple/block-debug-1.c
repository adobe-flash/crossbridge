/* APPLE LOCAL file 5939894 */
/* Verify that the desired debugging type is generated for a structure
   member that is a pointer to a block.  */

/* { dg-do compile } */
/* LLVM LOCAL -fverbose-asm */
/* { dg-options "-g -O0 -fblocks -dA -fverbose-asm" } */
/* { dg-final { scan-assembler "__block_descriptor.*DW_AT_name" } } */
/* { dg-final { scan-assembler "__block_literal_generic.*DW_AT_name" } } */

struct inStruct {
  void (^genericBlockPtr)();
} is;

