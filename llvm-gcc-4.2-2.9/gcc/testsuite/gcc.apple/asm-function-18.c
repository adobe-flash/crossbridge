/* APPLE LOCAL file CW asm blocks */
/* Test that gcc does not issue error on use of r31 (pic register) */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks -faltivec" } */

asm void saveNonvolatileRegistersPPC(register unsigned long *saveBuffer)
{
        // zero offset register
        xor r10, r10, r10
        // store stack pointer  (4 bytes)
        stwx r1, r3, r10
        addi r10, r10, 4
        // store gpr's 13 through 31  (72 bytes)
        stwx r13, r3, r10
        addi r10, r10, 4
        stwx r14, r3, r10
        addi r10, r10, 4
        stwx r15, r3, r10
        addi r10, r10, 4
        stwx r16, r3, r10
        addi r10, r10, 4
        stwx r17, r3, r10
        addi r10, r10, 4
        stwx r18, r3, r10
        addi r10, r10, 4
        stwx r19, r3, r10
        addi r10, r10, 4
        stwx r20, r3, r10
        addi r10, r10, 4
        stwx r21, r3, r10
        addi r10, r10, 4
        stwx r22, r3, r10
        addi r10, r10, 4
        stwx r23, r3, r10
        addi r10, r10, 4
        stwx r24, r3, r10
        addi r10, r10, 4
        stwx r25, r3, r10
        addi r10, r10, 4
        stwx r26, r3, r10
        addi r10, r10, 4
        stwx r27, r3, r10
        addi r10, r10, 4
        stwx r28, r3, r10
        addi r10, r10, 4
        stwx r29, r3, r10
        addi r10, r10, 4
        stwx r30, r3, r10
        addi r10, r10, 4
        stwx r31, r3, r10
        addi r10, r10, 4

        // Store VRSAVE (4 bytes)
        mfspr r11, 256
        stwx r11, r3, r10
        addi r10, r10, 4

        // store vector registers 20 - 31 (176 bytes)
        stvx v20, r3, r10
        addi r10, r10, 16
        stvx v21, r3, r10
        addi r10, r10, 16
        stvx v22, r3, r10
        addi r10, r10, 16
        stvx v23, r3, r10
        addi r10, r10, 16
        stvx v24, r3, r10
        addi r10, r10, 16
        stvx v25, r3, r10
        addi r10, r10, 16
        stvx v26, r3, r10
        addi r10, r10, 16
        stvx v27, r3, r10
        addi r10, r10, 16
        stvx v28, r3, r10
        addi r10, r10, 16
        stvx v29, r3, r10
        addi r10, r10, 16
        stvx v30, r3, r10
        addi r10, r10, 16
        stvx v31, r3, r10
       
        blr
}
