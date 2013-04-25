/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __nanojit_NativeARM__
#define __nanojit_NativeARM__

#ifdef PERFM
#include "../vprof/vprof.h"
#define count_instr() _nvprof("arm",1)
#define count_prolog() _nvprof("arm-prolog",1); count_instr();
#define count_imt() _nvprof("arm-imt",1) count_instr()
#else
#define count_instr()
#define count_prolog()
#define count_imt()
#endif

#ifdef DEBUG
#define ARM_ARCH_AT_LEAST(wanted) (_config.arm_arch >= (wanted))
#define ARM_VFP (_config.arm_vfp)
#else
/* Note: Non DEBUG builds will ignore arm_arch if it is lower than
   NJ_COMPILER_ARM_ARCH, and will ignore arm_vfp if NJ_COMPILER_ARM_ARCH
   is greater or equal to 7. */
#define ARM_ARCH_AT_LEAST(wanted) \
    ((NJ_COMPILER_ARM_ARCH >= (wanted)) || (_config.arm_arch >= (wanted)))
#define ARM_VFP ((NJ_COMPILER_ARM_ARCH >= 7) || (_config.arm_vfp))
#endif

namespace nanojit
{
// GCC defines __ARM_PCS_VFP if it uses hardware floating point ABI
// See http://gcc.gnu.org/viewcvs?view=revision&revision=162637
#ifdef __ARM_PCS_VFP
#  define NJ_ARM_EABI_HARD_FLOAT 1
#endif

#ifdef NJ_ARM_EABI_HARD_FLOAT
#  define ARM_EABI_HARD true
#else
#  define ARM_EABI_HARD false
#endif

// only d0-d7 are used; in addition, we'll use d0 as s0-s1 for i2d/u2f/etc.
#define NJ_VFP_MAX_REGISTERS            8
#define NJ_MAX_REGISTERS                (11 + NJ_VFP_MAX_REGISTERS)
#define NJ_MAX_STACK_ENTRY              4096
#define NJ_MAX_PARAMETERS               16
#define NJ_ALIGN_STACK                  8

#define NJ_JTBL_SUPPORTED               1
#define NJ_EXPANDED_LOADSTORE_SUPPORTED 1
#define NJ_F2I_SUPPORTED                1
#define NJ_SOFTFLOAT_SUPPORTED          1
#define NJ_DIVI_SUPPORTED               0

#define NJ_JTBL_ALLOWED_IDX_REGS        GpRegs

#define RA_REGISTERS_OVERLAP            1
#define firstAvailableReg               getAvailableReg
#define getFatherReg                    _allocator.getSuitableRegFor

#define NJ_CONSTANT_POOLS
const int NJ_MAX_CPOOL_OFFSET = 4096;
const int NJ_CPOOL_SIZE = 16;

const int LARGEST_UNDERRUN_PROT = 32;  // largest value passed to underrunProtect

typedef int NIns;

// Bytes of icache to flush after Assembler::patch
const size_t LARGEST_BRANCH_PATCH = 2 * sizeof(NIns);

/* ARM registers */
typedef uint32_t Register;
static const Register
    R0  = { 0 },
    R1  = { 1 },
    R2  = { 2 },
    R3  = { 3 },
    R4  = { 4 },
    R5  = { 5 },
    R6  = { 6 },
    R7  = { 7 },
    R8  = { 8 },
    R9  = { 9 },
    R10 = { 10 },
    FP  = { 11 },
    IP  = { 12 },
    SP  = { 13 },
    LR  = { 14 },
    PC  = { 15 },

    // VFP regs
    S0  = { 16 },
    S1  = S0 + 1,
    S2  = S0 + 2,
    S3  = S0 + 3,
    S4  = S0 + 4,
    S5  = S0 + 5,
    S6  = S0 + 6,
    S7  = S0 + 7,
    S8  = S0 + 8,
    S9  = S0 + 9,
    S10 = S0 +10,
    S11 = S0 +11,
    S12 = S0 +12,
    S13 = S0 +13,
    S14 = S0 +14,
    S15 = S0 +15,
    S16 = S0 +16,
    S17 = S0 +17,
    S18 = S0 +18,
    S19 = S0 +19,
    S20 = S0 +20,
    S21 = S0 +21,
    S22 = S0 +22,
    S23 = S0 +23,
    S24 = S0 +24,
    S25 = S0 +25,
    S26 = S0 +26,
    S27 = S0 +27,
    S28 = S0 +28,
    S29 = S0 +29,
    S30 = S0 +30,
    S31 = S0 +31,
    // We reverse the order of the Dn register banks: first, the Dn regs that have no Sn subregs
    D16 = { 48 },
    D17 = D16+ 1,
    D18 = D16+ 2,
    D19 = D16+ 3,
    D20 = D16+ 4,
    D21 = D16+ 5,
    D22 = D16+ 6,
    D23 = D16+ 7,
    D24 = D16+ 8,
    D25 = D16+ 9,
    D26 = D16+10,
    D27 = D16+11,
    D28 = D16+12,
    D29 = D16+13,
    D30 = D16+14,
    D31 = D16+15,
    // The following regs are just alternate names for gruops of registers already listed above
    D0  = { 64 },
    D1  = D0 + 1,
    D2  = D0 + 2,
    D3  = D0 + 3,
    D4  = D0 + 4,
    D5  = D0 + 5,
    D6  = D0 + 6,
    D7  = D0 + 7,
    D8  = D0 + 8,
    D9  = D0 + 9,
    D10 = D0 +10,
    D11 = D0 +11,
    D12 = D0 +12,
    D13 = D0 +13,
    D14 = D0 +14,
    D15 = D0 +15,
    
    Q0  = { 80 },
    Q1  = { 81 },
    Q2  = { 82 },
    Q3  = { 83 },
    Q4  = { 84 },
    Q5  = { 85 },
    Q6  = { 86 },
    Q7  = { 87 },
    Q8  = { 88 },
    Q9  = { 89 },
    Q10 = { 90 },
    Q11 = { 91 },
    Q12 = { 92 },
    Q13 = { 93 },
    Q14 = { 94 },
    Q15 = { 95 },

    FirstDFloatReg = D16,
    FirstQFloatReg = Q0,
    FirstSFloatReg = S0,

    UnspecifiedReg = { 128 },
    deprecated_UnknownReg = { 128 },     // XXX: remove eventually, see bug 538924

    SBZ = { 0 } ;   // Used for 'should-be-zero' fields in instructions with
                    // unused register fields.

static const uint32_t FirstRegNum = 0; /* R0 */
static const uint32_t LastRegNum = 95; /* Q15 */ 
}

#define NJ_USE_UINT32_REGISTER 1
#include "NativeCommon.h"

namespace nanojit
{

/* ARM condition codes */
typedef enum {
    EQ = 0x0, // Equal
    NE = 0x1, // Not Equal
    CS = 0x2, // Carry Set (or HS)
    HS = 0x2,
    CC = 0x3, // Carry Clear (or LO)
    LO = 0x3,
    MI = 0x4, // MInus
    PL = 0x5, // PLus
    VS = 0x6, // oVerflow Set
    VC = 0x7, // oVerflow Clear
    HI = 0x8, // HIgher
    LS = 0x9, // Lower or Same
    GE = 0xA, // Greater or Equal
    LT = 0xB, // Less Than
    GT = 0xC, // Greater Than
    LE = 0xD, // Less or Equal
    AL = 0xE, // ALways

    // Note that condition code NV is unpredictable on ARMv3 and ARMv4, and has
    // special meaning for ARMv5 onwards. As such, it should never be used in
    // an instruction encoding unless the special (ARMv5+) meaning is required.
    NV = 0xF  // NeVer
} ConditionCode;
#define IsCond(cc)        (((cc) >= EQ) && ((cc) <= AL))

// Bit 0 of the condition code can be flipped to obtain the opposite condition.
// However, this won't work for AL because its opposite — NV — has special
// meaning.
#define OppositeCond(cc)  ((ConditionCode)((unsigned int)(cc)^0x1))


typedef uint64_t RegisterMask;
#define rmask(r) ARM_REG_MASKS[REGNUM(r)]
extern const RegisterMask ARM_REG_MASKS[LastRegNum+1];

typedef struct _FragInfo {
    RegisterMask    needRestoring;
    NIns*           epilogue;
} FragInfo;

typedef struct _ParameterRegisters {
    int stkd;
    Register r;
#ifdef NJ_ARM_EABI_HARD_FLOAT
    RegisterMask float_r;
#endif
} ParameterRegisters;

#ifdef NJ_ARM_EABI_HARD_FLOAT
#define init_params(a,b,c) { (a), (b), (c) }
#else
#define init_params(a,b,c) { (a), (b) }
#endif

// The saved FP register mask should include  S16-31, aliased to Q4-Q7 and D8-D15.
// Unfortunately, our current implementation of callee-saved registers cannot handle
// floating-point registers, so we simply restrict the set of allocatable FP registers
// to those that need not be saved.

static const RegisterMask SavedFpRegs = 0;
static const RegisterMask SavedRegs = 1<<R4 | 1<<R5 | 1<<R6 | 1<<R7 | 1<<R8 | 1<<R9 | 1<<R10;
static const int NumSavedRegs = 7;

// Some VFP CPUs (non-NEON) lack the full complement of 32 FP registers, implementing only S0-S15.
// Since we do not dynamically configure for the ARM variant in use, we must compile for the
// lowest common denominator among supported targets, currently VFP-D16 (Tegra-2).
// FIXME: We should identify the architectural variant at runtime. See bugs 718811 and 704111.

const RegisterMask FpSRegs = 0x00000000ffff0000LL; // S0-S15
const RegisterMask FpDRegs = 0x00000000ffff0000LL; // D0-D7
const RegisterMask FpQRegs = 0x00000000ffff0000LL; // Q0-Q3
const RegisterMask GpRegs  = 0x000000000000ffffLL; // R0-R10,FP,IP,SP,LR,PC
const RegisterMask AllowableFlagRegs = 0x07ff;     // R0-R10


inline bool IsFpSReg(Register _r) { return _r >=FirstSFloatReg && _r < FirstDFloatReg; }
inline bool IsFpDReg(Register _r) { return _r >=FirstDFloatReg && _r < FirstQFloatReg; }
inline bool IsFpQReg(Register _r) { return _r >=FirstQFloatReg && _r <= Q15; }
inline bool IsGpReg(Register _r) { return _r < FirstSFloatReg; }
#define FpSRegNum(_fpr)  (REGNUM(_fpr) - FirstSFloatReg)
#define FpDRegNum(_fpr)  ((REGNUM(_fpr) - FirstDFloatReg) ^ 16) // mind the reversed order of the banks
#define FpQRegNum(_fpr)  (REGNUM(_fpr) - FirstQFloatReg)
#define SReg(num)  (FirstSFloatReg + (num) )
#define DReg(num)  (FirstDFloatReg + ((num) ^ 16))  // mind the reversed order of the register banks
#define QReg(num)  (FirstQFloatReg + (num) )

#define firstreg()      R0

verbose_only( extern const char* regNames[]; )
verbose_only( extern const char* condNames[]; )
verbose_only( extern const char* shiftNames[]; )

// abstract to platform specific calls
#define nExtractPlatformFlags(x)    0

#define DECLARE_PLATFORM_STATS()

#define DECLARE_PLATFORM_REGALLOC()                                     \
    const static Register argRegs[4], retRegs[2];                       \
    Register getAvailableReg(LIns* ins, Register regClass, RegisterMask allow);\
    Register getSuitableRegFor(Register r, LIns* curins);               \

#ifdef DEBUG
# define DECLARE_PLATFORM_ASSEMBLER_DEBUG()                             \
    inline bool         isOp2Imm(uint32_t literal);                     \
    inline uint32_t     decOp2Imm(uint32_t enc);
#else
// define stubs, for code that defines NJ_VERBOSE without DEBUG
# define DECLARE_PLATFORM_ASSEMBLER_DEBUG()                             \
    inline bool         isOp2Imm(uint32_t ) { return true; }            \
    inline uint32_t     decOp2Imm(uint32_t ) { return 0; }
#endif

#define DECLARE_PLATFORM_ASSEMBLER()                                            \
                                                                                \
    DECLARE_PLATFORM_ASSEMBLER_DEBUG()                                          \
                                                                                \
    void        BranchWithLink(NIns* addr);                                     \
    inline void BLX(Register addr, bool chk = true);                            \
    void        JMP_far(NIns*);                                                 \
    void        B_cond_chk(ConditionCode, NIns*, bool);                         \
    void        underrunProtect(int bytes);                                     \
    void        nativePageReset();                                              \
    void        nativePageSetup();                                              \
    bool        hardenNopInsertion(const Config& /*c*/) { return false; }       \
    void        asm_immd_nochk(Register, int32_t, int32_t);                     \
    void        asm_regarg(ArgType, LIns*, Register);                           \
    void        asm_stkarg(LIns* p, int stkd);                                  \
    void        asm_pushstate();                                                \
    void        asm_popstate();                                                 \
    void        asm_savepc();                                                   \
    void        asm_restorepc();                                                \
    void        asm_discardpc();                                                \
    void        asm_cmpi(Register, int32_t imm);                                \
    void        asm_ldr_chk(Register d, Register b, int32_t off, bool chk);     \
    int32_t     asm_str(Register rt, Register rr, int32_t off);                 \
    void        asm_cmp(LIns *cond);                                            \
    void        asm_cmpd(LIns *cond);                                           \
    void        asm_ld_imm(Register d, int32_t imm, bool chk = true);           \
    void        asm_arg(ArgType ty, LIns* arg, ParameterRegisters& params);     \
    void        asm_arg_float(LIns* arg, ParameterRegisters& params);           \
    void        asm_add_imm(Register rd, Register rn, int32_t imm, int stat = 0);   \
    void        asm_sub_imm(Register rd, Register rn, int32_t imm, int stat = 0);   \
    void        asm_and_imm(Register rd, Register rn, int32_t imm, int stat = 0);   \
    void        asm_orr_imm(Register rd, Register rn, int32_t imm, int stat = 0);   \
    void        asm_eor_imm(Register rd, Register rn, int32_t imm, int stat = 0);   \
    inline bool     encOp2Imm(uint32_t literal, uint32_t * enc);                \
    inline uint32_t CountLeadingZeroes(uint32_t data);                          \
                                                                                \
    void        asm_immf_nochk(Register, int32_t);                              \
    void        asm_cmpf4(LIns *cond);                                          \
    Register    getAvailableReg(LIns* ins, RegisterMask m, Register regClass)   \
               { return _allocator.getAvailableReg(ins, m, regClass); }         \
                                                                                \
    int *       _nSlot;                                                         \
    int *       _nExitSlot;                                                     \
    int         max_out_args; /* bytes */

#define IMM32(imm)  *(--_nIns) = (NIns)((imm));

#define OP_IMM  (1<<25)
#define OP_STAT (1<<20)

#define COND_AL ((uint32_t)AL<<28)

typedef enum {
    LSL_imm = 0, // LSL #c - Logical Shift Left
    LSL_reg = 1, // LSL Rc - Logical Shift Left
    LSR_imm = 2, // LSR #c - Logical Shift Right
    LSR_reg = 3, // LSR Rc - Logical Shift Right
    ASR_imm = 4, // ASR #c - Arithmetic Shift Right
    ASR_reg = 5, // ASR Rc - Arithmetic Shift Right
    ROR_imm = 6, // Rotate Right (c != 0)
    RRX     = 6, // Rotate Right one bit with extend (c == 0)
    ROR_reg = 7  // Rotate Right
} ShiftOperator;
#define IsShift(sh)    (((sh) >= LSL_imm) && ((sh) <= ROR_reg))

#define LD32_size 8

#define BEGIN_NATIVE_CODE(x)                    \
    { DWORD* _nIns = (uint8_t*)x

#define END_NATIVE_CODE(x)                      \
    (x) = (dictwordp*)_nIns; }

// BX
#define BX(_r)  do {                                                    \
        underrunProtect(4);                                             \
        NanoAssert(IsGpReg(_r));                                        \
        *(--_nIns) = (NIns)( COND_AL | (0x12<<20) | (0xFFF<<8) | (1<<4) | (_r)); \
        asm_output("bx %s", gpn(_r)); } while(0)

/*
 * ALU operations
 */

enum {
    ARM_and = 0,
    ARM_eor = 1,
    ARM_sub = 2,
    ARM_rsb = 3,
    ARM_add = 4,
    ARM_adc = 5,
    ARM_sbc = 6,
    ARM_rsc = 7,
    ARM_tst = 8,
    ARM_teq = 9,
    ARM_cmp = 10,
    ARM_cmn = 11,
    ARM_orr = 12,
    ARM_mov = 13,
    ARM_bic = 14,
    ARM_mvn = 15
};
#define IsOp(op)      (((ARM_##op) >= ARM_and) && ((ARM_##op) <= ARM_mvn))

// ALU operation with register and 8-bit immediate arguments
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  op2imm  - operand 2 immediate. Use encOp2Imm (from NativeARM.cpp) to calculate this.
#define ALUi(cond, op, S, rd, rl, op2imm)   ALUi_chk(cond, op, S, rd, rl, op2imm, 1)
#define ALUi_chk(cond, op, S, rd, rl, op2imm, chk) do {\
        if (chk) underrunProtect(4);\
        NanoAssert(IsCond(cond));\
        NanoAssert(IsOp(op));\
        NanoAssert(((S)==0) || ((S)==1));\
        NanoAssert(IsGpReg(rd) && IsGpReg(rl));\
        NanoAssert(isOp2Imm(op2imm));\
        *(--_nIns) = (NIns) ((cond)<<28 | OP_IMM | (ARM_##op)<<21 | (S)<<20 | (rl)<<16 | (rd)<<12 | (op2imm));\
        if (ARM_##op == ARM_mov || ARM_##op == ARM_mvn) {               \
            asm_output("%s%s%s %s, #0x%X", #op, condNames[cond], (S)?"s":"", gpn(rd), decOp2Imm(op2imm));\
        } else if (ARM_##op >= ARM_tst && ARM_##op <= ARM_cmn) {         \
            NanoAssert(S==1);\
            asm_output("%s%s %s, #0x%X", #op, condNames[cond], gpn(rl), decOp2Imm(op2imm));\
        } else {                                                        \
            asm_output("%s%s%s %s, %s, #0x%X", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rl), decOp2Imm(op2imm));\
        }\
    } while (0)

// ALU operation with two register arguments
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  rr  - first (left) operand register
#define ALUr(cond, op, S, rd, rl, rr)   ALUr_chk(cond, op, S, rd, rl, rr, 1)
#define ALUr_chk(cond, op, S, rd, rl, rr, chk) do {\
        if (chk) underrunProtect(4);\
        NanoAssert(IsCond(cond));\
        NanoAssert(IsOp(op));\
        NanoAssert(((S)==0) || ((S)==1));\
        NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rr));\
        *(--_nIns) = (NIns) ((cond)<<28 |(ARM_##op)<<21 | (S)<<20 | (rl)<<16 | (rd)<<12 | (rr));\
        if (ARM_##op == ARM_mov || ARM_##op == ARM_mvn) {               \
            asm_output("%s%s%s %s, %s", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rr));\
        } else if (ARM_##op >= ARM_tst && ARM_##op <= ARM_cmn) {         \
            NanoAssert(S==1);\
            asm_output("%s%s  %s, %s", #op, condNames[cond], gpn(rl), gpn(rr));\
        } else {                                                        \
            asm_output("%s%s%s %s, %s, %s", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rl), gpn(rr));\
        }\
    } while (0)

// ALU operation with two register arguments, with rr operated on by a shift and shift immediate
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  rr  - second (right) operand register
//  sh  - a ShiftOperator
//  imm - immediate argument to shift operator, 5 bits (0..31)
#define ALUr_shi(cond, op, S, rd, rl, rr, sh, imm) do {\
        underrunProtect(4);\
        NanoAssert(IsCond(cond));\
        NanoAssert(IsOp(op));\
        NanoAssert(((S)==0) || ((S)==1));\
        NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rr));\
        NanoAssert(IsShift(sh));\
        NanoAssert((imm)>=0 && (imm)<32);\
        *(--_nIns) = (NIns) ((cond)<<28 |(ARM_##op)<<21 | (S)<<20 | (rl)<<16 | (rd)<<12 | (imm)<<7 | (sh)<<4 | (rr));\
        if (ARM_##op == ARM_mov || ARM_##op == ARM_mvn) {               \
            NanoAssert(rl==0);                                          \
            asm_output("%s%s%s %s, %s, %s #%d", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rr), shiftNames[sh], (imm));\
        } else if (ARM_##op >= ARM_tst && ARM_##op <= ARM_cmn) {         \
            NanoAssert(S==1);\
            NanoAssert(rd==0);\
            asm_output("%s%s  %s, %s, %s #%d", #op, condNames[cond], gpn(rl), gpn(rr), shiftNames[sh], (imm));\
        } else {                                                        \
            asm_output("%s%s%s %s, %s, %s, %s #%d", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rl), gpn(rr), shiftNames[sh], (imm));\
        }\
    } while (0)

// ALU operation with two register arguments, with rr operated on by a shift and shift register
//  S   - bit, 0 or 1, whether the CPSR register is updated
//  rd  - destination register
//  rl  - first (left) operand register
//  rr  - first (left) operand register
//  sh  - a ShiftOperator
//  rs  - shift operand register
#define ALUr_shr(cond, op, S, rd, rl, rr, sh, rs) do {\
        underrunProtect(4);\
        NanoAssert(IsCond(cond));\
        NanoAssert(IsOp(op));\
        NanoAssert(((S)==0) || ((S)==1));\
        NanoAssert(IsGpReg(rd) && IsGpReg(rl) && IsGpReg(rr) && IsGpReg(rs));\
        NanoAssert(IsShift(sh));\
        *(--_nIns) = (NIns) ((cond)<<28 |(ARM_##op)<<21 | (S)<<20 | (rl)<<16 | (rd)<<12 | (rs)<<8 | (sh)<<4 | (rr));\
        if (ARM_##op == ARM_mov || ARM_##op == ARM_mvn) {               \
            asm_output("%s%s%s %s, %s, %s %s", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rr), shiftNames[sh], gpn(rs));\
        } else if (ARM_##op >= ARM_tst && ARM_##op <= ARM_cmn) {         \
            NanoAssert(S==1);\
            asm_output("%s%s  %s, %s, %s %s", #op, condNames[cond], gpn(rl), gpn(rr), shiftNames[sh], gpn(rs));\
        } else {                                                        \
            asm_output("%s%s%s %s, %s, %s, %s %s", #op, condNames[cond], (S)?"s":"", gpn(rd), gpn(rl), gpn(rr), shiftNames[sh], gpn(rs));\
        }\
    } while (0)

// --------
// Basic arithmetic operations.
// --------
// Argument naming conventions for these macros:
//  _d      Destination register.
//  _l      First (left) operand.
//  _r      Second (right) operand.
//  _op2imm An operand 2 immediate value. Use encOp2Imm to calculate this.
//  _s      Set to 1 to update the status flags (for subsequent conditional
//          tests). Otherwise, set to 0.

// _d = _l + decOp2Imm(_op2imm)
#define ADDis(_d,_l,_op2imm,_s) ALUi(AL, add, _s, _d, _l, _op2imm)
#define ADDi(_d,_l,_op2imm)     ALUi(AL, add,  0, _d, _l, _op2imm)

// _d = _l & ~decOp2Imm(_op2imm)
#define BICis(_d,_l,_op2imm,_s) ALUi(AL, bic, _s, _d, _l, _op2imm)
#define BICi(_d,_l,_op2imm)     ALUi(AL, bic,  0, _d, _l, _op2imm)

// _d = _l - decOp2Imm(_op2imm)
#define SUBis(_d,_l,_op2imm,_s) ALUi(AL, sub, _s, _d, _l, _op2imm)
#define SUBi(_d,_l,_op2imm)     ALUi(AL, sub,  0, _d, _l, _op2imm)

// _d = _l & decOp2Imm(_op2imm)
#define ANDis(_d,_l,_op2imm,_s) ALUi(AL, and, _s, _d, _l, _op2imm)
#define ANDi(_d,_l,_op2imm)     ALUi(AL, and,  0, _d, _l, _op2imm)

// _d = _l | decOp2Imm(_op2imm)
#define ORRis(_d,_l,_op2imm,_s) ALUi(AL, orr, _s, _d, _l, _op2imm)
#define ORRi(_d,_l,_op2imm)     ALUi(AL, orr,  0, _d, _l, _op2imm)

// _d = _l ^ decOp2Imm(_op2imm)
#define EORis(_d,_l,_op2imm,_s) ALUi(AL, eor, _s, _d, _l, _op2imm)
#define EORi(_d,_l,_op2imm)     ALUi(AL, eor,  0, _d, _l, _op2imm)

// _d = _l | _r
#define ORRs(_d,_l,_r,_s)   ALUr(AL, orr, _s, _d, _l, _r)
#define ORR(_d,_l,_r)       ALUr(AL, orr,  0, _d, _l, _r)

// _d = _l & _r
#define ANDs(_d,_l,_r,_s)   ALUr(AL, and, _s, _d, _l, _r)
#define AND(_d,_l,_r)       ALUr(AL, and,  0, _d, _l, _r)

// _d = _l ^ _r
#define EORs(_d,_l,_r,_s)   ALUr(AL, eor, _s, _d, _l, _r)
#define EOR(_d,_l,_r)       ALUr(AL, eor,  0, _d, _l, _r)

// _d = _l + _r
#define ADDs(_d,_l,_r,_s)   ALUr(AL, add, _s, _d, _l, _r)
#define ADD(_d,_l,_r)       ALUr(AL, add,  0, _d, _l, _r)

// _d = _l - _r
#define SUBs(_d,_l,_r,_s)   ALUr(AL, sub, _s, _d, _l, _r)
#define SUB(_d,_l,_r)       ALUr(AL, sub,  0, _d, _l, _r)

// --------
// Other operations.
// --------

// [_d_hi,_d] = _l * _r
#define SMULL(_d, _d_hi, _l, _r)  do {                                                          \
        underrunProtect(4);                                                                     \
        NanoAssert(ARM_ARCH_AT_LEAST(6) || ((_d   ) != (_l)));                               \
        NanoAssert(ARM_ARCH_AT_LEAST(6) || ((_d_hi) != (_l)));                               \
        NanoAssert(IsGpReg(_d) && IsGpReg(_d_hi) && IsGpReg(_l) && IsGpReg(_r));                \
        NanoAssert(((_d) != PC) && ((_d_hi) != PC) && ((_l) != PC) && ((_r) != PC));            \
        *(--_nIns) = (NIns)( COND_AL | 0xc00090 | (_d_hi)<<16 | (_d)<<12 | (_r)<<8 | (_l) );    \
        asm_output("smull %s, %s, %s, %s",gpn(_d),gpn(_d_hi),gpn(_l),gpn(_r));                  \
} while(0)

// _d = _l * _r
#define MUL(_d, _l, _r)  do {                                               \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_ARCH_AT_LEAST(6) || ((_d) != (_l)));              \
        NanoAssert(IsGpReg(_d) && IsGpReg(_l) && IsGpReg(_r));              \
        NanoAssert(((_d) != PC) && ((_l) != PC) && ((_r) != PC));           \
        *(--_nIns) = (NIns)( COND_AL | (_d)<<16 | (_r)<<8 | 0x90 | (_l) );  \
        asm_output("mul %s, %s, %s",gpn(_d),gpn(_l),gpn(_r));               \
} while(0)

// RSBS _d, _r
// _d = 0 - _r
#define RSBS(_d,_r) ALUi(AL, rsb, 1, _d, _r, 0)

// MVN
// _d = ~_r (one's compliment)
#define MVN(_d,_r)                          ALUr(AL, mvn, 0, _d, 0, _r)
#define MVNis_chk(_d,_op2imm,_stat,_chk)    ALUi_chk(AL, mvn, _stat, _d, 0, op2imm, _chk)
#define MVNis(_d,_op2imm,_stat)             MVNis_chk(_d,_op2imm,_stat,1);

// Logical Shift Right (LSR) rotates the bits without maintaining sign extensions.
// MOVS _d, _r, LSR <_s>
// _d = _r >> _s
#define LSR(_d,_r,_s) ALUr_shr(AL, mov, 1, _d, 0, _r, LSR_reg, _s)

// Logical Shift Right (LSR) rotates the bits without maintaining sign extensions.
// MOVS _d, _r, LSR #(_imm & 0x1f)
// _d = _r >> (_imm & 0x1f)
#define LSRi(_d,_r,_imm)  ALUr_shi(AL, mov, 1, _d, 0, _r, LSR_imm, (_imm & 0x1f))

// Arithmetic Shift Right (ASR) maintains the sign extension.
// MOVS _d, _r, ASR <_s>
// _d = _r >> _s
#define ASR(_d,_r,_s) ALUr_shr(AL, mov, 1, _d, 0, _r, ASR_reg, _s)

// Arithmetic Shift Right (ASR) maintains the sign extension.
// MOVS _r, _r, ASR #(_imm & 0x1f)
// _d = _r >> (_imm & 0x1f)
#define ASRi(_d,_r,_imm) ALUr_shi(AL, mov, 1, _d, 0, _r, ASR_imm, (_imm & 0x1f))

// Logical Shift Left (LSL).
// MOVS _d, _r, LSL <_s>
// _d = _r << _s
#define LSL(_d, _r, _s) ALUr_shr(AL, mov, 1, _d, 0, _r, LSL_reg, _s)

// Logical Shift Left (LSL).
// MOVS _d, _r, LSL #(_imm & 0x1f)
// _d = _r << (_imm & 0x1f)
#define LSLi(_d, _r, _imm) ALUr_shi(AL, mov, 1, _d, 0, _r, LSL_imm, (_imm & 0x1f))

// TST
#define TST(_l,_r)      ALUr(AL, tst, 1, 0, _l, _r)
#define TSTi(_d,_imm)   ALUi(AL, tst, 1, 0, _d, _imm)

// CMP
#define CMP(_l,_r)  ALUr(AL, cmp, 1, 0, _l, _r)
#define CMN(_l,_r)  ALUr(AL, cmn, 1, 0, _l, _r)

// MOV
#define MOVis_chk(_d,_op2imm,_stat,_chk)    ALUi_chk(AL, mov, _stat, _d, 0, op2imm, _chk)
#define MOVis(_d,_op2imm,_stat)             MOVis_chk(_d,_op2imm,_stat,1)
#define MOVi(_d,_op2imm)                    MOVis(_d,_op2imm,0);

#define MOV_cond(_cond,_d,_s)               ALUr(_cond, mov, 0, _d, 0, _s)

#define MOV(dr,sr)   MOV_cond(AL, dr, sr)

// _d = [_b+off]
#define LDR(_d,_b,_off)        asm_ldr_chk(_d,_b,_off,1)
#define LDR_nochk(_d,_b,_off)  asm_ldr_chk(_d,_b,_off,0)

// _d = [_b + _x<<_s]
#define LDR_scaled(_d, _b, _x, _s) do { \
        NanoAssert(((_s)&31) == _s);\
        NanoAssert(IsGpReg(_d) && IsGpReg(_b) && IsGpReg(_x));\
        underrunProtect(4);\
        *(--_nIns) = (NIns)(COND_AL | (0x79<<20) | ((_b)<<16) | ((_d)<<12) | ((_s)<<7) | (_x));\
        asm_output("ldr %s, [%s, +%s, LSL #%d]", gpn(_d), gpn(_b), gpn(_x), (_s));\
    } while (0)

// _d = #_imm
#define LDi(_d,_imm) asm_ld_imm(_d,_imm)

// MOVW and MOVT are ARMv6T2 or newer only

// MOVW -- writes _imm into _d, zero-extends.
#define MOVWi_cond_chk(_cond,_d,_imm,_chk) do {                         \
        NanoAssert(isU16(_imm));                                        \
        NanoAssert(IsGpReg(_d));                                        \
        NanoAssert(IsCond(_cond));                                      \
        if (_chk) underrunProtect(4);                                   \
        *(--_nIns) = (NIns)( (_cond)<<28 | 3<<24 | 0<<20 | (((_imm)>>12)&0xf)<<16 | (_d)<<12 | ((_imm)&0xfff) ); \
        asm_output("movw%s %s, #0x%x", condNames[_cond], gpn(_d), (_imm)); \
    } while (0)

#define MOVWi(_d,_imm)              MOVWi_cond_chk(AL, _d, _imm, 1)
#define MOVWi_chk(_d,_imm,_chk)     MOVWi_cond_chk(AL, _d, _imm, _chk)
#define MOVWi_cond(_cond,_d,_imm)   MOVWi_cond_chk(_cond, _d, _imm, 1)

// MOVT -- writes _imm into top halfword of _d, does not affect bottom halfword
#define MOVTi_cond_chk(_cond,_d,_imm,_chk) do {                         \
        NanoAssert(isU16(_imm));                                        \
        NanoAssert(IsGpReg(_d));                                        \
        NanoAssert(IsCond(_cond));                                      \
        if (_chk) underrunProtect(4);                                   \
        *(--_nIns) = (NIns)( (_cond)<<28 | 3<<24 | 4<<20 | (((_imm)>>12)&0xf)<<16 | (_d)<<12 | ((_imm)&0xfff) ); \
        asm_output("movt%s %s, #0x%x", condNames[_cond], gpn(_d), (_imm)); \
    } while (0)

#define MOVTi(_d,_imm)              MOVTi_cond_chk(AL, _d, _imm, 1)
#define MOVTi_chk(_d,_imm,_chk)     MOVTi_cond_chk(AL, _d, _imm, _chk)
#define MOVTi_cond(_cond,_d,_imm)   MOVTi_cond_chk(_cond, _d, _imm, 1)

// i386 compat, for Assembler.cpp
#define MR(d,s)                     MOV(d,s)
#define ST(base,offset,reg)         STR(reg,base,offset)

// Load a byte (8 bits). The offset range is ±4095.
#define LDRB(_d,_n,_off) do {                                           \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        underrunProtect(4);                                             \
        if (_off < 0) {                                                 \
            NanoAssert(isU12(-(_off)));                                 \
            *(--_nIns) = (NIns)( COND_AL | (0x55<<20) | ((_n)<<16) | ((_d)<<12) | ((-(_off))&0xfff)  ); \
        } else {                                                        \
            NanoAssert(isU12(_off));                                    \
            *(--_nIns) = (NIns)( COND_AL | (0x5D<<20) | ((_n)<<16) | ((_d)<<12) | ((_off)&0xfff)  ); \
        }                                                               \
        asm_output("ldrb %s, [%s,#%d]", gpn(_d),gpn(_n),(_off));        \
    } while(0)

// Load a byte (8 bits), sign-extend to 32 bits. The offset range is
// ±255 (different from LDRB, same as LDRH/LDRSH)
#define LDRSB(_d,_n,_off) do {                                          \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        underrunProtect(4);                                             \
        if (_off < 0) {                                                 \
            NanoAssert(isU8(-(_off)));                                  \
            *(--_nIns) = (NIns)( COND_AL | (0x15<<20) | ((_n)<<16) | ((_d)<<12) | ((0xD)<<4) | (((-(_off))&0xf0)<<4) | ((-(_off))&0xf) ); \
        } else {                                                        \
            NanoAssert(isU8(_off));                                     \
            *(--_nIns) = (NIns)( COND_AL | (0x1D<<20) | ((_n)<<16) | ((_d)<<12) | ((0xD)<<4) | (((_off)&0xf0)<<4) | ((_off)&0xf) ); \
        }                                                               \
        asm_output("ldrsb %s, [%s,#%d]", gpn(_d),gpn(_n),(_off));       \
    } while(0)

// Load and sign-extend a half word (16 bits). The offset range is ±255, and
// must be aligned to two bytes on some architectures (the caller is responsible
// for ensuring appropriate alignment)
#define LDRH(_d,_n,_off) do {                                           \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        underrunProtect(4);                                             \
        if (_off < 0) {                                                 \
            NanoAssert(isU8(-(_off)));                                  \
            *(--_nIns) = (NIns)( COND_AL | (0x15<<20) | ((_n)<<16) | ((_d)<<12) | ((0xB)<<4) | (((-(_off))&0xf0)<<4) | ((-(_off))&0xf) ); \
        } else {                                                        \
            NanoAssert(isU8(_off));                                     \
            *(--_nIns) = (NIns)( COND_AL | (0x1D<<20) | ((_n)<<16) | ((_d)<<12) | ((0xB)<<4) | (((_off)&0xf0)<<4) | ((_off)&0xf) ); \
        }                                                               \
        asm_output("ldrh %s, [%s,#%d]", gpn(_d),gpn(_n),(_off));       \
    } while(0)

// Load and sign-extend a half word (16 bits). The offset range is ±255, and
// must be aligned to two bytes on some architectures (the caller is responsible
// for ensuring appropriate alignment)
#define LDRSH(_d,_n,_off) do {                                          \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        underrunProtect(4);                                             \
        if (_off < 0) {                                                 \
            NanoAssert(isU8(-(_off)));                                  \
            *(--_nIns) = (NIns)( COND_AL | (0x15<<20) | ((_n)<<16) | ((_d)<<12) | ((0xF)<<4) | (((-(_off))&0xf0)<<4) | ((-(_off))&0xf) ); \
        } else {                                                        \
            NanoAssert(isU8(_off));                                     \
            *(--_nIns) = (NIns)( COND_AL | (0x1D<<20) | ((_n)<<16) | ((_d)<<12) | ((0xF)<<4) | (((_off)&0xf0)<<4) | ((_off)&0xf) ); \
        }                                                               \
        asm_output("ldrsh %s, [%s,#%d]", gpn(_d),gpn(_n),(_off));       \
    } while(0)

// Valid offset for STR and STRB is +/- 4095, STRH only has +/- 255
#define STR(_d,_n,_off) do {                                            \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        NanoAssert(isU12(_off) || isU12(-(_off)));                      \
        underrunProtect(4);                                             \
        if ((_off)<0)   *(--_nIns) = (NIns)( COND_AL | (0x50<<20) | ((_n)<<16) | ((_d)<<12) | ((-(_off))&0xFFF) ); \
        else            *(--_nIns) = (NIns)( COND_AL | (0x58<<20) | ((_n)<<16) | ((_d)<<12) | ((_off)&0xFFF) ); \
        asm_output("str %s, [%s, #%d]", gpn(_d), gpn(_n), (_off)); \
    } while(0)

#define STRB(_d,_n,_off) do {                                           \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        NanoAssert(isU12(_off) || isU12(-(_off)));                      \
        underrunProtect(4);                                             \
        if ((_off)<0)   *(--_nIns) = (NIns)( COND_AL | (0x54<<20) | ((_n)<<16) | ((_d)<<12) | ((-(_off))&0xFFF) ); \
        else            *(--_nIns) = (NIns)( COND_AL | (0x5C<<20) | ((_n)<<16) | ((_d)<<12) | ((_off)&0xFFF) ); \
        asm_output("strb %s, [%s, #%d]", gpn(_d), gpn(_n), (_off)); \
    } while(0)

// Only +/- 255 range, unlike STRB/STR
#define STRH(_d,_n,_off) do {                                           \
        NanoAssert(IsGpReg(_d) && IsGpReg(_n));                         \
        underrunProtect(4);                                             \
        if ((_off)<0) {                                                 \
            NanoAssert(isU8(-(_off)));                                  \
            *(--_nIns) = (NIns)( COND_AL | (0x14<<20) | ((_n)<<16) | ((_d)<<12) | (((-(_off))&0xF0)<<4) | (0xB<<4) | ((-(_off))&0xF) ); \
        } else {                                                        \
            NanoAssert(isU8(_off));                                     \
            *(--_nIns) = (NIns)( COND_AL | (0x1C<<20) | ((_n)<<16) | ((_d)<<12) | (((_off)&0xF0)<<4) | (0xB<<4) | ((_off)&0xF) ); \
        }                                                               \
        asm_output("strh %s, [%s, #%d]", gpn(_d), gpn(_n), (_off));     \
    } while(0)

// Encode a breakpoint. The ID is not important and is ignored by the
// processor, but it can be useful as a marker when debugging emitted code.
#define BKPT_insn       ((NIns)( COND_AL | (0x12<<20) | (0x7<<4) ))
#define BKPTi_insn(id)  ((NIns)(BKPT_insn | ((id << 4) & 0xfff00) | (id & 0xf)));

#define BKPT_nochk()    BKPTi_nochk(0)
#define BKPTi_nochk(id) do {                                \
        NanoAssert((id & 0xffff) == id);                    \
        *(--_nIns) = BKPTi_insn(id);                        \
        } while (0)

// STMFD SP!, {reg}
#define PUSHr(_r)  do {                                                 \
        underrunProtect(4);                                             \
        NanoAssert(IsGpReg(_r));                                        \
        *(--_nIns) = (NIns)( COND_AL | (0x92<<20) | (SP<<16) | rmask(_r) ); \
        asm_output("push %s",gpn(_r)); } while (0)

// STMFD SP!,{reglist}
#define PUSH_mask(_mask)  do {                                          \
        underrunProtect(4);                                             \
        NanoAssert(isU16(_mask));                                       \
        *(--_nIns) = (NIns)( COND_AL | (0x92<<20) | (SP<<16) | (_mask) ); \
        asm_output("push %x", (_mask));} while (0)

// LDMFD SP!,{reg}
#define POPr(_r) do {                                                   \
        underrunProtect(4);                                             \
        NanoAssert(IsGpReg(_r));                                        \
        *(--_nIns) = (NIns)( COND_AL | (0x8B<<20) | (SP<<16) | rmask(_r) ); \
        asm_output("pop %s",gpn(_r));} while (0)

// LDMFD SP!,{reglist}
#define POP_mask(_mask) do {                                            \
        underrunProtect(4);                                             \
        NanoAssert(isU16(_mask));                                       \
        *(--_nIns) = (NIns)( COND_AL | (0x8B<<20) | (SP<<16) | (_mask) ); \
        asm_output("pop %x", (_mask));} while (0)

// PC always points to current instruction + 8, so when calculating pc-relative
// offsets, use PC+8.
#define PC_OFFSET_FROM(target,frompc) ((intptr_t)(target) - ((intptr_t)(frompc) + 8))

#define B_cond(_c,_t)                           \
    B_cond_chk(_c,_t,1)

#define B_nochk(_t)                             \
    B_cond_chk(AL,_t,0)

#define B(t)    B_cond(AL,t)
#define BHI(t)  B_cond(HI,t)
#define BLS(t)  B_cond(LS,t)
#define BHS(t)  B_cond(HS,t)
#define BLO(t)  B_cond(LO,t)
#define BEQ(t)  B_cond(EQ,t)
#define BNE(t)  B_cond(NE,t)
#define BLT(t)  B_cond(LT,t)
#define BGE(t)  B_cond(GE,t)
#define BLE(t)  B_cond(LE,t)
#define BGT(t)  B_cond(GT,t)
#define BVS(t)  B_cond(VS,t)
#define BVC(t)  B_cond(VC,t)
#define BCC(t)  B_cond(CC,t)
#define BCS(t)  B_cond(CS,t)

#define JMP(t) B(t)
#define JMP_nochk(t) B_nochk(t)

// MOV(cond) _r, #1
// MOV(!cond) _r, #0
#define SET(_r,_cond) do {                                              \
    ConditionCode _opp = OppositeCond(_cond);                           \
    underrunProtect(8);                                                 \
    *(--_nIns) = (NIns)( ( _opp<<28) | (0x3A<<20) | ((_r)<<12) | (0) ); \
    *(--_nIns) = (NIns)( (_cond<<28) | (0x3A<<20) | ((_r)<<12) | (1) ); \
    asm_output("mov%s %s, #1", condNames[_cond], gpn(_r));              \
    asm_output("mov%s %s, #0", condNames[_opp], gpn(_r));               \
    } while (0)

#define SETEQ(r)    SET(r,EQ)
#define SETNE(r)    SET(r,NE)
#define SETLT(r)    SET(r,LT)
#define SETLE(r)    SET(r,LE)
#define SETGT(r)    SET(r,GT)
#define SETGE(r)    SET(r,GE)
#define SETLO(r)    SET(r,LO)
#define SETLS(r)    SET(r,LS)
#define SETHI(r)    SET(r,HI)
#define SETHS(r)    SET(r,HS)
#define SETVS(r)    SET(r,VS)
#define SETCS(r)    SET(r,CS)

// Load and sign extend a 16-bit value into a reg
#define MOVSX(_d,_off,_b) do {                                          \
        if ((_off)>=0) {                                                \
            if ((_off)<256) {                                           \
                underrunProtect(4);                                     \
                *(--_nIns) = (NIns)( COND_AL | (0x1D<<20) | ((_b)<<16) | ((_d)<<12) |  ((((_off)>>4)&0xF)<<8) | (0xF<<4) | ((_off)&0xF)  ); \
            } else if ((_off)<=510) {                                   \
                underrunProtect(8);                                     \
                int rem = (_off) - 255;                                 \
                NanoAssert(rem<256);                                    \
                *(--_nIns) = (NIns)( COND_AL | (0x1D<<20) | ((_d)<<16) | ((_d)<<12) |  ((((rem)>>4)&0xF)<<8) | (0xF<<4) | ((rem)&0xF)  ); \
                *(--_nIns) = (NIns)( COND_AL | OP_IMM | (1<<23) | ((_b)<<16) | ((_d)<<12) | (0xFF) ); \
            } else {                                                    \
                underrunProtect(16);                                    \
                int rem = (_off) & 3;                                   \
                *(--_nIns) = (NIns)( COND_AL | (0x19<<20) | ((_b)<<16) | ((_d)<<12) | (0xF<<4) | (_d) ); \
                asm_output("ldrsh %s,[%s, #%d]",gpn(_d), gpn(_b), (_off)); \
                *(--_nIns) = (NIns)( COND_AL | OP_IMM | (1<<23) | ((_d)<<16) | ((_d)<<12) | rem ); \
                *(--_nIns) = (NIns)( COND_AL | (0x1A<<20) | ((_d)<<12) | (2<<7)| (_d) ); \
                *(--_nIns) = (NIns)( COND_AL | (0x3B<<20) | ((_d)<<12) | (((_off)>>2)&0xFF) ); \
                asm_output("mov %s,%d",gpn(_d),(_off));                \
            }                                                           \
        } else {                                                        \
            if ((_off)>-256) {                                          \
                underrunProtect(4);                                     \
                *(--_nIns) = (NIns)( COND_AL | (0x15<<20) | ((_b)<<16) | ((_d)<<12) |  ((((-(_off))>>4)&0xF)<<8) | (0xF<<4) | ((-(_off))&0xF)  ); \
                asm_output("ldrsh %s,[%s, #%d]",gpn(_d), gpn(_b), (_off)); \
            } else if ((_off)>=-510){                                   \
                underrunProtect(8);                                     \
                int rem = -(_off) - 255;                                \
                NanoAssert(rem<256);                                    \
                *(--_nIns) = (NIns)( COND_AL | (0x15<<20) | ((_d)<<16) | ((_d)<<12) |  ((((rem)>>4)&0xF)<<8) | (0xF<<4) | ((rem)&0xF)  ); \
                *(--_nIns) = (NIns)( COND_AL | OP_IMM | (1<<22) | ((_b)<<16) | ((_d)<<12) | (0xFF) ); \
            } else NanoAssert(0);                                        \
        }                                                               \
    } while(0)

/*
 * VFP, general encoding utilities
 */

#define split_qreg_enc(_Qd,D,Vd)  do{  Vd = FpQRegNum(_Qd);NanoAssert(Vd <= 15);\
                                       Vd *= 2;                                 \
                                       D = (Vd & 0x10) >> 4;                    \
                                       Vd = Vd & 0xf; } while(0)
#define split_dreg_enc(_Dd,D,Vd)  do{  Vd = FpDRegNum(_Dd);NanoAssert(Vd <= 31);\
                                       D = (Vd & 0x10) >> 4;                    \
                                       Vd = Vd & 0xf; } while(0)
#define split_sreg_enc(_Sd,D,Vd)  do{  Vd = FpSRegNum(_Sd);NanoAssert(Vd <= 31);\
                                       D = (Vd & 0x1);                          \
                                       Vd = Vd >> 1; } while(0)
    
#define encodeCommon(_Dm,M,Vm,sOk,dOk,qOk)                                      \
        if(IsFpSReg(_Dm)){                                                      \
            NanoAssert( sOk || (#_Dm" register is Sn but Sn inacceptable"==0) );\
            split_sreg_enc(_Dm,M,Vm);                                           \
        } else if(IsFpDReg(_Dm)){                                               \
            NanoAssert( dOk || (#_Dm" register is Dn but Dn inacceptable"==0) );\
            split_dreg_enc(_Dm,M,Vm);                                           \
        } else if(IsFpQReg(_Dm)){                                               \
            NanoAssert( qOk || (#_Dm" register is Qn but Qn inacceptable"==0) );\
            split_qreg_enc(_Dm,M,Vm);                                           \
        } else {                                                                \
            NanoAssert( dOk || (#_Dm" register should be FP register"==0) );    \
            M=Vm=0;                                                             \
        }


#define __encodeRegM(_Dm,sOk,dOk,qOk)                               \
        int M,Vm;                                                   \
        encodeCommon(_Dm,M,Vm,sOk,dOk,qOk);                         \
        DmEnc = (M << 5) | Vm;                                      
#define __encodeRegN(_Dn,sOk,dOk,qOk)                               \
        int N,Vn;                                                   \
        encodeCommon(_Dn,N,Vn,sOk,dOk,qOk);                         \
        DnEnc = (N<<7) | (Vn<<16);                                  
#define __encodeRegD(_Dd,sOk,dOk,qOk)                               \
        int D,Vd;                                                   \
        encodeCommon(_Dd,D,Vd,sOk,dOk,qOk);                         \
        DdEnc = (Vd<<12) | (D<<22);                                 
#define DFlag(_Dd) ( IsFpDReg(_Dd) ? (1 << 8) : 0 )
    
//#define encodeRegM(...) __encodeRegM(__VA_ARGS__)        
//#define encodeRegN(...) __encodeRegN(__VA_ARGS__)        
//#define encodeRegD(...) __encodeRegD(__VA_ARGS__)        
#define encodeRegM(a,b,c,d) __encodeRegM(a,b,c,d)        
#define encodeRegN(a,b,c,d) __encodeRegN(a,b,c,d)        
#define encodeRegD(a,b,c,d) __encodeRegD(a,b,c,d)        

//#define SD_  true , true , false
//#define S__  true , false, false
//#define _D_  false, true , false
//#define __Q  false, false, true
    
/*
 * VFP, instructions
 */

#define FMDRR(_Dm,_Rd,_Rn) do {                                         \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(IsGpReg(_Rd) && IsGpReg(_Rn));                       \
        int DmEnc; encodeRegM(_Dm,false, true , false);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xC4<<20) | ((_Rn)<<16) | ((_Rd)<<12) | (0xB1<<4) | DmEnc ); \
        asm_output("fmdrr %s,%s,%s", gpn(_Dm), gpn(_Rd), gpn(_Rn));     \
    } while (0)

#define FMRRD(_Rd,_Rn,_Dm) do {                                         \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(IsGpReg(_Rd) && IsGpReg(_Rn));                       \
        int DmEnc; encodeRegM(_Dm,false, true , false);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xC5<<20) | ((_Rn)<<16) | ((_Rd)<<12) | (0xB1<<4) | DmEnc ); \
        asm_output("fmrrd %s,%s,%s", gpn(_Rd), gpn(_Rn), gpn(_Dm));     \
    } while (0)

#define FMRDH(_Rd,_Dn) do {                                             \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(IsGpReg(_Rd) );                                      \
        int DnEnc; encodeRegN(_Dn,false, true , false);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xE3<<20) | DnEnc | ((_Rd)<<12) | 0xB10 ); \
        asm_output("fmrdh %s,%s", gpn(_Rd), gpn(_Dn));                  \
    } while (0)

#define FMRDL(_Rd,_Dn) do {                                             \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(IsGpReg(_Rd));                                       \
        int DnEnc; encodeRegN(_Dn,DnEnc,false, true , false);                           \
        *(--_nIns) = (NIns)( COND_AL | (0xE1<<20) | DnEnc | ((_Rd)<<12) | 0xB10 ); \
        asm_output("fmrdh %s,%s", gpn(_Rd), gpn(_Dn));                  \
    } while (0)

#define VLDR_chk(_Dd,_Rn,_offs,_chk) do{ \
        if(_chk) underrunProtect(4);                                    \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(((_offs) % 4) == 0);                                 \
        NanoAssert(isU8((_offs)/4) || isU8(-(_offs)/4));                \
        NanoAssert( IsGpReg(_Rn));                                      \
        int negflag = 1<<23;                                            \
        int dflag = DFlag(_Dd);                                         \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                 \
        intptr_t offs = (_offs);                                        \
        if (_offs < 0) {                                                \
            negflag = 0<<23;                                            \
            offs = -(offs);                                             \
        }                                                               \
        *(--_nIns) = (NIns)( COND_AL | (0xD1<<20) | ((_Rn)<<16) | DdEnc | (0xA<<8) | dflag | negflag | ((offs>>2)&0xff) ); \
        asm_output("fld%c %s,%s(%d)", dflag?'d':'s', gpn(_Dd), gpn(_Rn), _offs); \
    } while(0)

#define VLDQR(_Qd,_Rn) do {                                             \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert( IsGpReg(_Rn));                                      \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xC9<<20) | ((_Rn)<<16) | DdEnc | 0xB04 );        \
        asm_output("vldm <d%d,d%d>,%s // i.e. %s",  Vd+D*16,Vd+D*16+1,gpn(_Rn),gpn(_Qd)); \
    } while (0)

#define FLDD_chk VLDR_chk
#define FLDS_chk VLDR_chk
#define FLDD(_Dd,_Rn,_offs) FLDD_chk(_Dd,_Rn,_offs,1)
#define FLDS(_Sd,_Rn,_offs) FLDS_chk(_Sd,_Rn,_offs,1)
#define VLDR FLDD
    
    
#define VSTR(_Dd,_Rn,_offs)  do {                                       \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert(((_offs)%4) == 0);                                   \
        NanoAssert(isU8((_offs)/4) || isU8(-(_offs)/4));                \
        NanoAssert( IsGpReg(_Rn));                                      \
        int dflag = DFlag(_Dd);                                         \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                 \
        int negflag = 1<<23;                                            \
        intptr_t offs = (_offs);                                        \
        if (_offs < 0) {                                                \
            negflag = 0<<23;                                            \
            offs = -(offs);                                             \
        }                                                               \
        *(--_nIns) = (NIns)( COND_AL | (0xD0<<20) | ((_Rn)<<16) | (DdEnc) | (0xA<<8) | dflag | negflag | ((offs>>2)&0xff) ); \
        asm_output("fst%c %s,%s(%d)", dflag?'d':'s',gpn(_Dd), gpn(_Rn), _offs);           \
    } while (0)


#define VSTQR(_Qd,_Rn) do {                                             \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        NanoAssert( IsGpReg(_Rn));                                      \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xC8<<20) | ((_Rn)<<16) | DdEnc | 0xB04 );        \
        asm_output("vstm %s,<d%d,d%d> // i.e. %s", gpn(_Rn), Vd+D*16,Vd+D*16+1,gpn(_Qd)); \
     } while (0)
#define FSTS  VSTR
#define FSTD  VSTR

#define VNEGvfp(_Dd,_Dm) do {                                           \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int dflag = DFlag(_Dd); NanoAssert(dflag==DFlag(_Dm));          \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                 \
        int DmEnc; encodeRegM(_Dm,true , true , false);                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xEB10A40) | DdEnc | DmEnc | dflag ); \
        asm_output("fneg%c %s,%s",dflag?'d':'s', gpn(_Dd), gpn(_Dm));   \
     } while (0)

#define VNEGneon(_Qd,_Qm) do {                                          \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                 \
        int DmEnc; encodeRegM(_Qm,false, false, true);                                 \
        *(--_nIns) = (NIns)( 0xF3B907C0 | DdEnc | DmEnc );              \
        asm_output("fnegq %s,%s", gpn(_Qd), gpn(_Qm));                  \
     } while (0)


#define GenericVfp(NAME,_Dd,_Dn,_Dm,SPECIFIC_ENC) do {                  \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int dflag = DFlag(_Dd); NanoAssert(dflag==DFlag(_Dm));NanoAssert(dflag==DFlag(_Dn)); \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                 \
        int DmEnc; encodeRegM(_Dm,true , true , false);                                 \
        int DnEnc; encodeRegN(_Dn,true , true , false);                                 \
        *(--_nIns) = (NIns)( COND_AL | SPECIFIC_ENC | DnEnc | DdEnc | DmEnc | dflag);        \
        asm_output(#NAME"%c %s,%s,%s",dflag?'d':'s', gpn(_Dd), gpn(_Dn), gpn(_Dm));          \
     } while (0)

#define GenericNeon(NAME,_Qd,_Qn,_Qm,SPECIFIC_ENC) do {                 \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                 \
        int DmEnc; encodeRegM(_Qm,false, false, true);                                 \
        int DnEnc; encodeRegN(_Qn,false, false, true);                                 \
        *(--_nIns) = (NIns)( (0xF2 << 24) | (SPECIFIC_ENC) | DdEnc | DnEnc| DmEnc );         \
        asm_output(#NAME" %s,%s,%s", gpn(_Qd), gpn(_Qn), gpn(_Qm));     \
    } while (0)


#define VADDvfp(_Dd,_Dn,_Dm)  GenericVfp(fadd,_Dd,_Dn,_Dm,0xE300A00)
#define VSUBvfp(_Dd,_Dn,_Dm)  GenericVfp(fsub,_Dd,_Dn,_Dm,0xE300A40)
#define VMULvfp(_Dd,_Dn,_Dm)  GenericVfp(fmul,_Dd,_Dn,_Dm,0xE200A00)
#define VDIVvfp(_Dd,_Dn,_Dm)  GenericVfp(fdiv,_Dd,_Dn,_Dm,0xE800A00)

#define VADDneon(_Qd,_Qn,_Qm) GenericNeon(vadd,_Qd,_Qn,_Qm, 0xD40)
#define VSUBneon(_Qd,_Qn,_Qm) GenericNeon(vsub,_Qd,_Qn,_Qm, 0x200D40)
#define VMULneon(_Qd,_Qn,_Qm) GenericNeon(vmul,_Qd,_Qn,_Qm, 0x1000D50)
  /* neon doesn't have VDIV; we'll do it on the components */
#define VDIVneon(_Qd,_Qn,_Qm) do{                                       \
            NanoAssert(IsFpQReg(_Qn) && IsFpQReg(_Qd) && IsFpQReg(_Qm));\
            Register Sn = S0 + (FpQRegNum(_Qn) << 2);                   \
            Register Sm = S0 + (FpQRegNum(_Qm) << 2);                   \
            Register Sd = S0 + (FpQRegNum(_Qd) << 2);                   \
            VDIVvfp(Sd,Sn,Sm); Sn = Sn + 1; Sm = Sm + 1; Sd = Sd + 1;   \
            VDIVvfp(Sd,Sn,Sm); Sn = Sn + 1; Sm = Sm + 1; Sd = Sd + 1;   \
            VDIVvfp(Sd,Sn,Sm); Sn = Sn + 1; Sm = Sm + 1; Sd = Sd + 1;   \
            VDIVvfp(Sd,Sn,Sm);                                          \
        } while (0) 

#define VADD(_Dd,_Dn,_Dm) if(IsFpQReg(_Dd)) VADDneon(_Dd,_Dn,_Dm); else VADDvfp(_Dd,_Dn,_Dm);
#define VSUB(_Dd,_Dn,_Dm) if(IsFpQReg(_Dd)) VSUBneon(_Dd,_Dn,_Dm); else VSUBvfp(_Dd,_Dn,_Dm);
#define VMUL(_Dd,_Dn,_Dm) if(IsFpQReg(_Dd)) VMULneon(_Dd,_Dn,_Dm); else VMULvfp(_Dd,_Dn,_Dm);
#define VDIV(_Dd,_Dn,_Dm) if(IsFpQReg(_Dd)) VDIVneon(_Dd,_Dn,_Dm); else VDIVvfp(_Dd,_Dn,_Dm);
#define VNEG(_Dd, _Dm)    if(IsFpQReg(_Dd)) VNEGneon(_Dd,_Dm);     else VNEGvfp(_Dd,_Dm);
#define VCEQ(_Qd,_Qn,_Qm) GenericNeon(vceq.f32,_Qd,_Qn,_Qm,0xE40)

#define VMOVN(_Dd,_Qm)  do { \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int DdEnc; encodeRegD(_Dd,false, true , false);                                 \
        int DmEnc; encodeRegM(_Qm,false, false, true);                                 \
        *(--_nIns) = (NIns)( 0xF3B60200 | DdEnc | DmEnc );              \
        asm_output("vmovn.i32 %s,%s", gpn(_Dd), gpn(_Qm));              \
    } while (0)

#define VMVN(_Dd,_Dm)  do { \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int DdEnc; encodeRegD(_Dd,false, true , false);                                 \
        int DmEnc; encodeRegM(_Dm,false, true , false);                                 \
        *(--_nIns) = (NIns)( 0xF3B00580 | DdEnc | DmEnc );              \
        asm_output("vmvn %s,%s", gpn(_Dd), gpn(_Dm));                   \
    } while (0)

#define VPMINU16(_Dd,_Dn,_Dm)  do { \
        underrunProtect(4);                                             \
        NanoAssert(ARM_VFP);                                            \
        int DdEnc; encodeRegD(_Dd,false, true , false);                                 \
        int DmEnc; encodeRegM(_Dm,false, true , false);                                 \
        int DnEnc; encodeRegN(_Dn,false, true , false);                                 \
        *(--_nIns) = (NIns)( 0xF3100A10 | DnEnc | DdEnc | DmEnc );      \
        asm_output("vpmin.u16 %s,%s,%s", gpn(_Dd), gpn(_Dn), gpn(_Dm)); \
    } while (0)
    
#define VDUP(_Qd,_Sm) do {\
        underrunProtect(4);                                                   \
        NanoAssert(ARM_VFP);                                                  \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                       \
        int M,Vm; NanoAssert(IsFpSReg(_Sm));                                  \
        split_sreg_enc(_Sm,M,Vm);                                             \
   /* M is part of th 'imm4' encoding; the first bit. the rest is 0x4 (100)*/ \
   /* The actual M is always 0 - Sn registers never get to D16-31 range    */ \
        *(--_nIns) = (NIns)( 0xF3B40C40 | DdEnc | Vm | (M<<19) );             \
        asm_output("vdup.32 %s,%s", gpn(_Qd), gpn(_Sm));                      \
    } while (0)


#define FMSTAT() do {                                                       \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        *(--_nIns) = (NIns)( COND_AL | 0x0EF1FA10);                         \
        asm_output("fmstat");                                               \
    } while (0)

#define VCMP(_Dd,_Dm,_E) do {                                               \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        int dflag = DFlag(_Dd); NanoAssert(dflag==DFlag(_Dm));              \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                     \
        int DmEnc; encodeRegM(_Dm,true , true , false);                                     \
        NanoAssert(((_E)==0) || ((_E)==1));                                 \
        *(--_nIns) = (NIns)( COND_AL | (0xEB4<<16) | DdEnc | 0xA40 | ((_E)<<7) | dflag | DmEnc ); \
        asm_output("fcmp%s%c %s,%s", (((_E)==1)?"e":""),dflag?'d':'s', gpn(_Dd), gpn(_Dm));       \
    } while (0)

#define VCMPZ(_Dd) do {                                                     \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        int DdEnc; encodeRegD(_Dd,false, true , false);                                     \
        *(--_nIns) = (NIns)( COND_AL | (0xEB5<<16) | DdEnc | 0xBC0 );       \
        asm_output("vcmpz %s", gpn(_Dd) );                                  \
    } while (0)

#define VMOV_cond(_cond,_Dd,_Dm) do {                                       \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        NanoAssert(IsCond(_cond));                                          \
        int dflag = DFlag(_Dd); NanoAssert(dflag==DFlag(_Dm));              \
        int DdEnc; encodeRegD(_Dd,true , true , false);                                     \
        int DmEnc; encodeRegM(_Dm,true , true , false);                                     \
        *(--_nIns) = (NIns)( ((_cond)<<28) | (0xEB0<<16) | DdEnc | 0xA40 | DmEnc | dflag );       \
        asm_output("fcpy%c%s %s,%s", dflag?'d':'s', condNames[_cond], gpn(_Dd), gpn(_Dm));        \
     } while (0)

#define FCPYS(_Sd,_Sm)      VMOV_cond(AL,_Sd,_Sm)
#define FCPYD(_Dd,_Dm)      VMOV_cond(AL,_Dd,_Dm)
#define FCPYD_cond          VMOV_cond
#define FCPYQ(_Qd,_Qm)  do {                                                \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP); /*NEON!*/                                      \
        int DdEnc; encodeRegD(_Qd,false, false, true);                                     \
        int DmEnc; encodeRegM(_Qm,false, false, true);                                     \
        int DnEnc; encodeRegN(_Qm,false, false, true);                                     \
        *(--_nIns) = (NIns)( (0xF2200150) | DdEnc  | DmEnc  | DnEnc );      \
        asm_output("fcpyq %s,%s", gpn(_Qd), gpn(_Qm));                      \
    } while (0)

#define FMSR(_Sn,_Rd) do {                                                  \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        NanoAssert( IsGpReg(_Rd) );                                         \
        int DnEnc; encodeRegN(_Sn,true , false, false);                                     \
        *(--_nIns) = (NIns)( COND_AL | (0xE0<<20) |((_Rd)<<12) | 0xA10 | DnEnc ); \
        asm_output("fmsr %s,%s", gpn(_Sn), gpn(_Rd));                       \
    } while (0)

#define FMRS(_Rd,_Sn) do {                                                  \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        NanoAssert( IsGpReg(_Rd) );                                         \
        int DnEnc; encodeRegN(_Sn,true , false, false);                                     \
        *(--_nIns) = (NIns)( COND_AL | (0xE1<<20) |((_Rd)<<12) | 0xA10 | DnEnc ); \
        asm_output("fmrs %s,%s", gpn(_Rd), gpn(_Sn));                       \
    } while (0)


/* Conversions: to/from int(signed, unsigned), to/from single/double precision */

#define VCVTx(NAME,_Rd,_Rm,SPECIFIC_ENC,check_dst) do {                     \
        underrunProtect(4);                                                 \
        NanoAssert(ARM_VFP);                                                \
        int dflag = DFlag((check_dst?_Rd:_Rm));                             \
        int DdEnc; encodeRegD(_Rd,true , true , false);                                     \
        int DmEnc; encodeRegM(_Rm,true , true , false);                                     \
        *(--_nIns) = (NIns)( COND_AL | (0xEB<<20) | SPECIFIC_ENC | dflag | DdEnc | DmEnc ); \
        asm_output(#NAME" %s,%s",gpn(_Rd), gpn(_Rm));                       \
    } while(0)
#define VCVT(A,B,C,D) VCVTx(A,B,C,D,true)

#define FCVTDS(_Dd,_Sm) do {                                               \
        NanoAssert(IsFpDReg(_Dd) && IsFpSReg(_Sm));                        \
        VCVTx(fcvtds,_Dd,_Sm,0x70AC0,false);                               \
    } while (0)

#define FCVTSD(_Sd,_Dm) do {                                               \
        NanoAssert(IsFpSReg(_Sd) && IsFpDReg(_Dm));                        \
        VCVTx(fcvtsd,_Sd,_Dm,0x70AC0,false);                               \
    } while (0)

#define FSITOD(_Dd,_Sm) do {                                               \
        NanoAssert(IsFpDReg(_Dd) && IsFpSReg(_Sm));                        \
        VCVT(fsitod,_Dd,_Sm,0x80AC0);                                      \
    } while (0)

#define FUITOD(_Dd,_Sm) do {                                               \
        NanoAssert(IsFpDReg(_Dd) && IsFpSReg(_Sm));                        \
        VCVT(fuitod,_Dd,_Sm,0x80A40);                                      \
    } while (0)

#define FSITOS(_Sd,_Sm) do {                                              \
        NanoAssert(IsFpSReg(_Sd) && IsFpSReg(_Sm));                       \
        VCVT(fsitos,_Sd,_Sm,0x80AC0);                                     \
    } while (0)

//  unsigned int, to single-precision float
#define FUITOS(_Sd,_Sm) do {                                              \
        NanoAssert(IsFpSReg(_Sd) && IsFpSReg(_Sm));                       \
        VCVT(fuitos,_Sd,_Sm,0x80A40);                                     \
    } while (0)

//  Double to signed int, stored in Sn reg
#define FTOSID(_Sd,_Dm)do {                                               \
        NanoAssert(IsFpSReg(_Sd) && IsFpDReg(_Dm));                       \
        VCVTx(ftosid,_Sd,_Dm,0xD0A40,false);                              \
    } while (0)

//  Single-precision float to signed int, stored in Sn reg
#define FTOSIS(_Sd,_Sm)do {                                               \
        NanoAssert(IsFpSReg(_Sd) && IsFpSReg(_Sm));                       \
        VCVTx(ftosis,_Sd,_Sm,0xD0A40,false);                              \
    } while (0)

} // namespace nanojit
#endif // __nanojit_NativeARM__
