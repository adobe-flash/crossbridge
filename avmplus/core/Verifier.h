/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Verifier__
#define __avmplus_Verifier__

namespace avmplus
{
    /**
     * We are simulating the effect that each instruction has on the stack
     * frame state.  Type information originates from the following sources:
     *    - formal parameter types of the curent method
     *    - traits of objects we manipulate
     *    - types of slots in traits
     *    - return values of methods we call
     *    - push (constant) instructions
     *    - check (cast) instructions
     *    - results of opcodes that produce known types
     *
     * When we have branch instructions, we merge the frame state at the end
     * of the block with the starting frame state of the target block.
     * incompatible frame states cause verify errors.
     */

#ifdef VMCFG_RESTARG_OPTIMIZATION
    // Helper used for optimization of ...rest parameters and the 'arguments' array.
    //
    // The analysis is virtually the same for the two cases; for 'rest array'
    // read 'rest array or arguments array' except as noted.
    //
    // We recognize benign uses of the rest array and rewrite generic code
    // sequences to instead use new instructions RESTARGC and RESTARG, and set
    // the _lazyRest attribute.  The optimization kicks in if the rest array
    // is not closed over and all uses of the rest array are to extract its
    // length or extract a property from it.  Even then the RESTARGC/RESTARG
    // instructions may fall back to the full array, if the extracted property
    // names are not integer values in the range of the actual argument count.
    //
    // The RESTARGC/RESTARG instructions are only valid if the excess arguments
    // are all represented as atoms.  That's always true for ...rest arguments,
    // but it's only true for the 'arguments' object if all fixed and optional
    // arguments are untyped, so the analysis must be guarded on that.  (It is
    // possible to do better, maybe, but the 'arguments' object is supported for
    // the sake of JS compatibility and in that case the arguments will usually
    // be untyped.  We can expect typed arguments to be used with ...rest.)
    //
    // See Bugzilla 569321 and 571469 for discussion, further work, etc.
    //
    // Recognition is based on a lightweight escape analysis during pass1,
    // implemented by RestArgAnalyzer.  (The analysis is repeated during pass2
    // to drive optimized code generation.)
    //
    // Analysis:
    //
    // We aim to determine whether the 'rest array' is used in only two ways:
    //
    // - to access the 'length' property with either the public namespace or a public
    //   namespace in the namespace set
    //
    // - to access an unknown property with either the public namespace or a public
    //   namespace in the namespace set
    //
    // If those are the only uses of the rest array then the rest array is eligible
    // for a speculative optimization that delays or avoids its construction.
    //
    // We say the analysis "fails" if the optimization is rejected because we can't
    // prove that the rest array does not escape.
    //
    // A debugger causes the array to escape; the analysis fails if a debugger is
    // present.
    //
    // The rest array is stored in a distinguished local slot in the ABC code, and we
    // know which slot this is.  We assume ASC does not do something clever with the
    // array, eg, by moving it to a different local slot. (It is possible to do better.)
    //
    // The analysis needs to prove the following:
    //
    // * If the rest local is read by a GETLOCAL and the rest array value is produced,
    //   and if that value is consumed by an instruction (rather than being discarded
    //   by an exception) then that value must be consumed by a GETPROPERTY instruction
    //   of one of two forms:
    //
    //   - It reads a "length" property with a public namespace or a public namespace
    //     in its namespace set.
    //
    //   - It reads a property with a run-time name, with a public namespace or a
    //     public namespace in its namespace set.
    //
    // * If the rest local is read by any other instruction than a GETLOCAL, or is
    //   consumed or inspected by any other instruction than a GETPROPERTY of the
    //   two forms above, then the analysis fails.
    //
    // The analysis is fairly weak but handles code that occurs in practice.
    //
    // A bit on each value in the FrameState, isRestArray, tracks whether a value is a
    // rest array.  This bit is set by GETLOCAL.  The bit is checked by every other
    // instruction: the operands to the instruction are checked.  If the bit is set
    // but the instruction is not a GETPROPERTY of the correct form then the analysis
    // fails.  (This means the bit does not propagate with the value as the value is
    // duplicated, moved into locals, etc.)
    //
    // The analysis checks the values that remain in the FrameState at block
    // boundaries.  If isRestArray is set on any value, the analysis fails.  (It is
    // possible to do better but not necessary to do so at this point.)

    class RestArgAnalyzer : public NullWriter
    {
    public:
        RestArgAnalyzer();
        ~RestArgAnalyzer();

        void init(AvmCore* core, MethodInfo* info, uint32_t frameSize);

        // Insert this CodeWriter into the pipeline before next, or not.  On pass2,
        // set the _lazyRest flag on the MethodInfo if appropriate.
        CodeWriter* hookup(CodeWriter* next, bool pass2=false);

        void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type);
        void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits* type);
        void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
        void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type);
        void writeOpcodeVerified(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);

        bool getProperty(const FrameState* state, const Multiname& multiname, int obj_offset);

        bool optimize;

    private:
        AvmCore* core;
        MethodInfo* info;
        PoolObject* pool;
        uint32_t frameSize;
        uint32_t restVar;
        bool *isRestArray;

        void operate(const FrameState* state, const uint8_t *pc, AbcOpcode opcode);
        void endBlock();
        void fail();
    };
#endif // VMCFG_RESTARG_OPTIMIZATION

    class Verifier: public CodegenDriver
    {
        friend class ScopeWriter;
    public:

        Verifier(MethodInfo *info, MethodSignaturep ms,
                 Toplevel* toplevel, AbcEnv* abc_env
#ifdef AVMPLUS_VERBOSE
            , bool secondTry=false
#endif
            );
        virtual ~Verifier();

        /**
         * Verifies the method specified by info.  If verification fails,
         * an exception will be thrown, of type VerifyError.
         * @param info the method to verify
         */
        void verify(CodeWriter* coder);

    public:
        // CodegenDriver interface
        const uint8_t* getTryFrom() const;
        const uint8_t* getTryTo() const;
        bool hasFrameState(const uint8_t* pc) const;
        int getBlockCount() const;
        bool hasReachableExceptions() const;
        FrameState* getFrameState(const uint8_t* pc) const;

    private:
        const uint8_t* code_pos;  // start of abc bytecode
        int code_length;
        const uint8_t* tryFrom;   // start of earliest try region
        const uint8_t* tryTo;     // end of latest try region
        MethodInfo *info;
        const MethodSignaturep ms;
        PoolObject *pool;
        CodeWriter* coder;
        FrameState *worklist;
        Toplevel* toplevel;
        AbcEnv*   abc_env;
        typedef HeapSortedMap< UnmanagedPointerList<const uint8_t*>, UnmanagedPointerList<FrameState*> > BlockStatesType;
        BlockStatesType *blockStates;
        FrameState *state;
        bool emitPass;
        bool handlerIsReachable;
        AvmCore *core;
#ifdef VMCFG_RESTARG_OPTIMIZATION
        RestArgAnalyzer restArgAnalyzer;
#endif

        void checkFrameDefinition();
        const uint8_t* verifyBlock(const uint8_t* pc);
        const uint8_t* loadBlockState(FrameState* blk);
        void checkParams();
        FrameValue& checkLocal(int local);
        MethodInfo* checkMethodInfo(uint32_t method_id);
        Traits* checkClassInfo(uint32_t class_id);
        void checkTarget(const uint8_t* current, const uint8_t* target, bool isExceptionEdge = false);
        bool mergeState(FrameState*);
        void checkStringOperand(uint32_t index);
        void checkNameOperand(uint32_t index);
        void checkConstantMultiname(uint32_t index, Multiname &m);
        Traits* checkSlot(Traits* traits, int slot_id);
    public:
        static Traits* findCommonBase(Traits* t1, Traits* t2);
    private:
        void checkEarlySlotBinding(Traits* traits);
        Traits* peekType(Traits* requiredType, int n=1);
        Traits* emitCoerceSuper(int index);
        void checkCallMultiname(AbcOpcode opcode, Multiname* multiname) const;
        void checkPropertyMultiname(uint32_t &depth, Multiname& multiname);
        void parseBodyHeader();
        void checkStack(uint32_t pop, uint32_t push);
        void checkStackMulti(uint32_t pop, uint32_t push, Multiname* m);
        void emitFindProperty(AbcOpcode opcode, Multiname& multiname, uint32_t imm30, const uint8_t *pc);
        void emitGetProperty(Multiname &multiname, int n, uint32_t imm30, const uint8_t *pc);
        Traits* checkGetGlobalScope();
        void emitNip();

        void emitCallproperty(AbcOpcode opcode, int& sp, Multiname& multiname, uint32_t multiname_index, uint32_t argc, const uint8_t* pc);
        bool emitCallpropertyMethod(AbcOpcode opcode, Traits* t, Binding b, Multiname& multiname, uint32_t multiname_index, uint32_t argc, const uint8_t* pc);
        bool emitCallpropertySlot(AbcOpcode opcode, int& sp, Traits* t, Binding b, uint32_t argc, const uint8_t *pc);
#ifdef VMCFG_WORDCODE
        uint32_t allocateCacheSlot(uint32_t imm30);
#endif
        Binding findMathFunction(TraitsBindingsp math, const Multiname& name, Binding b, int argc);
        Binding findStringFunction(TraitsBindingsp string, const Multiname& name, Binding b, int argc);
        void parseExceptionHandlers();
        void emitCoerceArgs(MethodInfo* m, int argc);
        void emitCoerce(Traits* target, int i);
        void emitCoerceToNumeric(int i);
        void emitCheckNull(int index);
        Traits* checkTypeName(uint32_t name_index);
        Traits* readBinding(Traits* traits, Binding b) const;
        void verifyFailed(int errorID, Stringp a1=0, Stringp a2=0, Stringp a3=0) const;

#ifdef AVMPLUS_VERBOSE
    public:
        bool verbose;
        bool secondTry;
        void printScope(const char* title, const ScopeTypeChain*);
        void printState(StringBuffer& prefix, FrameState* state);
        void printOpcode(const uint8_t* pc, const uint8_t* code_end);
    private:
        void printValue(FrameValue& v);
#endif
    };
}

#endif /* __avmplus_Verifier__ */
