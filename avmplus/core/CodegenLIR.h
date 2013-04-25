/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_CodegenLIR__
#define __avmplus_CodegenLIR__

#include "LirHelper.h"
#include "InvokerCompiler.h"

namespace avmplus
{
    using namespace nanojit;

   /**
    * Each InEdge record tracks an unpatched branch.  When the target code
    * is generated in emitLabel(), we patch each tracked instruction or
    * jtbl entry.
    */
    struct InEdge {
        LIns *branchIns;        // the branch instruction that needs patching
        uint32_t index;         // if br is a jtbl, which index in table to patch
        InEdge(LIns *br) : branchIns(br), index(0) {}
        InEdge(LIns *jtbl, uint32_t index) : branchIns(jtbl), index(index) {}
    };

    /**
     * CodegenLabel: information about a LIR label that hasn't been generated yet.
     * Once code at the label is generated, we fill in bb.  Later, we'll patch
     * branch instructions to point to this bb.  We have one of
     * these for each verifier FrameState.
     */
    class CodegenLabel {
    public:
        LIns *labelIns;                 // the LIR_label instruction for this code position
        nanojit::BitSet *notnull;       // merged nullability information for vars at this label
        Seq<InEdge> *unpatchedEdges;    // branches to this label that need patching
        LIns** varTracker;              // merged var tracker state at this label
        LIns** tagTracker;              // merged tag tracker state at this label
#ifdef NJ_VERBOSE
        const char* name;
        CodegenLabel() : labelIns(0), notnull(0), unpatchedEdges(0), varTracker(0), tagTracker(0), name(0)
        {}
        CodegenLabel(const char* name) : labelIns(0), notnull(0), unpatchedEdges(0), varTracker(0), tagTracker(0), name(name)
        {}
#else
        CodegenLabel() : labelIns(0), notnull(0), unpatchedEdges(0), varTracker(0), tagTracker(0)
        {}
        CodegenLabel(const char*) : labelIns(0), notnull(0), unpatchedEdges(0), varTracker(0), tagTracker(0)
        {}
#endif

    };

    /**
     * The JITNoise class implements
     */
    class JITNoise : public nanojit::Noise {
    public :
        JITNoise();
        // produce a random number from 0-maxValue for the JIT to use in attack mitigation
        uint32_t getValue(uint32_t maxValue);
    private:
        TRandomFast randomSeed;
    };

    // Binding Cache Design
    //
    // When we don't know the type of the base object at a point we access a property,
    // we must look up the property at runtime./ We cache the results of this lookup
    // in a BindingCache instance, and install a handler specialized for the object
    // type and binding kind. Each BindingCache consists of:
    //
    //    call_handler      initially, points to callprop_miss(), later points
    //                      to the specialized handler
    //
    //    vtable or tag     VTable* or Atom tag for the base object.  specialized
    //                      handlers quickly check this and "miss" when they dont match
    //                      the object seen at runtime.
    //
    //    slot_offset or    precomputed offset for a slot, for fast loads, or a preloaded
    //    method            MethodEnv* for a method, for fast calls
    //
    //    name              Multiname* for this dynamic access.  Used on a cache miss,
    //                      and also for handlers that access dynamic properties.
    //
    // If jit'd code for a method needs cache entries, they are allocated and
    // saved on MethodInfo::_abc.call_cache. The table is only allocated when jit
    // compilation was successful, so jit'd code must load this pointer at runtime
    // since we don't have the address at compile time.
    //
    // cache instances are in unmanaged memory, so VTable and MethodEnv pointers
    // are effectively weak references: gc-visible pointers must exist elsewhere,
    // and the cache doesn't cause them to be pinned.
    //
    // we allocate one entry for each unique Multiname in each method (only at
    // late-bound reference sites, of course).
    //
    // Limitations:
    //
    //   * we only use a binding cache if the name has no runtime parts.
    //   * only implemented for OP_callproperty OP_callproplex, OP_getproperty,
    //     and OP_setproperty
    //   * only common cases observed in testing are handled, others use a generic
    //     handler that's slightly slower than not using a cache at all.
    //     see callprop_miss() in jit-calls.h for detail on handled cases.
    //
    // Alternatives that led to current design:
    //
    //   * specializing slot accessors on slot type, and storing slot_offset,
    //     avoids calling ScriptObject::getSlotAtom()  (15% faster for gets & calls)
    //   * decl_type:  when an object type doesn't match the cached type, sometimes the
    //     binding doesn't change.  for example calling a method declared on
    //     a base class, on many different subclasses that don't override the
    //     method.  the subsequent misses can be handled faster if we saved the method_id,
    //     and checked that the actual type & cached type are related by inheritance.
    //     This doesn't help for interface methods and the fast case didn't occur often
    //     enough for the extra cache size and code complexity.
    //   * vtable: When a single type (Traits*) is used in different environments we'll
    //     have distinct VTable*'s that point to the same Traits*.  the cache might hit
    //     more often if we stored Traits*.  however, specializing the cache on VTable*
    //     lest us pre-load the MethodEnv* for calls, saving one load.  also, comparing
    //     vtable pointers instead of traits pointers saves one more load. (5% median speedup)
    //   * we pass the base object to CallCache::Handler instead of accessing it indirectly
    //     via args[], to save a load on the fast path.  Worth 2-3%, and enables the same
    //     code to handle OP_callproperty and OP_callproplex.
    //   * we put Multiname* in the cache instead of passing it as a constant parameter,
    //     because the increase in cache size is smaller than the savings in code size,
    //     less parameters is faster, and the multiname is only used on relatively slow paths.
    //   * we allocate cache instances while generating code, which lets us embed
    //     the cache address in code instead of loading from methodinfo.  some stats on ESC:
    //        call cache entries: 489
    //        bytes of call cache entries: 7,824
    //        instructions saved by early allocation (static): 1,445 (loads, movs, adds)
    //        MethodInfo instances: 2436
    //        bytes saved by eliminating 2 cache pointers on MethodInfo: 19,448
    //
    //   (footnote: Times are from the tests in tamarin/test/performance as well as a
    //   selection of Flash/Flex benchmark apps)
    //
    // Alternatives not yet investigated:
    //
    //   * we could put the whole Multiname in the cache, and maybe eliminate
    //     PoolObject::precomputedMultinames[].  Not all multinames are used in late bound
    //     call sites, so this could save some memory.  precomputed multinames have been
    //     measured at over 10% of code size.  Need to study cache size increase vs pool
    //     memory decrease.  Later when we have a code cache, this could be more compelling.
    //   * we could specialize BKIND_METHOD handlers on method return type, enabling us
    //     to inline the native-value boxing logic from MethodEnv::endCoerce()
    //   * we could specialize getter & setter handlers on return/parameter type,
    //     allowing us to inline boxing & unboxing code from coerceEnter AND endCoerce
    //   * the MethodEnv* passed to each handler is only used on slow paths.  Could we
    //     put it somewhere else?  maybe core->currentMethodFrame->env?
    //   * on x86, FASTCALL might be faster, if it doesn't inhibit tail calls in handlers
    //   * other cache instance groupings:
    //       * one per call site instead of per-unique-multiname?
    //       * share them between methods?
    //   * handlers that access primitive dynamic properites have to call toplevel->toPrototype(val),
    //     maybe we could save the result and check its validity with (env->toplevel() == saved_proto->toplevel())
    //   * we could specialize on primitive type too, inlining just the toPrototype() path we need,
    //     instead of using a single set of handlers for all primitives types
    //   * OP_initproperty is rarely late bound in jit code; if this evidence changes
    //     then we should consider an inline cache for it.

    // binding cache common code
    class BindingCache {
    public:
        BindingCache(const Multiname*, BindingCache* next);
        union {
            VTable* vtable;         // for kObjectType receivers
            Atom tag;               // for primitive receivers
        };
        union {
            ptrdiff_t slot_offset;  // for gets of a slot
            MethodEnv* method;      // calls to a method
        };
        const Multiname* const name;      // multiname for this entry, saved when cache created.
        BindingCache* const next;         // singly-linked list
    };

    // cache for late bound calls
    class CallCache: public BindingCache {
    public:
        typedef Atom (*Handler)(CallCache&, Atom base, int argc, Atom* args, MethodEnv*);
        CallCache(const Multiname*, BindingCache* next);
        Handler call_handler;
    };

    // cache for late bound gets
    class GetCache: public BindingCache {
    public:
        typedef Atom (*Handler)(GetCache&, MethodEnv*, Atom);
        GetCache(const Multiname*, BindingCache* next);
        Handler get_handler;
    };

    // bind cache for sets.  This is necessarily larger than for gets because
    // an assignment implies a coercion, and the target type is an additional
    // cached parameter to the coercion
    class SetCache: public BindingCache {
    public:
        typedef void (*Handler)(SetCache&, Atom obj, Atom val, MethodEnv*);
        SetCache(const Multiname*, BindingCache* next);
        Handler set_handler;
        union {
            Traits* slot_type;  // slot or setter type, for implicit coercion
            GC* gc;             // saved GC* for set-handlers that call WBATOM
        };
    };

    /** helper class for allocating binding caches during jit compilation */
    template <class C>
    class CacheBuilder
    {
        SeqBuilder<C*> caches;   // each entry points to an initialized cache
        CodeMgr& codeMgr;                   // this allocator is used for new caches
        C* findCacheSlot(const Multiname*);
    public:
        /** each new entry will be initialized with the given handler.
         *  temp_alloc is the allocator for this cache builder, with short lifetime.
         *  codeMgr.allocator is used to allocate memory for the method's caches, with method lifetime */
        CacheBuilder(Allocator& builder_alloc, CodeMgr& codeMgr)
            : caches(builder_alloc), codeMgr(codeMgr) {}

        /** allocate a new cache slot or reuse an existing one with the same multiname */
        C* allocateCacheSlot(const Multiname* name);
    };

    class VarTracker;
    class MopsRangeCheckFilter;
    class PrologWriter;

    typedef HashMap<LIns*, nanojit::BitSet*> LabelBitSet;

    // We may replace an existing function call with another based on the
    // context in which it appears.  For example, if the result is to be
    // coerced to another type, there may be another function that directly
    // generates the same value.   For each such context, an array of
    // Specialization structures provides a mapping from the old function
    // to its replacement.

    struct Specialization {
        const CallInfo* oldFunc;
        const CallInfo* newFunc;
    };

    // A compact representation of the possible types of a numeric vector
    // index suitable for indexing a vector of accessor function ids.  This
    // is presently used only for an argument to CodegenLIR::emitGetIndexedProperty()
    // and CodegenLIR::emitSetIndexedProperty().
    typedef enum { VI_UINT, VI_INT, VI_DOUBLE, VI_SIZE } IndexKind;

    /**
     * CodegenLIR is a kitchen sink class containing all state for all passes
     * of the JIT.  It is intended to be instantiated on the stack once for each
     * jit-compiled method, and is a terminator of a CodeWriter pipeline.
     */
    class CodegenLIR : public LirHelper, public CodeWriter {
    public:

       #ifdef VTUNE
       bool hasDebugInfo;
       GCList<JITCodeInfo> jitInfoList;
       GCList<LineNumberRecord> jitPendingRecords;
       void jitPushInfo();
       JITCodeInfo* jitCurrentInfo();

       LineNumberRecord* jitAddRecord(uintptr_t pos, uint32_t filename, uint32_t line, bool pending=false);
       void jitFilenameUpdate(uint32_t filename);
       void jitLineNumUpdate(uint32_t line);
       void jitCodePosUpdate(uint32_t pos);
       #endif /* VTUNE */

    protected:
        friend struct JitInitVisitor;
        MethodInfo *info;
        const MethodSignaturep ms;
        Toplevel* toplevel;
        PoolObject *pool;
        OSR* osr;
        CodegenDriver* driver;
        const FrameState *state;
        const uint8_t* code_pos;
        const uint8_t* try_from;
        const uint8_t* try_to;
        MopsRangeCheckFilter* mopsRangeCheckFilter;
        LIns *vars, *tags;
        LIns *env_param, *argc_param, *ap_param;
        LIns *_save_eip, *_ef;
        LIns *methodFrame;
        LIns *csn;
        LIns *undefConst;
        LIns *restArgc; // NULL or the expression that computes the number of rest arguments
        int restLocal;  // -1 or the local var that holds the rest array
        bool interruptable;
        CodegenLabel npe_label;
        CodegenLabel upe_label;
        CodegenLabel interrupt_label;
        CodegenLabel mop_rangeCheckFailed_label;
        CodegenLabel catch_label;
        CodegenLabel call_error_label;
        const uint8_t* lastPcSave;
        LIns *setjmpResult;
        VarTracker *varTracker;
        int framesize;
#ifdef DEBUGGER
        int dbg_framesize; // count of locals & scopes visible to the debugger
#endif
        int labelCount;
        bool inlineFastpath; // Allow aggressive or speculative inlining, e.g., introducing new labels.
        CacheBuilder<CallCache> call_cache_builder;
        CacheBuilder<GetCache> get_cache_builder;
        CacheBuilder<SetCache> set_cache_builder;
        PrologWriter *prolog;
        LIns* skip_ins; // Skips from start of body to end of prologue.
        HashMap<LIns*, LIns*> *specializedCallHashMap;
        HashMap<uint32_t, uint32_t> *builtinFunctionOptimizerHashMap;
        HashMap<const uint8_t*, CodegenLabel*> *blockLabels;
        LirWriter* redirectWriter;
        CseFilter* cseFilter; // The CseFilter instance for this method, or NULL if none.
        JITNoise noise;
        verbose_only(VerboseWriter *vbWriter;)
        verbose_only(LInsPrinter* vbNames;)
        JITDebugInfo *jit_debug_info;

#ifdef DEBUGGER
        bool haveDebugger;
#else
        static const bool haveDebugger = false;
#endif
#if defined VMCFG_VTUNE
        static const bool haveVTune = true;
#else
        static const bool haveVTune = false;
#endif
#ifdef DEBUG
        /** jit_sst is an array of sst_mask bytes, used to double check that we
         *  are modeling storage types the same way the verifier did for us.
         *  Mismatches are caught in writeOpcodeVerified() after the Verifier has
         *  updated FrameValue.sst_mask. */
        uint16_t *jit_sst;   // array of SST masks to sanity check with FrameState
        ValidateWriter* validate3; // ValidateWriter for method body.
#endif

        LIns* insAlloc(int32_t);
        LIns* insAllocForTraits(Traits *t);
        LIns* atomToNativeRep(int loc, LIns *i);
        LIns* atomToNativeRep(Traits *, LIns *i);
        LIns* ptrToNativeRep(Traits*, LIns*);
        LIns* loadAtomRep(int i);
        LIns* localGet(int i);
        LIns* localGetp(int i);
        LIns* localGetd(int i);
        LIns* localGetf(int i); // Aborts if float not enabled
        LIns* localGetf4(int i);  // Aborts if float not enabled
        LIns* localGetf4Addr(int i);  // Aborts if float not enabled
        LIns* localCopy(int i); // sniff's type from FrameState
        void branchToLabel(LOpcode op, LIns *cond, CodegenLabel& label);
        LIns* branchJovToLabel(LOpcode op, LIns *a, LIns *b, CodegenLabel& label);
        void branchToAbcPos(LOpcode op, LIns *cond, const uint8_t* target);
        LIns* mopAddrToRangeCheckedRealAddrAndDisp(LIns* mopAddr, int32_t const size, int32_t* disp);
        LIns* loadEnvScope();
        LIns* loadEnvVTable();
        LIns* loadEnvAbcEnv();
        LIns* loadEnvDomainEnv();
        LIns* loadEnvToplevel();
        LIns* loadEnvFinddefTable();
        LIns* copyMultiname(const Multiname* multiname);
        LIns* initMultiname(const Multiname* multiname, int& csp, bool isDelete =false);
        LIns* storeAtomArgs(int count, int index);
        LIns* storeAtomArgs(LIns *obj, int count, int index);
        LIns* promoteNumberIns(Traits *t, int i);
#ifdef VMCFG_FLOAT
        LIns* promoteFloatIns(Traits *t, int i);
        LIns* promoteFloat4Ins(Traits *t, int i);
#endif // VMCFG_FLOAT
        LIns* loadVTable(LIns* obj, Traits* t);
        LIns* cmpEq(const CallInfo *fid, int lhsi, int rhsi);
        LIns* cmpLt(int lhsi, int rhsi);
        LIns* cmpLe(int lhsi, int rhsi);
        LIns* cmpOptimization(int lhsi, int rhsi, LOpcode icmp, LOpcode ucmp, LOpcode fcmp, bool strictOperation = false);
        debug_only( bool isPointer(int i); )
        void emitSetPc(const uint8_t* pc);
        void emitSampleCheck();
        bool verbose();
        CodegenLabel& getCodegenLabel(const uint8_t* pc);
        CodegenLabel& createLabel(const char *name);
        CodegenLabel& createLabel(const char *prefix, int id);
        void patchLater(LIns *br, CodegenLabel &);
        void patchLater(LIns *jtbl, const uint8_t* pc, uint32_t index);
        void emitLabel(CodegenLabel &l);
        void deadvars();
        void deadvars_analyze(Allocator& alloc,
                nanojit::BitSet& varlivein, LabelBitSet& varlabels,
                nanojit::BitSet& taglivein, LabelBitSet& taglabels);
        void deadvars_kill(Allocator& alloc,
                nanojit::BitSet& varlivein, LabelBitSet& varlabels,
                nanojit::BitSet& taglivein, LabelBitSet& taglabels);
        void copyParam(int i, int &offset);
        bool haveSSE2() const;

        /**
         * Overwrite the given instruction with a LIR_skip and adjust
         * skip_ins if it was pointing to the skipped instruction.
         */
        void eraseIns(LIns* ins, LIns* prevIns);

        LIns* loadIns(LOpcode op, int32_t disp, LIns *base, AccSet accSet, LoadQual loadQual=LOAD_NORMAL);
        LIns* storeIns(LOpcode op, LIns* val, int32_t disp, LIns *base, AccSet accSet);
        LIns* Ins(LOpcode op);
        LIns* Ins(LOpcode op, LIns *a);
        LIns* i2dIns(LIns* v);
        LIns* ui2dIns(LIns* v);
        LIns* callIns(const CallInfo *, uint32_t argc, ...);

#ifdef VMCFG_FLOAT
         /* Handle 1- and 2-operand numeric operations. Assumption: "number"/"int" is the "fastpath",
            the rest may be slow (should really be typed if the user wants speed)   */
        void emitNumericOp1(int32_t op1, const LIREmitter &emitIns);
        void emitNumericOp2(int32_t op1, int32_t op2, const LIREmitter& emitIns);
#endif // VMCFG_FLOAT

        /** emit a constructor call, and early bind if possible */
        void emitConstruct(int argc, LIns* ctor, Traits* ctraits);

        void emitCall(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodSignaturep);
        void emitCall(AbcOpcode opcode, intptr_t method_id, int argc, LIns* obj, Traits* objType, Traits* result, MethodSignaturep);

        /** Verifier has already coerced args and emitted the null check; JIT just double checks and emits code */
        void emitTypedCall(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodInfo*);

        /** emit a JIT-discovered early bound call.  JIT must coerce args and receiver to the proper type */
        void emitCoerceCall(AbcOpcode opcode, intptr_t method_id, int argc, MethodSignaturep mms);

        /** emit a JIT-discovered early bound call to newInstance and invoke the initializer function */
        void emitConstructCall(intptr_t method_id, int argc, LIns* ctor, Traits* ctraits);

        /** helper to coerce args to an early bound call to the required types */
        void coerceArgs(MethodSignaturep mms, int argc, int firstArg);

        void emit(AbcOpcode opcode, uintptr_t op1=0, uintptr_t op2=0, Traits* result=NULL);
        void emitIf(AbcOpcode opcode, const uint8_t* target, int lhs, int rhs);
        void emitSwap(int i, int j);
        void emitCopy(int src, int dest);
        void emitGetscope(int scope, int dest);
        void emitKill(int i);
#ifdef VMCFG_FASTPATH_ADD
#ifdef VMCFG_FASTPATH_ADD_INLINE
        void emitIntPlusAtomFastpath(int i, Traits* type, LIns* lhs, LIns* rhs, CodegenLabel &fallback);
#endif
        void emitAddIntToAtom(int i, int j, Traits* type);
        void emitAddDoubleToAtom(int i, int j, Traits* type);
        void emitAddAtomToInt(int i, int j, Traits* type);
        void emitAddAtomToDouble(int i, int j, Traits* type);
#endif
        void emitAddAtomToAtom(int i, int j, Traits* type);
        void emitAdd(int i, int j, Traits* type);
        void emitIntConst(int index, int32_t c, Traits* type);
        void emitPtrConst(int index, void* c, Traits* type);
        void emitDoubleConst(int index, const double* pd);
#ifdef VMCFG_FLOAT
        void emitFloatConst(int index, const float f);
        void emitFloat4Const(int index, const float4_t* pf4);
#endif // VMCFG_FLOAT
        void emitGetslot(int slot, int ptr_index, Traits *slotType);
        void emitSetslot(AbcOpcode opcode, int slot, int ptr_index);
        void emitSetslot(AbcOpcode opcode, int slot, int ptr_index, LIns* value);
        void emitGetGlobalScope(int dest);
        void emitCoerce(uint32_t index, Traits* type);
        void emitCheckNull(LIns* ptr, Traits* type);
        LIns* emitGetIndexedProperty(int objIndex, LIns *index, Traits *result, IndexKind idxKind);
        LIns* emitInlineVectorRead(int objIndexOnStack, 
                                   LIns* index,
                                   size_t arrayDataOffset, size_t lenOffset, size_t entriesOffset, int scale, LOpcode load_item,
                                   const CallInfo* helper);
        LIns* emitInlineSpeculativeArrayRead(int objIndexOnStack,
                                             LIns* index,
                                             const CallInfo* helper);

        void emitInlineVectorWrite(int objIndexOnStack, 
                                   LIns* index,
                                   LIns* value,
                                   size_t arrayDataOffset, size_t lenOffset, size_t entriesOffset, int scale, LOpcode store_item,
                                   const CallInfo* helper);
        void emitSetIndexedProperty(int objIndex, int valIndex, LIns *index, IndexKind idxKind);
        void localSet(int i, LIns* o, Traits* type);
        LIns* convertToString(int i, bool preserveNull);
        LIns* coerceToString(int i);
        LIns* coerceToNumber(int i);
#ifdef VMCFG_FLOAT
        LIns* coerceToFloat(int i);
        LIns* coerceToFloat4(int i);
        LIns* coerceToNumeric(int i);
        LIns* coerceFloatToInt(int i);
        bool matchShuffler(MethodInfo* m, uint8_t* shuffle_mask);
#endif // VMCFG_FLOAT
        LIns* loadFromSlot(int ptr_index, int slot, Traits* slotType);
        LIns* coerceToType(int i, Traits*);
        void emitInitializers();
        void emitDebugEnter();
        void emitOsrBranch();

        bool isPromote(LOpcode op);
        LIns* imm2Int(LIns* imm);
        LIns* coerceNumberToInt(int i);

        bool inlineBuiltinFunction(AbcOpcode opcode, intptr_t method_id, int argc, Traits* result, MethodInfo* mi);
        LIns* optimizeIntCmpWithNumberCall(int callIndex, int otherIndex, LOpcode icmp, bool swap);
        LIns* optimizeStringCmpWithStringCall(int callIndex, int otherIndex, LOpcode icmp, bool swap);

        void suspendCSE();
        void resumeCSE();

        LIns* getSpecializedCall(LIns* call);
        LIns* addSpecializedCall(LIns* origCall, LIns* specializedCall);
        LIns* specializeIntCall(LIns *call, const Specialization* specs);

        LIns* emitStringCall(int index, const CallInfo* stringCall, bool preserveNull);

        typedef void (CodegenLIR::*EmitMethod)(Traits* t);
        typedef struct {
            int32_t methodId;
            int32_t argCount;
            BuiltinType argType[2];
            const CallInfo *newFunction;
            EmitMethod emitFunction;
        } FunctionMatch;

        static const FunctionMatch specializedFunctions[];

        int32_t determineBuiltinMaskForArg (int argOffset);
        LIns* getSpecializedArg (int argOffset, BuiltinType newBt);
        void genBuiltinFunctionOptimizerHashMap();

        void emitIsNaN(Traits* result);
        void emitIntMathMin(Traits* result);
        void emitIntMathMax(Traits* result);
        void emitStringLength(Traits* result);

#ifdef VMCFG_FLOAT
        // helpers
        void emitFloatUnary(Traits* result, LOpcode op);
        void emitFloat4unary(Traits*, LOpcode opcode);
        void emitFloat4binary(Traits*, LOpcode opcode);
        void emitFloat4dot(Traits*, LOpcode opcode);
        LIns* magnitude(LOpcode dot_op, LIns* x);
        void emitMagnitude(Traits* result, LOpcode dot_op);
        void emitDistance(Traits* result, LOpcode dot_op);

        // emitters for inline float/float4 functions
        void emitFloatAbs(Traits* result);
        void emitFloatReciprocal(Traits* result);
        void emitFloatRsqrt(Traits* result);
        void emitFloatSqrt(Traits* result);
        void emitFloat4abs(Traits* result);
        void emitFloat4max(Traits* result);
        void emitFloat4min(Traits* result);
        void emitFloat4reciprocal(Traits* result);
        void emitFloat4rsqrt(Traits* result);
        void emitFloat4sqrt(Traits* result);
        void emitFloat4normalize(Traits* result);
        void emitFloat4cross(Traits* result);
        void emitFloat4dot(Traits* result);
        void emitFloat4dot2(Traits* result);
        void emitFloat4dot3(Traits* result);
        void emitFloat4magnitude(Traits* result);
        void emitFloat4magnitude2(Traits* result);
        void emitFloat4magnitude3(Traits* result);
        void emitFloat4distance(Traits* result);
        void emitFloat4distance2(Traits* result);
        void emitFloat4distance3(Traits* result);
        LIns* emitDot(LOpcode, LIns*, LIns*);
        void emitFloat4Greater(Traits* result);
        void emitFloat4GreaterOrEqual(Traits* result);
        void emitFloat4Less(Traits* result);
        void emitFloat4LessOrEqual(Traits* result);
        void emitFloat4Equal(Traits* result);
        void emitFloat4NotEqual(Traits* result);
#endif

        LIns *optimizeIndexArgumentType(int32_t sp, Traits** indexType);

        JITDebugInfo* initJitDebugInfo();

    public:
        ~CodegenLIR();
        CodegenLIR(MethodInfo* info, MethodSignaturep ms, Toplevel* toplevel,
                   OSR *osr_state);
        GprMethodProc emitMD();

        // May return true if JIT will always fail based on information known prior to invocation.
        static bool jitWillFail(const MethodSignaturep ms);

        // CodeWriter methods
        void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type);
        void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits* type);
        void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
        void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type);
        void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
        void writeCheckNull(const FrameState* state, uint32_t index);
        void writeCoerce(const FrameState* state, uint32_t index, Traits *type);
        void writeCoerceToNumeric(const FrameState* state, uint32_t index);
        void writeCoerceToFloat4(const FrameState* state, uint32_t index);
        void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver*);
        void writeEpilogue(const FrameState* state);
        void writeBlockStart(const FrameState* state);
        void writeOpcodeVerified(const FrameState* state, const uint8_t* pc, AbcOpcode opcode);
        void writeFixExceptionsAndLabels(const FrameState* state, const uint8_t* pc);
        void cleanup();
    };
}

#include "CodegenLIR-inlines.h"

#endif /* __avmplus_CodegenLIR__ */
