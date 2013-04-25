/* This Source Code Form is subject to the terms of the Mozilla Public
    * License, v. 2.0. If a copy of the MPL was not distributed with this
    * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nanojit_NativeThumb2__
#define __nanojit_NativeThumb2__

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
// Or we're compiling for WinRT
#if defined (__ARM_PCS_VFP) || defined(UNDER_RT)
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

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=637809
#define NJ_JTBL_ALLOWED_IDX_REGS        GpRegs
#endif

#define RA_REGISTERS_OVERLAP            1
#define firstAvailableReg               getAvailableReg
#define getFatherReg                    _allocator.getSuitableRegFor

#define NJ_CONSTANT_POOLS
const int NJ_MAX_CPOOL_OFFSET = 4096;
const int NJ_CPOOL_SIZE = 16;

const int LARGEST_UNDERRUN_PROT = 32;  // largest value passed to underrunProtect

typedef uint16_t NIns;
typedef uint32_t NIns32;

// Bytes of icache to flush after Assembler::patch
const size_t LARGEST_BRANCH_PATCH = 2 * 2 * sizeof(NIns);

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

typedef enum {
    ALU_and = 0,
    ALU_bic = 1,
    ALU_orr = 2,
    ALU_eor = 4,
    ALU_add = 8,
    ALU_adc = 10,
    ALU_sbc = 11,
    ALU_sub = 13,
    ALU_rsb = 14,

    ALU_OP_LIMIT
} ALU_Op;

typedef enum {
    TST_tst = 0,
    TST_teq = 4,
    TST_cmn = 8,
    TST_cmp = 13,

    TST_OP_LIMIT
} Test_Op;

typedef enum {
    MOV_mov = 2,
    MOV_mvn = 3,

    MOV_OP_LIMIT
} Move_Op;

typedef enum {
    SHIFT_lsl = 0,
    SHIFT_lsr = 1,
    SHIFT_asr = 2,

    SHIFT_OP_LIMIT
} Shift_Op;

// ARM condition codes
// Allowed only in branches (Thumb2)

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
    AL = 0xE, // ALways,

    // Note that condition code NV is unpredictable on ARMv3 and ARMv4, and has
    // special meaning for ARMv5 onwards. As such, it should never be used in
    // an instruction encoding unless the special (ARMv5+) meaning is required.
    NV = 0xF, // NeVer

    CONDITION_CODE_LIMIT
} ConditionCode;

} // namespace nanojit

#define NJ_USE_UINT32_REGISTER 1
#include "NativeCommon.h"

namespace nanojit
{

typedef uint64_t RegisterMask;
#define rmask(r) ARM_REG_MASKS[REGNUM(r)]
extern const RegisterMask ARM_REG_MASKS[LastRegNum+1];

typedef struct _FragInfo {
    RegisterMask    needRestoring;
    NIns*           epilogue;
} FragInfo;

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

class ParameterRegisters {
   public:
    int stkd;
    Register r;
#ifdef NJ_ARM_EABI_HARD_FLOAT
    RegisterMask float_mask;
#endif
    ParameterRegisters(): stkd(0), r(R0)
#ifdef NJ_ARM_EABI_HARD_FLOAT
                        , float_mask(FpSRegs)
#endif
    { NanoAssert(FpSRegs == FpQRegs); }
};


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

#define DECLARE_PLATFORM_REGALLOC()                                                                               \
    const static Register argRegs[4], retRegs[2];                                                                 \
    Register getAvailableReg(LIns* ins, Register regClass, RegisterMask allow);                                   \
    Register getSuitableRegFor(Register r, LIns* curins);

#ifdef DEBUG
# define DECLARE_PLATFORM_ASSEMBLER_DEBUG()                                                                       \
    inline bool         isThumb2Imm(uint32_t literal);                                                            \
    inline uint32_t     decThumb2Imm(uint32_t enc);
#else
// define stubs, for code that defines NJ_VERBOSE without DEBUG
# define DECLARE_PLATFORM_ASSEMBLER_DEBUG()                                                                       \
    inline bool         isThumb2Imm(uint32_t ) { return true; }                                                   \
    inline uint32_t     decThumb2Imm(uint32_t ) { return 0; }
#endif

#define DECLARE_PLATFORM_ASSEMBLER()                                                                              \
    inline void xALUi(ALU_Op op, uint32_t S, Register rd, Register rl, uint32_t op2imm);                          \
    inline void xTSTi(Test_Op op, Register rl, uint32_t op2imm);                                                  \
    inline void xMOVi(Move_Op op, Register rd, uint32_t op2imm);                                                  \
    inline void xALUr(ALU_Op op, uint32_t S, Register rd, Register rl, Register rr);                              \
    inline void xTSTr(Test_Op op, Register rl, Register rr);                                                      \
    inline void xMOVr(Move_Op op, Register rd, Register rr);                                                      \
    inline void xSHIFTi(Shift_Op op, Register rd, Register rl, int32_t imm);                                      \
    inline void xSHIFTr(Shift_Op op, Register rd, Register rl, Register rs);                                      \
    inline void MUL(Register rd, Register rl, Register rr);                                                       \
    inline void SMULL(Register rd_lo, Register rd_hi, Register rl, Register rr);                                  \
    inline void MOVWi(Register rd, uint32_t imm);                                                                 \
    inline void MOVTi(Register d, uint32_t imm);                                                                  \
    inline void LDRi(Register rd, Register rn, int32_t off);                                                      \
    inline void LDRr(Register rd, Register rn, Register rm);                                                      \
    inline void LDR_scaled(Register rd, Register rb, Register rx, int32_t s);                                     \
    inline void LDRB(Register rd, Register rn, int32_t off);                                                      \
    inline void LDRSB(Register rd, Register rn, int32_t off);                                                     \
    inline void LDRH(Register rd, Register rn, int32_t off);                                                      \
    inline void LDRSH(Register rd, Register rn, int32_t off);                                                     \
    inline void STR(Register rd, Register rn, int32_t off);                                                       \
    inline void STRB(Register rd, Register rn, int32_t off);                                                      \
    inline void STRH(Register rd, Register rn, int32_t off);                                                      \
    inline void PUSHr(Register r);                                                                                \
    inline void PUSH_mask(RegisterMask mask);                                                                     \
    inline void POPr(Register r);                                                                                 \
    inline void POP_mask(RegisterMask mask);                                                                      \
    inline void BKPT(uint32_t id);                                                                                \
    inline void B(NIns* addr);                                                                                    \
    inline void LDR_PC_longbranch(NIns* addr);                                                                    \
    inline void Bcc(ConditionCode cc, NIns* addr);                                                                \
    inline void Bcc_short(ConditionCode cc, NIns* addr);                                                          \
    inline void BXr(Register r);                                                                                  \
    inline void BLi(NIns* addr);                                                                                  \
    inline void BLXi(NIns* addr);                                                                                 \
    inline void BLXr(Register r);                                                                                 \
    inline void SET(Register r, ConditionCode cond);                                                              \
    inline void CMPr_asr(Register rl, Register rr, int32_t s);                                                    \
    void MOV(Register rd, Register rs);                                                                           \
                                                                                                                  \
    DECLARE_PLATFORM_ASSEMBLER_DEBUG()                                                                            \
                                                                                                                  \
    void        BranchWithLink(uintptr_t addr);                                                                   \
    void        JMP_far(NIns*);                                                                                   \
    bool        insnsMustBeContiguous;                                                                            \
    void        B_cond(ConditionCode, NIns*);                                                                     \
    void        underrunProtect(int bytes);                                                                       \
    void        nativePageReset();                                                                                \
    void        nativePageSetup();                                                                                \
    bool        hardenNopInsertion(const Config& /*c*/) { return false; }                                         \
    void        asm_immd(Register, int32_t, int32_t);                                                             \
    void        asm_regarg(ArgType, LIns*, Register);                                                             \
    void        asm_stkarg(LIns* p, int stkd);                                                                    \
    void        asm_pushstate();                                                                                  \
    void        asm_popstate();                                                                                   \
    void        asm_savepc();                                                                                     \
    void        asm_restorepc();                                                                                  \
    void        asm_discardpc();                                                                                  \
    void        asm_cmpi(Register, int32_t imm);                                                                  \
    void        asm_ldr(Register d, Register b, int32_t off);                                                     \
    int32_t     asm_str(Register rt, Register rr, int32_t off);                                                   \
    void        asm_cmp(LIns *cond);                                                                              \
    void        asm_cmpd(LIns *cond);                                                                             \
    void        asm_ld_imm(Register d, int32_t imm);                                                              \
    void        asm_arg(ArgType ty, LIns* arg, ParameterRegisters& params);                                       \
    void        asm_arg_float(LIns* arg, ParameterRegisters& params);                                             \
    void        asm_add_imm(Register rd, Register rn, int32_t imm, int stat = 0);                                 \
    void        asm_sub_imm(Register rd, Register rn, int32_t imm, int stat = 0);                                 \
    void        asm_and_imm(Register rd, Register rn, int32_t imm, int stat = 0);                                 \
    void        asm_orr_imm(Register rd, Register rn, int32_t imm, int stat = 0);                                 \
    void        asm_eor_imm(Register rd, Register rn, int32_t imm, int stat = 0);                                 \
    inline bool encThumb2Imm(uint32_t literal, uint32_t * enc);                                                   \
    inline uint32_t CountLeadingZeroes(uint32_t data);                                                            \
    void        asm_immf(Register, int32_t);                                                                      \
    void        asm_cmpf4(LIns *cond);                                                                            \
    Register    getAvailableReg(LIns* ins, Register regClass, RegisterMask m)                                     \
                        { return _allocator.getAvailableReg(ins, regClass, m); }                                  \
    int         max_out_args; /* bytes */                                                                         \
                                                                                                                  \
    void        emitT16(NIns ins);                                                                                \
    void        emitT32(NIns32 t32) ;                                                                             \
    void        emitImm(int32_t imm);                                                                             \
    void        emitPC(NIns* ptr);                                                                                \
                                                                                                                  \
    void        emitT32At(NIns* at, NIns32 t32);                                                                  \
    void        emitAt(NIns* at, NIns ins);                                                                       \
    void        emitImmAt(NIns* at, int32_t imm);                                                                 \
    void        emitPCAt(NIns* at, NIns* ptr);                                                                    \
                                                                                                                  \
    inline void FMDRR(Register Dm, Register _Rd, Register _Rn);                                                   \
    inline void FMRRD(Register _Rd, Register _Rn, Register _Dm);                                                  \
    inline void FMRDH(Register _Rd, Register _Dn);                                                                \
    inline void FMRDL(Register _Rd, Register _Dn);                                                                \
    inline void FLDD(Register _Dd, Register _Rn, int32_t _offs);                                                  \
    inline void FLDS(Register _Sd, Register _Rn, int32_t _offs);                                                  \
    inline void VLDR(Register _Dd, Register _Rn, int32_t _offs);                                                  \
    inline void VSTR(Register _Dd, Register _Rn, int32_t _offs);                                                  \
    inline void VLDQR(Register _Qd, Register _Rn);                                                                \
    inline void VSTQR(Register _Qd, Register _Rn);                                                                \
    inline void FSTS(Register _Dd, Register _Rn, int32_t _offs);                                                  \
    inline void FSTD(Register _Dd, Register _Rn, int32_t _offs);                                                  \
    inline void VNEGvfp(Register _Dd, Register _Dm);                                                              \
    inline void VNEGneon(Register _Qd, Register _Qm);                                                             \
    inline void GenericVfp(const char* NAME, Register _Dd,Register _Dn, Register _Dm, uint32_t SPECIFIC_ENC);     \
    inline void GenericNeon(const char* NAME, Register _Qd, Register _Qn, Register _Qm, uint32_t SPECIFIC_ENC);   \
    inline void VADDvfp(Register _Dd, Register _Dn, Register _Dm);                                                \
    inline void VSUBvfp(Register _Dd, Register _Dn, Register _Dm);                                                \
    inline void VMULvfp(Register _Dd, Register _Dn, Register _Dm);                                                \
    inline void VDIVvfp(Register _Dd, Register _Dn, Register _Dm);                                                \
    inline void VADDneon(Register _Qd, Register _Qn, Register _Qm);                                               \
    inline void VSUBneon(Register _Qd, Register _Qn, Register _Qm);                                               \
    inline void VMULneon(Register _Qd, Register _Qn, Register _Qm);                                               \
    inline void VDIVneon(Register _Qd, Register _Qn, Register _Qm);                                               \
    inline void VADD(Register _Dd, Register _Dn, Register _Dm);                                                   \
    inline void VSUB(Register _Dd, Register _Dn, Register _Dm);                                                   \
    inline void VMUL(Register _Dd, Register _Dn, Register _Dm);                                                   \
    inline void VDIV(Register _Dd, Register _Dn, Register _Dm);                                                   \
    inline void VNEG(Register _Dd, Register _Dm);                                                                 \
    inline void VCEQ(Register _Qd, Register _Qn, Register _Qm);                                                   \
    inline void VMOVN(Register _Dd, Register _Qm);                                                                \
    inline void VMVN(Register _Dd, Register _Dm);                                                                 \
    inline void VDUP(Register _Qd, Register _Sm);                                                                 \
    inline void FMSTAT();                                                                                         \
    inline void VCMP(Register _Dd, Register _Dm, int32_t _E);                                                     \
    inline void VCMPZ(Register _Dd);                                                                              \
    inline void VMOV(Register _Dd, Register _Dm);                                                                 \
    inline void FCPYS(Register _Sd, Register _Sm);                                                                \
    inline void FCPYD(Register _Dd, Register _Dm);                                                                \
    inline void FCPYD_cond();                                                                                     \
    inline void FCPYQ(Register _Qd, Register _Qm);                                                                \
    inline void FMSR(Register _Sn, Register _Rd);                                                                 \
    inline void FMRS(Register _Rd, Register _Sn);                                                                 \
    inline void VCVT(const char* name, Register _Rd, Register _Rm, uint32_t SPECIFIC_ENC, bool check_dst = true); \
    inline void FCVTDS(Register _Dd, Register _Sm);                                                               \
    inline void FCVTSD(Register _Sd, Register _Dm);                                                               \
    inline void FSITOD(Register _Dd, Register _Sm);                                                               \
    inline void FUITOD(Register _Dd, Register _Sm);                                                               \
    inline void FSITOS(Register _Sd, Register _Sm);                                                               \
    inline void FUITOS(Register _Sd, Register _Sm);                                                               \
    inline void FTOSID(Register _Sd, Register _Dm);                                                               \
    inline void FTOSIS(Register _Sd, Register _Sm);


// Required by target-independent code in Assembler.cpp.
// These really should be renamed using the asm_XXX convention.
#define MR(d, s)    MOV(d, s)
#define JMP(t)      B_cond(AL, t)

#define IMM32(imm)  *(--_nIns) = (NIns)((imm));

#define OP_IMM  (1<<25)
#define OP_STAT (1<<20)

#define COND_AL ((uint32_t)AL<<28)

//### FIXME: Verify this is still correct.
#define LD32_size 8
#define LDR_PC_size 8

#define BEGIN_NATIVE_CODE(x)                    \
    { DWORD* _nIns = (uint8_t*)x

#define END_NATIVE_CODE(x)                      \
    (x) = (dictwordp*)_nIns; }


// PC always points to current instruction + 4, so when calculating pc-relative
// offsets, use PC+4.  (Thumb only, offset is PC+8 for classic ARM.)
#define PC_OFFSET_FROM(target, frompc) ((intptr_t)(target) - ((intptr_t)(frompc) + 4))

} // namespace nanojit
#endif // __nanojit_NativeThumb2__
