/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//### FIXME: Assert that we require ARMv7 or later.

#include "nanojit.h"

#if defined(FEATURE_NANOJIT) && defined(NANOJIT_THUMB2)

namespace nanojit
{

#ifdef NJ_VERBOSE
const char* regNames[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9","r10", "fp", "ip", "sp", "lr", "pc",
                           "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9","s10","s11","s12","s13","s14","s15",
                          "s16","s17","s18","s19","s20","s21","s22","s23","s24","s25","s26","s27","s28","s29","s30","s31",
                          "d16","d17","d18","d19","d20","d21","d22","d23","d24","d25","d26","d27","d28","d29","d30","d31",
                           "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9","d10","d11","d12","d13","d14","d15",
                           "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9","q10","q11","q12","q13","q14","q15",
                         };

const char* condNames[] = {"eq","ne","cs","cc","mi","pl","vs","vc","hi","ls","ge","lt","gt","le",""/*al*/,"nv"};
const char* shiftNames[] = { "lsl", "lsl", "lsr", "lsr", "asr", "asr", "ror", "ror" };
#endif

const Register RegAlloc::argRegs[] = { R0, R1, R2, R3 };
const Register RegAlloc::retRegs[] = { R0, R1 };
// FIXME: We neither use nor save callee-saved FP registers.
const Register RegAlloc::savedRegs[] = { R4, R5, R6, R7, R8, R9, R10 };

const RegisterMask FpArgRegs=      (RegisterMask)0x00000000ffff0000LL; // Q0-3, D0-D7, or S0-S15
const RegisterMask LowBankFPMask = FpSRegs; // Q0-7, D0-D15, or S0-S31
const RegisterMask ARM_REG_MASKS[] = {
    0x0000000000000001ULL , // r0, 
    0x0000000000000002ULL , // r1, 
    0x0000000000000004ULL , // r2, 
    0x0000000000000008ULL , // r3, 
    0x0000000000000010ULL , // r4, 
    0x0000000000000020ULL , // r5, 
    0x0000000000000040ULL , // r6, 
    0x0000000000000080ULL , // r7, 
    0x0000000000000100ULL , // r8, 
    0x0000000000000200ULL , // r9, 
    0x0000000000000400ULL , // r10, 
    0x0000000000000800ULL , // fp,
    0x0000000000001000ULL , // ip, 
    0x0000000000002000ULL , // sp, 
    0x0000000000004000ULL , // lr, 
    0x0000000000008000ULL , // pc, 
    0x0000000000010000ULL , // s0
    0x0000000000020000ULL , // s1
    0x0000000000040000ULL , // s2
    0x0000000000080000ULL , // s3
    0x0000000000100000ULL , // s4
    0x0000000000200000ULL , // s5
    0x0000000000400000ULL , // s6
    0x0000000000800000ULL , // s7
    0x0000000001000000ULL , // s8
    0x0000000002000000ULL , // s9
    0x0000000004000000ULL , // s10
    0x0000000008000000ULL , // s11
    0x0000000010000000ULL , // s12
    0x0000000020000000ULL , // s13
    0x0000000040000000ULL , // s14
    0x0000000080000000ULL , // s15
    0x0000000100000000ULL , // s16
    0x0000000200000000ULL , // s17
    0x0000000400000000ULL , // s18
    0x0000000800000000ULL , // s19
    0x0000001000000000ULL , // s20
    0x0000002000000000ULL , // s21
    0x0000004000000000ULL , // s22
    0x0000008000000000ULL , // s23
    0x0000010000000000ULL , // s24
    0x0000020000000000ULL , // s25
    0x0000040000000000ULL , // s26
    0x0000080000000000ULL , // s27
    0x0000100000000000ULL , // s28
    0x0000200000000000ULL , // s29
    0x0000400000000000ULL , // s30
    0x0000800000000000ULL , // s31
    0x0001000000000000ULL , // d16
    0x0002000000000000ULL , // d17
    0x0004000000000000ULL , // d18
    0x0008000000000000ULL , // d19
    0x0010000000000000ULL , // d20
    0x0020000000000000ULL , // d21
    0x0040000000000000ULL , // d22
    0x0080000000000000ULL , // d23
    0x0100000000000000ULL , // d24
    0x0200000000000000ULL , // d25
    0x0400000000000000ULL , // d26
    0x0800000000000000ULL , // d27
    0x1000000000000000ULL , // d28
    0x2000000000000000ULL , // d29
    0x4000000000000000ULL , // d30
    0x8000000000000000ULL , // d31
    0x0000000000030000ULL , // d0=s0,s1
    0x00000000000C0000ULL , // d1=s2,s3
    0x0000000000300000ULL , // d2=s4,s5
    0x0000000000C00000ULL , // d3=s6,s7
    0x0000000003000000ULL , // d4=s8,s9
    0x000000000C000000ULL , // d5=s10,s11
    0x0000000030000000ULL , // d6=s12,s13
    0x00000000C0000000ULL , // d7=s14,s15
    0x0000000300000000ULL , // d8=s16,s17
    0x0000000C00000000ULL , // d9=s18,s19
    0x0000003000000000ULL , // d10=s20,s21
    0x000000C000000000ULL , // d11=s22,s23
    0x0000030000000000ULL , // d12=s24,s25
    0x00000C0000000000ULL , // d13=s26,s27
    0x0000300000000000ULL , // d14=s28,s29
    0x0000C00000000000ULL , // d15=s30,s31
    0x00000000000F0000ULL , // q0=d0,d1=s0,s1,s2,s3
    0x0000000000F00000ULL , // q1=d2,d3=s4,s5,s6,s7
    0x000000000F000000ULL , // q2=d4,d5=s8,s9,s10,s11
    0x00000000F0000000ULL , // q3=d6,d7=s12,s13,s14,s15
    0x0000000F00000000ULL , // q4=d8,d9=s16,s17,s18,s19
    0x000000F000000000ULL , // q5=d10,d11=s20,s21,s22,s23
    0x00000F0000000000ULL , // q6=d12,d13=s24,s25,s26,s27
    0x0000F00000000000ULL , // q7=d14,d15=s28,s29,s30,s31
    0x0003000000000000ULL , // q8=d16,d17
    0x000C000000000000ULL , // q9=d18,d19
    0x0030000000000000ULL , // q10=d20,d21
    0x00C0000000000000ULL , // q11=d22,d23
    0x0300000000000000ULL , // q12=d24,d25
    0x0C00000000000000ULL , // q13=d26,d27
    0x3000000000000000ULL , // q14=d28,d29
    0xC000000000000000ULL , // q15=d30,d31
};

// --------------------------------
// Code buffer management.
// --------------------------------

#ifdef DEBUG
class ReserveContiguousSpace {
private:
    int size;
    bool save;
    Assembler* asmblr;
public:
    ReserveContiguousSpace(Assembler* as, int bytes)
    {
        as->underrunProtect(bytes);
        save = as->insnsMustBeContiguous;
        as->insnsMustBeContiguous = true;
        asmblr = as;
    }

    ~ReserveContiguousSpace()
    {
        asmblr->insnsMustBeContiguous = save;
    }
};
#else
inline void ReserveContiguousSpace(Assembler* as, int bytes)
{
    as->underrunProtect(bytes);
}
#endif

// --------------------------------
// ARM-specific utility functions.
// --------------------------------

#ifdef DEBUG
// Return true if enc is a valid Thumb2 encoding and thus can be used as-is
// in an ARM arithmetic operation that accepts such encoding.
//
// This utility does not know (or determine) the actual value that the encoded
// value represents, and thus cannot be used to ensure the correct operation of
// encThumb2Imm, but it does ensure that the encoded value can be used to encode a
// valid ARM instruction. decThumb2Imm can be used if you also need to check that
// a literal is correctly encoded (and thus that encThumb2Imm is working
// correctly).
inline bool
Assembler::isThumb2Imm(uint32_t enc)
{
    return ((enc & 0xfff) == enc);
}

// Decodes Thumb2 immediate values (for debug output and assertions).
// See section A6.3.2 of the ARM Architecture Reference Manual,
// "Modified immediate constants in Thumb instructions".
inline uint32_t
Assembler::decThumb2Imm(uint32_t enc)
{
    NanoAssert(isThumb2Imm(enc));

    uint32_t bits11_10 = (enc >> 10) & 0x3;
    uint32_t bits9_8 = (enc >> 8) & 0x3;
    uint32_t bits7_0 = enc & 0xff;

    if (bits11_10 == 0x0) {
        switch (bits9_8) {
        case 0x0:
            return bits7_0;
        case 0x1:
            NanoAssert(bits7_0 != 0);
            return (bits7_0 << 16) | bits7_0;
        case 0x2:
            NanoAssert(bits7_0 != 0);
            return (bits7_0 << 24) | (bits7_0 << 16);
        case 0x3:
            NanoAssert(bits7_0 != 0);
            return (bits7_0 << 24) | (bits7_0 << 16) | (bits7_0 << 8) | bits7_0;
        default:
            NanoAssert(false);
            return 0;
        }
    } else {
        uint32_t bits11_7 = (enc >> 7) & 0x1f;
        // Using a left shift instead of a rotate right is only
        // valid if the rotate count is at least 8.  The encoding
        // guarantees this, since we've peeled off the case of
        // bits11_10 == 0 above.
        NanoAssert(bits11_7 >= 8);
        // Compute 1:bits6_0 ROR bits11_7
        return (bits7_0 | 0x80) << (32 - bits11_7);
    }
}
#endif

// Calculate the number of leading zeroes in data.
static inline uint32_t
CountLeadingZeroesSlow(uint32_t data)
{
    // Other platforms must fall back to a C routine. This won't be as
    // efficient as the CLZ instruction, but it is functional.
    uint32_t    try_shift;

    uint32_t    leading_zeroes = 0;

    // This loop does a bisection search rather than the obvious rotation loop.
    // This should be faster, though it will still be no match for CLZ.
    for (try_shift = 16; try_shift != 0; try_shift /= 2) {
        uint32_t    shift = leading_zeroes + try_shift;
        if (((data << shift) >> shift) == data) {
            leading_zeroes = shift;
        }
    }

    return leading_zeroes;
}

inline uint32_t 
Assembler::CountLeadingZeroes(uint32_t data)
{
    uint32_t    leading_zeroes;

#if defined(__ARMCC__)
    // ARMCC can do this with an intrinsic.
    leading_zeroes = __clz(data);
#elif defined(__GNUC__)
    leading_zeroes = __builtin_clz(data);
#else
    leading_zeroes = CountLeadingZeroesSlow(data);
#endif

    // Assert that the operation worked!
    NanoAssert(((0xffffffff >> leading_zeroes) & data) == data);

    return leading_zeroes;
}

// The Thumb2 instruction set uses a complex encoding scheme for immediate operands
// that allows representation of certain values outside the normal range of a 12-bit
// field, including short bit masks placed anywhere within a 32-bit word.  The encoding
// scheme is documented in section A6.3.2 of the ARM Architecture Reference Manual
// (ARM DDI 0406C).
//
// This function will return true to indicate that the encoding was successful,
// or false to indicate that the literal could not be encoded as an operand 2
// immediate. If successful, the encoded value will be written to *enc.
inline bool
Assembler::encThumb2Imm(uint32_t literal, uint32_t * enc)
{
    // Check the literal to see if it is a simple 8-bit value. I suspect that
    // most literals are in fact small values, so doing this check early should
    // give a decent speed-up.
    if (literal < 256)
    {
        *enc = literal;
        return true;
    }

    if (literal == 0xffffffff) {
        // Optimize for -1, which is common and encodes compactly.
        *enc = 0x3ff;
        return true;
    }

    // Determine the number of leading zeroes in the literal. This is used to
    // calculate the required rotation.
    uint32_t leading_zeroes = CountLeadingZeroes(literal);

    // We've already done a check to see if the literal is an 8-bit value, so
    // leading_zeroes must be less than (and not equal to) (32-8)=24. However,
    // if it is greater than 24, this algorithm will break, so debug code
    // should use an assertion here to check that we have a value that we
    // expect.
    NanoAssert(leading_zeroes < 24);

    // Assuming that we have a field of no more than 8 bits for a valid
    // literal, we can calculate the required rotation by subtracting
    // leading_zeroes from (32-8):
    //
    // Example:
    //      0: Known to be zero.
    //      1: Known to be one.
    //      X: Either zero or one.
    //      .: Zero in a valid operand 2 literal.
    //
    //  Literal:     [ 1XXXXXXX ........ ........ ........ ]
    //  leading_zeroes = 0
    //  Therefore rot (left) = 24.
    //  Rotated 8-bit literal:                  [ 1XXXXXXX ]
    //
    //  Literal:     [ ........ ..1XXXXX XX...... ........ ]
    //  leading_zeroes = 10
    //  Therefore rot (left) = 14.
    //  Rotated 8-bit literal:                  [ 1XXXXXXX ]

    uint32_t rot_left = 24 - leading_zeroes;
    uint32_t imm8 = literal >> rot_left;

    // The validity of the literal can be checked by reversing the
    // calculation. It is much easier to decode the immediate than it is to
    // encode it!
    if (literal != (imm8 << rot_left)) {
        // The encoding is not valid, so report the failure. Calling code
        // should use some other method of loading the value (such as LDR).
        return false;
    }

    // By construction, the high-order bit must be one.
    NanoAssert((imm8 & 0x80) != 0);

    // The operand is valid, so encode it.
    // Note that the Thumb2 encoding is actually described by a rotate to the _right_.
    // Calculating a left shift (rather than calculating a right rotation) simplifies
    // the above code.
    uint32_t rot_right = 32 - rot_left;

    // Using a left shift is only valid in case the rotate count is >= 8, which is
    // also required by the encoding, which assigns special meaning to counts 0..7.
    NanoAssert(rot_right >= 8);

    // Note that the high bit of imm8 is implicitly one, so it need not appear
    // in the encoding.
    *enc = ((rot_right << 7) & 0xf80) | (imm8 & 0x7f);

    // Make sure that we haven't spilled over into the encodings for the
    // non-rotated special-case forms.  See Table A6-11.
    NanoAssert(((*enc >> 10) & 0x3) != 0x0);

    // Assert that the operand was properly encoded.
    NanoAssert(decThumb2Imm(*enc) == literal);

    return true;
}

//### FIXME: We are generating the two halfwords of a 32-bit instruction in the wrong
//### order.  As a temporary expedient, we'll just swap the two halfwords rather than
//### revising all of the instruction templates.
static inline NIns32 swap(NIns32 insn)
{
    return (NIns32)((uint32_t)insn >> 16 | (uint32_t)insn << 16);
}

static inline NIns32 unpack32(NIns* insn)
{
    NIns32 r = insn[1] << 16 | insn[0];
    NanoAssert(r == *(NIns32*)insn);
    return r;
}

inline void Assembler::emitT16(NIns t16)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    *(--_nIns) = t16;
}

inline void Assembler::emitT32(NIns32 t32)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    _nIns -= 2;
    *((NIns32*)_nIns) = swap(t32);
}

inline void Assembler::emitImm(int32_t t32)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    _nIns -= 2;
    *((NIns32*)_nIns) = t32;
}

inline void Assembler::emitPC(NIns* pc)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    NIns32 target = (NIns32)((uintptr_t)pc | 0x1);  // Thumb target
    _nIns -= 2;
    *((NIns32*)_nIns) = target;                     // <address>
}

inline void Assembler::emitT32At(NIns* ins, NIns32 t32)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    *((NIns32*)ins) = swap(t32);
}

inline void Assembler::emitImmAt(NIns* ins, int32_t t32)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    *((NIns32*)ins) = t32;
}

inline void Assembler::emitPCAt(NIns* ins, NIns* pc)
{
    NanoAssert((((uintptr_t)_nIns) & 0x1) == 0);
    NIns32 target = (NIns32)((uintptr_t)pc | 0x1);  // Thumb target
    *((NIns32*)ins) = target;                       // <address>
}

// --------------------------------
// ARM instructions (Thumb2)
// --------------------------------

/*
 * ALU operations
 */

// General Thumb2 scheme for encoding ALU operations with an immediate operand.
// Example: A8.8.13:  AND (immediate) encoding T1
// Example: A8.8.4:   ADD (immediate, Thumb) encoding T3
// Some operations support alternate encodings with a different range of immediate operand.
// We choose this one because it gives us essentially the same range as the classic ARM encoding
// and works uniformly across all operations that we care about.
// Moves and tests will also fit this scheme, with reserved values for registers or the S bit.
static inline NIns32
ALU_imm_insn(uint32_t op, uint32_t S, Register rd, Register rl, uint32_t op2imm)
{
    // A6.3.1 Table A6-10: Data-processing (modified immediate)
    return 0xF0000000 | ((op2imm >> 11) & 0x1) << 26 | op << 21 | S << 20 | rl << 16 | ((op2imm >> 8) & 0x7) << 12 | rd << 8 | (op2imm & 0xff);
}

// ALU operation with register and 8-bit immediate arguments (with rotation, etc.)
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  op2imm  - operand 2 immediate. Use encThumb2Imm (from NativeARM.cpp) to calculate this.

static const char* ALU_OpNames[ALU_OP_LIMIT] = {
    "and" /*  0 */,
    "bic" /*  1 */,
    "orr" /*  2 */,
    0     /*  3 */,
    "eor" /*  4 */,
    0     /*  5 */,
    0     /*  6 */,
    0     /*  7 */,
    "add" /*  8 */,
    0     /*  9 */,
    "adc" /* 10 */,
    "sbc" /* 11 */,
    0     /* 12 */,
    "sub" /* 13 */,
    "rsb" /* 14 */
};

static inline bool IsALUOp(int32_t op) { return op >= 0 && op < ALU_OP_LIMIT && ALU_OpNames[op] != 0; }

inline void
Assembler::xALUi(ALU_Op op, uint32_t S, Register rd, Register rl, uint32_t op2imm)
{
    NanoAssert(IsALUOp(op));
    NanoAssert(S == 0 || S == 1);
    NanoAssert(IsGpReg(rd) && IsGpReg(rl));
    NanoAssert(rl != PC);
    NanoAssert(op == ALU_add || op == ALU_sub || rl != SP);
    NanoAssert(rd != PC);
    NanoAssert(op == ALU_add || op == ALU_sub || rd != SP);
    NanoAssert(isThumb2Imm(op2imm));
    underrunProtect(4);
    emitT32(ALU_imm_insn((uint32_t)op, S, rd, rl, op2imm));
    asm_output("%s%s %s, %s, #0x%X", ALU_OpNames[op], (S ? "s" : ""), gpn(rd), gpn(rl), decThumb2Imm(op2imm));
}

static const char* Test_OpNames[TST_OP_LIMIT] = {
    "tst" /*  0 */,
    0     /*  1 */,
    0     /*  2 */,
    0     /*  3 */,
    "teq" /*  4 */,
    0     /*  5 */,
    0     /*  6 */,
    0     /*  7 */,
    "cmn" /*  8 */,
    0     /*  9 */,
    0     /* 10 */,
    0     /* 11 */,
    0     /* 12 */,
    "cmp" /* 13 */
};

static inline bool IsTestOp(int32_t op) { return op >= 0 && op < TST_OP_LIMIT && Test_OpNames[op] != 0; }

inline void 
Assembler::xTSTi(Test_Op op, Register rl, uint32_t op2imm)
{
    NanoAssert(IsTestOp(op));
    NanoAssert(IsGpReg(rl));
    NanoAssert(rl != PC);
    NanoAssert(rl != SP);
    NanoAssert(isThumb2Imm(op2imm));
    underrunProtect(4);
    emitT32(ALU_imm_insn((uint32_t)op, 1, 0xf, rl, op2imm));
    asm_output("%s %s, #0x%X", Test_OpNames[op], gpn(rl), decThumb2Imm(op2imm));
}

static const char* Move_OpNames[MOV_OP_LIMIT] = {
    0     /*  0 */,
    0     /*  1 */,
    "mov" /*  2 */,
    "mvn" /*  3 */
};

static inline bool IsMoveOp(int32_t op) { return op >= 0 && op < MOV_OP_LIMIT && Move_OpNames[op] != 0; }

inline void
Assembler::xMOVi(Move_Op op, Register rd, uint32_t op2imm)
{
    NanoAssert(IsMoveOp(op));
    NanoAssert(IsGpReg(rd));
    NanoAssert(rd != PC);
    NanoAssert(rd != SP);
    NanoAssert(isThumb2Imm(op2imm));
    underrunProtect(4);
    emitT32(ALU_imm_insn((uint32_t)op, 0, rd, 0xf, op2imm));
    asm_output("%s %s, #0x%X", Move_OpNames[op], gpn(rd), decThumb2Imm(op2imm));
}

// General Thumb2 scheme for encoding ALU operations with two register operands.
// Example: A8.8.14:  AND (register) encoding T2
// Example: A8.8.6:   ADD (register, Thumb) encoding T3
// Moves and tests will also fit this scheme, with reserved values for registers or the S bit.
static inline NIns32
ALU_reg_insn(uint32_t op, uint32_t S, Register rd, Register rl, uint32_t rr)
{
    // A6.3.11 Table A6-22 Data-processing (shifted register)
    // We leave the shifts zero here, and use a different templates for LSL, LSR, and ASR.
    return 0xEA000000 | op << 21 | S << 20 | rl <<16 | rd << 8 | rr;
}

// ALU operation with two register arguments
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  rr  - first (left) operand register
inline void
Assembler::xALUr(ALU_Op op, uint32_t S, Register rd, Register rl, Register rr)
{
    NanoAssert(IsALUOp(op));
    NanoAssert(((S)==0) || ((S)==1));
    NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rr));
    NanoAssert(rd != PC);
    NanoAssert(op == ALU_add || op == ALU_sub || rd != SP);
    NanoAssert(rl != PC);
    NanoAssert(op == ALU_add || op == ALU_sub || rl != SP);
    NanoAssert(rr != PC);
    NanoAssert(op == ALU_add || op == ALU_sub || rr != SP);
    underrunProtect(4);
    emitT32(ALU_reg_insn((uint32_t)op, S, rd, rl, rr));
    asm_output("%s%s %s, %s, %s", ALU_OpNames[op], (S ? "s" : ""), gpn(rd), gpn(rl), gpn(rr));
}

inline void
Assembler::xTSTr(Test_Op op, Register rl, Register rr)
{
    NanoAssert(IsTestOp(op));
    NanoAssert(IsGpReg(rl) && IsGpReg(rr));
    NanoAssert(rl != PC);
    NanoAssert(rl != SP);
    NanoAssert(rr != PC);
    NanoAssert(rr != SP);
    underrunProtect(4);
    emitT32(ALU_reg_insn((uint32_t)op, 1, 0xf, rl, rr));
    asm_output("%s  %s, %s", Test_OpNames[op], gpn(rl), gpn(rr));
}

inline void
Assembler::xMOVr(Move_Op op, Register rd, Register rs)
{
    underrunProtect(4);
    NanoAssert(IsMoveOp(op));
    NanoAssert(IsGpReg(rd) && IsGpReg(rs));
    NanoAssert(rd != PC);
    NanoAssert(rs != PC);
    // "MOV r, SP" and "MOV SP, r" are allowed where r != SP.
    // "MVN r, SP" and "MVN SP, r" are forbidden.  We disallow
    // S==1 entirely (by construction), else we'd have to enforce
    // some additional conditions.
    NanoAssert(rd != SP || rs != SP);
    NanoAssert(op == MOV_mov || rd != SP);
    NanoAssert(op == MOV_mov || rs != SP);
    underrunProtect(4);
    emitT32(ALU_reg_insn((uint32_t)op, 0, rd, 0xf, rs));
    asm_output("%s %s, %s", Move_OpNames[op], gpn(rd), gpn(rs));
}

static const char* Shift_OpNames[SHIFT_OP_LIMIT] = {
    "lsl" /*  0 */,
    "lsr" /*  1 */,
    "asr" /*  2 */
};

static inline bool IsShiftOp(int32_t op) { (void)Shift_OpNames/*dummy use*/; return op >= 0 && op < SHIFT_OP_LIMIT; }

// Shift operator, with immediate shift count
//  op  - a shift operator
//  rd  - destination register
//  rl  - first (left) operand register
//  imm - immediate argument to shift operator, 5 bits (1..31)
inline void
Assembler::xSHIFTi(Shift_Op op, Register rd, Register rl, int32_t imm)
{
    NanoAssert(IsShiftOp(op));
    NanoAssert(IsGpReg(rd) && IsGpReg(rl));
    NanoAssert(rd != PC);
    NanoAssert(rd != SP);
    NanoAssert(rl != PC);
    NanoAssert(rl != SP);
    NanoAssert(imm >= 0 && imm < 32);
    underrunProtect(4);
    if (imm == 0) {
        // Immediate shift count zero is treated specially for right shifts,
        // so just use a MOV for zero-count shifts to avoid problems.
        emitT32(ALU_reg_insn(MOV_mov, 0, rd, 0xf, rl));
        asm_output("mov %s, %s  # shift 0", gpn(rd), gpn(rl));
    } else {
        // Example: LSL immediate (A8.8.94 T2)
        // See A6.3.11 Table A6-23.
        emitT32(0xEA4F0000 | ((imm >> 2) & 0x7) << 12 | rd << 8 | (imm & 0x3) << 6 | op << 4 | rl);
        asm_output("%s %s, %s, #%d", Shift_OpNames[op], gpn(rd), gpn(rl), imm);
    }
}

// Shift operator, with shift count in register
//  op  - a shift operator
//  rd  - destination register
//  rl  - first (left) operand register
//  rs  - shift operand register
inline void
Assembler::xSHIFTr(Shift_Op op, Register rd, Register rl, Register rs)
{
    NanoAssert(IsShiftOp(op));
    NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rs));
    NanoAssert(rd != PC);
    NanoAssert(rd != SP);
    NanoAssert(rl != PC);
    NanoAssert(rl != SP);
    NanoAssert(rs != PC);
    NanoAssert(rs != SP);
    underrunProtect(4);
    // Example: LSL register (A8.8.95 T2)
    // See A6.3.12 Table A6-24.
    emitT32(0xFA00F000 | op << 21 | rl << 16 | rd << 8 | rs);
    asm_output("%s %s, %s, %s", Shift_OpNames[op], gpn(rd), gpn(rl), gpn(rs));
}

// --------
// Basic arithmetic operations.
// --------
// Argument naming conventions for these macros:
//  _d      Destination register.
//  _l      First (left) operand.
//  _r      Second (right) operand.
//  _op2imm An operand 2 immediate value. Use encThumb2Imm to calculate this.
//  _s      Set to 1 to update the status flags (for subsequent conditional
//          tests). Otherwise, set to 0.

// Unless otherwise indicated, neither PC nor SP can be used as
// as a source or destination in the ALU ops.  For add and subtract,
// SP is an allowable source and destination, providing for stack
// adjustments.

// _d = _l + decThumb2Imm(_op2imm)
#define ADDis(_d,_l,_op2imm,_s) xALUi(ALU_add, _s, _d, _l, _op2imm)
#define ADDi(_d,_l,_op2imm)     xALUi(ALU_add,  0, _d, _l, _op2imm)

// _d = _l & ~decThumb2Imm(_op2imm)
#define BICis(_d,_l,_op2imm,_s) xALUi(ALU_bic, _s, _d, _l, _op2imm)
#define BICi(_d,_l,_op2imm)     xALUi(ALU_bic,  0, _d, _l, _op2imm)

// _d = _l - decThumb2Imm(_op2imm)
#define SUBis(_d,_l,_op2imm,_s) xALUi(ALU_sub, _s, _d, _l, _op2imm)
#define SUBi(_d,_l,_op2imm)     xALUi(ALU_sub,  0, _d, _l, _op2imm)

// _d = _l & decThumb2Imm(_op2imm)
#define ANDis(_d,_l,_op2imm,_s) xALUi(ALU_and, _s, _d, _l, _op2imm)
#define ANDi(_d,_l,_op2imm)     xALUi(ALU_and,  0, _d, _l, _op2imm)

// _d = _l | decThumb2Imm(_op2imm)
#define ORRis(_d,_l,_op2imm,_s) xALUi(ALU_orr, _s, _d, _l, _op2imm)
#define ORRi(_d,_l,_op2imm)     xALUi(ALU_orr,  0, _d, _l, _op2imm)

// _d = _l ^ decThumb2Imm(_op2imm)
#define EORis(_d,_l,_op2imm,_s) xALUi(ALU_eor, _s, _d, _l, _op2imm)
#define EORi(_d,_l,_op2imm)     xALUi(ALU_eor,  0, _d, _l, _op2imm)

// _d = _l | _r
#define ORRs(_d,_l,_r,_s)   xALUr(ALU_orr, _s, _d, _l, _r)
#define ORR(_d,_l,_r)       xALUr(ALU_orr,  0, _d, _l, _r)

// _d = _l & _r
#define ANDs(_d,_l,_r,_s)   xALUr(ALU_and, _s, _d, _l, _r)
#define AND(_d,_l,_r)       xALUr(ALU_and,  0, _d, _l, _r)

// _d = _l ^ _r
#define EORs(_d,_l,_r,_s)   xALUr(ALU_eor, _s, _d, _l, _r)
#define EOR(_d,_l,_r)       xALUr(ALU_eor,  0, _d, _l, _r)

// _d = _l + _r
#define ADDs(_d,_l,_r,_s)   xALUr(ALU_add, _s, _d, _l, _r)
#define ADD(_d,_l,_r)       xALUr(ALU_add,  0, _d, _l, _r)

// _d = _l - _r
#define SUBs(_d,_l,_r,_s)   xALUr(ALU_sub, _s, _d, _l, _r)
#define SUB(_d,_l,_r)       xALUr(ALU_sub,  0, _d, _l, _r)

// --------
// Other operations.
// --------

// _d = _l * _r
inline void
Assembler::MUL(Register rd, Register rl, Register rr)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rr));
    NanoAssert((rd != PC) && (rl != PC) && (rr != PC));
    NanoAssert((rd != SP) && (rl != SP) && (rr != SP));
    underrunProtect(4);
    // A8.8.114 T2
    emitT32(0xFB00F000 | rl << 16 | rd << 8 | rr);
    asm_output("mul %s, %s, %s", gpn(rd), gpn(rl), gpn(rr));
}

// [_d_hi,_d] = _l * _r
inline void
Assembler::SMULL(Register rd_lo, Register rd_hi, Register rl, Register rr)
{
    NanoAssert(rd_lo != rd_hi);
    NanoAssert(IsGpReg(rd_lo) && IsGpReg(rd_hi) && IsGpReg(rl) && IsGpReg(rr));
    NanoAssert((rd_lo != PC) && (rd_hi != PC) && (rl != PC) && (rr != PC));
    NanoAssert((rd_lo != SP) && (rd_hi != SP) && (rl != SP) && (rr != SP));
    underrunProtect(4);
    // A8.8.189 T1
    emitT32(0xFB800000 | rl << 16 | rd_lo << 12 | rd_hi << 8 | rr);
    asm_output("smull %s, %s, %s, %s", gpn(rd_lo), gpn(rd_hi), gpn(rl), gpn(rr));
}

//### FIXME: Instructions with register operand 2 should use XXXr naming convention.

// RSBS0 _d, _r
// _d = 0 - _r
#define RSBS0(_d, _r) xALUi(ALU_rsb, 1, _d, _r, 0)

// MVN
// _d = ~_r (one's complement)
#define MVN(_d, _r) xMOVr(MOV_mvn, _d, _r)
#define MVNi(_d, _op2imm) xMOVi(MOV_mvn, _d, op2imm)

// Logical Shift Right (LSR) rotates the bits without maintaining sign extensions.
// MOVS _d, _r, LSR <_s>
// _d = _r >> _s
#define LSR(_d, _r, _s) xSHIFTr(SHIFT_lsr, _d, _r, _s)

// Logical Shift Right (LSR) rotates the bits without maintaining sign extensions.
// MOVS _d, _r, LSR #(_imm & 0x1f)
// _d = _r >> (_imm & 0x1f)
#define LSRi(_d, _r, _imm) xSHIFTi(SHIFT_lsr, _d, _r, (_imm & 0x1f))

// Arithmetic Shift Right (ASR) maintains the sign extension.
// MOVS _d, _r, ASR <_s>
// _d = _r >> _s
#define ASR(_d, _r, _s) xSHIFTr(SHIFT_asr, _d, _r, _s)

// Arithmetic Shift Right (ASR) maintains the sign extension.
// MOVS _r, _r, ASR #(_imm & 0x1f)
// _d = _r >> (_imm & 0x1f)
#define ASRi(_d, _r, _imm) xSHIFTi(SHIFT_asr, _d, _r, (_imm & 0x1f))

// Logical Shift Left (LSL).
// MOVS _d, _r, LSL <_s>
// _d = _r << _s
#define LSL(_d, _r, _s) xSHIFTr(SHIFT_lsl, _d, _r, _s)

// Logical Shift Left (LSL).
// MOVS _d, _r, LSL #(_imm & 0x1f)
// _d = _r << (_imm & 0x1f)
#define LSLi(_d, _r, _imm) xSHIFTi(SHIFT_lsl, _d, _r, (_imm & 0x1f))

// TST
#define TST(_l, _r)      xTSTr(TST_tst, _l, _r)
#define TSTi(_l, _imm)   xTSTi(TST_tst, _l, _imm)

// CMP
#define CMP(_l, _r)      xTSTr(TST_cmp, _l, _r)
#define CMPi(_l, _imm)   xTSTi(TST_cmp, _l, _imm)

// CMN
#define CMN(_l, _r)      xTSTr(TST_cmn, _l, _r)
#define CMNi(_l, _imm)   xTSTi(TST_cmn, _l, _imm)

// MOV
//#define MOV(dr, sr) xMOVr(MOV_mov, dr, sr)
//### FIXME: Needs to be visible to Assembler.cpp.
void Assembler::MOV(Register dr, Register sr) { xMOVr(MOV_mov, dr, sr); }
#define MOVi(_d, _op2imm) xMOVi(MOV_mov, _d, _op2imm)

// MOVW -- writes imm into rd, zero-extends.
inline void
Assembler::MOVWi(Register rd, uint32_t imm)
{
    // A8.8.102 T3
    NanoAssert(isU16(imm));
    NanoAssert(IsGpReg(rd));
    NanoAssert(rd != PC);
    NanoAssert(rd != SP);
    underrunProtect(4);
    emitT32(0xF2400000 | ((imm >> 11) & 0x1) << 26 | ((imm >> 12) & 0xf) << 16 | ((imm >> 8) & 0x7) << 12 | rd << 8 | (imm & 0xff));
    asm_output("movw %s, #0x%x", gpn(rd), imm);
}

// MOVT -- writes _imm into top halfword of _d, does not affect bottom halfword
inline void
Assembler::MOVTi(Register rd, uint32_t imm)
{
    // A8.8.106 T1
    NanoAssert(isU16(imm));
    NanoAssert(IsGpReg(rd));
    NanoAssert(rd != PC);
    NanoAssert(rd != SP);
    underrunProtect(4);
    emitT32(0xF2C00000 | ((imm >> 11) & 0x1) << 26 | ((imm >> 12) & 0xf) << 16 | ((imm >> 8) & 0x7) << 12 | rd << 8 | (imm & 0xff));
    asm_output("movt %s, #0x%x", gpn(rd), imm);
}

// Load a word (32 bits). The offset range is -255..4095.
inline void
Assembler::LDRi(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.62 T4
        emitT32(0xF8500C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.62 T3
        emitT32(0xF8D00000 | rn << 16 | rd << 12 | off); 
    }
    asm_output("ldr %s, [%s, #%d]", gpn(rd), gpn(rn), off);
}

// Load a word (32 bits). The offset is in register rm.
inline void
Assembler::LDRr(Register rd, Register rn, Register rm)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn) && IsGpReg(rm));
    NanoAssert(rn != PC);  // Conflicts with LDR (literal)
    NanoAssert(rm != PC);
    NanoAssert(rm != SP);
    underrunProtect(4);
    // A8.8.65 T2
    emitT32(0xF8500000 | rn << 16 | rd << 12 | rm);
    asm_output("ldr %s, [%s, %s]", gpn(rd), gpn(rn), gpn(rm));
}

// _d = [_b + _x<<_s]
inline void
Assembler::LDR_scaled(Register rd, Register rb, Register rx, int32_t s)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rb) && IsGpReg(rx));
    NanoAssert((s & 0x3) == s);
    NanoAssert(rb != PC);  // Conflicts with LDR (literal)
    NanoAssert(rx != PC);
    NanoAssert(rx != SP);
    NanoAssert(s >= 0 && s < 4);
    underrunProtect(4);
    // A8.8.65 T2
    emitT32(0xF8500000 | rb << 16 | rd << 12 | s << 4 | rx);
    asm_output("ldr %s, [%s, +%s, LSL #%d]", gpn(rd), gpn(rb), gpn(rx), s);
}

// Load a byte (8 bits). The offset range is -255..4095.
inline void
Assembler::LDRB(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    // If rn == PC, encoding A8.8.67 T2 is identical to
    // A8.8.69 T1, which additionally allows for a negative
    // offset.  We don't use PC in this context, however.
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.67 T2
        emitT32(0xF8100C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.67 T3
        emitT32(0xF8900000 | rn << 16 | rd << 12 | off);
    }
    asm_output("ldrb %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Load a byte (8 bits), sign-extend to 32 bits. The offset range is -255..4095.
inline void
Assembler::LDRSB(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.84 T2
        emitT32(0xF9100C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.84 T3
        emitT32(0xF9900000 | rn << 16 | rd << 12 | off);
    }
    asm_output("ldrsb %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Load and sign-extend a half word (16 bits). The offset range is -255..4095, and
// must be aligned to two bytes on some architectures (the caller is responsible
// for ensuring appropriate alignment)
inline void
Assembler::LDRH(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.79 T2
        emitT32(0xF8300C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.79 T1
        emitT32(0xF8B00000 | rn << 16 | rd << 12 | off);
    }
    asm_output("ldrh %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Load and sign-extend a half word (16 bits). The offset range is -255..4095, and
// must be aligned to two bytes on some architectures (the caller is responsible
// for ensuring appropriate alignment)
inline void
Assembler::LDRSH(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.88 T2
        emitT32(0xF9300C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.88 T1
        emitT32(0xF9B00000 | rn << 16 | rd << 12 | off);
    }
    asm_output("ldrsh %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Store a word (32 bits). The offset range is -255..4095.
inline void
Assembler::STR(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.203 T4
        emitT32(0xF8400C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.203 T3
        emitT32(0xF8C00000 | rn << 16 | rd << 12 | off);
    }
    asm_output("str %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Store a byte (8 bits). The offset range is -255..4095.
inline void
Assembler::STRB(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.206 T3
        emitT32(0xF8000C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.206 T2
        emitT32(0xF8800000 | rn << 16 | rd << 12 | off);
    }
    asm_output("strb %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// Store a halfword (16 bits). The offset range is -255..4095.
inline void
Assembler::STRH(Register rd, Register rn, int32_t off)
{
    NanoAssert(IsGpReg(rd) && IsGpReg(rn));
    NanoAssert(rd != SP);
    NanoAssert(rd != PC);
    NanoAssert(rn != PC);
    underrunProtect(4);
    if (off < 0) {
        NanoAssert(isU8(-off));
        // A8.8.216 T3
        emitT32(0xF8200C00 | rn << 16 | rd << 12 | (-off & 0xff));
    } else {
        NanoAssert(isU12(off));
        // A8.8.216 T2
        emitT32(0xF8A00000 | rn << 16 | rd << 12 | off);
    }
    asm_output("strh %s, [%s,#%d]", gpn(rd), gpn(rn), off);
}

// PUSH {reg}
inline void
Assembler::PUSHr(Register r)
{
    NanoAssert(IsGpReg(r));
    NanoAssert(r != PC);
    NanoAssert(r != SP);
    underrunProtect(4);
    // A8.8.133 T3
    emitT32(0xF84D0D04 | r << 12);
    asm_output("push %s", gpn(r));
}

// PUSH {reglist}
inline void
Assembler::PUSH_mask(RegisterMask mask)
{
    NanoAssert(isU16(mask));
    NanoAssert((mask & (1ULL << PC)) == 0);
    NanoAssert((mask & (1ULL << SP)) == 0);
    // Error if 0 or 1 bits are set in mask.
    // Result is unpredictable in that case.
    NanoAssert(mask != 0 && (mask & (mask-1)) != 0);
    underrunProtect(4);
    // A8.8.133 T2
    emitT32(0xE92D0000 | (uint32_t)mask);
    // TODO: List the set of registers by name.
    asm_output("push %x", (uint32_t)mask);
}

// POP {reg}
inline void
Assembler::POPr(Register r)
{
    NanoAssert(IsGpReg(r));
    NanoAssert(r != SP);
    underrunProtect(4);
    // A8.8.131 T3
    emitT32(0xE85D0B04 | r << 12);
    asm_output("pop %s", gpn(r));
}

// POP {reglist}
inline void
Assembler::POP_mask(RegisterMask mask)
{
    NanoAssert(isU16(mask));
    NanoAssert((mask & (1ULL << SP)) == 0);
    NanoAssert((mask & (1ULL << PC)) == 0 || ((mask & (1ULL << LR)) == 0));
    // Error if 0 or 1 bits are set in mask.
    // Result is unpredictable in that case.
    NanoAssert(mask != 0 && (mask & (mask-1)) != 0);
    underrunProtect(4);
    // A8.8.131 T2
    emitT32(0xE8BD0000 | (uint32_t)mask);
    // TODO: List the set of registers by name.
    asm_output("pop %x", (uint32_t)mask);
}

// Encode a breakpoint. The ID is not important and is ignored by the
// processor, but it can be useful as a marker when debugging emitted code.
// There is no 32-bit Thumb2 encoding of BKPT, so we generate a 16-bit
// encoding followed by a 16-bit NOP to keep everything 32-bit aligned.
#define BKPT_insn(id)  ((NIns32)(0xBE00BF04 | (id & 0xff) << 16))

inline void
Assembler::BKPT(uint32_t id)
{
    NanoAssert((id & 0xff) == id);
    underrunProtect(4);
    emitT32(BKPT_insn(id));
    asm_output("bkpt #%d", id);
}

static inline NIns32 B_insn(int32_t offset)
{
    NanoAssert(isS25(offset));
    NanoAssert((offset & 0x1) == 0);  // halfword aligned
    // A8.8.18 T4
    // Note the odd encoding where the sense of bits 22 and 23 depend on bit 24.
    uint32_t imm11 = (offset >> 1) & 0x7ff;
    uint32_t imm10 = (offset >> 12) & 0x3ff;
    uint32_t s = (offset >> 24) & 0x1;
    uint32_t j1 = ((~offset >> 23) & 0x1) ^ s;
    uint32_t j2 = ((~offset >> 22) & 0x1) ^ s;
    return 0xF0009000 | s << 26 | imm10 << 16 | j1 << 13 | j2 << 11 | imm11;
}

// Branch unconditionally to offset relative to PC, -16M..16M-1, even only (halfword address)
inline void
Assembler::B(NIns* addr)
{
    underrunProtect(4);
    intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 2);
    emitT32(B_insn(offset));
    asm_output("b %p", (void*)addr);
}

// Generate an unconditional branch to a full 32-bit absolute address, as a PC-relative
// load into the PC of the contents of the following word:
//
//        LDR PC, [PC, #-0]
//        <address>
//
inline void
Assembler::LDR_PC_longbranch(NIns* addr)
{
    NanoAssert(LDR_PC_size == 4*2);
    underrunProtect(LDR_PC_size);
    NanoAssert(((uintptr_t)addr & 0x1) == 0);       // halfword aligned
    emitPC(addr);
    emitT32(0xF85FF000);                  // LDR PC, [PC, $-0]
    //###FIXME: .word should show the actual value, with the interworking bit set.
    asm_output("ldr PC, [PC, #-0] ; .word %p  # -> %p", (void*)addr, (void*)addr);
}

static inline bool IsCond(int32_t cc) { return cc >= EQ && cc <= AL; }

// Bit 0 of the condition code can be flipped to obtain the opposite condition.
// However, this won't work for AL because its opposite — NV — has special
// meaning.
#define OppositeCond(cc)  ((ConditionCode)((unsigned int)(cc)^0x1))

//### NOTE: This shows a general problem with instructions that may be encoded
//### either in a 16-bit or 32-bit encoding, and contain an offset relative to
//### the instruction address.  Since we enter with _nIns pointing at the address
//### *after* the instruction, we must account for the length of the instruction
//### when computing the offset relative to the beginning of the instruction (or
//### rather 4 bytes *beyond* the beginning of the instruction, regardless of
//### its length).  This calculation is simpler on processors like the x86 where
//### the base address for relative branches is always the address of the next
//### instruction.

// Branch conditionally to offset relative to PC, -1M..1M-1, even only (halfword address)
inline void
Assembler::Bcc(ConditionCode cc, NIns* addr)
{
    NanoAssert(IsCond(cc));
    NanoAssert(cc != AL);  // Illegal for 16-bit encoding.  Should use B anyhow.
    NanoAssert(cc != NV);  // Illegal.
    // See if 16-bit branch will work.
    //### FIXME: This logic does not work.
    //### Problem is possibly with the nPatchBranch fixup logic when 16-bit
    //### branches are generated for longbranch branchovers.
    /*
    underrunProtect(2);
    intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 1);
    NanoAssert((offset & 0x1) == 0);  // halfword aligned
    if (isS8(offset >> 1) && (addr != 0)) {
        // Offset is short, and target is known.
        // A8.8.18 T1
        emitT16(0xD000 | cc << 8 | (offset >> 1));
    } else*/
    {
        // Need a 32-bit branch.
        underrunProtect(4);
        intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 2);
        NanoAssert((offset & 0x1) == 0);  // halfword aligned
        NanoAssert(isS21(offset));
        // A8.8.18 T3
        // Note that encoding here is not the same as for B().
        uint32_t imm11 = (offset >> 1) & 0x7ff;
        uint32_t imm6 = (offset >> 12) & 0x3f;
        uint32_t s = (offset >> 20) & 0x1;
        uint32_t j1 = (offset >> 18) & 0x1;
        uint32_t j2 = (offset >> 19) & 0x1;
        emitT32(0xF0008000 | s << 26 | cc << 22 | imm6 << 16 | j1 << 13 | j2 << 11 | imm11);
    }
    asm_output("b%s %p", condNames[cc], (void*)addr);
}

// Use this when a short branch is guaranteed, e.g, branchovers within instruction templates.
inline void
Assembler::Bcc_short(ConditionCode cc, NIns* addr)
{
    NanoAssert(addr != 0);  // This branch is not patchable!
    NanoAssert(IsCond(cc));
    NanoAssert(cc != AL);
    NanoAssert(cc != NV);
    underrunProtect(2);
    intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 2);
    NanoAssert((offset & 0x1) == 0);  // halfword aligned
    NanoAssert(isS8(offset >> 1));
    // A8.8.18 T1
    emitT16(NIns((0xD000 | cc << 8 | (offset >> 1))));
    asm_output("b%s %p", condNames[cc], (void*)addr);
}

// Branch to target address t with condition c, doing underrun
// checks (chk == 1) or skipping them (chk == 0).
//
// Set the target address (t) to 0 if the target is not yet known and the
// branch will be patched up later.
//
// If the jump is to a known address (with t != 0) and it fits in a relative
// jump, emit that. If the jump is unconditional, emit the dest address inline
// in the instruction stream and load it into pc.  If it is conditional,
// generate the same instructions, with a preceding skip (relative branch) on
// the inverse of the desired condition.

//### FIXME: Rename this to something like "asm_branch".
//### The unconditional form is simply an instruction emitter that asserts
//### that the offset is in range, expecting the caller to determine if the
//### short branch form is applicable.

void
Assembler::B_cond(ConditionCode c, NIns* t)
{
    intptr_t offs = PC_OFFSET_FROM(t, _nIns - 2);
    //nj_dprintf("B_cond_chk target: 0x%08x offset: %d @0x%08x\n", t, offs, _nIns-1);

    // Emit one of the following patterns:
    //
    //  --- Short unconditional branch.
    //      This can never be emitted if the branch target is not known.
    //          B     ±16MB
    //
    //  --- Short conditional branch.
    //      This can never be emitted if the branch target is not known.
    //          Bcc   ±1MB
    //
    //  --- Long unconditional branch. (patchable)
    //          LDR     PC, #lit
    //  lit:    #target
    //
    //  --- Long conditional branch.   (patchable)
    //          Bcc_inv skip        # where cc_inv == OppositeCond(cc)
    //          LDR     PC, #lit
    //  lit:    #target
    //  skip:   [...]

    if ((isS21(offs) || (c == AL && isS25(offs))) && (t != 0)) {
        // Speculate that we can use B or Bcc.
        underrunProtect(4);
        // Recalculate the offset, because underrunProtect may have
        // moved _nIns to a new page.  Unfortunately, Bcc may no longer
        // be applicable, so we have to check again.
        offs = PC_OFFSET_FROM(t, _nIns - 2);
    }
    if (isS21(offs) && (t != 0)) {
        if (c == AL) {
            // Always generate the specialized unconditional form,
            // though I don't think this is strictly required, because
            // we've already verified that this branch is not patchable.
            B(t);
        } else {
            Bcc(c, t);
        }
    } else if (c == AL) {
        if (isS25(offs)) {
            B(t);
        } else {
            LDR_PC_longbranch(t);
        }
    } else {
        ReserveContiguousSpace(this, 4 + LDR_PC_size);
        LDR_PC_longbranch(t);
        Bcc(OppositeCond(c), _nIns + 4);
    }
}

#define BHI(t)  B_cond(HI, t)
#define BLS(t)  B_cond(LS, t)
#define BHS(t)  B_cond(HS, t)
#define BLO(t)  B_cond(LO, t)
#define BEQ(t)  B_cond(EQ, t)
#define BNE(t)  B_cond(NE, t)
#define BLT(t)  B_cond(LT, t)
#define BGE(t)  B_cond(GE, t)
#define BLE(t)  B_cond(LE, t)
#define BGT(t)  B_cond(GT, t)
#define BVS(t)  B_cond(VS, t)
#define BVC(t)  B_cond(VC, t)
#define BCC(t)  B_cond(CC, t)
#define BCS(t)  B_cond(CS, t)

//### FIXME: The XXX_nochk operations are not needed.
//### In any case where it is valid to use one, it must be guarded by
//### a underrunProtect(n) reserving sufficient space that the check will
//### have no effect.
#define B_nochk(t) B_cond(AL, t)

// Emit a branch to an address held in a register.
//### FIXME: This is not presently used.
// There is no 32-bit Thumb2 encoding for BX, so we generate the 16-bit
// encoding followed by a 16-bit NOP, to keep instructions 32-bit aligned.
inline void Assembler::BXr(Register r)
{
    NanoAssert(IsGpReg(r));
    NanoAssert(r != PC);
    underrunProtect(4);
    // A8.8.27 T1 ; A8.8.119 T1
    emitT32(0x4700BF00 | r << 19);
    asm_output("bx %s", gpn(r));
}

// Generate a patchable long branch.
//### This is used only for exit branches.  We could also optimize the
//### unconditional case of B_cond() along these lines, reserving space
//### for a long branch, but emitting a short branch + BKPT that can still
//### still be rewritten in place.
void
Assembler::JMP_far(NIns* addr)
{
    // Even if a simple branch is all that is required, this function must emit
    // two words so that the branch can be arbitrarily patched later on.  We must
    // reserve space before computing the offset, as otherwise _nIns may change.
    ReserveContiguousSpace(this, NJ_MAX(4*2, LDR_PC_size));
    
    intptr_t offs = PC_OFFSET_FROM(addr, _nIns - 2);
    if (isS25(offs)) {
        // Emit a BKPT to ensure that we reserve enough space for a full 32-bit
        // branch patch later on. The BKPT should never be executed.
        BKPT(0);
        B(addr);
    } else {
        LDR_PC_longbranch(addr);
    }
}

// Emit a branch-and-link to an address specified by immediate PC-relative offset.
// Callee must also be Thumb2 code.
inline void
Assembler::BLi(NIns* addr)
{
    underrunProtect(4);
    intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 2);
    NanoAssert(isS25(offset));
    NanoAssert((offset & 0x1) == 0);  // halfword aligned
    // A8.8.25 T1
    // Note the odd encoding where the sense of bits 22 and 23 depend on bit 24.
    // The immediate value encoding is the same as that for B().
    uint32_t imm11 = (offset >> 1) & 0x7ff;
    uint32_t imm10 = (offset >> 12) & 0x3ff;
    uint32_t s = (offset >> 24) & 0x1;
    uint32_t j1 = ((~offset >> 23) & 0x1) ^ s;
    uint32_t j2 = ((~offset >> 22) & 0x1) ^ s;
    emitT32(0xF000D000 | s << 26 | imm10 << 16 | j1 << 13 | j2 << 11 | imm11);
    asm_output("bl %p", (void*)addr);
}

// Emit a branch-and-link to an address specified by immediate PC-relative offset.
// Switch to ARM mode at the branch.
inline void
Assembler::BLXi(NIns* addr)
{
    underrunProtect(4);
    intptr_t offset = PC_OFFSET_FROM(addr, _nIns - 2);
    NanoAssert(isS25(offset));
    NanoAssert((offset & 0x3) == 0);  // fullword aligned
    // A8.8.25 T2
    // Note the odd encoding where the sense of bits 22 and 23 depend on bit 24.
    // The immediate value encoding is the same as that for B().
    uint32_t imm11 = (offset >> 1) & 0x7ff;
    uint32_t imm10 = (offset >> 12) & 0x3ff;
    uint32_t s = (offset >> 24) & 0x1;
    uint32_t j1 = ((~offset >> 23) & 0x1) ^ s;
    uint32_t j2 = ((~offset >> 22) & 0x1) ^ s;
    emitT32(0xF000C000 | s << 26 | imm10 << 16 | j1 << 13 | j2 << 11 | imm11);
    asm_output("blx %p", (void*)addr);
}

// Emit a branch-and-link to an address held in a register.
// There is no 32-bit Thumb2 encoding for BLX, so we generate the 16-bit
// NOP followed by a 16-bit BLX, to keep instructions 32-bit aligned.
// The interworking bit of the address must be set correctly.
inline void
Assembler::BLXr(Register r)
{
    NanoAssert(IsGpReg(r));
    NanoAssert(r != PC);
    underrunProtect(4);
    // A8.8.119 T1 ; A8.8.26 T1
    emitT32(0xBF004780 | r << 3);
    asm_output("blx %s", gpn(r));
}

// Perform a branch with link, and ARM/Thumb exchange if necessary. The actual
// BLX instruction is only available from ARMv5 onwards, but as we don't
// support anything older than that this function will not attempt to output
// pre-ARMv5 sequences.
void
Assembler::BranchWithLink(uint32_t addr)
{
    // We do not create patchable branches, so the target must be known.
    // This is sufficient, because we are invoked only to generate calls
    // to pre-existing functions (JIT-compiled or statically compiled).
    NanoAssert(addr != 0);

    // Note that the address is received here as a uintptr_t, not a NIns*.
    // The address is not a pointer to an instruction generated by Nanojit,
    // but instead a code address in the host.  It is presumed that such
    // addresses may be given to us with the low-order bit set for interworking.
    // We assume the interworking bit of the address has been set correctly.
    
    // Most branches emitted by TM are loaded through a register, so always
    // reserve enough space for the LDR sequence. This should give us a slight
    // net gain over reserving the exact amount required for shorter branches.
    // This _must_ be called before PC_OFFSET_FROM as it can move _nIns!
    ReserveContiguousSpace(this, 4 + LD32_size);

    // Calculate the offset from the instruction that is about to be
    // written (at _nIns-1) to the target.
    NIns* target = (NIns*)(addr & ~0x1);
    intptr_t offset = PC_OFFSET_FROM(target, _nIns - 2);
    if (isS25(offset)) {
        if (addr & 0x1) {
            // Thumb target.
            BLi(target);
        } else {
            // ARM target.
            BLXi(target);
        }
    } else {
        // Load the target address into IP and branch to that. We've already
        // done underrunProtect, so we can skip that here.
        // TODO: Target address is not clearly indicated in disassembly.
        BLXr(IP);
        asm_ld_imm(IP, addr);
    }
}

static inline bool
branch_is_B(NIns* branch)
{
    // Recognize A8.8.18 T4 only.
    // This form is always unconditional.
    return (unpack32(branch) & swap(0xF800D000)) == swap(0xF0009000);
}

static inline bool
branch_is_Bcc16(NIns* branch)
{
    // Recognize A8.8.18 T1 only.
    return (*branch & 0xF000) == 0xD000;
}

static inline bool
branch_is_Bcc32(NIns* branch)
{
    // Recognize A8.8.18 T3 only.
    return (unpack32(branch) & swap(0xF800D000)) == swap(0xF0008000);
}

static inline bool
branch_is_LDR_PC_0(NIns* branch)
{
    // Recognize A8.8.64 T2, and only for the
    // case of branching to the address following
    // the instruction, i.e., LDR PC, [PC, #-0].
    return unpack32(branch) == swap(0xF85FF000);
}

void
Assembler::nPatchBranch(NIns* branch, NIns* target)
{
    // Patch the jump in a loop (backward branch)
    //
    // There are two feasible cases here, the first of which has 2 sub-cases:
    //
    //   (1) We are patching a patchable unconditional jump emitted by
    //       JMP_far.  All possible encodings we may be looking at with
    //       involve 2 words, though we *may* have to change from 1 word to
    //       2 or vice verse.
    //
    //          1a:  B ±16MB ; BKPT
    //          1b:  LDR PC [PC, #-0] ; $imm
    //
    //   (2) We are patching a patchable conditional jump emitted by
    //       B_cond_chk.  Short conditional jumps are non-patchable, so we
    //       won't have one here; will only ever have an instruction of the
    //       following form:
    //
    //          Bcc .+8 ; LDR PC [PC, #-0] ; $imm
    //
    // For purposes of handling our patching task, we group cases 1b and 2
    // together, and handle case 1a on its own as it might require expanding
    // from a short-jump to a long-jump.
    //
    // We do not handle contracting from a long-jump to a short-jump, though
    // this is a possible future optimisation for case 1b. For now it seems
    // not worth the trouble.

    if (branch_is_B(branch)) {
        // Case 1a
        // A short B branch, must be unconditional.

        intptr_t offset = PC_OFFSET_FROM(target, branch);
        if (isS25(offset)) {
            asm_output("# short branch @ %p -> %p\n", (void*)branch, (void*)target);
            emitT32At(branch, B_insn(offset));
        } else {
            asm_output("# expanding branch @ %p -> %p\n", (void*)branch, (void*)target);
            // We need to expand the existing branch to a long jump.
            // Make sure the next instruction is a dummy BKPT.
            NanoAssert(*(branch+1) == swap(BKPT_insn(0)));
            emitT32At(branch, 0xF85FF000);  // LDR PC, [PC, $-0]
            emitImmAt(branch + 2, (int32_t)target);
        }
    } else {
        // Case 1b & 2
        // A long branch (LDR PC) or a short conditional branch (Bcc) over a long branch.

        if (branch_is_Bcc16(branch)) {
            asm_output("# long cond branch @ %p -> %p\n", (void*)branch, (void*)target);
            // Skip the 16-bit branchover if present.
            branch += 1;
        } else if (branch_is_Bcc32(branch)) {
            //### FIXME -- We should not generate these, as branchovers are always short.
            asm_output("# long cond branch @ %p -> %p\n", (void*)branch, (void*)target);
            // Skip the 32-bit branchover if present.
            branch += 2;
        } else {
            asm_output("# long branch @ %p -> %p\n", (void*)branch, (void*)target);
        }
        NanoAssert(branch_is_LDR_PC_0(branch));
        // Just redirect the jump target, leave the insn alone.
        // Set the Thumb bit in the address.
        emitImmAt(branch + 2, (uintptr_t)target | 0x1);
    }
}

// This can be done more efficiently with conditional moves, but we are then
// limited to 16-bit Thumb encodings with restricted register addressing.
inline void
Assembler::SET(Register r, ConditionCode cond)
{
    underrunProtect(12);
    // A8.8.102 T2
    emitT32(0xF04F0000 | r << 8);
    asm_output("mov %s, #0", gpn(r));
    // A8.8.18 T3
    emitT32(0xF0008002 | cond << 22);
    asm_output("b%s .+4", condNames[cond]);
    // A8.8.102 T2
    emitT32(0xF04F0001 | r << 8);
    asm_output("mov %s, #1", gpn(r));
}

#define SETEQ(r)    SET(r, EQ)
#define SETNE(r)    SET(r, NE)
#define SETLT(r)    SET(r, LT)
#define SETLE(r)    SET(r, LE)
#define SETGT(r)    SET(r, GT)
#define SETGE(r)    SET(r, GE)
#define SETLO(r)    SET(r, LO)
#define SETLS(r)    SET(r, LS)
#define SETHI(r)    SET(r, HI)
#define SETHS(r)    SET(r, HS)
#define SETVS(r)    SET(r, VS)
#define SETCS(r)    SET(r, CS)

// Emit the code required to load a memory address into a register as follows:
// d = *(b+off)
void
Assembler::asm_ldr(Register d, Register b, int32_t off)
{
    if (ARM_VFP && !IsGpReg(d) ) {
        if (IsFpQReg(d)) {
            NanoAssert(IsGpReg(b));
            VLDQR(d, IP);
            asm_add_imm(IP, b, off);
        } else {
            if (off % 4 == 0) {
                VLDR(d, IP, 0);
                asm_add_imm(IP, FP, off);
            } else if (isU8(off/4) || isU8(-off/4)) {
                VLDR(d, b, off);  // this can handle Dn and Sn registers
            } else {
                VLDR(d, IP, off%1024);
                asm_add_imm(IP, FP, off-(off%1024));
            }
        }
        return;
    }

    NanoAssert(IsGpReg(d));
    NanoAssert(IsGpReg(b));

    // We don't allow PC as the base register, as underrunProtect() might move the PC.
    NanoAssert(b != PC);

    if (isU12(off) || isU8(-off)) {
        LDRi(d, b, off);
    } else {
        // The offset is outside the range of LDR, so we need
        // to add a level of indirection to get the address into IP.
        // Because of that, we can't do a PC-relative load unless it
        // fits within the single-instruction forms above.
        NanoAssert(b != PC);
        NanoAssert(b != IP);
        LDRr(d, b, IP);
        asm_ld_imm(IP, off);
    }
}

// Emit a store, using a register base and an arbitrary immediate offset. This
// behaves like a STR instruction, but doesn't care about the offset range, and
// emits one of the following instruction sequences:
//
// ----
// STR  rt, [rr, #offset]
// ----
// asm_add_imm  ip, rr, #(offset & ~0xfff)
// STR  rt, [ip, #(offset & 0xfff)]
// ----
// # This one's fairly horrible, but should be rare.
// asm_add_imm  rr, rr, #(offset & ~0xfff)
// STR  rt, [ip, #(offset & 0xfff)]
// asm_sub_imm  rr, rr, #(offset & ~0xfff)
// ----
// SUB-based variants (for negative offsets) are also supported.
// ----
//
// The return value is 1 if a simple STR could be emitted, or 0 if the required
// sequence was more complex.
int32_t
Assembler::asm_str(Register rt, Register rr, int32_t offset)
{
    // We can't do PC-relative stores, and we can't store the PC value, because
    // we use macros (such as STR) which call underrunProtect, and this can
    // push _nIns to a new page, thus making any PC value impractical to
    // predict.
    NanoAssert(rr != PC);
    NanoAssert(rt != PC);
    if (offset >= 0) {
        // The offset is positive, so use ADD (and variants).
        if (isU12(offset)) {
            STR(rt, rr, offset);
            return 1;
        }

        if (rt != IP) {
            STR(rt, IP, offset & 0xfff);
            asm_add_imm(IP, rr, offset & ~0xfff);
        } else {
            int32_t adj = offset & ~0xfff;
            asm_sub_imm(rr, rr, adj);
            STR(rt, rr, offset-adj);
            asm_add_imm(rr, rr, adj);
        }
    } else {
        // The offset is negative, so use SUB (and variants).
        if (isU8(-offset)) {
            STR(rt, rr, offset);
            return 1;
        }

        if (rt != IP) {
            STR(rt, IP, -((-offset) & 0xff));
            asm_sub_imm(IP, rr, (-offset) & ~0xff);
        } else {
            int32_t adj = ((-offset) & ~0xff);
            asm_add_imm(rr, rr, adj);
            STR(rt, rr, offset+adj);
            asm_sub_imm(rr, rr, adj);
        }
    }

    return 0;
}

// Emit the code required to load an immediate value (imm) into general-purpose
// register d. Optimal (MOV-based) mechanisms are used if the immediate can be
// encoded using ARM's operand 2 encoding. Otherwise, a slot is used on the
// literal pool and LDR is used to load the value.
//
// This function guarantees not to use more than LD32_size bytes of space.
void
Assembler::asm_ld_imm(Register d, int32_t imm)
{
    NanoAssert(IsGpReg(d));
    NanoAssert(LD32_size == 4*2);

    // We should not be loading the PC here, as branches are emitted elsewhere.
    NanoAssert(d != PC);

    // Thumb2 immediate encoding.
    uint32_t    op2imm;

    // Attempt to encode the immediate using the second operand of MOV or MVN.
    // This is the simplest solution and generates the shortest and fastest
    // code, but can only encode a limited set of values.

    if (encThumb2Imm(imm, &op2imm)) {
        // Use MOV to encode the literal.
        MOVi(d, op2imm);
        return;
    }

    if (encThumb2Imm(~imm, &op2imm)) {
        // Use MVN to encode the inverted literal.
        MVNi(d, op2imm);
        return;
    }

    // Use MOVW/MOVT, which is applicable since d != PC.
    // These instructions are available on all ARM cores supporting Thumb2
    // except for ARMv6T2/ARM1156, which is a real-time core that nanojit is
    // unlikely ever to target.

    uint32_t    high_h = (uint32_t)imm >> 16;
    uint32_t    low_h = imm & 0xffff;

    if (high_h != 0) {
        // Load the high half-word (if necessary).
        MOVTi(d, high_h);
    }
    
    // Load the low half-word. This also zeroes the high half-word, and
    // thus must execute _before_ MOVT, and is necessary even if low_h is 0
    // because MOVT will not change the existing low half-word.
    MOVWi(d, low_h);

    // NOTE:
    // On earlier ARM targets, we had to load some values from the literal
    // pool.  We no longer need to do this, so the literal pool is obsolete.
    // We still need literals for long branches, but they can be dropped
    // in the instruction stream immediately following the branch.
}

// Encode "rd = rn + imm" using an appropriate instruction sequence.
// Set stat to 1 to update the status flags. Otherwise, set it to 0 or omit it.
// (The declaration in NativeARM.h defines the default value of stat as 0.)
//
// It is not valid to call this function if:
//   (rd == IP) AND (rn == IP) AND !encThumb2Imm(imm) AND !encThumb2Imm(-imm)
// Where: if (encThumb2Imm(imm)), imm can be encoded as an ARM operand 2 using the
// encThumb2Imm method.
void
Assembler::asm_add_imm(Register rd, Register rn, int32_t imm, int stat /* =0 */)
{
    // TODO: Generate encoding A8.8.4 T4 where possible (12-bit zero-extended).

    // Thumb2 immediate encoding for operand 2.
    uint32_t    op2imm;

    NanoAssert(IsGpReg(rd));
    NanoAssert(IsGpReg(rn));
    NanoAssert((stat & 1) == stat);

    // As a special case to simplify code elsewhere, emit nothing where we
    // don't want to update the flags (stat == 0), the second operand is 0 and
    // (rd == rn). Such instructions are effectively NOPs.
    if ((imm == 0) && (stat == 0) && (rd == rn)) {
        return;
    }

    // Try to encode the value directly as an operand 2 immediate value, then
    // fall back to loading the value into a register.
    if (encThumb2Imm(imm, &op2imm)) {
        ADDis(rd, rn, op2imm, stat);
    } else if (encThumb2Imm(-imm, &op2imm)) {
        // We could not encode the value for ADD, so try to encode it for SUB.
        // Note that this is valid even if stat is set, _unless_ imm is 0, but
        // that case is caught above.
        NanoAssert(imm != 0);
        SUBis(rd, rn, op2imm, stat);
    } else {
        // We couldn't encode the value directly, so use an intermediate
        // register to encode the value. We will use IP to do this unless rn is
        // IP; in that case we can reuse rd. This allows every case other than
        // "ADD IP, IP, =#imm".
        Register    rm = (rn == IP) ? (rd) : (IP);
        NanoAssert(rn != rm);

        ADDs(rd, rn, rm, stat);
        asm_ld_imm(rm, imm);
    }
}

// Encode "rd = rn - imm" using an appropriate instruction sequence.
// Set stat to 1 to update the status flags. Otherwise, set it to 0 or omit it.
// (The declaration in NativeARM.h defines the default value of stat as 0.)
//
// It is not valid to call this function if:
//   (rd == IP) AND (rn == IP) AND !encThumb2Imm(imm) AND !encThumb2Imm(-imm)
// Where: if (encThumb2Imm(imm)), imm can be encoded as an ARM operand 2 using the
// encThumb2Imm method.
void
Assembler::asm_sub_imm(Register rd, Register rn, int32_t imm, int stat /* =0 */)
{
    // TODO: Generate encoding A8.8.221 T4 where possible (12-bit zero-extended).

    // Thumb2 immediate encoding for operand 2.
    uint32_t    op2imm;

    NanoAssert(IsGpReg(rd));
    NanoAssert(IsGpReg(rn));
    NanoAssert((stat & 1) == stat);

    // As a special case to simplify code elsewhere, emit nothing where we
    // don't want to update the flags (stat == 0), the second operand is 0 and
    // (rd == rn). Such instructions are effectively NOPs.
    if ((imm == 0) && (stat == 0) && (rd == rn)) {
        return;
    }

    // Try to encode the value directly as an operand 2 immediate value, then
    // fall back to loading the value into a register.
    if (encThumb2Imm(imm, &op2imm)) {
        SUBis(rd, rn, op2imm, stat);
    } else if (encThumb2Imm(-imm, &op2imm)) {
        // We could not encode the value for SUB, so try to encode it for ADD.
        // Note that this is valid even if stat is set, _unless_ imm is 0, but
        // that case is caught above.
        NanoAssert(imm != 0);
        ADDis(rd, rn, op2imm, stat);
    } else {
        // We couldn't encode the value directly, so use an intermediate
        // register to encode the value. We will use IP to do this unless rn is
        // IP; in that case we can reuse rd. This allows every case other than
        // "SUB IP, IP, =#imm".
        Register    rm = (rn == IP) ? (rd) : (IP);
        NanoAssert(rn != rm);

        SUBs(rd, rn, rm, stat);
        asm_ld_imm(rm, imm);
    }
}

// Encode "rd = rn & imm" using an appropriate instruction sequence.
// Set stat to 1 to update the status flags. Otherwise, set it to 0 or omit it.
// (The declaration in NativeARM.h defines the default value of stat as 0.)
//
// It is not valid to call this function if:
//   (rd == IP) AND (rn == IP) AND !encThumb2Imm(imm) AND !encThumb2Imm(~imm)
// Where: if (encThumb2Imm(imm)), imm can be encoded as an ARM operand 2 using the
// encThumb2Imm method.
void
Assembler::asm_and_imm(Register rd, Register rn, int32_t imm, int stat /* =0 */)
{
    // Thumb2 immediate encoding for operand 2.
    uint32_t    op2imm;

    NanoAssert(IsGpReg(rd));
    NanoAssert(IsGpReg(rn));
    NanoAssert((stat & 1) == stat);

    // Try to encode the value directly as an operand 2 immediate value, then
    // fall back to loading the value into a register.
    if (encThumb2Imm(imm, &op2imm)) {
        ANDis(rd, rn, op2imm, stat);
    } else if (encThumb2Imm(~imm, &op2imm)) {
        // Use BIC with the inverted immediate.
        BICis(rd, rn, op2imm, stat);
    } else {
        // We couldn't encode the value directly, so use an intermediate
        // register to encode the value. We will use IP to do this unless rn is
        // IP; in that case we can reuse rd. This allows every case other than
        // "AND IP, IP, =#imm".
        Register    rm = (rn == IP) ? (rd) : (IP);
        NanoAssert(rn != rm);

        ANDs(rd, rn, rm, stat);
        asm_ld_imm(rm, imm);
    }
}

// Encode "rd = rn | imm" using an appropriate instruction sequence.
// Set stat to 1 to update the status flags. Otherwise, set it to 0 or omit it.
// (The declaration in NativeARM.h defines the default value of stat as 0.)
//
// It is not valid to call this function if:
//   (rd == IP) AND (rn == IP) AND !encThumb2Imm(imm)
// Where: if (encThumb2Imm(imm)), imm can be encoded as an ARM operand 2 using the
// encThumb2Imm method.
void
Assembler::asm_orr_imm(Register rd, Register rn, int32_t imm, int stat /* =0 */)
{
    // Thumb2 immediate encoding for operand 2.
    uint32_t    op2imm;

    NanoAssert(IsGpReg(rd));
    NanoAssert(IsGpReg(rn));
    NanoAssert((stat & 1) == stat);

    // Try to encode the value directly as an operand 2 immediate value, then
    // fall back to loading the value into a register.
    if (encThumb2Imm(imm, &op2imm)) {
        ORRis(rd, rn, op2imm, stat);
    } else {
        // We couldn't encode the value directly, so use an intermediate
        // register to encode the value. We will use IP to do this unless rn is
        // IP; in that case we can reuse rd. This allows every case other than
        // "ORR IP, IP, =#imm".
        Register    rm = (rn == IP) ? (rd) : (IP);
        NanoAssert(rn != rm);

        ORRs(rd, rn, rm, stat);
        asm_ld_imm(rm, imm);
    }
}

// Encode "rd = rn ^ imm" using an appropriate instruction sequence.
// Set stat to 1 to update the status flags. Otherwise, set it to 0 or omit it.
// (The declaration in NativeARM.h defines the default value of stat as 0.)
//
// It is not valid to call this function if:
//   (rd == IP) AND (rn == IP) AND !encThumb2Imm(imm)
// Where: if (encThumb2Imm(imm)), imm can be encoded as an ARM operand 2 using the
// encThumb2Imm method.
void
Assembler::asm_eor_imm(Register rd, Register rn, int32_t imm, int stat /* =0 */)
{
    // Thumb2 immediate encoding for operand 2.
    uint32_t    op2imm;

    NanoAssert(IsGpReg(rd));
    NanoAssert(IsGpReg(rn));
    NanoAssert((stat & 1) == stat);

    // Try to encode the value directly as an operand 2 immediate value, then
    // fall back to loading the value into a register.
    if (encThumb2Imm(imm, &op2imm)) {
        EORis(rd, rn, op2imm, stat);
    } else {
        // We couldn't encoder the value directly, so use an intermediate
        // register to encode the value. We will use IP to do this unless rn is
        // IP; in that case we can reuse rd. This allows every case other than
        // "EOR IP, IP, =#imm".
        Register    rm = (rn == IP) ? (rd) : (IP);
        NanoAssert(rn != rm);

        EORs(rd, rn, rm, stat);
        asm_ld_imm(rm, imm);
    }
}

// --------------------------------
// Assembler functions.
// --------------------------------

void Assembler::nBeginAssembly()
{
    max_out_args = 0;
    insnsMustBeContiguous = false;
}

NIns*
Assembler::genPrologue()
{
    // NJ_RESV_OFFSET is space at the top of the stack for us
    // to use for parameter passing (8 bytes at the moment)
    uint32_t stackNeeded = max_out_args + STACK_GRANULARITY * _activation.stackSlotsNeeded();
    uint32_t savingCount = 2;

	RegisterMask savingMask = rmask(FP) | rmask(LR);

    // so for alignment purposes we've pushed return addr and fp
    uint32_t stackPushed = STACK_GRANULARITY * savingCount;
    uint32_t aligned = alignUp(stackNeeded + stackPushed, NJ_ALIGN_STACK);
    int32_t amt = aligned - stackPushed;

    // Make room on stack for what we are doing
    if (amt)
        asm_sub_imm(SP, SP, amt);

    verbose_only( asm_output("## %p:",(void*)_nIns); )
    verbose_only( asm_output("## patch entry"); )
    NIns *patchEntry = _nIns;

    MOV(FP, SP);
    PUSH_mask(savingMask);
    return patchEntry;
}

void
Assembler::nFragExit(LIns* guard)
{
    // FIXME
    // The Thumb2 port need not support the LIR_exit instruction, as we do not
    // use it in Tamarin.  Ditching this support would let us avoid extra overhead
    // of using IP rather than R0 to pass the function result to the epilogue.

    SideExit *  exit = guard->record()->exit;
    Fragment *  frag = exit->target;

    bool        target_is_known = frag && frag->fragEntry;

    if (target_is_known) {
        // The target exists so we can simply emit a branch to its location.
        JMP_far(frag->fragEntry);
    } else {
        // The target doesn't exit yet, so emit a jump to the epilogue. If the
        // target is created later on, the jump will be patched.

        GuardRecord *gr = guard->record();

        if (!_epilogue)
            _epilogue = genEpilogue();

        // Jump to the epilogue. This may get patched later, but JMP_far always
        // emits two instructions even when only one is required, so patching
        // will work correctly.
        JMP_far(_epilogue);

        // In the future you may want to move this further down so that we can
        // overwrite the r0 guard record load during a patch to a different
        // fragment with some assumed input-register state. Not today though.
        gr->jmp = _nIns;

        // NB: this is a workaround for the fact that, by patching a
        // fragment-exit jump, we could be changing the *meaning* of the R0
        // register we're passing to the jump target. If we jump to the
        // epilogue, ideally R0 means "return value when exiting fragment".
        // If we patch this to jump to another fragment however, R0 means
        // "incoming 0th parameter". This is just a quirk of ARM ABI. So
        // we compromise by passing "return value" to the epilogue in IP,
        // not R0, and have the epilogue MOV(R0, IP) first thing.

        asm_ld_imm(IP, int(gr));
    }

#ifdef NJ_VERBOSE
    if (_config.arm_show_stats) {
        // load R1 with Fragment *fromFrag, target fragment
        // will make use of this when calling fragenter().
        int fromfrag = int((Fragment*)_thisfrag);
        asm_ld_imm(RegAlloc::argRegs[1], fromfrag);
    }
#endif

    // profiling for the exit
    verbose_only(
       if (_logc->lcbits & LC_FragProfile) {
           asm_inc_m32( &guard->record()->profCount );
       }
    )

    // Pop the stack frame.
    MOV(SP, FP);
}

NIns*
Assembler::genEpilogue()
{
        // On ARMv5+, loading directly to PC correctly handles interworking.
    // This doesn't really matter, though, as we are now assuming Thumb2
    // everywhere.

    POP_mask(rmask(FP) | rmask(PC));

    // NB: this is the later half of the dual-nature patchable exit branch
    // workaround noted above in nFragExit. IP has the "return value"
    // incoming, we need to move it to R0.
    MOV(R0, IP);

    return _nIns;
}

/*
 * asm_arg will encode the specified argument according to the current ABI, and
 * will update r and stkd as appropriate so that the next argument can be
 * encoded.
 *
 * Linux has used ARM's EABI for some time; support for the legacy ABI
 * has now been removed.
 *
 * Under EABI:
 * - doubles are 64-bit aligned both in registers and on the stack.
 *   If the next available argument register is R1, it is skipped
 *   and the double is placed in R2:R3.  If R0:R1 or R2:R3 are not
 *   available, the double is placed on the stack, 64-bit aligned.
 * - 32-bit arguments are placed in registers and 32-bit aligned
 *   on the stack.
 *
 * Under EABI with hardware floating-point procedure-call variant:
 * - Same as EABI, but doubles are passed in D0..D7 registers.
 */
void
Assembler::asm_arg(ArgType ty, LIns* arg, ParameterRegisters& params)
{
    // The stack pointer must always be at least aligned to 4 bytes.
    NanoAssert((params.stkd & 3) == 0);

    if (ty == ARGTYPE_D || ty == ARGTYPE_F || ty == ARGTYPE_F4) {
        // This task is fairly complex and so is delegated to asm_arg_float.
        asm_arg_float(arg, params);
    } else {
        NanoAssert(ty == ARGTYPE_I || ty == ARGTYPE_UI);
        // pre-assign registers R0-R3 for arguments (if they fit)
        if (params.r < R4) {
            asm_regarg(ty, arg, params.r);
            params.r = Register(params.r + 1);
        } else {
            asm_stkarg(arg, params.stkd);
            params.stkd += 4;
        }
    }
}

// Encode a floating-point argument using the appropriate ABI.
// This function operates in the same way as asm_arg, except that it will only
// handle arguments where (ArgType)ty == ARGTYPE_D/F/F4.

#ifdef NJ_ARM_EABI_HARD_FLOAT
void
Assembler::asm_arg_float(LIns* arg, ParameterRegisters& params)
{
    NanoAssert(ARM_VFP);

    // The stack pointer must always be at least aligned to 4 bytes.
    NanoAssert((params.stkd & 3) == 0);
    // The only use for this function when we are using soft floating-point
    // is for LIR_ii2d.
    NanoAssert(ARM_VFP || arg->isop(LIR_ii2d));
    bool singlePrecision = arg->isF();
    bool qwordPrecision =  arg->isF4();

    Register base = D0;
    int shift = 1;
    if (singlePrecision) { shift = 0; base = S0; }
    if (qwordPrecision) { shift = 2; base = Q0; }

    int max_reg = 16;
    RegisterMask start_mask = ((1 << (1 << (shift))) - 1) << 16;
    int float_r = 0;

    while (float_r < max_reg && (params.float_mask & start_mask) != start_mask) {
        start_mask <<= (1ULL << shift);
        float_r += (1 << shift);
    }
    if (float_r < max_reg) {
        Register    ra = (float_r >> shift) | base;
        params.float_mask &= ~start_mask;

        verbose_only(if (_logc->lcbits & LC_Native)
                         _logc->printf("Selected %s\nParams.float_mask now: %x\n", gpn(ra), (int)params.float_mask));

        if (arg->isInReg()) {
            asm_nongp_copy(ra, arg->getReg());
    } else {
            findSpecificRegForUnallocated(arg, ra);
        }
    } else {
        // The argument won't fit in registers, so pass on to asm_stkarg.
        // EABI requires that 64-bit arguments are 64-bit aligned.
        if ( !singlePrecision && ((params.stkd & 7) != 0) ){
            // stkd will always be aligned to at least 4 bytes; this was
            // asserted on entry to this function.
            params.stkd += 4;
        }
        asm_stkarg(arg, params.stkd);
        params.stkd += singlePrecision ? 4  :
                       qwordPrecision ? 16 : 8;
    }
}
#else // !NJ_ARM_EABI_HARD_FLOAT, aka softfp abi
void
Assembler::asm_arg_float(LIns* arg, ParameterRegisters& params)
{
    // The stack pointer must always be at least aligned to 4 bytes.
    NanoAssert((params.stkd & 3) == 0);
    // The only use for this function when we are using soft floating-point
    // is for LIR_ii2d.
    NanoAssert(ARM_VFP || arg->isop(LIR_ii2d));
    bool singlePrecision = arg->isF();
    bool qwordPrecision =  arg->isF4();

    // EABI requires that 64-bit and 128-bit arguments are aligned on even-numbered
    // registers, as R0:R1 or R2:R3. If the register base is at an
    // odd-numbered register, advance it. Note that this will push r past
    // R3 if r is R3 to start with, and will force the argument to go on
    // the stack. Also, for 128-bit arguments this means that half of the argument
    // may be in registers (R2:R3) and half on stack

    if( !singlePrecision && ((params.r == R1) || (params.r == R3)) ) {
        params.r = Register(params.r + 1);
    }

    verbose_only(if (_logc->lcbits & LC_Native)
        _logc->printf("Param.r now: %d (%s)\n",REGNUM(params.r),gpn(params.r)));
    if (params.r <= R3) {
        Register    ra = params.r;
        Register dm = 0; // FIXME

        if (singlePrecision) {
            NanoAssert(ARM_VFP);
            dm = findRegFor(arg, FpSRegs);
            FMRS(ra,dm);
            params.r = Register(ra + 1);
            return;
        }

        Register    rb = Register(params.r + 1);
        params.r = Register(rb + 1);

        // EABI requires that 64-bit arguments are aligned on even-numbered
        // registers, as R0:R1 or R2:R3.
        NanoAssert( ((ra == R0) && (rb == R1)) || ((ra == R2) && (rb == R3)) );

        // Put the argument in ra and rb. If the argument is in a VFP register,
        // use FMRRD to move it to ra and rb. Otherwise, let asm_regarg deal
        // with the argument as if it were two 32-bit arguments.
        if (ARM_VFP) {
            if (qwordPrecision) {
                dm = findRegFor(arg, FpQRegs);
                verbose_only(if (_logc->lcbits & LC_Native)
                    _logc->printf("FOUND QREG: %d (%s)",REGNUM(dm),gpn(dm)));
                dm = DReg (FpQRegNum(dm)*2); // get first Dn reg. in pair
                verbose_only(if (_logc->lcbits & LC_Native)
                    _logc->printf(" -> TRANSFORMED TO DREG: %d (%s)\n",REGNUM(dm),gpn(dm)));
                NanoAssert(IsFpDReg(dm));
            } else {
                dm = findRegFor(arg, FpDRegs);
            }
            FMRRD(ra, rb, dm);
        } else {
            NanoAssert(!qwordPrecision && !singlePrecision);
            asm_regarg(ARGTYPE_I, arg->oprnd1(), ra);
            asm_regarg(ARGTYPE_I, arg->oprnd2(), rb);
        }
        if(!qwordPrecision) return; // we are done, unless the arg is float4

        // handle the second half of the float 4; note that it could go to stack!
        qwordPrecision = false; 
        dm = dm + 1;
        NanoAssert(ARM_VFP); NanoAssert(IsFpDReg(dm));
        
        if(params.r<R3){
            ra = params.r;
            rb = Register(params.r + 1);
            NanoAssert( (ra == R2) && (rb == R3) ); NanoAssert(ARM_VFP);
            params.r = Register(rb + 1);
            FMRRD(ra, rb, dm);
        } else {
            // handle storing here, it's too complicated to defer to arm_stkarg
            if ((params.stkd & 7) != 0) 
                params.stkd += 4;
             FSTD(dm, SP, params.stkd);
             params.stkd += 8;
        }
    } else {
        // The argument won't fit in registers, so pass on to asm_stkarg.
        // EABI requires that 64-bit arguments are 64-bit aligned.
        if ( !singlePrecision && ((params.stkd & 7) != 0) ){
            // stkd will always be aligned to at least 4 bytes; this was
            // asserted on entry to this function.
            params.stkd += 4;
        }
        if (ARM_VFP) {
            asm_stkarg(arg, params.stkd);
        } else {
            asm_stkarg(arg->oprnd1(), params.stkd);
            asm_stkarg(arg->oprnd2(), params.stkd+4);
        }
        params.stkd += singlePrecision ? 4  :
                        qwordPrecision ? 16 : 8;
    }
}
#endif // NJ_ARM_EABI_HARD_FLOAT

void
Assembler::asm_regarg(ArgType ty, LIns* p, Register rd)
{
    // Note that we don't have to prepareResultReg here because it is already
    // done by the caller, and the target register is passed as 'rd'.
    // Similarly, we don't have to freeResourcesOf(p).

    if (ty == ARGTYPE_I || ty == ARGTYPE_UI)
    {
        // Put the argument in register rd.
        if (p->isImmI()) {
            asm_ld_imm(rd, p->immI());
        } else {
            if (p->isInReg()) {
                MOV(rd, p->getReg());
            } else {
                // Re-use the target register if the source is no longer
                // required. This saves a MOV instruction.
                findSpecificRegForUnallocated(p, rd);
            }
        }
    } else {
        NanoAssert(ty == ARGTYPE_D);
        // Floating-point arguments are handled as two integer arguments.
        NanoAssert(false);
    }
}

void
Assembler::asm_stkarg(LIns* arg, int stkd)
{
    // The ABI doesn't allow accesses below the SP.
    NanoAssert(stkd >= 0);
    // The argument resides somewhere in registers, so we simply need to
    // push it onto the stack.
    if (arg->isI()) {
        Register rt = findRegFor(arg, GpRegs);
        asm_str(rt, SP, stkd);
    } else {
        // According to the comments in asm_arg_float, LIR_ii2d
        // can have a 64-bit argument even if VFP is disabled. However,
        // asm_arg_float will split the argument and issue two 32-bit
        // arguments to asm_stkarg so we can ignore that case here.
        bool singlePrecision = arg->isF();
        bool quad = arg->isF4();
        NanoAssert(arg->isD() || singlePrecision || quad);
        NanoAssert(ARM_VFP);
        Register dt = findRegFor(arg, singlePrecision? FpSRegs:
                                                 quad? FpQRegs:FpDRegs);
        if (singlePrecision) {
            NanoAssert((stkd % 4) == 0);
            FSTS(dt,SP,stkd);
        } else if (quad) {
            // EABI requires that 128-bit arguments are 64-bit aligned.
            NanoAssert((stkd % 8) == 0);
            VSTQR(dt, IP);
            asm_add_imm(IP, SP, stkd);
        } else {
            // EABI requires that 64-bit arguments are 64-bit aligned.
            NanoAssert((stkd % 8) == 0);
            FSTD(dt, SP, stkd);
        }
    }
}

void 
Assembler::asm_pushstate()
{
    NanoAssert(false);
}

void 
Assembler::asm_popstate()
{
    NanoAssert(false);
}

void 
Assembler::asm_savepc()
{
    NanoAssert(false);
}

void 
Assembler::asm_restorepc()
{
    NanoAssert(false);
}

void 
Assembler::asm_discardpc()
{
    NanoAssert(false);
}

void
Assembler::asm_call(LIns* ins)
{
    bool handled = false;
    NanoAssert(ARM_VFP);
    RegisterMask nonVolatile = 0;
    if (ARM_VFP) {
        // Because ARM actually returns the result in (R0,R1), and not in a
        // floating point register, the code to move the result into a correct
        // register is below.  We do nothing here.
        //
        // The reason being that if we did something here, the final code
        // sequence we'd get would be something like:
        //     MOV {R0-R3},params        [from below]
        //     BL function               [from below]
        //     MOV {R0-R3},spilled data  [from evictScratchRegsExcept()]
        //     MOV Dx,{R0,R1}            [from here]
        // which is clearly broken.
        //
        // This is not a problem for non-floating point calls, because the
        // restoring of spilled data into R0 is done via a call to
        // prepareResultReg(R0) in the other branch of this if-then-else,
        // meaning that evictScratchRegsExcept() will not modify R0. However,
        // prepareResultReg is not aware of the concept of using a register
        // pair (R0,R1) for the result of a single operation, so it can only be
        // used here with the ultimate VFP register, and not R0/R1, which
        // potentially allows for R0/R1 to get corrupted as described.

#ifdef NJ_ARM_EABI_HARD_FLOAT
        // With ARM hardware floating point ABI, D0 is used to return the double
        // from the function. We need to prepare it like we do for R0 in the else
        // branch.
        if(ins->isop(LIR_calld)){
           prepareResultReg(ins, rmask(D0));
           freeResourcesOf(ins);
           handled = true;
        }
        else if(ins->isop(LIR_callf)){
            prepareResultReg(ins, rmask(S0));
            freeResourcesOf(ins);
            handled = true;
        } 
        else if(ins->isop(LIR_callf4)){
            prepareResultReg(ins, rmask(Q0));
            freeResourcesOf(ins);
            handled = true;
        };
#else   //!NJ_ARM_EABI_HARD_FLOAT
        bool isFloat = ins->isop(LIR_callf4) || ins->isop(LIR_callf);
        if (ins->isop(LIR_calld) || isFloat) {
            if (ins->isInReg()) {
                nonVolatile = rmask(ins->getReg());
                asm_maybe_spill(ins, false);
            }
            handled = true;
        }
#endif
    }
    if (!handled && !ins->isop(LIR_callv)) {
        verbose_only(if (_logc->lcbits & LC_Native)
           _logc->printf(" ASM_CALL preparing result reg %d\n",RegAlloc::retRegs[0]));
        prepareResultReg(ins, rmask(RegAlloc::retRegs[0]));
        // Immediately free the resources as we need to re-use the register for
        // the arguments.
        freeResourcesOf(ins);
    }

    // Do this after we've handled the call result, so we don't
    // force the call result to be spilled unnecessarily.
    verbose_only(if (_logc->lcbits & LC_Native) 
        _logc->printf(" ASM_CALL evicting scratch registers\n"));
    evictScratchRegsExcept(nonVolatile);
    verbose_only(if (_logc->lcbits & LC_Native)
        _logc->printf(" ASM_CALL evicted scratch registers\n"));

    const CallInfo* ci = ins->callInfo();
    ArgType argTypes[MAXARGS];
    uint32_t argc = ci->getArgTypes(argTypes);
    bool indirect = ci->isIndirect();

    // If we aren't using VFP, assert that the LIR operation is an integer
    // function call.
    NanoAssert(ARM_VFP || ins->isop(LIR_callv) || ins->isop(LIR_calli));

    // If we're using VFP, but not hardware floating point ABI, and
    // the return type is a double, it'll come back in R0/R1.
    // We need to either place it in the result fp reg, or store it.
    // See comments above for more details as to why this is necessary here
    // for floating point calls, but not for integer calls.
    if (!ARM_EABI_HARD && ARM_VFP && ins->isExtant()) {
        // If the result size is a floating-point value, treat the result
        // specially, as described previously.
        ArgType rtype = ci->returnType();
        bool returnsFloat = rtype == ARGTYPE_F || rtype == ARGTYPE_F4;
        if (rtype==ARGTYPE_D || returnsFloat) {
            if(ins->isInReg()){
                Register dd = ins->getReg();
                // Copy the result to the (VFP) result register.
                if (rtype==ARGTYPE_D){
                    NanoAssert(ins->isop(LIR_calld));
                    FMDRR(dd, R0, R1);
                } 
                else if (rtype == ARGTYPE_F ){
                    NanoAssert(ins->isop(LIR_callf));
                    FMSR(dd,R0);
                } else {
                    NanoAssert(ins->isop(LIR_callf4) && rtype== ARGTYPE_F4);
                    Register d0 = DReg ( FpQRegNum(dd)*2 );
                    Register d1 = d0 + 1; // ok since d0 can't be D15
                    NanoAssert(IsFpDReg(d0) && IsFpDReg(d1));
                    FMDRR(d0, R0, R1);
                    FMDRR(d1, R2, R3);
                }
                freeResourcesOf(ins);
            } else {
                int d = findMemFor(ins);
                // Immediately free the resources so the arguments can re-use
                // the slot.
                freeResourcesOf(ins);

                // The result doesn't have a register allocated, so store the
                // result (in R0[,R1[,R2,R3]]) directly to its stack slot.
                asm_str(R0, FP, d+0);
                if (rtype != ARGTYPE_F) {
                    asm_str(R1, FP, d+4);
                    if (rtype == ARGTYPE_F4) {
                        NanoAssert(ins->isop(LIR_callf4));
                        asm_str(R2, FP, d+8);
                        asm_str(R3, FP, d+12);
                    } else {
                        NanoAssert(ins->isop(LIR_calld));
                    }
                } else {
                    NanoAssert(ins->isop(LIR_callf));
                }
            }
        } else {
            NanoAssert(false); // should never get here!! rtype must be ARGTYPE_D, F or F4
        }
    }

    // Emit the branch.
    if (!indirect) {
        verbose_only(if (_logc->lcbits & LC_Native)
            outputf("        %p:", _nIns);
        )
        BranchWithLink(ci->_address);
    } else {
        // Indirect call: we assign the address arg to LR
        BLXr(LR);
        asm_regarg(ARGTYPE_I, ins->arg(--argc), LR);
    }

    verbose_only(if (_logc->lcbits & LC_Native)
        _logc->printf(" ASM_CALL preparing %d params!\n", argc));

    // Encode the arguments, starting at R0 and with an empty argument stack (0).
    // With hardware fp ABI, floating point arguments start from D0.
    ParameterRegisters params;

    // Iterate through the argument list and encode each argument according to
    // the ABI.
    // Note that we loop through the arguments backwards as LIR specifies them
    // in reverse order.
    uint32_t    i = argc;
    while(i--) {
        verbose_only(if (_logc->lcbits & LC_Native)
            _logc->printf(" ASM_CALL preparing param %d of type %x!\n", i, argTypes[i]));
        asm_arg(argTypes[i], ins->arg(i), params);
    }

    if (params.stkd > max_out_args) {
        max_out_args = params.stkd;
    }
    verbose_only(if (_logc->lcbits & LC_Native) _logc->printf(" ASM_CALL DONE!\n"));
}

inline Register 
getSuitableRegFor(Register r, enum LTy insType)
{
    switch(insType){
        case LTy_I: NanoAssert(IsGpReg(r));  return r;
        case LTy_F: NanoAssert(IsFpSReg(r)); return r;
        case LTy_D: NanoAssert(IsFpSReg(r) || IsFpDReg(r) ); 
                    if(IsFpSReg(r)) return DReg(FpSRegNum(r)/2);
                                    return r;
        case LTy_F4:NanoAssert(!IsGpReg(r));
                    if(IsFpSReg(r)) return QReg(FpSRegNum(r)/4);
                    if(IsFpDReg(r)) return QReg(FpDRegNum(r)/2);
                    return r;

        default:    NanoAssertMsg(0, "TODO: Soft-float implementation of LIR_stf.");
    }
    return r;
}

Register 
RegAlloc::getSuitableRegFor(Register r,LIns* ins)
{
    NanoAssert(ins);
    if(!ins) return r;
    return nanojit::getSuitableRegFor(r,ins->retType());
}

#ifdef NANOJIT_64BIT
#error ARM CAN'T HANDLE 64bit YET'
#endif

Register 
RegAlloc::getAvailableReg(LIns* ins, Register regClass, RegisterMask m)
{
    NanoAssert(!ins || ins->opcode()==LIR_label || !ins->isV());
    if (m == 0)
        return UnspecifiedReg;
    if (!ins) {
        Register ret = msReg(m);
        return ret;
    }
    if (ins->isI() || IsGpReg(regClass)) {
        NanoAssert(m != 0);
        return msReg(m);
    }
    enum LTy insType = (ins->isF()  || IsFpSReg(regClass)) ? LTy_F :
                       (ins->isD()  || IsFpDReg(regClass)) ? LTy_D :
                       (ins->isF4() || IsFpQReg(regClass)) ? LTy_F4 :
                       LTy_V;
    NanoAssert(insType != LTy_V);
    while (m != 0) {
        Register reg = msReg(m);
        reg = nanojit::getSuitableRegFor(reg, insType);
        RegisterMask rm = rmask(reg);
        if ((m & rm) == rm)
            return reg;
        m &= ~rm;
    }
    NanoAssert(insType != LTy_F); // shouldn't get here with non-composite registers
    return UnspecifiedReg;
}

RegisterMask
RegAlloc::nInitManagedRegisters()
{
#define _config _assembler->_config  //hack to make ARM_VFP macro work from here
    RegisterMask retval = ( (RegisterMask)0x7ff) | ((RegisterMask)1<< REGNUM(LR)); // R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,LR 
    if (ARM_VFP) {
        retval |= FpQRegs;
    }
    return retval;
#undef _config
}

// FIXME
// Peephole optimization of adjacent register spill/reload instructions.
// This is tricky code with dependencies on the classic ARM instruction encoding.
// For now, this is disabled for Thumb2.
#ifdef NJ_ARM_SPILL_PEEPHOLE
static inline ConditionCode
get_cc(NIns *ins)
{
    return ConditionCode((*ins >> 28) & 0xF);
}

static inline bool
branch_is_B(NIns* branch)
{
    return (unpack32(branch) & 0x0E000000) == 0x0A000000;
}

static inline bool
branch_is_LDR_PC(NIns* branch)
{
    return (unpack32(branch) & 0x0F7FF000) == 0x051FF000;
}

// Is this an instruction of the form  ldr/str reg, [fp, #-imm] ?
static inline bool
is_ldstr_reg_fp_minus_imm(/*OUT*/uint32_t* isLoad, /*OUT*/uint32_t* rX,
                          /*OUT*/uint32_t* immX, NIns i1)
{
    if ((i1 & 0xFFEF0000) != 0xE50B0000)
        return false;
    *isLoad = (i1 >> 20) & 1;
    *rX     = (i1 >> 12) & 0xF;
    *immX   = i1 & 0xFFF;
    return true;
}

// Is this an instruction of the form  ldmdb/stmdb fp, regset ?
static inline bool
is_ldstmdb_fp(/*OUT*/uint32_t* isLoad, /*OUT*/uint32_t* regSet, NIns i1)
{
    if ((i1 & 0xFFEF0000) != 0xE90B0000)
        return false;
    *isLoad = (i1 >> 20) & 1;
    *regSet = i1 & 0xFFFF;
    return true;
}

// Make an instruction of the form ldmdb/stmdb fp, regset
static inline NIns
mk_ldstmdb_fp(uint32_t isLoad, uint32_t regSet)
{
    return 0xE90B0000 | (regSet & 0xFFFF) | ((isLoad & 1) << 20);
}

// Compute the number of 1 bits in the lowest 16 bits of regSet
static inline uint32_t
size_of_regSet(uint32_t regSet)
{
   uint32_t x = regSet;
   x = (x & 0x5555) + ((x >> 1) & 0x5555);
   x = (x & 0x3333) + ((x >> 2) & 0x3333);
   x = (x & 0x0F0F) + ((x >> 4) & 0x0F0F);
   x = (x & 0x00FF) + ((x >> 8) & 0x00FF);
   return x;
}

// See if two ARM instructions, i1 and i2, can be combined into one
static bool
do_peep_2_1(/*OUT*/NIns* merged, NIns i1, NIns i2)
{
    uint32_t rX, rY, immX, immY, isLoadX, isLoadY, regSet;
    /*   ld/str rX, [fp, #-8]
         ld/str rY, [fp, #-4]
         ==>
         ld/stmdb fp, {rX, rY}
         when
         X < Y and X != fp and Y != fp and X != 15 and Y != 15
    */
    if (is_ldstr_reg_fp_minus_imm(&isLoadX, &rX, &immX, i1) &&
        is_ldstr_reg_fp_minus_imm(&isLoadY, &rY, &immY, i2) &&
        immX == 8 && immY == 4 && rX < rY &&
        isLoadX == isLoadY &&
        rX != FP && rY != FP &&
         rX != 15 && rY != 15) {
        *merged = mk_ldstmdb_fp(isLoadX, (1 << rX) | (1<<rY));
        return true;
    }
    /*   ld/str   rX, [fp, #-N]
         ld/stmdb fp, regset
         ==>
         ld/stmdb fp, union(regset,{rX})
         when
         regset is nonempty
         X < all elements of regset
         N == 4 * (1 + card(regset))
         X != fp and X != 15
    */
    if (is_ldstr_reg_fp_minus_imm(&isLoadX, &rX, &immX, i1) &&
        is_ldstmdb_fp(&isLoadY, &regSet, i2) &&
        regSet != 0 &&
        (regSet & ((1 << (rX + 1)) - 1)) == 0 &&
        immX == 4 * (1 + size_of_regSet(regSet)) &&
        isLoadX == isLoadY &&
        rX != FP && rX != 15) {
        *merged = mk_ldstmdb_fp(isLoadX, regSet | (1 << rX));
        return true;
    }
    return false;
}

// Determine whether or not it's safe to look at _nIns[1].
// Necessary condition for safe peepholing with do_peep_2_1.
static inline bool
does_next_instruction_exist(NIns* _nIns, NIns* codeStart, NIns* codeEnd,
                            NIns* exitStart, NIns* exitEnd)
{
    return (exitStart <= _nIns && _nIns+2 < exitEnd) ||
           (codeStart <= _nIns && _nIns+2 < codeEnd);
}
#endif /* NJ_ARM_SPILL_PEEPHOLE */

const RegisterMask PREFER_SPECIAL = ~ ((RegisterMask)0);
// Init per-opcode register hint table.  Defaults to no hints for all instructions 
// (initialized to 0 )
static bool
nHintsInit(RegisterMask Hints[])
{
    VMPI_memset(Hints, 0, sizeof(RegisterMask) * LIR_sentinel );
    Hints[LIR_calli]  = rmask(RegAlloc::retRegs[0]);
    Hints[LIR_hcalli] = rmask(RegAlloc::retRegs[1]);
    Hints[LIR_paramp] = PREFER_SPECIAL;

    return true;
}

RegisterMask
RegAlloc::nHint(LIns* ins)
{
    static RegisterMask  Hints[LIR_sentinel+1]; // effectively const, save for the initialization
    static bool initialized = nHintsInit(Hints); (void)initialized; 
    
    RegisterMask prefer = Hints[ins->opcode()];
    if(prefer != PREFER_SPECIAL) return prefer;

    NanoAssert(ins->isop(LIR_paramp));
    if (ins->paramKind() == 0)
        if (ins->paramArg() < 4)
            prefer = rmask(argRegs[ins->paramArg()]);
    return prefer;
}

void
Assembler::asm_qjoin(LIns *ins)
{
    int d = findMemFor(ins);
    NanoAssert(d);
    LIns* lo = ins->oprnd1();
    LIns* hi = ins->oprnd2();

    Register rlo;
    Register rhi;

    findRegFor2(GpRegs, lo, rlo, GpRegs, hi, rhi);

    asm_str(rhi, FP, d+4);
    asm_str(rlo, FP, d);

    freeResourcesOf(ins);
}

void
Assembler::asm_store32(LOpcode op, LIns *value, int dr, LIns *base)
{
    Register ra, rb;
    getBaseReg2(GpRegs, value, ra, GpRegs, base, rb, dr);

    switch (op) {
        case LIR_sti:
            if (isU8(-dr) || isU12(dr)) {
                STR(ra, rb, dr);
            } else {
                STR(ra, IP, 0);
                asm_add_imm(IP, rb, dr);
            }
            return;
        case LIR_sti2c:
            if (isU8(-dr) || isU12(dr)) {
                STRB(ra, rb, dr);
            } else {
                STRB(ra, IP, 0);
                asm_add_imm(IP, rb, dr);
            }
            return;
        case LIR_sti2s:
            if (isU8(-dr) || isU12(dr)) {
                STRH(ra, rb, dr);
            } else {
                STRH(ra, IP, 0);
                asm_add_imm(IP, rb, dr);
            }
            return;
        default:
            NanoAssertMsg(0, "asm_store32 should never receive this LIR opcode");
            return;
    }
}

bool
canRematALU(LIns *ins)
{
    // Return true if we can generate code for this instruction that neither
    // sets CCs, clobbers an input register, nor requires allocating a register.
    switch (ins->opcode()) {
    case LIR_addi:
    case LIR_subi:
    case LIR_andi:
    case LIR_ori:
    case LIR_xori:
        return ins->oprnd1()->isInReg() && ins->oprnd2()->isImmI();
    default:
        ;
    }
    return false;
}

bool
RegAlloc::canRemat(LIns* ins)
{
    return ins->isImmI() || ins->isop(LIR_allocp) || canRematALU(ins);
}

void
Assembler::asm_restore(LIns* i, Register r)
{
    // The following registers should never be restored:
    NanoAssert(r != PC);
    NanoAssert(r != IP);
    NanoAssert(r != SP);

    if (i->isop(LIR_allocp)) {
        int d = findMemFor(i);
        asm_add_imm(r, FP, d);
    } else if (i->isImmI()) {
        asm_ld_imm(r, i->immI());
    } else if (canRematALU(i)) {
        Register rn = i->oprnd1()->getReg();
        int32_t imm = i->oprnd2()->immI();
        switch (i->opcode()) {
        case LIR_addi: asm_add_imm(r, rn, imm, /*stat=*/ 0); break;
        case LIR_subi: asm_sub_imm(r, rn, imm, /*stat=*/ 0); break;
        case LIR_andi: asm_and_imm(r, rn, imm, /*stat=*/ 0); break;
        case LIR_ori:  asm_orr_imm(r, rn, imm, /*stat=*/ 0); break;
        case LIR_xori: asm_eor_imm(r, rn, imm, /*stat=*/ 0); break;
        default:       NanoAssert(0);                        break;
        }
    } else {
        // We can't easily load immediate values directly into FP registers, so
        // ensure that memory is allocated for the constant and load it from
        // memory.
        int d = findMemFor(i);
        if (ARM_VFP && IsFpDReg(r)) {
            if (d % 4 != 0) {
                FLDD(r, IP, 0);
                asm_add_imm(IP, FP, d);
            } else if (isU8(d/4) || isU8(-d/4)) {
                FLDD(r, FP, d);
            } else {
                FLDD(r, IP, d%1024);
                asm_add_imm(IP, FP, d-(d%1024));
            }
        } else {
            asm_ldr(r, FP, d);
#ifdef NJ_ARM_SPILL_PEEPHOLE
            NIns merged;
            // See if we can merge this load into an immediately following
            // one, by creating or extending an LDM instruction.
            if (/* is it safe to poke _nIns[1] ? */
                does_next_instruction_exist(_nIns, codeStart, codeEnd,
                                                   exitStart, exitEnd)
                && /* can we merge _nIns[0] into _nIns[1] ? */
                   do_peep_2_1(&merged, _nIns[0], _nIns[1])) {
                _nIns[1] = merged;
                _nIns++;
                verbose_only(
                    _nInsAfter++;
                    asm_output("merge next into LDMDB");
                )
            }
#endif /* NJ_ARM_SPILL_PEEPHOLE */
        }
    }
}

void
Assembler::asm_spill(Register rr, int d, int8_t /*nWords*/)
{
    NanoAssert(d);
    // The following registers should never be spilled:
    NanoAssert(rr != PC && rr != IP && rr != SP);

    bool isFPR = IsFpDReg(rr) || IsFpSReg(rr) || IsFpQReg(rr);
    NanoAssert(ARM_VFP || !isFPR);
    if (ARM_VFP && isFPR) {
        if (IsFpQReg(rr)) {
            VSTQR(rr, IP);
            asm_add_imm(IP, FP, d);
        } else if (d % 4 != 0) {
            VSTR(rr, IP, 0);
            asm_add_imm(IP, FP, d);
        } else if (isU8(d/4) || isU8(-d/4)) {
            VSTR(rr, FP, d);
        } else {
            VSTR(rr, IP, d%1024);
            asm_add_imm(IP, FP, d-(d%1024));
        }
    } else {
        // asm_str always succeeds, but returns '1' to indicate that it emitted
        // a simple, easy-to-merge STR.
        if (asm_str(rr, FP, d)) {
#ifdef NJ_ARM_SPILL_PEEPHOLE
            NIns merged;
            // See if we can merge this store into an immediately following one,
            // one, by creating or extending a STM instruction.
            if (/* is it safe to poke _nIns[1] ? */
                    does_next_instruction_exist(_nIns, codeStart, codeEnd,
                        exitStart, exitEnd)
                    && /* can we merge _nIns[0] into _nIns[1] ? */
                    do_peep_2_1(&merged, _nIns[0], _nIns[1])) {
                _nIns[1] = merged;
                _nIns++;
                verbose_only(
                    _nInsAfter++;
                    asm_output("merge next into STMDB");
                )
            }
#endif /* NJ_ARM_SPILL_PEEPHOLE */
        }
    }
}

void
Assembler::asm_load128(LIns* ins)
{
    LIns*       base = ins->oprnd1();
    Register    rn = findRegFor(base, GpRegs);
    int         offset = ins->disp();
    Register qd = prepareResultReg(ins, FpQRegs );

    VLDQR(qd, IP);
    asm_add_imm(IP, rn, offset);
    
    freeResourcesOf(ins);
}

void
Assembler::asm_load64(LIns* ins)
{
    NanoAssert(ins->isD() || ins->isF());

    if (ARM_VFP) {
        Register    dd;
        LIns*       base = ins->oprnd1();
        Register    rn = findRegFor(base, GpRegs);
        int         offset = ins->disp();

        if (ins->isInReg()) {
            dd = prepareResultReg(ins, ins->isD()? (FpDRegs & ~rmask(D0)) : (FpSRegs& ~rmask(S0)) );
        } else {
            // If the result isn't already in a register, use the VFP scratch
            // register for the result and store it directly into memory.
            NanoAssert(ins->isInAr());
            int d = arDisp(ins);
            evictIfActive(D0);
            dd = ins->isD()? D0:S0;
            // VFP can only do loads and stores with a range of ±1020, so we
            // might need to do some arithmetic to extend its range.
            if (d % 4 != 0) {
                VSTR(dd, IP, 0);
                asm_add_imm(IP, FP, d);
            } else if (isU8(d/4) || isU8(-d/4)) {
                VSTR(dd, FP, d);
            } else {
                VSTR(dd, IP, d%1024);
                asm_add_imm(IP, FP, d-(d%1024));
            }
        }

        switch (ins->opcode()) {
            case LIR_ldf:
            case LIR_ldd:
                if (offset % 4 != 0) {
                    VLDR(dd, IP, 0);
                    asm_add_imm(IP, rn, offset);
                } else if (isU8(offset/4) || isU8(-offset/4)) {
                    VLDR(dd, rn, offset);
                } else {
                    VLDR(dd, IP, offset%1024);
                    asm_add_imm(IP, rn, offset-(offset%1024));
                }
                break;
            case LIR_ldf2d:
                evictIfActive(D0);
                FCVTDS(dd, S0);
                if (offset % 4 != 0) {
                    FLDS(S0, IP, 0);
                    asm_add_imm(IP, rn, offset);
                } else if (isU8(offset/4) || isU8(-offset/4)) {
                    FLDS(S0, rn, offset);
                } else {
                    FLDS(S0, IP, offset%1024);
                    asm_add_imm(IP, rn, offset-(offset%1024));
                }
                break;
            default:
                NanoAssertMsg(0, "LIR opcode unsupported by asm_load64.");
                break;
        }
    } else {
        NanoAssert(ins->isInAr());
        int         d = arDisp(ins);

        LIns*       base = ins->oprnd1();
        Register    rn = findRegFor(base, GpRegs);
        int         offset = ins->disp();

        switch (ins->opcode()) {
            case LIR_ldd:
                asm_mmq(FP, d, rn, offset);
                break;
            case LIR_ldf:
                NanoAssertMsg(0, "LIR_ldf is not yet implemented for soft-float.");
                break;
            case LIR_ldf2d:
                NanoAssertMsg(0, "LIR_ldf2d is not yet implemented for soft-float.");
                break;
            default:
                NanoAssertMsg(0, "LIR opcode unsupported by asm_load64.");
                break;
        }
    }

    freeResourcesOf(ins);
}

void
Assembler::asm_store128(LOpcode op, LIns* value, int dr, LIns* base)
{
    NanoAssert(op==LIR_stf4); (void)op;
    NanoAssert(ARM_VFP && value->isF4());
    Register rn = findRegFor(base, GpRegs);
    Register qd = findRegFor(value, FpQRegs);

    VSTQR(qd, IP);
    asm_add_imm(IP, rn, dr);
}

void
Assembler::asm_store64(LOpcode op, LIns* value, int dr, LIns* base)
{
    NanoAssert(value->isD() || value ->isF());

    if (ARM_VFP) {
        Register rn = findRegFor(base, GpRegs);
        Register dd = value->isD()? findRegFor(value, FpDRegs)
                                  : findRegFor(value, FpSRegs);

        switch (op) {
            case LIR_stf:
            case LIR_std:
                // VFP can only do stores with a range of ±1020, so we might
                // need to do some arithmetic to extend its range.
                if (dr % 4 != 0) {
                    VSTR(dd, IP, 0); // FSTS or FSTD
                    asm_add_imm(IP, rn, dr);
                } else if (isU8(dr/4) || isU8(-dr/4)) {
                    VSTR(dd, rn, dr);      // FSTS or FSTD
                } else {
                    VSTR(dd, IP, dr%1024); // FSTS or FSTD
                    asm_add_imm(IP, rn, dr-(dr%1024));
                }

                break;
            case LIR_std2f: {
                // VFP can only do stores with a range of ±1020, so we might
                // need to do some arithmetic to extend its range.
                Register tmp = _allocator.allocTempReg(FpSRegs,S0);
                if (dr % 4 != 0) {
                    FSTS(tmp, IP, 0);
                    asm_add_imm(IP, rn, dr);
                } else if (isU8(dr/4) || isU8(-dr/4)) {
                    FSTS(tmp, rn, dr);
                } else {
                    FSTS(tmp, IP, dr%1024);
                    asm_add_imm(IP, rn, dr-(dr%1024));
                }

                FCVTSD(tmp, dd);

                break;      
            }
            default:
                NanoAssertMsg(0, "LIR opcode unsupported by asm_store64.");
                break;
        }
    } else {
        int         d = findMemFor(value);
        Register    rn = findRegFor(base, GpRegs);

        switch (op) {
            case LIR_stf:
                NanoAssertMsg(0, "TODO: Soft-float implementation of LIR_stf.");
                break;
            case LIR_std:
                // Doubles in soft-float never get registers allocated, so this
                // is always a simple two-word memcpy.
                // *(uint64_t*)(rb+dr) = *(uint64_t*)(FP+da)
                asm_mmq(rn, dr, FP, d);
                break;
            case LIR_std2f:
                NanoAssertMsg(0, "TODO: Soft-float implementation of LIR_std2f.");
                break;
            default:
                NanoAssertMsg(0, "LIR opcode unsupported by asm_store64.");
                break;
        }
    }
}

// Load the float32 specified by immFasI into VFP register dd.
void
Assembler::asm_immf(Register dd, int32_t immFasI)
{
    ReserveContiguousSpace(this, 4*3);

    // Why 8? In Thumb2 state, the value of the PC is the address of 
    // the current instruction plus 4 bytes.
    // See also coments in asm_immd for explanations
    FLDS(dd, PC, -8);
    emitImm(immFasI);
    B_nochk(_nIns + 2);
}

// Load the float64 specified by immDhi:immDlo into VFP register dd.
void
Assembler::asm_immd(Register dd, int32_t immDlo, int32_t immDhi)
{
    // We're not going to use a slot, because it might be too far
    // away.  Instead, we're going to stick a branch in the stream to
    // jump over the constants, and then load from a short PC relative
    // offset.

    // stream should look like:
    //    branch A
    //    immDlo
    //    immDhi
    // A: FLDD PC-12

    ReserveContiguousSpace(this, 4*4);

    FLDD(dd, PC, -12);
    emitImm(immDhi);
    emitImm(immDlo);
    B_nochk(_nIns + 4);
}

void
Assembler::asm_immd(LIns* ins)
{
    // If the value isn't in a register, it's simplest to use integer
    // instructions to put the value in its stack slot. Otherwise, use a VFP
    // load to get the value from a literal pool.
    if (ARM_VFP && ins->isInReg()) {
        Register dd = prepareResultReg(ins, FpDRegs);
        asm_immd(dd, ins->immDlo(), ins->immDhi());
    } else {
        NanoAssert(ins->isInAr());
        int d = arDisp(ins);
        asm_str(IP, FP, d+4);
        asm_ld_imm(IP, ins->immDhi());
        asm_str(IP, FP, d);
        asm_ld_imm(IP, ins->immDlo());
    }

    freeResourcesOf(ins);
}

RegisterMask
RegAlloc::nRegCopyCandidates(Register r, RegisterMask allow) {
    if (IsGpReg(r))
        return allow & GpRegs;
    if (IsFpDReg(r))
        return allow & FpDRegs;
    if (IsFpSReg(r))
        return allow & FpSRegs;
    NanoAssert(IsFpQReg(r)); // It must be; other cases already handled.
    return allow & FpQRegs;
}

void
Assembler::asm_nongp_copy(Register r, Register s)
{
    if (ARM_VFP && IsFpDReg(r) && IsFpDReg(s)) {
        // fp D -> D
        FCPYD(r, s);
    } else if (ARM_VFP && IsFpQReg(r) && IsFpQReg(s)) {
        // fp Q -> Q
        FCPYQ(r, s);
    } else if (ARM_VFP && IsFpSReg(r) && IsFpSReg(s)) {
        // fp S -> S
        FCPYS(r, s);
    } else {
        verbose_only(if (_logc->lcbits & LC_Native) _logc->printf("nongpcpy %d<-%d\n",r,s));
        // We can't move between registers of different precisions, so
        // assert that no calling code is trying to do that.
        // Theoretically we could move between Sn and GPR, but we shouldn't
        // need to, we we leave that unhandled
        NanoAssert(!"Illegal register combination in asm_nongp_copy()");
    }
}

// copy 64 bits: (rd+dd) <- (rs+ds)
void
Assembler::asm_mmq(Register rd, int dd, Register rs, int ds)
{
    // The value is either a 64bit struct or maybe a float that isn't live in
    // an FPU reg.  Either way, don't put it in an FPU reg just to load & store
    // it.
    // This operation becomes a simple 64-bit memcpy.

    // In order to make the operation optimal, we will require two GP
    // registers. We can't allocate a register here because the caller may have
    // called deprecated_freeRsrcOf, and allocating a register here may cause something
    // else to spill onto the stack which has just be conveniently freed by
    // deprecated_freeRsrcOf (resulting in stack corruption).
    //
    // Falling back to a single-register implementation of asm_mmq is better
    // than adjusting the callers' behaviour (to allow us to allocate another
    // register here) because spilling a register will end up being slower than
    // just using the same register twice anyway.
    //
    // Thus, if there is a free register which we can borrow, we will emit the
    // following code:
    //  LDR rr, [rs, #ds]
    //  LDR ip, [rs, #(ds+4)]
    //  STR rr, [rd, #dd]
    //  STR ip, [rd, #(dd+4)]
    // (Where rr is the borrowed register.)
    //
    // If there is no free register, don't spill an existing allocation. Just
    // do the following:
    //  LDR ip, [rs, #ds]
    //  STR ip, [rd, #dd]
    //  LDR ip, [rs, #(ds+4)]
    //  STR ip, [rd, #(dd+4)]
    //
    // Note that if rs+4 or rd+4 is outside the LDR or STR range, extra
    // instructions will be emitted as required to make the code work.

    // Ensure that the PC is not used as either base register. The instruction
    // generation macros call underrunProtect, and a side effect of this is
    // that we may be pushed onto another page, so the PC is not a reliable
    // base register.
    NanoAssert(rs != PC);
    NanoAssert(rd != PC);

    // We use IP as a swap register, so check that it isn't used for something
    // else by the caller.
    NanoAssert(rs != IP);
    NanoAssert(rd != IP);

    // Find a free GPR register for temporary usage
    Register    tmp= _allocator.allocTempRegIfAvailable(AllowableFlagRegs, R0);

    // Ensure that ds and dd are within the -255..4095 offset range of STR and
    // LDR. If either is out of range, adjust and modify rd or rs so that the
    // load works correctly.
    // The modification here is performed after the LDR/STR block (because code
    // is emitted backwards), so this one is the reverse operation.

    int32_t dd_adj = 0;
    int32_t ds_adj = 0;

    if ((dd+4) >= 0x1000) {
        dd_adj = ((dd+4) & ~0xfff);
    } else if (dd <= -0x100) {
        dd_adj = -((-dd) & ~0xff);
    }
    if ((ds+4) >= 0x1000) {
        ds_adj = ((ds+4) & ~0xfff);
    } else if (ds <= -0x100) {
        ds_adj = -((-ds) & ~0xff);
    }

    // These will emit no code if d*_adj is 0.
    asm_sub_imm(rd, rd, dd_adj);
    asm_sub_imm(rs, rs, ds_adj);

    ds -= ds_adj;
    dd -= dd_adj;

    if (tmp!=UnspecifiedReg) {
        // Emit the actual instruction sequence.
        STR(IP , rd, dd+4);
        STR(tmp, rd, dd);
        LDRi(IP , rs, ds+4);
        LDRi(tmp, rs, ds);
    } else {
        // There are no free registers, so fall back to using IP twice.
        STR(IP, rd, dd+4);
        LDRi(IP, rs, ds+4);
        STR(IP, rd, dd);
        LDRi(IP, rs, ds);
    }

    // Re-adjust the base registers. (These will emit no code if d*_adj is 0.
    asm_add_imm(rd, rd, dd_adj);
    asm_add_imm(rs, rs, ds_adj);
}

// Increment the 32-bit profiling counter at pCtr, without
// changing any registers.
#ifdef NJ_THUMB2_TODO
verbose_only(
void Assembler::asm_inc_m32(uint32_t* pCtr)
{

    // We need to temporarily free up two registers to do this, so
    // just push r0 and r1 on the stack.  This assumes that the area
    // at r13 - 8 .. r13 - 1 isn't being used for anything else at
    // this point (this is guaranteed by the EABI).
    //
    // Plan: emit the following bit of code.  It's not efficient, but
    // this is for profiling debug builds only, and is self contained,
    // except for above comment re stack use.
    //
    // E92D0003                 push    {r0,r1}
    // E59F0000                 ldr     r0, [r15]   ; pCtr
    // EA000000                 b       .+8         ; jump over imm
    // 12345678                 .word   0x12345678  ; pCtr
    // E5901000                 ldr     r1, [r0]
    // E2811001                 add     r1, r1, #1
    // E5801000                 str     r1, [r0]
    // E8BD0003                 pop     {r0,r1}

    // We need keep the 4 words beginning at "ldr r0, [r15]"
    // together.  Simplest to underrunProtect the whole thing.
    underrunProtect(8*4);
    IMM32(0xE8BD0003);       //  pop     {r0,r1}
    IMM32(0xE5801000);       //  str     r1, [r0]
    IMM32(0xE2811001);       //  add     r1, r1, #1
    IMM32(0xE5901000);       //  ldr     r1, [r0]
    IMM32((uint32_t)pCtr);   //  .word   pCtr
    IMM32(0xEA000000);       //  b       .+8
    IMM32(0xE59F0000);       //  ldr     r0, [r15]
    IMM32(0xE92D0003);       //  push    {r0,r1}
})
#else
verbose_only(
void Assembler::asm_inc_m32(uint32_t* pCtr)
{
	// TODO: This is used only in verbose mode, to support fragment profiling.
	// I believe this is used only by TraceMonkey, and is ignored by Tamarin.
	// I can certainly see this becoming useful in the future, however, so let's
    // keep this around as a reminder.
    NanoAssert(!"No fragment profiling support on Thumb2");
    (void)pCtr;
})
#endif

// --------------------------------
// Code buffer management.
// --------------------------------

void
Assembler::nativePageReset()
{
    // nothing to do
}

void
Assembler::nativePageSetup()
{
    NanoAssert(!_inExit);
    if (!_nIns)
        codeAlloc(codeStart, codeEnd, _nIns verbose_only(, codeBytes), NJ_MAX_CPOOL_OFFSET);
}

void
Assembler::underrunProtect(int bytes)
{
    NanoAssertMsg(bytes<=LARGEST_UNDERRUN_PROT, "constant LARGEST_UNDERRUN_PROT is too small");
    uintptr_t pc = uintptr_t(_nIns);
    uintptr_t top = uintptr_t(codeStart);
    if (pc - bytes < top)
    {
        NanoAssert(!insnsMustBeContiguous);
        verbose_only(verbose_outputf("        %p:", _nIns);)
        NIns* target = _nIns;
        // This may be in a normal code chunk or an exit code chunk.
        codeAlloc(codeStart, codeEnd, _nIns verbose_only(, codeBytes), NJ_MAX_CPOOL_OFFSET);

        //### FIXME: This may have to emit a long branch.
        //### Do we always get here with two words to spare?
        B_nochk(target);
    }
}

/*
 * VFP
 */

void
Assembler::asm_i2d(LIns* ins)
{
    Register dd = prepareResultReg(ins, FpDRegs);
    Register tmp = _allocator.allocTempReg(FpSRegs & ~rmask(dd), S0);
    Register rt = findRegFor(ins->oprnd1(), GpRegs);

    FSITOD(dd, tmp);
    FMSR(tmp, rt);

    freeResourcesOf(ins);
}

void
Assembler::asm_ui2d(LIns* ins)
{
    Register dd = prepareResultReg(ins, FpDRegs);
    Register tmp = _allocator.allocTempReg(FpSRegs & ~rmask(dd), S0);
    Register rt = findRegFor(ins->oprnd1(), GpRegs);

    FUITOD(dd, tmp);
    FMSR(tmp, rt);

    freeResourcesOf(ins);
}

void Assembler::asm_d2i(LIns* ins)
{
    Register tmp = _allocator.allocTempReg(FpSRegs, S0);
    if (ins->isInReg()) {
        Register rt = ins->getReg();
        FMRS(rt, tmp);
    } else {
        // There's no active result register, so store the result directly into
        // memory to avoid the FP->GP transfer cost on Cortex-A8.
        int32_t d = arDisp(ins);
        // VFP can only do stores with a range of ±1020, so we might need to do
        // some arithmetic to extend its range.
        if (isU8(d/4) || isU8(-d/4)) {
            FSTS(tmp, FP, d);
        } else {
            FSTS(tmp, IP, d%1024);
            asm_add_imm(IP, FP, d-(d%1024));
        }
    }

    // note: tmp is enough; if half of reg isn't available, the reg isn't available. 
    Register dm = findRegFor(ins->oprnd1(), FpDRegs & ~rmask(tmp));

    FTOSID(tmp, dm);

    freeResourcesOf(ins);
}

void Assembler::asm_ui2f(LIns *ins) {
    Register dd = prepareResultReg(ins, FpSRegs );
    Register rt = findRegFor(ins->oprnd1(), GpRegs);
    Register tmp = _allocator.allocTempReg(FpSRegs & ~rmask(dd) ,S0);
    FUITOS(dd, tmp);
    FMSR(tmp, rt);
    
    freeResourcesOf(ins);
}

void Assembler::asm_i2f(LIns *ins) {
    Register dd = prepareResultReg(ins, FpSRegs );
    Register rt = findRegFor(ins->oprnd1(), GpRegs);
    Register tmp = _allocator.allocTempReg(FpSRegs & ~rmask(dd) ,S0);
    FSITOS(dd, tmp);
    FMSR(tmp, rt);
    
    freeResourcesOf(ins);
}

void Assembler::asm_f2i(LIns *ins) {
    Register rt = prepareResultReg(ins, GpRegs );
    Register dd = findRegFor(ins->oprnd1(), FpSRegs);
    Register tmp = _allocator.allocTempReg(FpSRegs & ~rmask(dd),S0);
    FMRS(rt,tmp);
    FTOSIS(tmp, dd);
    
    freeResourcesOf(ins);
}

void Assembler::asm_f2d(LIns *ins) {
    Register dd = prepareResultReg(ins, FpDRegs );
    Register sm = findRegFor(ins->oprnd1(), FpSRegs);
    FCVTDS(dd, sm);

    freeResourcesOf(ins);
}

void Assembler::asm_d2f(LIns *ins) {
    Register sd = prepareResultReg(ins, FpSRegs );
    Register dm = findRegFor(ins->oprnd1(), FpDRegs);
    FCVTSD(sd, dm);

    freeResourcesOf(ins);
}

void Assembler::asm_immf(LIns *ins) {
    //TODO: use fconsts/fconstd?
    
    // If the value isn't in a register, it's simplest to use integer
    // instructions to put the value in its stack slot. Otherwise, use a VFP
    // load to get the value from a literal pool.
    if (ARM_VFP && ins->isInReg()) {
        Register dd = prepareResultReg(ins, FpSRegs);
        asm_immf(dd, ins->immFasI());
    } else {
        NanoAssert(ins->isInAr());
        int d = arDisp(ins);
        asm_str(IP, FP, d);
        asm_ld_imm(IP, ins->immFasI());
    }
    
    freeResourcesOf(ins);
}
    
void Assembler::asm_immf4(LIns *ins) {
    union {
        float4_t f4;
        NIns32 fp;
        } u;
    u.f4 = ins->immF4();
    const NIns32* fp = &u.fp;

    if (ARM_VFP && ins->isInReg()) {
        Register dd = prepareResultReg(ins, FpQRegs);

        // The float4 literal will be dropped in the instruction stream below, with
        // a branchover.  Here, the 128-bit value is loaded from meory as a pair of
        // 64-bit FP register loads using VLDM (vector load multiple). 
        VLDQR(dd,IP);
        
        // We generate at most 7 words below.  They must be kept contiguous
        // so that correct alignment can be maintained.
        ReserveContiguousSpace(this, 4*7);
                
        // Instructions are aligned to 4-byte boundaries, but the embedded
        // float4 literal must be aligned to an 8-byte boundary, as we are
        // moving a pair of words at a time.  See the description of VLDM
        // in section A8.8.333.  Predict the alignment at the ADR below, which
        // will be opposite to the alignment at present, since the ADR will
        // occupy 4 bytes.
        bool is_aligned = (((uintptr_t)_nIns) & 7) != 0;

        // When we get to the ADR below, the PC will actually be 4 bytes ahead
        // of the instruction (in Thumb state), pointing to the next.  To find
        // the the address of the float4 literal, we must index backward over the
        // 4 bytes of the ADR itself, then the alignment padding, and
        // then the four 4-byte floats themselves.  The offset is a negative value
        // with respect to the PC, but will be represented as a positive value
        // that is subtracted.  TODO: We might re-introduce a literal pool and
        // avoid the branchover.
        uint32_t offset = 4 + (is_aligned ? 0 : 4) + 4*4;
 
        // ADR IP #-offset
        // Encoding A8.8.12 T2
        emitT32(0xF2AF0000 | IP << 8 | offset);

        // Verify the alignment that we predicted above.
        NanoAssert((is_aligned && (((uintptr_t)_nIns) & 7) == 0) ||
                   (!is_aligned && (((uintptr_t)_nIns) & 7) != 0));

        // Padding inserted here at the end will guarantee that the first word
        // of the literal is properly aligned.        
        if(!is_aligned) {
            BKPT(0);
        }

        // Emit the 4 floats comprising the literal (128 bits total).
        emitImm(fp[3]);
        emitImm(fp[2]);
        emitImm(fp[1]);
        emitImm(fp[0]);
        
        // Branch over the literal.
        B_nochk(_nIns+ (is_aligned?8:10));
    } else {
        NanoAssert(ins->isInAr());
        int d = arDisp(ins);
        asm_str(IP, FP, d);
        asm_ld_imm(IP, fp[0]);
        asm_str(IP, FP, d+4);
        asm_ld_imm(IP, fp[1]);
        asm_str(IP, FP, d+8);
        asm_ld_imm(IP, fp[2]);
        asm_str(IP, FP, d+12);
        asm_ld_imm(IP, fp[3]);
    }
    
    freeResourcesOf(ins);
}
                    
void Assembler::asm_f2f4(LIns *ins) {
    NanoAssert(ARM_VFP);
    Register qd = prepareResultReg(ins, FpQRegs );
    Register rs = findRegFor(ins->oprnd1(), FpSRegs);
    VDUP(qd, rs);
    freeResourcesOf(ins);
}

void Assembler::asm_ffff2f4(LIns *ins) {
    NanoAssert(ARM_VFP);
    Register qd = prepareResultReg(ins, LowBankFPMask );
    Register rx = findRegFor(ins->oprnd1(),FpSRegs & ~ rmask(qd));
    FCPYS( SReg((FpQRegNum(qd) << 2) + 0), rx);
    Register ry = findRegFor(ins->oprnd2(),FpSRegs & ~ rmask(qd));
    FCPYS( SReg((FpQRegNum(qd) << 2) + 1), ry);
    Register rz = findRegFor(ins->oprnd3(),FpSRegs & ~ rmask(qd));
    FCPYS( SReg((FpQRegNum(qd) << 2) + 2), rz);
    Register rw = findRegFor(ins->oprnd4(),FpSRegs & ~ rmask(qd));
    FCPYS( SReg((FpQRegNum(qd) << 2) + 3), rw);
    freeResourcesOf(ins);
}
    
void Assembler::asm_f4comp(LIns *ins) {
    LIns *a = ins->oprnd1();
    if (!ins->isop(LIR_swzf4)) {
        // LIR_f4x,f4y,f4z,f4w
        NanoAssert(ins->isF() && a->isF4());
        Register rd = prepareResultReg(ins, FpSRegs);
        Register rs = findRegFor(a, LowBankFPMask);
        int idx = 0;
        switch (ins->opcode()) {
            default: NanoAssertMsg(0,"unsupported opcode in asm_f4comp"); break;
            case LIR_f4x: idx = 0; break;
            case LIR_f4y: idx = 1; break;
            case LIR_f4z: idx = 2; break;
            case LIR_f4w: idx = 3; break;
        }
        
        rs = SReg(4 * FpQRegNum(rs)) + idx;
        NanoAssert(IsFpSReg(rs));
        FCPYS(rd, rs);
        freeResourcesOf(ins);
    } else {
        // LIR_swzf4:
        // Emit four single-precision moves, according to mask.  Since
        // the mask can implement swap and rotates, rd and rs cannot be
        // the same register.
        NanoAssert(ins->isF4() && a->isF4());
        Register rd = prepareResultReg(ins, LowBankFPMask);
        Register rs = findRegFor(a, LowBankFPMask & ~rmask(rd));
        int mask = ins->mask();
        // source component registers; choose based on mask.
        Register rs0 = SReg(4 * FpQRegNum(rs)) + ((mask >> 0) & 3);
        Register rs1 = SReg(4 * FpQRegNum(rs)) + ((mask >> 2) & 3);
        Register rs2 = SReg(4 * FpQRegNum(rs)) + ((mask >> 4) & 3);
        Register rs3 = SReg(4 * FpQRegNum(rs)) + ((mask >> 6) & 3);
        // dest register components: hard coded 0,1,2,3
        Register rd0 = SReg(4 * FpQRegNum(rd)) + 0;
        Register rd1 = rd0 + 1;
        Register rd2 = rd0 + 2;
        Register rd3 = rd0 + 3;
        // emit four copies
        FCPYS(rd0, rs0);
        FCPYS(rd1, rs1);
        FCPYS(rd2, rs2);
        FCPYS(rd3, rs3);
        // done.
        freeResourcesOf(ins);
    }
}
    
void Assembler::asm_condf4(LIns *ins) {
    Register rd = prepareResultReg(ins, GpRegs);
    SETEQ(rd);
    freeResourcesOf(ins);
    asm_cmpf4(ins);
}
               
// WARNING: This function cannot generate any code that will affect the
// condition codes prior to the generation of the ucomisd.  See asm_cmpi()
// for more details.
void Assembler::asm_cmpf4(LIns *ins) {

    NanoAssert(ins->opcode()== LIR_eqf4);
    NanoAssert(ARM_VFP);

    LIns* lhs = ins->oprnd1();
    LIns* rhs = ins->oprnd2();
    NanoAssert( lhs->isF4() && rhs->isF4() );
   
    /* We do this:
      - compare vectors for equality (VCEQ) - 4 32-bit elements, either 0xff(eq) or 0x0 (neq)
      - Narrow result (VMOVN) - 4 16-bit elements in a Dn reg (the lower half of the Qn)
      - Negate result (VMVN) - 4 16-bit elements, 0x0 for eq, 0xff for new
      - Compare Dn reg with 0
      - FMSTAT - transfer to flags
    */
    
    FMSTAT();

    Register tmp = _allocator.allocTempReg(FpQRegs,Q0);
    Register tmp0 = DReg(FpQRegNum(tmp)*2);
    Register tmp1 = tmp0+1; // ok since tmp0 can't be D15
    VCMPZ(tmp1);
    VMVN(tmp1,tmp0);
    VMOVN(tmp0,tmp);

    Register ra, rb;
    RegisterMask allow = FpQRegs & ~rmask(tmp);
    findRegFor2(allow, lhs, ra, allow, rhs, rb);
    VCEQ(tmp,ra,rb);
}

void
Assembler::asm_neg_abs(LIns* ins)
{
    LIns* lhs = ins->oprnd1();
    NanoAssert((lhs->isD()  && ins->isD())  ||
               (lhs->isF4() && ins->isF4()) ||
               (lhs->isF()  && ins->isF()));
    RegisterMask resultMask = ins->isD() ? FpDRegs :
                              ins->isF() ? FpSRegs :
                                           FpQRegs;
    Register dd = prepareResultReg(ins,resultMask );
    // If the argument doesn't have a register assigned, re-use dd.
    Register dm = lhs->isInReg() ? lhs->getReg() : dd;

    VNEG(dd, dm);

    freeResourcesOf(ins);
    if (dd == dm) {
        NanoAssert(!lhs->isInReg());
        findSpecificRegForUnallocated(lhs, dd);
    }
}

void Assembler::asm_recip_sqrt(LIns*) {
    NanoAssert(!"not implemented");
}

void
Assembler::asm_fop(LIns* ins)
{
    LIns*   lhs = ins->oprnd1();
    LIns*   rhs = ins->oprnd2();

    NanoAssert((ins->isD() && lhs->isD() && rhs->isD())
            || (ins->isF() && lhs->isF() && rhs->isF())
            || (ins->isF4() && lhs->isF4() && rhs->isF4()));
    RegisterMask legalQRegs = FpQRegs;
    if (ins->opcode()==LIR_divf4) {
        // We do divf4 through 4 single-precision fdivs; we are restricted to S0-31 (Q0-7)
        legalQRegs = LowBankFPMask;
    }
    Register    dd = ins->isD()? prepareResultReg(ins, FpDRegs): 
                     ins->isF()? prepareResultReg(ins, FpSRegs)
                               : prepareResultReg(ins, legalQRegs);

    Register    dn = lhs->isInReg() ? lhs->getReg() : dd;
    Register    dm = rhs->isInReg() ? rhs->getReg() : dd;

    if (ins->opcode()==LIR_divf4) {
        if (lhs->isInReg()) {
            if(! (rmask(dn) & legalQRegs) ){
                if(rhs->isInReg()){ // try to reuse the dd Register
                    dn = dd; 
                } else {
                    dn = findRegFor(lhs,legalQRegs & ~rmask(dd) );
                }
            }
        }
        
        if (lhs==rhs) {
            dm = dn; 
        } else {
            if (rhs->isInReg()) {
                if(!(rmask(dm) & legalQRegs)) {
                    if (dn!=dd) { // try to reuse the dd Register, if possible
                        dm = dd;
                    } else {
                        dm = findRegFor(rhs, legalQRegs & ~rmask(dd) & ~rmask(dn));
                    }
                }
            }            
        }
        
    } 

    if ((dn == dm) && (lhs != rhs)) {
        // We can't re-use the result register for both arguments, so we'll force one
        // into its own register.
        RegisterMask allowedRegs = rhs->isD() ? FpDRegs :
                                   rhs->isF() ? FpSRegs :
                                                legalQRegs;
        dm = findRegFor(rhs, allowedRegs  & ~rmask(dd));
        NanoAssert(rhs->isInReg());
    }

    // TODO: Special cases for simple constants.

    switch (ins->opcode()) {
        case LIR_addf: case LIR_addf4: case LIR_addd:      VADD(dd,dn,dm);        break;
        case LIR_subf: case LIR_subf4: case LIR_subd:      VSUB(dd,dn,dm);        break;
        case LIR_mulf: case LIR_mulf4: case LIR_muld:      VMUL(dd,dn,dm);        break;
        case LIR_divf: case LIR_divf4: case LIR_divd:      VDIV(dd,dn,dm);        break;
        default: NanoAssert(false); break;
    }

    freeResourcesOf(ins);

    // If we re-used the result register, mark it as active.
    if (dn == dd) {
        if (lhs->isInReg())
            findSpecificRegFor(lhs,dd);
        else
            findSpecificRegForUnallocated(lhs, dd);
    } else if (dm == dd ) {
        if (rhs->isInReg()) {
            findSpecificRegFor(rhs, dd);
        } else {
            findSpecificRegForUnallocated(rhs, dd);
        }
    } else {
        NanoAssert(lhs->isInReg());
        NanoAssert(rhs->isInReg());
    }
}

void
Assembler::asm_cmpd(LIns* ins)
{
    LIns* lhs = ins->oprnd1();
    LIns* rhs = ins->oprnd2();
    LOpcode op = ins->opcode();
    bool singlePrecision = isCmpFOpcode(op);
    NanoAssert(ARM_VFP);
    NanoAssert(  (isCmpDOpcode(op) && lhs->isD() && rhs->isD()) 
               ||( singlePrecision && lhs->isF() && rhs->isF()));
    RegisterMask regClass = singlePrecision? FpSRegs:FpDRegs;

    Register ra, rb;
    findRegFor2(regClass, lhs, ra, regClass, rhs, rb);

    int e_bit = (op != LIR_eqd) & (op != LIR_eqf);

    // Do the comparison and get results loaded in ARM status register.
    // TODO: For asm_condd, we should put the results directly into an ARM
    // machine register, then use bit operations to get the result.
    FMSTAT();
    VCMP(ra, rb, e_bit);
}

/* Call this with targ set to 0 if the target is not yet known and the branch
 * will be patched up later.
 */
Branches
Assembler::asm_branch(bool branchOnFalse, LIns* cond, NIns* targ)
{
    LOpcode condop = cond->opcode();
    NanoAssert(cond->isCmp());
    NanoAssert(ARM_VFP || !isCmpDOpcode(condop));

    // The old "never" condition code has special meaning on newer ARM cores,
    // so use "always" as a sensible default code.
    ConditionCode cc = AL;

    // Detect whether or not this is a floating-point comparison.
    bool    fp_cond;

    // Select the appropriate ARM condition code to match the LIR instruction.
    switch (condop) {
        // Floating-point conditions. Note that the VFP LT/LE conditions
        // require use of the unsigned condition codes, even though
        // float-point comparisons are always signed.
        case LIR_eqf4:
        case LIR_eqd: case LIR_eqf:   cc = EQ;    fp_cond = true;     break;
        case LIR_ltd: case LIR_ltf:   cc = LO;    fp_cond = true;     break;
        case LIR_led: case LIR_lef:   cc = LS;    fp_cond = true;     break;
        case LIR_ged: case LIR_gef:   cc = GE;    fp_cond = true;     break;
        case LIR_gtd: case LIR_gtf:   cc = GT;    fp_cond = true;     break;

        // Standard signed and unsigned integer comparisons.
        case LIR_eqi:   cc = EQ;    fp_cond = false;    break;
        case LIR_lti:   cc = LT;    fp_cond = false;    break;
        case LIR_lei:   cc = LE;    fp_cond = false;    break;
        case LIR_gti:   cc = GT;    fp_cond = false;    break;
        case LIR_gei:   cc = GE;    fp_cond = false;    break;
        case LIR_ltui:  cc = LO;    fp_cond = false;    break;
        case LIR_leui:  cc = LS;    fp_cond = false;    break;
        case LIR_gtui:  cc = HI;    fp_cond = false;    break;
        case LIR_geui:  cc = HS;    fp_cond = false;    break;

        // Default case for invalid or unexpected LIR instructions.
        default:        cc = AL;    fp_cond = false;    break;
    }

    // Invert the condition if required.
    if (branchOnFalse)
        cc = OppositeCond(cc);

    // Ensure that we got a sensible condition code.
    NanoAssert((cc != AL) && (cc != NV));

    // Ensure that we don't hit floating-point LIR codes if VFP is disabled.
    NanoAssert(ARM_VFP || !fp_cond);

    // Emit a suitable branch instruction.
    B_cond(cc, targ);

    // Store the address of the branch instruction so that we can return it.
    // asm_[f]cmp will move _nIns so we must do this now.
    NIns *at = _nIns;

    asm_cmp(cond);

    return Branches(at);
}

NIns* Assembler::asm_branch_ov(LOpcode op, NIns* target)
{
    // Because MUL can't set the V flag, we use SMULL and CMP to set the Z flag
    // to detect overflow on multiply. Thus, if we have a LIR_mulxovi, we must
    // be conditional on !Z, not V.
    ConditionCode cc = ( (op == LIR_mulxovi) || (op == LIR_muljovi) ? NE : VS );

    // Emit a suitable branch instruction.
    B_cond(cc, target);
    return _nIns;
}

void
Assembler::asm_cmp(LIns *cond)
{
    LIns* lhs = cond->oprnd1();
    LIns* rhs = cond->oprnd2();

    // Forward floating-point comparisons directly to asm_cmpd to simplify
    // logic in other methods which need to issue an implicit comparison, but
    // don't care about the details of comparison itself.
    if (lhs->isD()) {
        NanoAssert(rhs->isD());
        asm_cmpd(cond);
        return;
    }
    if (lhs->isF()) {
        NanoAssert(rhs->isF());
        asm_cmpd(cond);
        return;
    }
    if (lhs->isF4()) {
        NanoAssert(rhs->isF4());
        asm_cmpf4(cond);
        return;
    }
    
    NanoAssert(lhs->isI() && rhs->isI());

    // ready to issue the compare
    if (rhs->isImmI()) {
        int c = rhs->immI();
        Register r = findRegFor(lhs, GpRegs);
        asm_cmpi(r, c);
    } else {
        Register ra, rb;
        findRegFor2(GpRegs, lhs, ra, GpRegs, rhs, rb);
        CMP(ra, rb);
    }
}

void
Assembler::asm_cmpi(Register r, int32_t imm)
{
    uint32_t    op2imm;

    NanoAssert(IsGpReg(r));

    if (encThumb2Imm(imm, &op2imm)) {
        CMPi(r, op2imm);
    } else if (encThumb2Imm(-imm, &op2imm)) {
        CMNi(r, op2imm);
    } else {
        CMP(r, IP);
        asm_ld_imm(IP, imm);
    }
}

void
Assembler::asm_condd(LIns* ins)
{
    Register rd = prepareResultReg(ins, GpRegs);

    LOpcode op = ins->opcode();
    if (isCmpFOpcode(op)) 
        op = getCmpDOpcode(op); // the only difference between float/double is in asm_cmpd

    // TODO: Modify cmpd to allow the FP flags to move directly to an ARM
    // machine register, then use simple bit operations here rather than
    // conditional moves.

    switch (op) {
        case LIR_eqd:   SETEQ(rd);      break;
        case LIR_ltd:   SETLO(rd);      break; // Note: VFP LT/LE operations require
        case LIR_led:   SETLS(rd);      break; // unsigned LO/LS condition codes!
        case LIR_ged:   SETGE(rd);      break;
        case LIR_gtd:   SETGT(rd);      break;
        default:        NanoAssert(0);  break;
    }

    freeResourcesOf(ins);

    asm_cmpd(ins);
}

void
Assembler::asm_cond(LIns* ins)
{
    Register rd = prepareResultReg(ins, GpRegs);
    LOpcode op = ins->opcode();

    switch(op)
    {
        case LIR_eqi:   SETEQ(rd);      break;
        case LIR_lti:   SETLT(rd);      break;
        case LIR_lei:   SETLE(rd);      break;
        case LIR_gti:   SETGT(rd);      break;
        case LIR_gei:   SETGE(rd);      break;
        case LIR_ltui:  SETLO(rd);      break;
        case LIR_leui:  SETLS(rd);      break;
        case LIR_gtui:  SETHI(rd);      break;
        case LIR_geui:  SETHS(rd);      break;
        default:        NanoAssert(0);  break;
    }

    freeResourcesOf(ins);

    asm_cmp(ins);
}

// Emit an instruction of the form:
//    CMP rl, rr, ASR #s
// We do not exploit the operand 2 shift for ALU ops in general,
// but we use this special case when translating LIR_muljovi.

inline void Assembler::CMPr_asr(Register rl, Register rr, int32_t s)
{
    NanoAssert(IsGpReg(rl) && IsGpReg(rr));
    NanoAssert(rl != PC);
    NanoAssert(rr != PC);
    NanoAssert(rr != SP);
    NanoAssert(s >= 0 && s < 32);
    underrunProtect(4);
    emitT32(0xEBB00F20 | rl << 16 | (s >> 2) << 12 | (s & 0x3) << 6 | rr);
    asm_output("cmp  %s, %s, asr #%d", gpn(rl), gpn(rr), s);
} 

void
Assembler::asm_arith(LIns* ins)
{
    LOpcode     op = ins->opcode();
    LIns*       lhs = ins->oprnd1();
    LIns*       rhs = ins->oprnd2();

    // We always need the result register and the first operand register, so
    // find them up-front. (If the second operand is constant it is encoded
    // differently.)
    Register    rd = prepareResultReg(ins, GpRegs);

    // Try to re-use the result register for operand 1.
    Register    rn = lhs->isInReg() ? lhs->getReg() : rd;

    // If the rhs is constant, we can use the instruction-specific code to
    // determine if the value can be encoded in an ARM instruction. If the
    // value cannot be encoded, it will be loaded into a register.
    //
    // Note that the MUL instruction can never take an immediate argument so
    // even if the argument is constant, we must allocate a register for it.
    if (rhs->isImmI() && (op != LIR_muli) && (op != LIR_mulxovi) && (op != LIR_muljovi))
    {
        int32_t immI = rhs->immI();

        switch (op)
        {
            case LIR_addi:       asm_add_imm(rd, rn, immI, 0);  break;
            case LIR_addjovi:
            case LIR_addxovi:    asm_add_imm(rd, rn, immI, 1);  break;
            case LIR_subi:       asm_sub_imm(rd, rn, immI, 0);  break;
            case LIR_subjovi:
            case LIR_subxovi:    asm_sub_imm(rd, rn, immI, 1);  break;
            case LIR_andi:       asm_and_imm(rd, rn, immI, 0);  break;
            case LIR_ori:        asm_orr_imm(rd, rn, immI, 0);  break;
            case LIR_xori:       asm_eor_imm(rd, rn, immI, 0);  break;
            case LIR_lshi:       LSLi(rd, rn, immI);            break;
            case LIR_rshi:       ASRi(rd, rn, immI);            break;
            case LIR_rshui:      LSRi(rd, rn, immI);            break;

            default:
                NanoAssertMsg(0, "Unsupported");
                break;
        }

        freeResourcesOf(ins);
        if (rd == rn) {
            // Mark the re-used register as active.
            NanoAssert(!lhs->isInReg());
            findSpecificRegForUnallocated(lhs, rd);
        }
        return;
    }

    // The rhs is either already in a register or cannot be encoded as an
    // Operand 2 constant for this operation.

    Register    rm = rhs->isInReg() ? rhs->getReg() : rd;
    if ((rm == rn) && (lhs != rhs)) {
        // We can't re-use the result register for both arguments, so force one
        // into its own register. We favour re-use for operand 2 (rm) here as
        // it is more likely to take a fast path for LIR_mul on ARMv5.
        rn = findRegFor(lhs, GpRegs & ~rmask(rd));
        NanoAssert(lhs->isInReg());
    }

    switch (op)
    {
        case LIR_addi:       ADDs(rd, rn, rm, 0);    break;
        case LIR_addjovi:
        case LIR_addxovi:    ADDs(rd, rn, rm, 1);    break;
        case LIR_subi:       SUBs(rd, rn, rm, 0);    break;
        case LIR_subjovi:
        case LIR_subxovi:    SUBs(rd, rn, rm, 1);    break;
        case LIR_andi:       ANDs(rd, rn, rm, 0);    break;
        case LIR_ori:        ORRs(rd, rn, rm, 0);    break;
        case LIR_xori:       EORs(rd, rn, rm, 0);    break;

        case LIR_muli:
            // Note that ARMvv7 allows rd == rn.
            MUL(rd, rn, rm);
            break;
        case LIR_muljovi:
        case LIR_mulxovi:
            // Note that ARMvv7 allows rd == rn.
            // ARM cannot automatically detect overflow from a MUL operation,
            // so we have to perform some other arithmetic:
            //   SMULL  rr, ip, ra, rb
            //   CMP    ip, rr, ASR #31
            // An explanation can be found in bug 521161. This sets Z if we did
            // _not_ overflow, and clears it if we did.
            CMPr_asr(IP, rd, 31);
            SMULL(rd, IP, rn, rm);
            break;

        // The shift operations need a mask to match the JavaScript
        // specification because the ARM architecture allows a greater shift
        // range than JavaScript.
        case LIR_lshi:
            LSL(rd, rn, IP);
            ANDi(IP, rm, 0x1f);
            break;
        case LIR_rshi:
            ASR(rd, rn, IP);
            ANDi(IP, rm, 0x1f);
            break;
        case LIR_rshui:
            LSR(rd, rn, IP);
            ANDi(IP, rm, 0x1f);
            break;
        default:
            NanoAssertMsg(0, "Unsupported");
            break;
    }

    freeResourcesOf(ins);
    // If we re-used the result register, mark it as active.
    if (rn == rd) {
        NanoAssert(!lhs->isInReg());
        findSpecificRegForUnallocated(lhs, rd);
    } else if (rm == rd) {
        NanoAssert(!rhs->isInReg());
        findSpecificRegForUnallocated(rhs, rd);
    } else {
        NanoAssert(lhs->isInReg());
        NanoAssert(rhs->isInReg());
    }
}

void
Assembler::asm_neg_not(LIns* ins)
{
    LIns* lhs = ins->oprnd1();
    Register rr = prepareResultReg(ins, GpRegs);

    // If 'lhs' isn't in a register, we can give it the result register.
    Register ra = lhs->isInReg() ? lhs->getReg() : rr;

    if (ins->isop(LIR_noti)) {
        MVN(rr, ra);
    } else {
        NanoAssert(ins->isop(LIR_negi));
        RSBS0(rr, ra);
    }

    freeResourcesOf(ins);
    if (!lhs->isInReg()) {
        NanoAssert(ra == rr);
        // Update the register state to indicate that we've claimed ra for lhs.
        findSpecificRegForUnallocated(lhs, ra);
    }
}

void
Assembler::asm_load32(LIns* ins)
{
    LOpcode op = ins->opcode();
    LIns*   base = ins->oprnd1();
    int     d = ins->disp();

    Register rt = prepareResultReg(ins, GpRegs);
    // Try to re-use the result register for the base pointer.
    Register rn = base->isInReg() ? base->getReg() : rt;

    // TODO: The x86 back-end has a special case where the base address is
    // given by LIR_addp. The same technique may be useful here to take
    // advantage of ARM's register+register addressing mode.

    switch (op) {
        case LIR_lduc2ui:
            if (isU8(-d) || isU12(d)) {
                LDRB(rt, rn, d);
            } else if (d < 0) {
                LDRB(rt, IP, d%256);
                asm_add_imm(IP, rn, d-(d%256));
            } else {
                LDRB(rt, IP, d%4096);
                asm_add_imm(IP, rn, d-(d%4096));
            }
            break;
        case LIR_ldus2ui:
            // Some ARM machines require 2-byte alignment here.
            if (isU8(-d) || isU12(d)) {
                LDRH(rt, rn, d);
            } else if (d < 0) {
                LDRH(rt, IP, d%256);
                asm_add_imm(IP, rn, d-(d%256));
            } else {
                LDRH(rt, IP, d%4096);
                asm_add_imm(IP, rn, d-(d%4096));
            }
            break;
        case LIR_ldi:
            // Some ARM machines require 4-byte alignment here.
            if (isU8(-d) || isU12(d)) {
                LDRi(rt, rn, d);
            } else if (d < 0) {
                LDRi(rt, IP, d%256);
                asm_add_imm(IP, rn, d-(d%256));
            } else {
                LDRi(rt, IP, d%4096);
                asm_add_imm(IP, rn, d-(d%4096));
            }
            break;
        case LIR_ldc2i:
            // Like LIR_lduc2ui, but sign-extend.
            // Some ARM machines require 2-byte alignment here.
            if (isU8(-d) || isU12(d)) {
                LDRSB(rt, rn, d);
            } else if (d < 0) {
                LDRSB(rn, IP, d%256);
                asm_add_imm(IP, rn, d-(d%256));
            } else {
                LDRSB(rn, IP, d%4096);
                asm_add_imm(IP, rn, d-(d%4096));
            }
            break;
        case LIR_lds2i:
            // Like LIR_ldus2ui, but sign-extend.
            if (isU8(-d) || isU12(d)) {
                LDRSH(rt, rn, d);
            } else if (d < 0) {
                LDRSH(rt, IP, d%256);
                asm_add_imm(IP, rn, d-(d%256));
            } else {
                LDRSH(rt, IP, d%4096);
                asm_add_imm(IP, rn, d-(d%4096));
            }
            break;
        default:
            NanoAssertMsg(0, "asm_load32 should never receive this LIR opcode");
            break;
    }

    freeResourcesOf(ins);

    if (rn == rt) {
        NanoAssert(!base->isInReg());
        findSpecificRegForUnallocated(base, rn);
    }
}

void
Assembler::asm_cmov(LIns* ins)
{
    LIns*           condval = ins->oprnd1();
    LIns*           iftrue  = ins->oprnd2();
    LIns*           iffalse = ins->oprnd3();
    RegisterMask    allow = ins->isD() ? FpDRegs :
                            ins->isF() ? FpSRegs :
                            ins->isF4()? FpQRegs : 
                                         GpRegs;
    ConditionCode   cc;

    NanoAssert(condval->isCmp());
    NanoAssert((ins->isop(LIR_cmovi) && iftrue->isI() && iffalse->isI())  ||
               (ins->isop(LIR_cmovf4)&& iftrue->isF4()&& iffalse->isF4()) ||
               (ins->isop(LIR_cmovf) && iftrue->isF() && iffalse->isF())  ||
               (ins->isop(LIR_cmovd) && iftrue->isD() && iffalse->isD())  );

    Register rd = prepareResultReg(ins, allow);

    // Try to re-use the result register for one of the arguments.
    Register rt = iftrue->isInReg() ? iftrue->getReg() : rd;
    Register rf = iffalse->isInReg() ? iffalse->getReg() : rd;
    // Note that iftrue and iffalse may actually be the same, though it
    // shouldn't happen with the LIR optimizers turned on.
    if ((rt == rf) && (iftrue != iffalse)) {
        // We can't re-use the result register for both arguments, so force one
        // into its own register.
        rf = findRegFor(iffalse, allow & ~rmask(rd));
        NanoAssert(iffalse->isInReg());
    }

    switch(condval->opcode()) {
        default:        NanoAssert(0);
        // Integer comparisons.
        case LIR_eqi:   cc = EQ;        break;
        case LIR_lti:   cc = LT;        break;
        case LIR_lei:   cc = LE;        break;
        case LIR_gti:   cc = GT;        break;
        case LIR_gei:   cc = GE;        break;
        case LIR_ltui:  cc = LO;        break;
        case LIR_leui:  cc = LS;        break;
        case LIR_gtui:  cc = HI;        break;
        case LIR_geui:  cc = HS;        break;
        // VFP comparisons.
        case LIR_eqf:
        case LIR_eqf4:
        case LIR_eqd:   cc = EQ;        break;
        case LIR_ltf:
        case LIR_ltd:   cc = LO;        break;
        case LIR_lef:
        case LIR_led:   cc = LS;        break;
        case LIR_gef:
        case LIR_ged:   cc = GE;        break;
        case LIR_gtf:
        case LIR_gtd:   cc = GT;        break;
    }

    // Emit something like this:
    //      CMP         [...]
    //      MOV(CC)     rd, rf
    //      MOV(!CC)    rd, rt
    // If the destination was re-used for an input, the corresponding MOV will
    // be omitted as it will be redundant.
    if (ins->isI()) {
        // In Thumb2, we only have conditional integer moves in 16-bit form
        // (allowing for deprecation of the IT prefix on 32-bit instructions),
        // so we generate a branchover instead.
        if (rd != rf && rd != rt) {
            ReserveContiguousSpace(this, 4*3);
            MOV(rd, rf);
            B_cond(cc, _nIns + 2);
            MOV(rd, rt);
        } else if (rd == rt) {
            if (rd != rf) {
                ReserveContiguousSpace(this, 4*2);
                MOV(rd, rf);
                B_cond(cc, _nIns + 2);
            }
        } else if (rd == rf) {
            if (rd != rt) {
                NanoAssert(rd == rf);
                ReserveContiguousSpace(this, 4*2);
                MOV(rd, rt);
                B_cond(OppositeCond(cc), _nIns + 2);
            }
        }
    } else if (ins->isD() || ins->isF()) {
        // The VFP sequence is similar to the integer sequence, but uses a
        // VFP instruction in place of MOV.
        NanoAssert(ARM_VFP);
        // In Thumb2, we only have conditional integer moves in 16-bit form
        // (allowing for deprecation of the IT prefix on 32-bit instructions),
        // so we generate a branchover instead.
        if (rd != rf && rd != rt) {
            ReserveContiguousSpace(this, 4*3);
            VMOV(rd, rf);
            B_cond(cc, _nIns + 2);
            VMOV(rd, rt);
        } else if (rd == rt) {
            if (rd != rf) {
                ReserveContiguousSpace(this, 4*2);
                VMOV(rd, rf);
                B_cond(cc, _nIns + 2);
            }
        } else if (rd == rf) {
            if (rd != rt) {
                NanoAssert(rd == rf);
                ReserveContiguousSpace(this, 4*2);
                VMOV(rd, rt);
                B_cond(OppositeCond(cc), _nIns + 2);
            }
        }
    } else {
        NanoAssert(ins->isF4()); 
        // There is no conditional move for Qn regs; we move the Dn regs
        Register rd0 = DReg(2*FpQRegNum(rd));
        NanoAssert(ARM_VFP);

        // In Thumb2, we only have conditional integer moves in 16-bit form
        // (allowing for deprecation of the IT prefix on 32-bit instructions),
        // so we generate a branchover instead.
        if (rd != rf && rd != rt) {
            ReserveContiguousSpace(this, 4*5);
            Register rf0 = DReg(2*FpQRegNum(rf));
            VMOV(rd0, rf0); rd0 = rd0 + 1; rf0 = rf0 + 1;
            VMOV(rd0, rf0); rd0 = rd0 - 1; //rd0 may be used below, restore.
            B_cond(cc, _nIns + 4);
            Register rt0 = DReg(2*FpQRegNum(rt));
            VMOV(rd0, rt0); rd0 = rd0 + 1; rt0 = rt0 + 1;
            VMOV(rd0, rt0);
        } else if (rd == rt) {
            if (rd != rf) {
                ReserveContiguousSpace(this, 4*3);
                Register rf0 = DReg(2*FpQRegNum(rf));
                VMOV(rd0, rf0); rd0 = rd0 + 1; rf0 = rf0 + 1;
                VMOV(rd0, rf0); rd0 = rd0 - 1; //rd0 may be used below, restore.
                B_cond(cc, _nIns + 4);
            }
        } else if (rd == rf) {
            if (rd != rt) {
                NanoAssert(rd == rf);
                ReserveContiguousSpace(this, 4*3);
                Register rt0 = DReg(2*FpQRegNum(rt));
                VMOV(rd0, rt0); rd0 = rd0 + 1; rt0 = rt0 + 1;
                VMOV(rd0, rt0);
                B_cond(OppositeCond(cc), _nIns + 4);
            }
        }
    }

    freeResourcesOf(ins);

    // If we re-used the result register, mark it as active for either iftrue
    // or iffalse (or both in the corner-case where they're the same).
    if (rt == rd) {
        NanoAssert(!iftrue->isInReg());
        findSpecificRegForUnallocated(iftrue, rd);
    } else if (rf == rd) {
        NanoAssert(!iffalse->isInReg());
        findSpecificRegForUnallocated(iffalse, rd);
    } else {
        NanoAssert(iffalse->isInReg());
        NanoAssert(iftrue->isInReg());
    }

    asm_cmp(condval);
}

void
Assembler::asm_qhi(LIns* ins)
{
    Register rd = prepareResultReg(ins, GpRegs);
    LIns *lhs = ins->oprnd1();
    int d = findMemFor(lhs);

    asm_ldr(rd, FP, d+4);

    freeResourcesOf(ins);
}

void
Assembler::asm_qlo(LIns* ins)
{
    Register rd = prepareResultReg(ins, GpRegs);
    LIns *lhs = ins->oprnd1();
    int d = findMemFor(lhs);

    asm_ldr(rd, FP, d);

    freeResourcesOf(ins);
}

void
Assembler::asm_param(LIns* ins)
{
    uint32_t a = ins->paramArg();
    uint32_t kind = ins->paramKind();
    if (kind == 0) {
        // Ordinary parameter. These are always (32-bit-)word-sized, and will
        // be in the first four registers (argRegs) and then on the stack.
        if (a < 4) {
            // Register argument.
            prepareResultReg(ins, rmask(RegAlloc::argRegs[a]));
        } else {
            // Stack argument.
            Register r = prepareResultReg(ins, GpRegs);
            int d = (a - 4) * sizeof(intptr_t) + 8;
            asm_ldr(r, FP, d);
        }
    } else {
        // Saved parameter.
        NanoAssert(a < (sizeof(RegAlloc::savedRegs)/sizeof(RegAlloc::savedRegs[0])));
        prepareResultReg(ins, rmask(RegAlloc::savedRegs[a]));
    }
    freeResourcesOf(ins);
}

void
Assembler::asm_immi(LIns* ins)
{
    Register rd = prepareResultReg(ins, GpRegs);
    asm_ld_imm(rd, ins->immI());
    freeResourcesOf(ins);
}

void
Assembler::asm_ret(LIns *ins)
{
    genEpilogue();
    bool isFPR = ins->isop(LIR_retd) || ins->isop(LIR_retf) || ins->isop(LIR_retf4);
    bool singlePrecision = ins->isop(LIR_retf);
    bool quad = ins->isop(LIR_retf4);

    // NB: our contract with genEpilogue is actually that the return value
    // we are intending for R0 is currently IP, not R0. This has to do with
    // the strange dual-nature of the patchable jump in a side-exit. See
    // nPatchBranch.
    //
    // With hardware floating point ABI we can skip this for retd/retf/retf4.
    if (!(ARM_EABI_HARD && isFPR) ) {
        MOV(IP, R0);
    }

    // Pop the stack frame.
    MOV(SP,FP);

    releaseRegisters();
    assignSavedRegs();
    LIns *value = ins->oprnd1();
    if (ins->isop(LIR_reti)) {
        findSpecificRegFor(value, R0);
    } else {
        NanoAssert(isFPR);
        if (ARM_VFP) {
#ifdef NJ_ARM_EABI_HARD_FLOAT
            findSpecificRegFor(value, singlePrecision ? S0 : quad ? Q0 : D0);
#else
            Register reg = findRegFor(value, singlePrecision ? FpSRegs : quad ? FpQRegs: FpDRegs);
            if (singlePrecision)
                FMRS(R0,reg);
            else {
                if(quad){
                    reg = DReg(2*FpQRegNum(reg)) + 1;
                    NanoAssert(IsFpDReg(reg));
                    FMRRD(R2,R3,reg);
                    reg = reg - 1;
                    NanoAssert(IsFpDReg(reg));
                }
                FMRRD(R0, R1, reg);
            }
#endif
        } else {
            NanoAssert(!singlePrecision && !quad);
            NanoAssert(value->isop(LIR_ii2d));
            findSpecificRegFor(value->oprnd1(), R0); // lo
            findSpecificRegFor(value->oprnd2(), R1); // hi
        }
    }
}

void
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=637809
Assembler::asm_jtbl(NIns** table, Register indexreg)
#else
Assembler::asm_jtbl(LIns* ins, NIns** table)
#endif
{
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=637809
#else
    Register indexreg = findRegFor(ins->oprnd1(), GpRegs);
#endif
    Register tmp = _allocator.allocTempReg(GpRegs & ~rmask(indexreg), R0);
    LDR_scaled(PC, tmp, indexreg, 2);      // LDR PC, [tmp + index*4]
    asm_ld_imm(tmp, (int32_t)table);       // tmp = #table
}

void Assembler::swapCodeChunks() {
    if (!_nExitIns)
        codeAlloc(exitStart, exitEnd, _nExitIns verbose_only(, exitBytes), NJ_MAX_CPOOL_OFFSET);
    SWAP(NIns*, _nIns, _nExitIns);
    SWAP(NIns*, codeStart, exitStart);
    SWAP(NIns*, codeEnd, exitEnd);
    verbose_only( SWAP(size_t, codeBytes, exitBytes); )
}

void Assembler::asm_insert_random_nop() {
    NanoAssert(0); // not supported
}

void Assembler::asm_label() {
    // do nothing right now
}
    
/*
 * VFP, encoding utilities
 */

enum {
    S__ = 0x1,          // Single-precision float register
    _D_ = 0x2,          // Double-precision float register
    __Q = 0x4,          // Quad (vector) float register

    SD_  = S__ | _D_    // Single- or Double-precision float register
};

static inline void split_qreg_enc(Register& Qd, unsigned& D, unsigned& Vd)
{
    Vd = FpQRegNum(Qd);
    NanoAssert(Vd <= 15);
    Vd *= 2;
    D = (Vd & 0x10) >> 4;
    Vd = Vd & 0xf;
}

static inline void split_dreg_enc(Register& Dd, unsigned& D, unsigned& Vd) 
{
    Vd = FpDRegNum(Dd);
    NanoAssert(Vd <= 31);
    D = (Vd & 0x10) >> 4;
    Vd = Vd & 0xf;
}

static inline void split_sreg_enc(Register& Sd, unsigned& D, unsigned& Vd)
{
    Vd = FpSRegNum(Sd);
    NanoAssert(Vd <= 31);
    D = (Vd & 0x1);
    Vd = Vd >> 1;
}
    
static inline void encodeCommon(Register& Dm, unsigned& M, unsigned& Vm, unsigned Ok)
{
    (void)Ok;
    if (IsFpSReg(Dm)) {
        NanoAssert(Ok & S__);
        split_sreg_enc(Dm, M, Vm);
    } else if(IsFpDReg(Dm)) {
        NanoAssert(Ok & _D_);
        split_dreg_enc(Dm, M, Vm);
    } else if(IsFpQReg(Dm)) {
        NanoAssert(Ok & __Q);
        split_qreg_enc(Dm, M, Vm);
    } else {
        NanoAssert(Ok & _D_);
        M = Vm = 0;
    }
}

static inline uint32_t encodeRegM(Register& Dm, unsigned Ok)
{
    unsigned M, Vm;
    encodeCommon(Dm, M, Vm, Ok);
    return (M << 5) | Vm;
}
                         
static inline uint32_t encodeRegN(Register& Dn, unsigned Ok)
{
    unsigned N, Vn;
    encodeCommon(Dn, N, Vn, Ok);
    return (N << 7) | (Vn << 16);
}

static inline uint32_t encodeRegD(Register& Dd, unsigned Ok)
{
    unsigned D, Vd;
    encodeCommon(Dd, D, Vd, Ok);
    return (Vd << 12) | (D << 22);
}

#ifdef NJ_VERBOSE
static inline Register decodeRegD(uint32_t enc)
{
    unsigned D = (enc >> 22) & 0x1;
    unsigned Vd = (enc >> 12) & 0xf;
    return Register(Vd + D*16);
}
#endif

static inline uint32_t DFlag(Register Dd) { return (IsFpDReg(Dd) ? (1 << 8) : 0); }

/*
 * VFP, instructions
 */

inline void Assembler::FMDRR(Register Dm, Register Rd, Register Rn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd) && IsGpReg(Rn));
    uint32_t DmEnc = encodeRegM(Dm, _D_);
    emitT32( COND_AL | (0xC4 << 20) | (Rn << 16) | (Rd << 12) | (0xB1 << 4) | DmEnc );
    asm_output("fmdrr %s,%s,%s", gpn(Dm), gpn(Rd), gpn(Rn));
}

inline void Assembler::FMRRD(Register Rd, Register Rn, Register Dm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd) && IsGpReg(Rn));
    uint32_t DmEnc = encodeRegM(Dm, _D_);
    emitT32( COND_AL | (0xC5 << 20) | (Rn << 16) | (Rd << 12) | (0xB1 << 4) | DmEnc );
    asm_output("fmrrd %s,%s,%s", gpn(Rd), gpn(Rn), gpn(Dm));
}

inline void Assembler::FMRDH(Register Rd, Register Dn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd));
    uint32_t DnEnc = encodeRegN(Dn, _D_);
    emitT32( COND_AL | (0xE3 << 20) | DnEnc | (Rd << 12) | 0xB10 );
    asm_output("fmrdh %s,%s", gpn(Rd), gpn(Dn));
}

inline void Assembler::FMRDL(Register Rd, Register Dn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd));
    uint32_t DnEnc = encodeRegN(Dn, _D_);
    emitT32( COND_AL | (0xE1 << 20) | DnEnc | (Rd << 12) | 0xB10 );
    asm_output("fmrdh %s,%s", gpn(Rd), gpn(Dn));
}

inline void Assembler::VLDR(Register Dd, Register Rn, int32_t offs)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert((offs % 4) == 0);
    NanoAssert(isU8(offs/4) || isU8(-offs/4));
    NanoAssert(IsGpReg(Rn));
    uint32_t negflag = 1 << 23;
    uint32_t dflag = DFlag(Dd);
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    if (offs < 0) {
        negflag = 0 << 23;
        offs = -offs;
    }
    emitT32( COND_AL | (0xD1 << 20) | (Rn << 16) | DdEnc | (0xA << 8) | dflag | negflag | ((offs >> 2) & 0xff) );
    asm_output("fld%c %s,%s(%d)", dflag?'d':'s', gpn(Dd), gpn(Rn), offs);
}

inline void Assembler::VLDQR(Register Qd, Register Rn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rn));
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    emitT32( COND_AL | (0xC9 << 20) | (Rn << 16) | DdEnc | 0xB04 );
    asm_output("vldm <d%d,d%d>,%s // i.e. %s",  decodeRegD(DdEnc), decodeRegD(DdEnc)+1, gpn(Rn), gpn(Qd));
}

//### FIXME: Do we need FLDD and FLDS?  Just use VLDR?
inline void Assembler::FLDD(Register Dd, Register Rn, int32_t offs) { VLDR(Dd, Rn, offs); }
inline void Assembler::FLDS(Register Dd, Register Rn, int32_t offs) { VLDR(Dd, Rn, offs); }

inline void Assembler::VSTR(Register Dd, Register Rn, int32_t offs)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert((offs%4) == 0);
    NanoAssert(isU8(offs/4) || isU8(-offs/4));
    NanoAssert(IsGpReg(Rn));
    uint32_t dflag = DFlag(Dd);
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    uint32_t negflag = 1 << 23;
    if (offs < 0) {
        negflag = 0 << 23;
        offs = -offs;
    }
    emitT32( COND_AL | (0xD0 << 20) | (Rn << 16) | DdEnc | (0xA << 8) | dflag | negflag | ((offs >> 2) & 0xff) );
    asm_output("fst%c %s,%s(%d)", dflag?'d':'s', gpn(Dd), gpn(Rn), offs);
}

inline void Assembler::VSTQR(Register Qd, Register Rn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rn));
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    emitT32( COND_AL | (0xC8 << 20) | (Rn << 16) | DdEnc | 0xB04 );
    asm_output("vstm %s,<d%d,d%d> // i.e. %s", gpn(Rn), decodeRegD(DdEnc), decodeRegD(DdEnc)+1, gpn(Qd));
}

inline void Assembler::FSTS(Register Dd, Register Rn, int32_t offs)  { VSTR(Dd, Rn, offs); }
inline void Assembler::FSTD(Register Dd, Register Rn, int32_t offs)  { VSTR(Dd, Rn, offs); }

inline void Assembler::VNEGvfp(Register Dd, Register Dm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t dflag = DFlag(Dd); NanoAssert(dflag == DFlag(Dm));
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    uint32_t DmEnc = encodeRegM(Dm, SD_);
    emitT32( COND_AL | 0xEB10A40 | DdEnc | DmEnc | dflag );
    asm_output("fneg%c %s,%s", dflag?'d':'s', gpn(Dd), gpn(Dm));
}

inline void Assembler::VNEGneon(Register Qd, Register Qm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    uint32_t DmEnc = encodeRegM(Qm, __Q);
    emitT32( 0xFFB907C0 | DdEnc | DmEnc );
    asm_output("fnegq %s,%s", gpn(Qd), gpn(Qm));
}


inline void Assembler::GenericVfp(const char* name, Register Dd, Register Dn, Register Dm, uint32_t SPECIFIC_ENC)
{
    (void)name;
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t dflag = DFlag(Dd); NanoAssert(dflag == DFlag(Dm)); NanoAssert(dflag == DFlag(Dn));
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    uint32_t DmEnc = encodeRegM(Dm, SD_);
    uint32_t DnEnc = encodeRegN(Dn, SD_);
    emitT32( COND_AL | SPECIFIC_ENC | DnEnc | DdEnc | DmEnc | dflag);
    asm_output("%s%c %s,%s,%s", name, dflag?'d':'s', gpn(Dd), gpn(Dn), gpn(Dm));
}

inline void Assembler::GenericNeon(const char* name, Register Qd, Register Qn, Register Qm, uint32_t SPECIFIC_ENC)
{
    (void)name;
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    uint32_t DmEnc = encodeRegM(Qm, __Q);
    uint32_t DnEnc = encodeRegN(Qn, __Q);
    emitT32( (0xEF << 24) | SPECIFIC_ENC | DdEnc | DnEnc| DmEnc );
    asm_output("%s %s,%s,%s", name, gpn(Qd), gpn(Qn), gpn(Qm));
}

inline void Assembler::VADDvfp(Register Dd, Register Dn, Register Dm)  { GenericVfp("fadd", Dd, Dn, Dm, 0xE300A00); }
inline void Assembler::VSUBvfp(Register Dd, Register Dn, Register Dm)  { GenericVfp("fsub", Dd, Dn, Dm, 0xE300A40); }
inline void Assembler::VMULvfp(Register Dd, Register Dn, Register Dm)  { GenericVfp("fmul", Dd, Dn, Dm, 0xE200A00); }
inline void Assembler::VDIVvfp(Register Dd, Register Dn, Register Dm)  { GenericVfp("fdiv", Dd, Dn, Dm, 0xE800A00); }

inline void Assembler::VADDneon(Register Qd, Register Qn, Register Qm) { GenericNeon("vadd", Qd, Qn, Qm, 0xD40); }
inline void Assembler::VSUBneon(Register Qd, Register Qn, Register Qm) { GenericNeon("vsub", Qd, Qn, Qm, 0x200D40); }
inline void Assembler::VMULneon(Register Qd, Register Qn, Register Qm) { GenericNeon("vmul", Qd, Qn, Qm, 0x10000D50); }

/* neon doesn't have VDIV; we'll do it on the components */
inline void Assembler::VDIVneon(Register Qd, Register Qn, Register Qm) {
    NanoAssert(IsFpQReg(Qn) && IsFpQReg(Qd) && IsFpQReg(Qm));
    Register Sn = S0 + (FpQRegNum(Qn) << 2);
    Register Sm = S0 + (FpQRegNum(Qm) << 2);
    Register Sd = S0 + (FpQRegNum(Qd) << 2);
    VDIVvfp(Sd, Sn, Sm); Sn++; Sm++; Sd++;
    VDIVvfp(Sd, Sn, Sm); Sn++; Sm++; Sd++;
    VDIVvfp(Sd, Sn, Sm); Sn++; Sm++; Sd++;
    VDIVvfp(Sd, Sn, Sm);
}

inline void Assembler::VADD(Register Dd, Register Dn, Register Dm)
{
    if (IsFpQReg(Dd)) VADDneon(Dd, Dn, Dm); else VADDvfp(Dd, Dn, Dm);
}

inline void Assembler::VSUB(Register Dd, Register Dn, Register Dm)
{
    if (IsFpQReg(Dd)) VSUBneon(Dd, Dn, Dm); else VSUBvfp(Dd, Dn, Dm);
}

inline void Assembler::VMUL(Register Dd, Register Dn, Register Dm)
{
    if (IsFpQReg(Dd)) VMULneon(Dd, Dn, Dm); else VMULvfp(Dd, Dn, Dm);
}

inline void Assembler::VDIV(Register Dd, Register Dn, Register Dm)
{
    if (IsFpQReg(Dd)) VDIVneon(Dd, Dn, Dm); else VDIVvfp(Dd, Dn, Dm);
}

inline void Assembler::VNEG(Register Dd, Register Dm)
{
    if (IsFpQReg(Dd)) VNEGneon(Dd, Dm);     else VNEGvfp(Dd, Dm);
}

inline void Assembler::VCEQ(Register Qd, Register Qn, Register Qm)
{
    GenericNeon("vceq.f32", Qd, Qn, Qm, 0xE40);
}

inline void Assembler::VMOVN(Register Dd, Register Qm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Dd, _D_);
    uint32_t DmEnc = encodeRegM(Qm, __Q);
    // Thumb1 encoding is 0XFF instead of 0xF3
    emitT32( 0xFFB60200 | DdEnc | DmEnc );
    asm_output("vmovn.i32 %s,%s", gpn(Dd), gpn(Qm));
}

inline void Assembler::VMVN(Register Dd, Register Dm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Dd, _D_);
    uint32_t DmEnc = encodeRegM(Dm, _D_);
    // Thumb1 encoding is 0XFF instead of 0xF3
    emitT32( 0xFFB00580 | DdEnc | DmEnc );
    asm_output("vmvn %s,%s", gpn(Dd), gpn(Dm));
}

inline void Assembler::VDUP(Register Qd, Register Sm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    unsigned M, Vm; NanoAssert(IsFpSReg(Sm));
    split_sreg_enc(Sm, M, Vm);
    /* M is part of th 'imm4' encoding; the first bit. the rest is 0x4 (100)*/
    /* The actual M is always 0 - Sn registers never get to D16-31 range    */
    // Thumb1 encoding is 0XFF instead of 0xF3
    emitT32( 0xFFB40C40 | DdEnc | Vm | (M << 19) );
    asm_output("vdup.32 %s,%s", gpn(Qd), gpn(Sm));
}

inline void Assembler::FMSTAT()
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    emitT32( COND_AL | 0x0EF1FA10);
    asm_output("fmstat");
}

inline void Assembler::VCMP(Register Dd, Register Dm, int32_t E)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t dflag = DFlag(Dd); NanoAssert(dflag == DFlag(Dm));
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    uint32_t DmEnc = encodeRegM(Dm, SD_);
    NanoAssert((E == 0) || (E == 1));
    emitT32( COND_AL | (0xEB4 << 16) | DdEnc | 0xA40 | (E << 7) | dflag | DmEnc );
    asm_output("fcmp%s%c %s,%s", ((E == 1)?"e":""), dflag?'d':'s', gpn(Dd), gpn(Dm));
}

inline void Assembler::VCMPZ(Register Dd)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t DdEnc = encodeRegD(Dd, _D_);
    emitT32( COND_AL | (0xEB5 << 16) | DdEnc | 0xBC0 );
    asm_output("vcmpz %s", gpn(Dd) );
}

inline void Assembler::VMOV(Register Dd, Register Dm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsCond(AL));
    uint32_t dflag = DFlag(Dd); NanoAssert(dflag == DFlag(Dm));
    uint32_t DdEnc = encodeRegD(Dd, SD_);
    uint32_t DmEnc = encodeRegM(Dm, SD_);
    emitT32( (AL << 28) | (0xEB0 << 16) | DdEnc | 0xA40 | DmEnc | dflag );
    asm_output("fcpy%c%s %s,%s", dflag?'d':'s', condNames[AL], gpn(Dd), gpn(Dm));
}

inline void Assembler::FCPYS(Register Sd, Register Sm) { VMOV(Sd, Sm); }
inline void Assembler::FCPYD(Register Dd, Register Dm) { VMOV(Dd, Dm); }

inline void Assembler::FCPYQ(Register Qd, Register Qm)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP); /*NEON!*/
    uint32_t DdEnc = encodeRegD(Qd, __Q);
    uint32_t DmEnc = encodeRegM(Qm, __Q);
    uint32_t DnEnc = encodeRegN(Qm, __Q);
    emitT32( 0xF2200150 | DdEnc  | DmEnc  | DnEnc );
    asm_output("fcpyq %s,%s", gpn(Qd), gpn(Qm));
}

inline void Assembler::FMSR(Register Sn, Register Rd)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd));
    uint32_t DnEnc = encodeRegN(Sn, S__);
    emitT32( COND_AL | (0xE0 << 20) |(Rd << 12) | 0xA10 | DnEnc );
    asm_output("fmsr %s,%s", gpn(Sn), gpn(Rd));
}

inline void Assembler::FMRS(Register Rd, Register Sn)
{
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    NanoAssert(IsGpReg(Rd));
    uint32_t DnEnc = encodeRegN(Sn, S__);
    emitT32( COND_AL | (0xE1 << 20) |(Rd << 12) | 0xA10 | DnEnc );
    asm_output("fmrs %s,%s", gpn(Rd), gpn(Sn));
}


/* Conversions: to/from int(signed, unsigned), to/from single/double precision */

inline void Assembler::VCVT(const char* name, Register Rd, Register Rm, uint32_t SPECIFIC_ENC, bool check_dst)
{
    (void)name;
    underrunProtect(4);
    NanoAssert(ARM_VFP);
    uint32_t dflag = DFlag((check_dst?Rd:Rm));
    uint32_t DdEnc = encodeRegD(Rd, SD_);
    uint32_t DmEnc = encodeRegM(Rm, SD_);
    emitT32( COND_AL | (0xEB << 20) | SPECIFIC_ENC | dflag | DdEnc | DmEnc );
    asm_output("%s %s,%s", name, gpn(Rd), gpn(Rm));
}

inline void Assembler::FCVTDS(Register Dd, Register Sm)
{
    NanoAssert(IsFpDReg(Dd) && IsFpSReg(Sm));
    VCVT("fcvtds", Dd, Sm, 0x70AC0, false);
}

inline void Assembler::FCVTSD(Register Sd, Register Dm)
{
    NanoAssert(IsFpSReg(Sd) && IsFpDReg(Dm));
    VCVT("fcvtsd", Sd, Dm, 0x70AC0, false);
}

inline void Assembler::FSITOD(Register Dd, Register Sm)
{
    NanoAssert(IsFpDReg(Dd) && IsFpSReg(Sm));
    VCVT("fsitod", Dd, Sm, 0x80AC0);
}

inline void Assembler::FUITOD(Register Dd, Register Sm)
{
    NanoAssert(IsFpDReg(Dd) && IsFpSReg(Sm));
    VCVT("fuitod", Dd, Sm, 0x80A40);
}

inline void Assembler::FSITOS(Register Sd, Register Sm)
{
    NanoAssert(IsFpSReg(Sd) && IsFpSReg(Sm));
    VCVT("fsitos", Sd, Sm, 0x80AC0);
}

//  unsigned int, to single-precision float
inline void Assembler::FUITOS(Register Sd, Register Sm)
{
    NanoAssert(IsFpSReg(Sd) && IsFpSReg(Sm));
    VCVT("fuitos", Sd, Sm, 0x80A40);
}

//  Double to signed int, stored in Sn reg
inline void Assembler::FTOSID(Register Sd, Register Dm)
{
    NanoAssert(IsFpSReg(Sd) && IsFpDReg(Dm));
    VCVT("ftosid", Sd, Dm, 0xD0A40, false);
}

//  Single-precision float to signed int, stored in Sn reg
inline void Assembler::FTOSIS(Register Sd, Register Sm)
{
    NanoAssert(IsFpSReg(Sd) && IsFpSReg(Sm));
    VCVT("ftosis", Sd, Sm, 0xD0A40, false);
}

}
#endif /* FEATURE_NANOJIT */
