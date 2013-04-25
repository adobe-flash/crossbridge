/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __nanojit_RegAlloc__
#define __nanojit_RegAlloc__


namespace nanojit
{
    class Assembler;

#define IS_REG_IN_MASK(r,m)   (((m) & rmask(r)) != 0)
#define IS_MASK_IN_MASK(rm,m)  (((m) & rm) == rm)
#define SET_REG_IN_MASK(r,m)  m |= rmask(r)
#define SET_MASK_IN_MASK(rm,m) m |= rm
#define CLR_REG_IN_MASK(r,m)  m &= ~ rmask(r)
#define CLR_MASK_IN_MASK(rm,m) m &= ~rm
#ifndef msReg
// Return the highest numbered Register in mask.
inline Register msReg(uint32_t mask) { Register r = { msbSet32(mask) }; return r; }
inline Register msReg(uint64_t mask) { Register r = { msbSet64(mask) }; return r; }

// Return the lowest numbered Register in mask.
inline Register lsReg(uint32_t mask) { Register r = { lsbSet32(mask) }; return r; }
inline Register lsReg(uint64_t mask) { Register r = { lsbSet64(mask) }; return r; }
#endif
// Clear bit r in mask, then return lsReg(mask).
inline Register nextLsReg(RegisterMask& mask,Register r) 
{
    CLR_REG_IN_MASK(r,mask);
    return lsReg(mask);
}
// Clear bit r in mask, then return msReg(mask).
inline Register nextMsReg(RegisterMask& mask,Register r) 
{
    CLR_REG_IN_MASK(r,mask);
    return msReg(mask);
}

    // Some basics on RegAlloc :
    //
    // - 'active' indicates which registers are active at a particular
    //   point, and for each active register, which instruction
    //   defines the value it holds.  At the start of register
    //   allocation no registers are active.
    //
    // - 'free' indicates which registers are free at a particular point
    //   and thus available for use.  At the start of register
    //   allocation most registers are free;  those that are not
    //   aren't available for general use, e.g. the stack pointer and
    //   frame pointer registers.
    //
    // - 'managed' is exactly this list of initially free registers,
    //   ie. the registers managed by the register allocator.
    //
    // - Each LIns has a "reservation" which includes a register value,
    //   'reg'.  Combined with 'active', this provides a two-way
    //   mapping between registers and LIR instructions.
    //
    // - Invariant 1: each register must be in exactly one of the
    //   following states at all times:  unmanaged, free, or active.
    //   In terms of the relevant fields:
    //
    //   * A register in 'managed' must be in 'active' or 'free' but
    //     not both.
    //
    //   * A register not in 'managed' must be in neither 'active' nor
    //     'free'.
    //
    // - Invariant 2: the two-way mapping between active registers and
    //   their defining instructions must always hold in both
    //   directions and be unambiguous.  More specifically:
    //
    //   * An LIns can appear at most once in 'active'.
    //
    //   * An LIns named by 'active[R]' must have an in-use
    //     reservation that names R.
    //
    //   * And vice versa:  an LIns with an in-use reservation that
    //     names R must be named by 'active[R]'.
    //
    //   * If an LIns's reservation names 'deprecated_UnknownReg' then LIns
    //     should not be in 'active'.
    //
    //  Invariant 2': On overlapping-registers RA,the two-way mapping between
    //  active registers & defining instructions is extended to all the subregisters.
    //  More specifically:
    //
    //   * An LIns can appear more than once in 'active'. If it appears in active[r]
    //   it must appear in all active[r1] where r1 is a subregister of r; and must not
    //   appear in active[r2] where there is no relationship between r and r2.
    //
    //   * An LIns named by 'active[R]' must have an in-use reservation
    //     that names R, or a parent of R
    //
    //   * And vice versa:  an LIns with an in-use reservation that
    //     names R must be named by 'active[R]', and also in all 'active[R1]' 
    //     where R1 is a subregister of R.
    //
    //   * In overlapping-registers allocators, a reservation should never reference
    //   deprecated_UnknownReg' 
    class RegAlloc
    {
    public:
                            RegAlloc()                    { VMPI_memset(this, 0, sizeof(*this)); }
        void                initialize(Assembler* a);

        RegisterMask        getManagedSet() const         { return _managed; } 
        bool                isFree(Register r) const      { return IS_REG_IN_MASK(r,_free); }
        int32_t             getPriority(Register r);

        // Return a mask containing the active registers.  For each register
        // in this set, getActive(register) will be a nonzero LIns pointer.
        RegisterMask        activeMask() const            { return ~_free  & _managed; }
        LIns*               getActive(Register r) const   { return _active[REGNUM(r)];}
        void                useActive(Register r);

        // Ins: instruction that gets the register; allow - the allowed set of (sub)registers
        // regClass: a hint as to what register is needed - useful when allocating temporary registers,
        // where the instruction itself provides no clue as to what kind of register is desired
        Register            allocReg(LIns* ins, RegisterMask allow, Register regClass = UnspecifiedReg );
        // Allocates temporary register; see also the implementation notes
        Register            allocTempReg(RegisterMask allow, Register regClass = UnspecifiedReg);   
        // Allocates temporary register; see also the implementation notes
        Register            allocTempRegIfAvailable(RegisterMask allow, Register regClass = UnspecifiedReg);   
        // Used to assign a register to an instruction
        Register            allocSpecificReg(LIns* ins, Register r);
        // The reverse of "allocSpecificReg" - signals ending the use of r
        void                retire(Register r);

        // Rematerializable instructions don't have to be saved & reloaded to spill,
        // they can just be recalculated cheaply.
        //
        // WARNING: this function must match asm_restore() -- it should return
        // true for the instructions that are handled explicitly without a spill
        // in asm_restore(), and false otherwise.
        //
        // If it doesn't match asm_restore(), the register allocator's decisions
        // about which values to evict will be suboptimal.
        static bool canRemat(LIns*);

        debug_only( bool    isConsistent(Register r, LIns* v) const; )
    private:
        LIns*           _active[LastRegNum + 1]; // active[REGNUM(r)] = LIns that defines r
        int32_t         _usepri[LastRegNum + 1]; // used priority. lower = more likely to spill.
        RegisterMask    _free;                   // Registers currently free.
        RegisterMask    _managed;                // Registers under management (invariant).
        int32_t         _priority;
        Assembler*      _assembler;              // the assembler that initialized this RegAlloc

        LIns* findVictim( RegisterMask allow, LIns* forIns = NULL, Register regClass = UnspecifiedReg );

    public:
        // Platform-specific methods
        RegisterMask nInitManagedRegisters();
        RegisterMask nRegCopyCandidates(Register r, RegisterMask allow);
        RegisterMask nHint(LIns* ins);

        // Callee-saved regs; we allocate an extra element in case NumSavedRegs == 0
        const static Register savedRegs[NumSavedRegs+1]; 
         // A special value for the _nHints[] array;  if an opcode has this value, we call
         // nHint() in the back-end.  Used for cases where you need to look at more
         // than just the opcode to decide.
        const static RegisterMask PREFER_SPECIAL = ~ ((RegisterMask)0);
;
        DECLARE_PLATFORM_REGALLOC();
    };

    /************** Inline method implementations for class RegAlloc  ********************/

    inline void RegAlloc::useActive(Register r)
    {
        NanoAssert(_active[REGNUM(r)] != NULL);
        _usepri[REGNUM(r)] = _priority++;
    }

    /* r must be: 
       - In use in one instruction
       - All subregisters in use by same instruction
       - No subregisters free (it shouldn't be free either)
       */
    inline void RegAlloc::retire(Register r)
    {
        NanoAssert(_active[REGNUM(r)] != NULL);
#if RA_REGISTERS_OVERLAP
        LIns* ins = _active[REGNUM(r)];
        r = ins->getReg(); // get "father" register

        RegisterMask rm = rmask(r);
        bool handle_r_separately = true;
        for (Register r1 = lsReg(rm); rm; r1 = nextLsReg(rm, r1))
        {
            NanoAssert(_active[REGNUM(r1)] == ins);
            _active[REGNUM(r1)] = NULL;
            if(r==r1) handle_r_separately = false;
        }

        if(handle_r_separately)
#endif
            _active[REGNUM(r)] = NULL;

        NanoAssert(IS_MASK_IN_MASK(rmask(r),~_free));
        SET_REG_IN_MASK(r,_free);
    }

    inline int32_t RegAlloc::getPriority(Register r) 
    {
        NanoAssert(_active[REGNUM(r)]);
        return _usepri[REGNUM(r)];
    }

/* Here we select how to pick a register from a register set; the platform may declare its 
   preference (lsreg, msreg), or may completely implement a cistp, method */

#if    defined(RA_PREFERS_MSREG) && !defined(firstAvailableReg)
#define firstAvailableReg(ins,regClass,mask) msReg(mask)
#elif defined(RA_PREFERS_LSREG) && !defined(firstAvailableReg)
#define firstAvailableReg(ins,regClass,mask) lsReg(mask)
#elif !defined(firstAvailableReg)
#error You must either define RA_PREFERS_MSREG or RA_PREFERS_LSREG, or define firstAvailableReg yourself.
#endif

    // Finds a register in 'allow' to store a temporary value (one not
    // associated with a particular LIns), evicting one if necessary.  The
    // returned register will be marked as being free immediately, it is the caller's
    // responsibility to make sure that the regstate is not inspected or updated
    // while the temp reg is still in use. 
    inline Register RegAlloc::allocTempReg(RegisterMask allow, Register regClass)
    {
        LIns dummyIns; 
        dummyIns.initLInsOp0(LIR_label);
        Register r = allocReg(&dummyIns, allow, regClass);
        retire(r);
        return r;
     }
    // Finds a register in 'allow' to store a temporary value (one not associated with a 
    // particular LIns). Never evicts instructions. Returns UnspecifiedRegister on failure.
    // PARTIAL IMPLEMENTATION. ONLY WORKS ON GPRs. ASSUMES GPRs ARE NON-OVERLAPPING.
    inline Register RegAlloc::allocTempRegIfAvailable(RegisterMask allow, Register regClass){
        NanoAssert( (allow & GpRegs) == allow); (void) regClass;
        RegisterMask candidates = allow & _free;
        if(candidates != 0){
            return firstAvailableReg(NULL, regClass,candidates);
        } else 
            return UnspecifiedReg;
    }     
}
#endif // __nanojit_RegAlloc__
