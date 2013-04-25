/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

//#define DOPROF
#include "../vprof/vprof.h"

namespace avmplus
{
#ifdef VMCFG_WORDCODE
    inline WordOpcode wordCode(AbcOpcode opcode) {
        return (WordOpcode)opcodeInfo[opcode].wordCode;
    }
#endif

    Verifier::Verifier(MethodInfo* info, MethodSignaturep ms, Toplevel* toplevel, AbcEnv* abc_env
#ifdef AVMPLUS_VERBOSE
        , bool secondTry
#endif
        ) : tryFrom(NULL), tryTo(NULL)
          , info(info), ms(ms)
          , worklist(NULL)
          , blockStates(NULL)
          , handlerIsReachable(false)
    {
#ifdef AVMPLUS_VERBOSE
        this->secondTry = secondTry;
#endif
        this->core   = info->pool()->core;
        this->pool   = info->pool();
        this->toplevel = toplevel;
        this->abc_env  = abc_env;

        state = NULL;

        #ifdef VMCFG_RESTARG_OPTIMIZATION
        restArgAnalyzer.init(core, info, ms->frame_size());
        #endif

        #ifdef AVMPLUS_VERBOSE
        verbose = pool->isVerbose(VB_verify, info);
        #endif

        pool->initPrecomputedMultinames();
    }

    Verifier::~Verifier()
    {
        mmfx_delete( this->state );
        if (blockStates) {
            for(int i = 0, n=blockStates->map.length(); i < n; i++)
                mmfx_delete( blockStates->map.at(i) );
            delete blockStates;
        }
    }

    bool Verifier::hasReachableExceptions() const
    {
        // Valid during code generation only.
        AvmAssert(emitPass);
        return handlerIsReachable;
    }

    void Verifier::parseBodyHeader()
    {
        // note: reading of max_stack, etc (and validating the values)
        // is handled by MethodInfo::resolveSignature.
        const uint8_t* pos = info->abc_body_pos();
        AvmCore::skipU32(pos, 4);
        code_length = AvmCore::readU32(pos);
        code_pos = pos;
        pos += code_length;
    }

    // ScopeWriter implements the scope-type-capturing functionality
    // for OP_newfunction and OP_newclass, as well as error checking
    // to handle illegally capturing incompatible scopes for the same
    // function.  All other opcodes are ignored.  Expected to be used
    // in the verifier's phase 2 when each bytecode is only visited once.
    class ScopeWriter: public NullWriter
    {
        MethodInfo* info;
        Toplevel* toplevel;
        Verifier *verifier;
    public:
        ScopeWriter(CodeWriter* coder, MethodInfo* info,
                Toplevel* toplevel, Verifier* verifier)
            : NullWriter(coder), info(info),
              toplevel(toplevel), verifier(verifier)
        {}

        void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits* type)
        {
            if (opcode == OP_newactivation) {
                // Capture activation scope the first time the method is verified.
                const ScopeTypeChain *scope = info->activationScope();
                if (scope == NULL) {
                    AvmAssert(type == info->activationTraits());
                    MMgc::GC* gc = info->pool()->core->GetGC();
                    scope = info->declaringScope()->cloneWithNewTraits(gc, type);
                    type->setDeclaringScopes(scope);
                    info->init_activationScope(scope);
                }
            }
            coder->write(state, pc, opcode, type);
        }

        void writeOp1(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, uint32_t imm30, Traits* type)
        {
            if (opcode == OP_newfunction) {
                PoolObject* pool = info->pool();
                AvmCore* core = pool->core;
                const ScopeTypeChain* scope = info->declaringScope();
                MethodInfo* f = pool->getMethodInfo(imm30);
                Traits* ftraits = core->traits.function_itraits;
                const ScopeTypeChain* fscope = ScopeTypeChain::create(core->GetGC(), ftraits, scope, state, NULL, NULL);
                // Duplicate function definitions aren't strictly legal, but can occur
                // in otherwise "well formed" ABC due to old, buggy versions of ASC.
                // Specifically, code of the form
                //
                //    public function simpleTest():void
                //    {
                //      var f:Function = function():void { }
                //      f();
                //      f = function functwo (x):void { }
                //      f(8);
                //    }
                //
                // could cause the second interior function ("functwo") to include a bogus, unused OP_newfunction
                // call to itself inside the body of functwo. This caused the scope to get reinitialized
                // and generally caused havok. However, we want to allow existing code of this form to continue
                // to work, so check to see if we already have a declaringScope, and if so, require that
                // it match this one.
                const ScopeTypeChain* curScope = f->declaringScope();
                if (curScope != NULL)
                {
                    if (!curScope->equals(fscope))
                    {
                        // if info->method_id() == imm30, f == info, and therefore
                        // curScope == scope -- don't redefine, don't fail verification,
                        // just accept it. see https://bugzilla.mozilla.org/show_bug.cgi?id=544370
                        if (info->method_id() != int32_t(imm30))
                            toplevel->throwVerifyError(kCorruptABCError);

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=563795
                        AvmAssert(type == core->traits.function_itraits);
                        if (type != curScope->traits())
                        {
                        #ifdef AVMPLUS_VERBOSE
                            core->console << "ERROR in ScopeChain::create, mismatched traits " << type << " " << curScope->traits() << "\n";
                        #endif
                            toplevel->throwVerifyError(kCorruptABCError);
                        }
#endif

                        AvmAssert(curScope->equals(scope));
                    }
                    AvmAssert(f->isResolved());
                }
                else
                {
                    f->makeIntoPrototypeFunction(toplevel, fscope);
                }

                #ifdef AVMPLUS_VERBOSE
                if (verifier->verbose)
                    verifier->printScope("function-scope", fscope);
                #endif
            } else if (opcode == OP_newclass) {
                PoolObject* pool = info->pool();
                AvmCore* core = pool->core;
                const ScopeTypeChain* scope = info->declaringScope();
                Traits* ctraits = type;
                // the actual result type will be the static traits of the new class.
                // make sure the traits came from this pool.  they have to because
                // the class_index operand is resolved from the current pool.
                AvmAssert(ctraits->pool == pool);
                Traits *itraits = ctraits->itraits;

                // add a type constraint for the "this" scope of static methods
                const ScopeTypeChain* cscope = ScopeTypeChain::create(core->GetGC(), ctraits, scope, state, NULL, ctraits);

                if (state->scopeDepth > 0)
                {
                    // innermost scope must be the base class object or else createInstance()
                    // will malfunction because it will use the wrong [base] class object to
                    // construct the instance.  See ScriptObject::createInstance()
                    Traits* baseCTraits = state->scopeValue(state->scopeDepth-1).traits;
                    if (!baseCTraits || baseCTraits->itraits != itraits->base)
                        verifier->verifyFailed(kCorruptABCError);
                }

                // add a type constraint for the "this" scope of instance methods
                const ScopeTypeChain* iscope = ScopeTypeChain::create(core->GetGC(), itraits, cscope, NULL, ctraits, itraits);

                ctraits->resolveSignatures(toplevel);
                itraits->resolveSignatures(toplevel);

                const ScopeTypeChain *cur_cscope = ctraits->declaringScope();
                const ScopeTypeChain *cur_iscope = itraits->declaringScope();
                if (!cur_cscope) {
                    // first time we have seen this class, capture scope types
                    ctraits->setDeclaringScopes(cscope);
                    itraits->setDeclaringScopes(iscope);
                } else {
                    // we have captured a scope already for this class.  it better match!
                    if (!cur_cscope->equals(cscope) ||
                        !cur_iscope ||
                        !cur_iscope->equals(iscope)) {
                        toplevel->throwVerifyError(kCorruptABCError);
                    }
                    // use the old ScopeTypeChains, discard the new ones
                    cscope = cur_cscope;
                    iscope = cur_iscope;
                }
                #ifdef AVMPLUS_VERBOSE
                if (verifier->verbose)
                    verifier->printScope("class-scope", cscope);
                #endif
            }
            coder->writeOp1(state, pc, opcode, imm30, type);
        }
    };

#ifdef VMCFG_RESTARG_OPTIMIZATION

    // Most interesting things happen in init()
    RestArgAnalyzer::RestArgAnalyzer()
        : NullWriter(NULL)
        , optimize(true)
        , core(NULL)
        , info(NULL)
        , pool(NULL)
        , frameSize(0)
        , restVar(0)
        , isRestArray(NULL)
    {
    }

    void RestArgAnalyzer::init(AvmCore* _core, MethodInfo* _info, uint32_t _frameSize)
    {
        core = _core;
        info = _info;
        pool = _info->pool();
        frameSize = _frameSize;

        if (_info->needRest() || (_info->needArguments() && _info->onlyUntypedParameters()))
            restVar = _info->getMethodSignature()->param_count() + 1;
        else
            optimize = false;
#ifdef DEBUGGER
        if (core->debugger() != NULL)
            optimize = false;
#endif
        if (optimize) {
            isRestArray = mmfx_new_array(bool, frameSize);
            VMPI_memset(isRestArray, 0, frameSize*sizeof(bool));
        }
    }

    RestArgAnalyzer::~RestArgAnalyzer()
    {
        mmfx_delete_array(isRestArray);
        isRestArray = NULL;
    }

    CodeWriter* RestArgAnalyzer::hookup(CodeWriter* next, bool pass2)
    {
        if (optimize) {
            if (pass2)
                info->setLazyRest();
            coder = next;
            return this;
        }
        return next;
    }

    // Called when the analysis fails (but not during setup).  Factored out to
    // allow easy breakpointing, etc.
    inline void RestArgAnalyzer::fail()
    {
        optimize = false;
    }

    void RestArgAnalyzer::endBlock()
    {
        if (!optimize)
            return;
        for ( uint32_t i=0 ; i < frameSize ; i++ )
            if (isRestArray[i])
                fail();
    }

    void RestArgAnalyzer::write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type)
    {
        if (optimize)
            operate(state, pc, opcode);
        coder->write(state, pc, opcode, type);
    }

    void RestArgAnalyzer::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits* type)
    {
        if (optimize)
            operate(state, pc, opcode);
        coder->writeOp1(state, pc, opcode, opd1, type);
    }

    void RestArgAnalyzer::writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type)
    {
        if (optimize)
            operate(state, pc, opcode);
        coder->writeOp2(state, pc, opcode, opd1, opd2, type);
    }

    void RestArgAnalyzer::writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo* m, uintptr_t disp_id, uint32_t argc, Traits* type)
    {
        if (optimize)
            operate(state, pc, opcode);
        coder->writeMethodCall(state, pc, opcode, m, disp_id, argc, type);
    }

    bool RestArgAnalyzer::getProperty(const FrameState* state, const Multiname& multiname, int obj_offset)
    {
        if (!optimize)
            return false;

        uint32_t sp = state->sp();

        // Be sure the arguments to run-time names aren't the rest array.
        uint32_t numprops = (multiname.isRtname() != 0) + (multiname.isRtns() != 0);
        if (numprops > 0) {
            if (isRestArray[sp])
                fail();
        }
        if (numprops > 1) {
            if (isRestArray[sp-1])
                fail();
        }
        if (!optimize)
            return false;

        if (isRestArray[sp-obj_offset+1])
        {
            isRestArray[sp-obj_offset+1] = false;

            if (multiname.isRtname() && multiname.containsAnyPublicNamespace())
            {
                // OP_restarg candidate
                // containsAnyPublicNamespace should imply n==2 - the multiname must have ns or nsset
                AvmAssert(obj_offset == 2);
            }
            else if (multiname.getName() == core->klength && multiname.containsAnyPublicNamespace())
            {
                // OP_restargc candidate
                AvmAssert(obj_offset == 1);
            }
            else
            {
                fail();
            }
            return optimize;
        }
        else {
            return false;
        }
    }

    void RestArgAnalyzer::writeOpcodeVerified(const FrameState* state, const uint8_t *pc, AbcOpcode opcode)
    {
        if (optimize)
        {
            switch (opcode)
            {
                case OP_getlocal:
                {
                    uint32_t imm30=0, imm30b=0;
                    int32_t imm8=0, imm24=0;
                    AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
                    if (restVar == imm30)
                        isRestArray[state->sp()] = true;
                    break;
                }
                case OP_getlocal0:
                case OP_getlocal1:
                case OP_getlocal2:
                case OP_getlocal3:
                {
                    if (restVar == uint32_t(opcode-OP_getlocal0))
                        isRestArray[state->sp()] = true;
                    break;
                }

                case OP_iflt:
                case OP_ifle:
                case OP_ifnlt:
                case OP_ifnle:
                case OP_ifgt:
                case OP_ifge:
                case OP_ifngt:
                case OP_ifnge:
                case OP_ifeq:
                case OP_ifstricteq:
                case OP_ifne:
                case OP_ifstrictne:
                case OP_iftrue:
                case OP_iffalse:
                case OP_jump:
                case OP_lookupswitch:
                case OP_throw:
                case OP_returnvalue:
                case OP_returnvoid:
                    endBlock();
                    break;
            }
        }

        coder->writeOpcodeVerified(state, pc, opcode);
    }

    // This would be less painful if we had a table stating the number of operands read
    // by most instructions.

    void RestArgAnalyzer::operate(const FrameState* state, const uint8_t *pc, AbcOpcode opcode)
    {
        uint32_t imm30=0, imm30b=0;
        int32_t imm8=0, imm24=0;
        uint32_t numprops = 0;

        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);

        switch (opcode) {
            // Ignored because they are generated post-analysis when the analysis succeeds.
            case OP_restarg:
            case OP_restargc:
                break;

            // Handled in writeOpcodeVerified
            case OP_getlocal:
            case OP_getlocal0:
            case OP_getlocal1:
            case OP_getlocal2:
            case OP_getlocal3:
                break;

            // Handled in getProperty
            case OP_getproperty:
                break;

            // Op0
            case OP_bkpt:
            case OP_bkptline:
            case OP_timestamp:
            case OP_nop:
            case OP_pushbyte:
            case OP_pushshort:
            case OP_pushtrue:
            case OP_pushfalse:
            case OP_pushnan:
            case OP_pushnull:
            case OP_pushundefined:
            case OP_pushstring:
            case OP_pushint:
            case OP_pushuint:
            case OP_pushdouble:
#ifdef VMCFG_FLOAT
            case OP_pushfloat:
            case OP_pushfloat4:
#endif
            case OP_pushnamespace:
            case OP_dxns:
            case OP_label:
            case OP_jump:
            case OP_pop:        // Does not 'read' the value
            case OP_popscope:
            case OP_newfunction:
            case OP_returnvoid:
            case OP_newactivation:
            case OP_newcatch:
            case OP_finddef:
            case OP_getlex:
            case OP_getglobalscope:
            case OP_getscopeobject:
            case OP_getouterscope:
            case OP_getglobalslot:
            case OP_debugline:
            case OP_debugfile:
            case OP_debug:
            case OP_findpropglobalstrict:   // Internal, poorly documented
            case OP_findpropglobal:         // Internal, poorly documented
                break;

            // Op1
            case OP_throw:
            case OP_iftrue:
            case OP_iffalse:
            case OP_convert_i:
            case OP_convert_u:
            case OP_convert_d:
#ifdef VMCFG_FLOAT
            case OP_convert_f:
            case OP_convert_f4:
            case OP_unplus:
#endif
            case OP_convert_b:
            case OP_convert_o:
            case OP_convert_s:
            case OP_coerce_b:
            case OP_coerce_a:
            case OP_coerce_i:
            case OP_coerce_d:
            case OP_coerce_s:
            case OP_coerce_u:
            case OP_coerce_o:
            case OP_coerce:
            case OP_dxnslate:
            case OP_li8:
            case OP_li16:
            case OP_li32:
            case OP_lf32:
            case OP_lf64:
            case OP_sxi1:
            case OP_sxi8:
            case OP_sxi16:
#ifdef VMCFG_FLOAT
            case OP_lf32x4:
            case OP_sf32x4:
#endif
            case OP_pushwith:
            case OP_lookupswitch:
            case OP_increment:
            case OP_decrement:
            case OP_typeof:
            case OP_not:
            case OP_bitnot:
            case OP_increment_i:
            case OP_decrement_i:
            case OP_astype:
            case OP_pushscope:
            case OP_negate:
            case OP_negate_i:
            case OP_dup:
            case OP_checkfilter:
            case OP_esc_xelem:
            case OP_esc_xattr:
            case OP_returnvalue:
            case OP_newclass:
            case OP_getslot:
            case OP_setglobalslot:
                if (isRestArray[state->sp()])
                    fail();
                break;

            // Op2
            case OP_add:
            case OP_subtract:
            case OP_multiply:
            case OP_divide:
            case OP_modulo:
            case OP_lshift:
            case OP_rshift:
            case OP_urshift:
            case OP_bitand:
            case OP_bitor:
            case OP_bitxor:
            case OP_equals:
            case OP_strictequals:
            case OP_lessthan:
            case OP_lessequals:
            case OP_greaterthan:
            case OP_greaterequals:
            case OP_instanceof:
            case OP_istype:
            case OP_istypelate:
            case OP_in:
            case OP_ifnlt:
            case OP_ifnle:
            case OP_ifngt:
            case OP_ifnge:
            case OP_ifeq:
            case OP_ifne:
            case OP_iflt:
            case OP_ifle:
            case OP_ifgt:
            case OP_ifge:
            case OP_ifstricteq:
            case OP_ifstrictne:
            case OP_si8:
            case OP_si16:
            case OP_si32:
            case OP_sf32:
            case OP_sf64:
            case OP_add_i:
            case OP_subtract_i:
            case OP_multiply_i:
            case OP_astypelate:
            case OP_swap:
            case OP_nextname:
            case OP_nextvalue:
            case OP_hasnext:
            case OP_setslot:
                if (isRestArray[state->sp()] || isRestArray[state->sp()-1])
                    fail();
                break;

            // Locals
            case OP_inclocal:
            case OP_declocal:
            case OP_inclocal_i:
            case OP_declocal_i:
            case OP_setlocal:
            case OP_kill:
            update_local:
                if (imm30 == restVar)
                    fail();
                break;
            case OP_setlocal0:
            case OP_setlocal1:
            case OP_setlocal2:
            case OP_setlocal3:
                imm30 = opcode-OP_setlocal0;
                goto update_local;

            // Locals
            case OP_hasnext2:
                if (imm30 == restVar || imm30b == restVar)
                    fail();
                break;

            // Workhorses for variable-number-of-operands instructions
            checkMultiname:
            {
                const Multiname* m = pool->precomputedMultiname(imm30);
                numprops += (m->isRtname() != 0) + (m->isRtns() != 0);
            }
            checkVariable:
            {
                uint32_t top = state->sp();
                for ( uint32_t i=0 ; i < numprops ; i++ )
                    if (isRestArray[top-i])
                        fail();
                break;
            }

            // 0 + 0/1/2 name components
            case OP_findpropstrict:
            case OP_findproperty:
                numprops = 0;
                goto checkMultiname;

            // 1 + 0/1/2 name components
            case OP_getsuper:
            case OP_getdescendants:
            case OP_deleteproperty:
                numprops = 1;
                goto checkMultiname;

            // 2 + 0/1/2 name components
            case OP_setsuper:
            case OP_setproperty:
            case OP_initproperty:
                numprops = 2;
                goto checkMultiname;

            case OP_call:
                numprops = 2 + imm30;
                goto checkVariable;

            case OP_construct:
            case OP_constructsuper:
            case OP_applytype:
                numprops = 1 + imm30;
                goto checkVariable;

            case OP_callmethod:
            case OP_callstatic:
                numprops = 1 + imm30b;
                goto checkVariable;

            case OP_callsuper:
            case OP_callsupervoid:
            case OP_callproperty:
            case OP_callproplex:
            case OP_callpropvoid:
            case OP_constructprop:
                numprops = 1 + imm30b;
                goto checkMultiname;

            case OP_newobject:
                numprops = 2 * imm30;
                goto checkMultiname;

            case OP_newarray:
                numprops = imm30;
                goto checkMultiname;

            default:
                AvmAssert(!"Can't happen");
        }
    }
#endif // VMCFG_RESTARG_OPTIMIZATION

    /**
     * (done) branches stay in code block
     * (done) branches end on even instr boundaries
     * (done) all local var operands stay inside [0..max_locals-1]
     * (done) no illegal opcodes
     * (done) cpool refs are inside [1..cpool_size-1]
     * (done) converging paths have same stack depth
     * (done) operand stack stays inside [0..max_stack-1]
     * (done) locals defined before use
     * (done) scope stack stays bounded
     * (done) getScopeObject never exceeds [0..info->maxScopeDepth()-1]
     * (done) global slots limits obeyed [0..var_count-1]
     * (done) callstatic method limits obeyed [0..method_count-1]
     * (done) cpool refs are correct type
     * (done) make sure we don't fall off end of function
     * (done) slot based ops are ok (slot must be legal)
     * (done) propref ops are ok: usage on actual type compatible with ref type.
     * dynamic lookup ops are ok (type must not have that binding & must be dynamic)
     * dont access superclass state in ctor until super ctor called.
     *
     * pipeline todos:
     * - early binding
     * - copy propagation
     *
     * @param pool
     * @param info
     */
    void Verifier::verify(CodeWriter *emitter)
    {
        SAMPLE_FRAME("[verify]", core);
        PERFM_NVPROF("abc-bytes", code_length);

        if (!info->abc_body_pos()) {
            // no body was supplied in abc
            toplevel->throwVerifyError(kNotImplementedError, core->toErrorString(info));
        }
        if (info->declaringTraits() == NULL) {
            // scope hasn't been captured yet.
            verifyFailed(kCannotVerifyUntilReferencedError);
        }

        // CodeWriter warning: Verify exceptions are thrown from here
        // and callees, so any CodeWriters declared with function scope
        // will not be destructed.  Presently, only CodeWriter, VerifyallWriter,
        // and ScopeWriter are declared this way, and none require cleanup.
        // if you add a new one, or add allocations in one of these,
        // a TRY/CATCH will be required.

        #ifdef AVMPLUS_VERBOSE
        if (verbose)
            core->console << "\ntypecheck " << info << '\n';
        secondTry = false;
        #endif

        #ifdef AVMPLUS_VERBOSE
        // Switch off Re-Queue messages
        bool saveVerbose = verbose;
        if (core->config.verifyquiet) {
            verbose = false;
        }
         #endif
        
        // Verify in two passes.  Phase 1 does type modelling and
        // iterates to a fixed point to determine the types and nullability
        // of each frame variable at branch targets.  Phase 2 includes the
        // emitter and ScopeWriter, and visits opcodes in linear order.
        // Errors detected by these additional CodeWriters can be reported
        // in phase 2.  In each phase, the CodeWriter protocol is obeyed:
        // writePrologue(), visits to explicit and implicit operations using
        // other writeXXX() methods, then writeEpilogue().

        emitPass = false;
        // phase 1 - iterate to a fixed point
        CodeWriter stubWriter;
#ifdef VMCFG_RESTARG_OPTIMIZATION
        coder = restArgAnalyzer.hookup(&stubWriter);
#else
        coder = &stubWriter;
#endif
        parseBodyHeader();          // set code_pos & code_length
        checkFrameDefinition();
        parseExceptionHandlers();   // resolve catch block types
        checkParams();
        #ifdef AVMPLUS_VERBOSE
        if (verbose) {
            printScope("outer-scope", info->declaringScope());
            StringBuffer buf(core);
            printState(buf, state);
        }
        #endif
        coder->writePrologue(state, code_pos, this);
        if (code_length > 0 && code_pos[0] == OP_label) {
            // a reachable block starts at code_pos; explicitly create it,
            // which puts it on the worklist.
            checkTarget(code_pos-1, code_pos);
        } else {
            // inital sequence of code is only reachable from procedure
            // entry, no block will be created, so verify it explicitly
            verifyBlock(code_pos);
        }
        for (FrameState* succ = worklist; succ != NULL; succ = worklist) {
            worklist = succ->wl_next;
            succ->wl_pending = false;
            verifyBlock(loadBlockState(succ));
        }
        coder->writeEpilogue(state);

        // phase 2 - traverse code in abc order and emit
        mmfx_delete(state);
#ifdef VMCFG_RESTARG_OPTIMIZATION
        coder = restArgAnalyzer.hookup(emitter, true);
#else
        coder = emitter;
#endif

        // save computed ScopeTypeChain for OP_newfunction and OP_newclass
        ScopeWriter scopeWriter(coder, info, toplevel, this);
        coder = &scopeWriter;
        
        #ifdef AVMPLUS_VERBOSE
        // Switch off Re-Queue messages
        if (core->config.verifyquiet) {
            verbose = saveVerbose;
        }
        #endif
        
        #ifdef AVMPLUS_VERBOSE
        if (verbose)
            core->console << "\nverify " << info << '\n';
        #endif

        emitPass = true;
        this->coder = coder;
        parseBodyHeader();          // reset code_pos & code_length
        checkFrameDefinition();
        checkParams();
        coder->writePrologue(state, code_pos, this);
        const uint8_t* end_pos = code_pos;
        // typically, first block is not in blockStates: verify it explicitly
        if (!hasFrameState(code_pos))
            end_pos = verifyBlock(code_pos);
        // visit blocks in linear order (blockStates is sorted by abc address)
        for (int i=0, n=getBlockCount(); i < n; i++) {
            const uint8_t* start_pos = loadBlockState(blockStates->map.at(i));
            // overlapping blocks indicates a branch to the middle of an instruction
            if (start_pos < end_pos)
                verifyFailed(kInvalidBranchTargetError);
            end_pos = verifyBlock(start_pos);
        }
        state->abc_pc = code_pos + code_length;
        coder->writeEpilogue(state);
    }

    const uint8_t* Verifier::loadBlockState(FrameState* blk)
    {
        // now load the saved state at this block
        state->init(blk);
        state->abc_pc = blk->abc_pc;

#ifdef AVMPLUS_VERBOSE
        if (verbose) {
            StringBuffer buf(core);
            buf << "B" << int(blk->abc_pc - code_pos) << ":";
            printState(buf, state);
        }
#endif

        // found the start of a new basic block
        coder->writeBlockStart(state);
        return blk->abc_pc;
    }

    static bool isU30(int32_t v) { return (v & 0xc0000000) == 0; }

    void Verifier::checkFrameDefinition()
    {
        // validate rational bounds for frame definition parameters read directly from the abc
        int32_t locals = ms->local_count();
        int32_t scope = ms->max_scope();
        int32_t stack = ms->max_stack();
        if ( !(isU30(stack) && isU30(locals) && isU30(scope)) )
            toplevel->throwVerifyError(kCorruptABCError);

        // also ensure that the computed size doesn't overflow (see MethodInfo._buildMethodSignature)
        int32_t frame = ms->frame_size();
        if ( (uint32_t)frame > ( 0x7fffffff / sizeof(FrameValue) ) )
            toplevel->throwVerifyError(kCorruptABCError);
    }

    void Verifier::checkParams()
    {
        const int param_count = ms->param_count();

        if (ms->local_count() < param_count+1) {
            // must have enough locals to hold all parameters including this
            toplevel->throwVerifyError(kCorruptABCError);
        }

        // initial scope chain types
        if (info->declaringTraits()->init != info && info->declaringScope() == NULL) {
            // this can occur when an activation scope inside a class instance method
            // contains a nested getter, setter, or method.  In that case the scope
            // is not captured when the containing function is verified.  This isn't a
            // bug because such nested functions aren't supported by AS3.  This
            // verify error is how we don't let those constructs run.
            verifyFailed(kNoScopeError, core->toErrorString(info));
        }

        state = mmfx_new( FrameState(ms, info));
        state->abc_pc = code_pos;

        // initialize method param types.
        // We already verified param_count is a legal register so
        // don't checkLocal(i) inside the loop.
        // MethodInfo::verify takes care of resolving param&return type
        // names to Traits pointers, and resolving optional param default values.
        for (int i=0; i <= param_count; i++)
            state->setType(i, ms->paramTraits(i), i == 0);

        int first_local = param_count+1;
        if (info->needRestOrArguments()) {
            // abcMethod_NEED_REST overrides abcMethod_NEED_ARGUMENTS when both are set
            checkLocal(first_local);  // ensure param_count+1 <= max_reg
            state->setType(first_local, ARRAY_TYPE, true);
            first_local++;
        } else {
            checkLocal(param_count);  // ensure param_count <= max_reg
        }

        for (int i=first_local, n = ms->local_count(); i < n; i++)
            state->setType(i, NULL); // void would be more precise.
    }

    // verify one superblock, return at the end.  The end of the block is when
    // we reach a terminal opcode (jump, lookupswitch, returnvalue, returnvoid,
    // or throw), or when we fall into the beginning of another block.
    // returns the address of the next instruction after the block end.
    const uint8_t* Verifier::verifyBlock(const uint8_t* start_pos)
    {
        _nvprof("verify-block", 1);
        CodeWriter *coder = this->coder; // Load into local var for expediency.
        ExceptionHandlerTable* exTable = info->abc_exceptions();
        bool isLoopHeader = state->targetOfBackwardsBranch;
        state->targetOfBackwardsBranch = false;
        state->targetOfExceptionBranch = false;
        const uint8_t* code_end = code_pos + code_length;
        for (const uint8_t *pc = start_pos, *nextpc = pc; pc < code_end; pc = nextpc)
        {
            // should we make a new sample frame in this method?
            // SAMPLE_CHECK();
            PERFM_NVPROF("abc-verify", 1);

            coder->writeFixExceptionsAndLabels(state, pc);

            AbcOpcode opcode = (AbcOpcode) *pc;
            const AbcOpcodeInfo* opcodeInfoPtr = FLOAT_ONLY(!pool->hasFloatSupport() ? opcodeInfoNoFloats :) opcodeInfo;
            if (opcodeInfoPtr[opcode].operandCount == -1)
                verifyFailed(kIllegalOpcodeError, core->toErrorString(info), core->toErrorString(opcode), core->toErrorString((int)(pc-code_pos)));

            state->abc_pc = pc;

            // test for the start of a new block
            if (pc != start_pos && (opcode == OP_label || hasFrameState(pc))) {
                checkTarget(pc-1, pc);
                return pc;
            }

            int sp = state->sp();

            if (pc < tryTo && pc >= tryFrom &&
                (opcodeInfo[opcode].canThrow || (isLoopHeader && pc == start_pos))) {
                // If this instruction can throw exceptions, treat it as an edge to
                // each in-scope catch handler.  The instruction can throw exceptions
                // if canThrow = true, or if this is the target of a backedge, where
                // the implicit interrupt check can throw an exception.
                for (int i=0, n=exTable->exception_count; i < n; i++) {
                    ExceptionHandler* handler = &exTable->exceptions[i];
                    if (pc >= code_pos + handler->from && pc < code_pos + handler->to) {
                        int saveStackDepth = state->stackDepth;
                        int saveScopeDepth = state->scopeDepth;
                        FrameValue stackEntryZero = saveStackDepth > 0 ? state->stackValue(0) : state->value(0);
                        state->stackDepth = 0;
                        state->scopeDepth = 0;

                        // add edge from try statement to catch block
                        const uint8_t* target = code_pos + handler->target;
                        // The thrown value is received as an atom but we will coerce it to
                        // the expected type before handing control to the catch block.
                        state->push(handler->traits);
                        checkTarget(pc, target, /*isExceptionEdge*/true);
                        state->pop();

                        state->stackDepth = saveStackDepth;
                        state->scopeDepth = saveScopeDepth;
                        if (saveStackDepth > 0)
                            state->stackValue(0) = stackEntryZero;

                        // Note that an exception may be caught in this method.
                        handlerIsReachable = true;
                    }
                }
            }

            uint32_t imm30=0, imm30b=0;
            int32_t imm8=0, imm24=0;
            AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);

            // make sure U30 operands are within bounds,
            // except for OP_pushshort, whose operand is sign extended from 16 bits
            if (opcode != OP_pushshort && ((imm30|imm30b) & 0xc0000000))
                verifyFailed(kCorruptABCError);
            if (nextpc > code_end)
                verifyFailed(kLastInstExceedsCodeSizeError);

            #ifdef AVMPLUS_VERBOSE
            if (verbose)
                printOpcode(pc, code_end);
            #endif

            _nvprof("verify-instr", 1);
            switch (opcode)
            {
            case OP_iflt:
            case OP_ifle:
            case OP_ifnlt:
            case OP_ifnle:
            case OP_ifgt:
            case OP_ifge:
            case OP_ifngt:
            case OP_ifnge:
            case OP_ifeq:
            case OP_ifstricteq:
            case OP_ifne:
            case OP_ifstrictne:
                checkStack(2,0);
                coder->writeOp1(state, pc, opcode, imm24);
                state->pop(2);
                checkTarget(pc, nextpc+imm24);
                break;

            case OP_iftrue:
            case OP_iffalse:
                checkStack(1,0);
                emitCoerce(BOOLEAN_TYPE, sp);
                coder->writeOp1(state, pc, opcode, imm24);
                state->pop();
                checkTarget(pc, nextpc+imm24);
                break;

            case OP_jump:
                //checkStack(0,0)
                coder->writeOp1(state, pc, opcode, imm24);
                checkTarget(pc, nextpc+imm24);  // target block;
                coder->writeOpcodeVerified(state, pc, opcode);
                return nextpc;

            case OP_lookupswitch:
            {
                checkStack(1,0);
                peekType(INT_TYPE);
                coder->write(state, pc, opcode);
                state->pop();
                checkTarget(pc, pc+imm24);
                uint32_t case_count = 1 + imm30b;
                // case_count*3 can't overflow a U32...
                uint32_t const case_skip = case_count * 3;
                // ...but adding it to nextpc could wrap around, so check.
                if (uintptr_t(nextpc) > uintptr_t(-1) - case_skip ||
                        nextpc + case_skip > code_end)
                    verifyFailed(kLastInstExceedsCodeSizeError);
                for (uint32_t i=0; i < case_count; i++)
                {
                    int off = AvmCore::readS24(nextpc);
                    checkTarget(pc, pc+off);
                    nextpc += 3;
                }
                coder->writeOpcodeVerified(state, pc, opcode);
                return nextpc;
            }

            case OP_throw:
                checkStack(1,0);
                coder->write(state, pc, opcode);
                state->pop();
                coder->writeOpcodeVerified(state, pc, opcode);
                return nextpc;

            case OP_returnvalue:
                checkStack(1,0);
                emitCoerce(ms->returnTraits(), sp);
                coder->write(state, pc, opcode);
                state->pop();
                coder->writeOpcodeVerified(state, pc, opcode);
                return nextpc;

            case OP_returnvoid:
                //checkStack(0,0)
                coder->write(state, pc, opcode);
                coder->writeOpcodeVerified(state, pc, opcode);
                return nextpc;

            case OP_pushnull:
                checkStack(0,1);
                coder->write(state, pc, opcode, NULL_TYPE);
                state->push(NULL_TYPE);
                break;

            case OP_pushundefined:
                checkStack(0,1);
                coder->write(state, pc, opcode, VOID_TYPE);
                state->push(VOID_TYPE);
                break;

            case OP_pushtrue:
                checkStack(0,1);
                coder->write(state, pc, opcode, BOOLEAN_TYPE);
                state->push(BOOLEAN_TYPE, true);
                break;

            case OP_pushfalse:
                checkStack(0,1);
                coder->write(state, pc, opcode, BOOLEAN_TYPE);
                state->push(BOOLEAN_TYPE, true);
                break;

            case OP_pushnan:
                checkStack(0,1);
                coder->write(state, pc, opcode, NUMBER_TYPE);
                state->push(NUMBER_TYPE, true);
                break;

            case OP_pushshort:
                checkStack(0,1);
                coder->write(state, pc, opcode, INT_TYPE);
                state->push(INT_TYPE, true);
                break;

            case OP_pushbyte:
                checkStack(0,1);
                coder->write(state, pc, opcode, INT_TYPE);
                state->push(INT_TYPE, true);
                break;

            case OP_debugfile:
                //checkStack(0,0)
                // fixme: bugzilla 552988: remove ifdef.
                #if defined(DEBUGGER) || defined(VMCFG_VTUNE)
                checkStringOperand(imm30);
                #endif
                coder->write(state, pc, opcode);
                break;

            case OP_dxns:
                //checkStack(0,0)
                if (!info->setsDxns())
                    verifyFailed(kIllegalSetDxns, core->toErrorString(info));
                checkStringOperand(imm30);
                coder->write(state, pc, opcode);
                break;

            case OP_dxnslate:
                checkStack(1,0);
                if (!info->setsDxns())
                    verifyFailed(kIllegalSetDxns, core->toErrorString(info));
                // codgen will call intern on the input atom.
                coder->write(state, pc, opcode);
                state->pop();
                break;

            case OP_pushstring:
                checkStack(0,1);
                checkStringOperand(imm30);
                coder->write(state, pc, opcode, STRING_TYPE);
                state->push(STRING_TYPE, pool->getString(imm30) != NULL);
                break;

            case OP_pushint:
                checkStack(0,1);
                // FIXME: zero check is incorrect but traditional
                if (imm30 == 0 || imm30 >= pool->constantIntCount)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantIntCount));
                coder->write(state, pc, opcode, INT_TYPE);
                state->push(INT_TYPE,true);
                break;

            case OP_pushuint:
                checkStack(0,1);
                // FIXME: zero check is incorrect but traditional
                if (imm30 == 0 || imm30 >= pool->constantUIntCount)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantUIntCount));
                coder->write(state, pc, opcode, UINT_TYPE);
                state->push(UINT_TYPE,true);
                break;

            case OP_pushdouble:
                checkStack(0,1);
                // FIXME: zero check is incorrect but traditional
                if (imm30 == 0 || imm30 >= pool->constantDoubleCount)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantDoubleCount));
                coder->write(state, pc, opcode, NUMBER_TYPE);
                state->push(NUMBER_TYPE, true);
                break;

#ifdef VMCFG_FLOAT
            case OP_pushfloat:
                checkStack(0,1);
                // FIXME: zero check is incorrect but traditional
                if (imm30 == 0 || imm30 >= pool->constantFloatCount)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantFloatCount));
                coder->write(state, pc, opcode, FLOAT_TYPE);
                state->push(FLOAT_TYPE, true);
                break;

            case OP_pushfloat4:
                checkStack(0,1);
                // FIXME: zero check is incorrect but traditional
                if (imm30 == 0 || imm30 >= pool->constantFloat4Count)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantFloat4Count));
                coder->write(state, pc, opcode, FLOAT4_TYPE);
                state->push(FLOAT4_TYPE, true);
                break;
#endif

            case OP_pushnamespace:
                checkStack(0,1);
                if (imm30 == 0 || imm30 >= pool->constantNsCount)
                    verifyFailed(kCpoolIndexRangeError, core->toErrorString(imm30), core->toErrorString(pool->constantNsCount));
                coder->write(state, pc, opcode, NAMESPACE_TYPE);
                state->push(NAMESPACE_TYPE, pool->cpool_ns[imm30] != NULL);
                break;

            case OP_setlocal:
            {
                checkStack(1,0);
                checkLocal(imm30);
                coder->write(state, pc, opcode);
                FrameValue &v = state->stackTop();
                state->setType(imm30, v.traits, v.notNull);
                state->pop();
                break;
            }

            case OP_setlocal0:
            case OP_setlocal1:
            case OP_setlocal2:
            case OP_setlocal3:
            {
                checkStack(1,0);
                int index = opcode-OP_setlocal0;
                checkLocal(index);
                coder->write(state, pc, opcode);
                FrameValue &v = state->stackTop();
                state->setType(index, v.traits, v.notNull);
                state->pop();
                break;
            }
            case OP_getlocal:
            {
                checkStack(0,1);
                FrameValue& v = checkLocal(imm30);
                coder->write(state, pc, opcode);
                state->push(v);
                break;
            }
            case OP_getlocal0:
            case OP_getlocal1:
            case OP_getlocal2:
            case OP_getlocal3:
            {
                checkStack(0,1);
                FrameValue& v = checkLocal(opcode-OP_getlocal0);
                coder->write(state, pc, opcode);
                state->push(v);
                break;
            }
            case OP_kill:
            {
                //checkStack(0,0)
                checkLocal(imm30);
                coder->write(state, pc, opcode, NULL);
                state->setType(imm30, NULL, false);
                break;
            }

            case OP_inclocal:
            case OP_declocal:
            {
#ifdef SECURITYFIX_ELLIS // https://bugzilla.mozilla.org/show_bug.cgi?id=786828
                checkLocal(imm30);
#endif /*SECURITYFIX_ELLIS*/
                Traits* retType = NUMBER_TYPE;
                bool already_coerced = false;
#ifdef VMCFG_FLOAT
                if(pool->hasFloatSupport())
                {
                    FrameValue& v = checkLocal(imm30);
                    BuiltinType bt = Traits::getBuiltinType(v.traits);
                    if(bt == BUILTIN_none || bt == BUILTIN_any || bt == BUILTIN_object)
                    {
                        emitCoerceToNumeric(imm30);
                        retType = OBJECT_TYPE;
                        already_coerced = true;
                    }
                    else if(bt == BUILTIN_float || bt == BUILTIN_float4)
                        retType = v.traits;
                    else
                        retType = NUMBER_TYPE;
                }
#endif // VMCFG_FLOAT
                if(!already_coerced)
                    emitCoerce(retType, imm30);
                coder->write(state, pc, opcode, retType);
                break;
            }

            case OP_inclocal_i:
            case OP_declocal_i:
                //checkStack(0,0);
                checkLocal(imm30);
                emitCoerce(INT_TYPE, imm30);
                coder->write(state, pc, opcode);
                break;

            case OP_newfunction:
            {
                checkStack(0,1);
                checkMethodInfo(imm30);
                Traits* ftraits = core->traits.function_itraits;
                coder->writeOp1(state, pc, opcode, imm30, ftraits);
                state->push(ftraits, true);
                break;
            }

            case OP_getlex:
            {
                if (state->scopeDepth + info->declaringScope()->size == 0)
                    verifyFailed(kFindVarWithNoScopeError);
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(0, 1, &multiname);
                if (multiname.isRuntime())
                    verifyFailed(kIllegalOpMultinameError, core->toErrorString(opcode), core->toErrorString(&multiname));
                emitFindProperty(OP_findpropstrict, multiname, imm30, pc);
                emitGetProperty(multiname, 1, imm30, pc);
                break;
            }

            case OP_findpropstrict:
            case OP_findproperty:
            {
                if (state->scopeDepth + info->declaringScope()->size == 0)
                    verifyFailed(kFindVarWithNoScopeError);
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(0, 1, &multiname);
                emitFindProperty(opcode, multiname, imm30, pc);
                break;
            }

            case OP_newclass:
            {
                checkStack(1, 1);
                // imm30 is the class_id of the class object to create.
                Traits* ctraits = checkClassInfo(imm30);
                emitCoerce(CLASS_TYPE, state->sp());
                coder->writeOp1(state, pc, opcode, imm30, ctraits);
                state->pop_push(1, ctraits, true);
                break;
            }

            case OP_finddef:
            {
                // must be a CONSTANT_Multiname.
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(0, 1, &multiname);
                if (!multiname.isBinding())
                {
                    // error, def name must be CT constant, regular name
                    verifyFailed(kIllegalOpMultinameError, core->toErrorString(opcode), core->toErrorString(&multiname));
                }
                MethodInfo* script = core->domainMgr()->findScriptInPoolByMultiname(pool, multiname);
                Traits* resultType;
                if (script != (MethodInfo*)BIND_NONE && script != (MethodInfo*)BIND_AMBIGUOUS) {
                    // found a single matching traits
                    resultType = script->declaringTraits();
                } else {
                    // no traits, or ambiguous reference.  use Object, anticipating
                    // a runtime exception
                    resultType = OBJECT_TYPE;
                }
#ifndef MARK_SECURITY_CHANGE // bugzilla.mozilla.org/show_bug.cgi?id=659122
                // If the script has already failed verification, it may
                // have a null declaringTraits.
                if (resultType == NULL)
                    verifyFailed(kCorruptABCError);
#endif
                coder->writeOp1(state, pc, opcode, imm30, resultType);
                state->push(resultType, true);
                break;
            }

            case OP_setproperty:
            case OP_initproperty:
            {
                // stack in: object [ns] [name] value
                Multiname multiname;
                checkConstantMultiname(imm30, multiname); // CONSTANT_Multiname
                checkStackMulti(2, 0, &multiname);

                uint32_t n=2;
                checkPropertyMultiname(n, multiname);

                Traitsp declarer = NULL;
                FrameValue& obj = state->peek(n);
                Binding b = (opcode == OP_initproperty) ?
                            toplevel->getBindingAndDeclarer(obj.traits, multiname, declarer) :
                            toplevel->getBinding(obj.traits, &multiname);
                Traits* propTraits = readBinding(obj.traits, b);

                emitCheckNull(sp-(n-1));

                if (AvmCore::isSlotBinding(b) &&
                    // it's a var, or a const being set from the init function
                    (!AvmCore::isConstBinding(b) ||
                        (opcode == OP_initproperty && declarer->init == info)))
                {
                    emitCoerce(propTraits, state->sp());
                    coder->writeOp2(state, pc, OP_setslot, (uint32_t)AvmCore::bindingToSlotId(b), sp-(n-1), propTraits);
                    state->pop(n);
                    break;
                }
                // else: setting const from illegal context, fall through

                // If it's an accessor that we can early bind, do so.
                // Note that this cannot be done on String or Namespace,
                // since those are represented by non-ScriptObjects
                if (AvmCore::hasSetterBinding(b))
                {
                    // invoke the setter
                    int disp_id = AvmCore::bindingToSetterId(b);
                    const TraitsBindingsp objtd = obj.traits->getTraitsBindings();
                    MethodInfo *f = objtd->getMethod(disp_id);
                    AvmAssert(f != NULL);
                    MethodSignaturep fms = f->getMethodSignature();
                    emitCoerceArgs(f, 1);
                    Traits* propType = fms->returnTraits();
                    coder->writeOp2(state, pc, opcode, imm30, n, propType);
                    state->pop(n);
                    break;
                }

                if(obj.traits == VECTORINT_TYPE
                   || obj.traits == VECTORUINT_TYPE
                   || obj.traits == VECTORDOUBLE_TYPE 
#ifdef VMCFG_FLOAT
                   || obj.traits == VECTORFLOAT_TYPE
                   || obj.traits == VECTORFLOAT4_TYPE
#endif
                   )
                {
                    bool attr = multiname.isAttr();
                    Traits* indexType = state->value(state->sp()-1).traits;

                    // NOTE a dynamic name should have the same version as the current pool
                    bool maybeIntegerIndex = !attr && multiname.isRtname() && multiname.containsAnyPublicNamespace();
                    if( maybeIntegerIndex && (indexType == UINT_TYPE
                                              || indexType == INT_TYPE
                                              || indexType == NUMBER_TYPE
#ifdef VMCFG_FLOAT
                                              || indexType == FLOAT_TYPE   // float indices are always absorbed by the vector
#endif
                                              ) )
                    {
                        if(obj.traits == VECTORINT_TYPE)
                            emitCoerce(INT_TYPE, state->sp());
                        else if(obj.traits == VECTORUINT_TYPE)
                            emitCoerce(UINT_TYPE, state->sp());
                        else if(obj.traits == VECTORDOUBLE_TYPE)
                            emitCoerce(NUMBER_TYPE, state->sp());
#ifdef VMCFG_FLOAT
                        else if(obj.traits == VECTORFLOAT_TYPE)
                            emitCoerce(FLOAT_TYPE, state->sp());
                        else if(obj.traits == VECTORFLOAT4_TYPE)
                            emitCoerce(FLOAT4_TYPE, state->sp());
#endif
                    }
                }

                // Default: do setproperty or initproperty at runtime.

                coder->writeOp2(state, pc, opcode, imm30, n, propTraits);
                state->pop(n);
                break;
            }

            case OP_getproperty:
            {
                // stack in: object [ns [name]]
                // stack out: value
                Multiname multiname;
                checkConstantMultiname(imm30, multiname); // CONSTANT_Multiname
                checkStackMulti(1, 1, &multiname);

                uint32_t n=1;
                checkPropertyMultiname(n, multiname);

#ifdef VMCFG_RESTARG_OPTIMIZATION
                bool emitOptimizedRestArg = restArgAnalyzer.optimize;

                if (emitOptimizedRestArg)
                    emitOptimizedRestArg = restArgAnalyzer.getProperty(state, multiname, n);

                if (emitPass && emitOptimizedRestArg)
                {
                    FrameValue& obj = state->peek(n);
                    if (multiname.isRtname())
                    {
                        // restarg assumes the property name is an atom, so we must coerce it to an atom on input
                        emitCoerce(NULL, state->sp());
                        coder->writeOp1(state, pc, OP_restarg, imm30, NULL);
                        state->pop_push(n, NULL);
                    }
                    else if (multiname.getName() == core->klength)
                    {
                        Binding b = toplevel->getBinding(obj.traits, &multiname);
                        Traits* propType = readBinding(obj.traits, b);
                        coder->write(state, pc, OP_restargc, propType);
                        state->pop_push(n, UINT_TYPE);
                        // restargc produces an uint, so we must coerce to the target type on return
                        emitCoerce(propType, sp);
                    }
                    else {
                        AvmAssert(!"Can't happen");
                    }

                }
                else {
                    emitGetProperty(multiname, n, imm30, pc);
                }
#else
                emitGetProperty(multiname, n, imm30, pc);
#endif
                break;
            }

            case OP_getdescendants:
            {
                // stack in: object [ns] [name]
                // stack out: value
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(1, 1, &multiname);

                uint32_t n=1;
                checkPropertyMultiname(n, multiname);
                emitCheckNull(sp-(n-1));
                coder->write(state, pc, opcode);
                state->pop_push(n, NULL);
                break;
            }

            case OP_checkfilter:
                checkStack(1, 1);
                emitCheckNull(sp);
                coder->write(state, pc, opcode);
                break;

            case OP_deleteproperty:
            {
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(1, 1, &multiname);
                uint32_t n=1;
                checkPropertyMultiname(n, multiname);
                emitCheckNull(sp-(n-1));
                coder->write(state, pc, opcode);
                state->pop_push(n, BOOLEAN_TYPE);
                break;
            }

            case OP_astype:
            {
                checkStack(1, 1);
                // resolve operand into a traits, and push that type.
                Traits *t = checkTypeName(imm30); // CONSTANT_Multiname
                int index = sp;
                Traits* rhs = state->value(index).traits;
                if (!Traits::canAssign(t, rhs))
                {
                    Traits* resultType = t;
                    // result is typed value or null, so if type can't hold null,
                    // then result type is Object.
                    if (t && t->isMachineType())
                        resultType = OBJECT_TYPE;
                    coder->write(state, pc, opcode, t);
                    state->pop_push(1, resultType);
                }
                break;
            }

            case OP_astypelate:
            {
                checkStack(2,1);
                FrameValue& classValue = state->peek(1); // rhs - class
                Traits* ct = classValue.traits;
                Traits* t = NULL;
                if (ct && (t=ct->itraits) != 0)
                    if (t->isMachineType())
                        t = OBJECT_TYPE;
                coder->write(state, pc, opcode, t);
                state->pop_push(2, t);
                break;
            }

            case OP_coerce:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = checkTypeName(imm30);
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_convert_b:
            case OP_coerce_b:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = BOOLEAN_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_coerce_o:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = OBJECT_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_coerce_a:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = NULL;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_convert_i:
            case OP_coerce_i:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = INT_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_convert_u:
            case OP_coerce_u:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = UINT_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_convert_d:
            case OP_coerce_d:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = NUMBER_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
#ifdef VMCFG_FLOAT
            case OP_convert_f:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = FLOAT_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_convert_f4:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = FLOAT4_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_unplus:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                BuiltinType bt = Traits::getBuiltinType(v.traits);
                Traits* type = v.traits;
                if(bt == BUILTIN_none || bt == BUILTIN_any || bt == BUILTIN_object)
                {
                    emitCoerceToNumeric(sp);
                    type = OBJECT_TYPE;
                }
                else if(bt != BUILTIN_float && bt != BUILTIN_float4)
                {
                    emitCoerce(NUMBER_TYPE, sp);
                    type = NUMBER_TYPE;
                }
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, true);
                break;
            }
#endif   // VMCFG_FLOAT
            case OP_coerce_s:
            {
                checkStack(1,1);
                FrameValue &v = state->value(sp);
                Traits *type = STRING_TYPE;
                coder->write(state, pc, opcode, type);
                state->setType(sp, type, v.notNull);
                break;
            }
            case OP_istype:
            {
                checkStack(1,1);
                // resolve operand into a traits, and test if value is that type
                Traits* t = checkTypeName(imm30); // CONSTANT_Multiname
                coder->write(state, pc, opcode, t);
                state->pop(1);
                state->push(BOOLEAN_TYPE);
                break;
            }
            case OP_istypelate:
                checkStack(2,1);
                coder->write(state, pc, opcode);
                // TODO if the only common base type of lhs,rhs is Object, then result is always false
                state->pop_push(2, BOOLEAN_TYPE);
                break;

            case OP_convert_o:
                checkStack(1,1);
                // ISSUE should result be Object, laundering the type?
                // ToObject throws an exception on null and undefined, so after this runs we
                // know the value is safe to dereference.
                emitCheckNull(sp);
                coder->write(state, pc, opcode);
                break;

            case OP_convert_s:
            case OP_esc_xelem:
            case OP_esc_xattr:
                checkStack(1,1);
                // this is the ECMA ToString and ToXMLString operators, so the result must not be null
                // (ToXMLString is split into two variants - escaping elements and attributes)
                coder->write(state, pc, opcode);
                state->pop_push(1, STRING_TYPE, true);
                break;

            case OP_callstatic:
            {
                //  Ensure that the method is eligible for callstatic.
                //  Note: This fails when called by verifyEarly(), since the
                //  data structures being checked have not been initialized.
                //  Need to either rearrange the initialization sequence or
                //  mark this verify pass as "needs late retry."
                if ( imm30 >= pool->methodCount() || ! abc_env->getMethod(imm30) )
                    verifyFailed(kCorruptABCError);

                MethodInfo* m = checkMethodInfo(imm30);
                const uint32_t argc = imm30b;
                checkStack(argc+1, 1);

                MethodSignaturep mms = m->getMethodSignature();
                if (!mms->paramTraits(0))
                {
                    verifyFailed(kDanglingFunctionError, core->toErrorString(m), core->toErrorString(info));
                }

                emitCheckNull(sp-argc);
                emitCoerceArgs(m, argc);

                Traits *resultType = mms->returnTraits();
                coder->writeOp2(state, pc, OP_callstatic, (uint32_t)m->method_id(), argc, resultType);
                state->pop_push(argc+1, resultType);
                break;
            }

            case OP_call:
            {
                const uint32_t argc = imm30;
                checkStack(argc+2, 1);
                // don't need null check, AvmCore::call() uses toFunction() for null check.

                /*
                    TODO optimizations
                        - if this is a class closure for a non-native type, call == coerce
                        - if this is a function closure, try early binding using the traits->call sig
                        - optimize simple cases of casts to builtin types
                */

                coder->writeOp1(state, pc, opcode, argc);
                state->pop_push(argc+2, NULL);
                break;
            }

            case OP_construct:
            {
                const uint32_t argc = imm30;
                checkStack(argc+1, 1);

                // don't need null check, AvmCore::construct() uses toFunction() for null check.
                Traits* ctraits = state->peek(argc+1).traits;
                Traits* itraits = ctraits ? (Traits*)(ctraits->itraits) : NULL;
                coder->writeOp1(state, pc, opcode, argc);
                state->pop_push(argc+1, itraits, true);
                break;
            }

            case OP_callmethod:
            {
                /*
                    OP_callmethod will always throw a verify error.  that's on purpose, it's a
                    last minute change before we shipped FP9 and was necessary when we added methods to class Object.

                    since then we realized that OP_callmethod need only have failed when used outside
                    of the builtin abc, but it's a moot point now.  We dont have to worry about it.

                    code has since been simplified but existing failure modes preserved.
                */
                const uint32_t argc = imm30b;
                checkStack(argc+1,1);

                const int disp_id = imm30-1;
                if (disp_id >= 0)
                {
                    FrameValue& obj = state->peek(argc+1);
                    if( !obj.traits )
                        verifyFailed(kCorruptABCError);
                    else
                        verifyFailed(kIllegalEarlyBindingError, core->toErrorString(obj.traits));
                }
                else
                {
                    verifyFailed(kZeroDispIdError);
                }
                break;
            }

            case OP_callproperty:
            case OP_callproplex:
            case OP_callpropvoid:
            {
                // stack in: obj [ns [name]] args
                // stack out: result
                const uint32_t argc = imm30b;
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(argc+1, 1, &multiname);
                checkCallMultiname(opcode, &multiname);

                uint32_t n = argc+1; // index of receiver
                checkPropertyMultiname(n, multiname);
                emitCallproperty(opcode, sp, multiname, imm30, imm30b, pc);
                break;
            }

            case OP_constructprop:
            {
                // stack in: obj [ns [name]] args
                const uint32_t argc = imm30b;
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(argc+1, 1, &multiname);
                checkCallMultiname(opcode, &multiname);

                uint32_t n = argc+1; // index of receiver
                checkPropertyMultiname(n, multiname);


                FrameValue& obj = state->peek(n); // make sure object is there
                Binding b = toplevel->getBinding(obj.traits, &multiname);
                Traits* ctraits = readBinding(obj.traits, b);
                emitCheckNull(sp-(n-1));
                coder->writeOp2(state, pc, opcode, imm30, argc, ctraits);

                Traits* itraits = ctraits ? (Traits*)(ctraits->itraits) : NULL;
                state->pop_push(n, itraits, itraits==NULL?false:true);
                break;
            }

            case OP_applytype:
            {
                // in: factory arg1..N
                // out: type
                const uint32_t argc = imm30;
                checkStack(argc+1, 1);
                coder->write(state, pc, opcode);
                state->pop_push(argc+1, NULL, true);
                break;
            }

            case OP_callsuper:
            case OP_callsupervoid:
            {
                // stack in: obj [ns [name]] args
                const uint32_t argc = imm30b;
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(argc+1, 1, &multiname);

                if (multiname.isAttr())
                    verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname));

                uint32_t n = argc+1; // index of receiver
                checkPropertyMultiname(n, multiname);

                Traits* base = emitCoerceSuper(sp-(n-1));
                const TraitsBindingsp basetd = base->getTraitsBindings();

                Binding b = toplevel->getBinding(base, &multiname);

                Traits *resultType = NULL;
                if (AvmCore::isMethodBinding(b))
                {
                    int disp_id = AvmCore::bindingToMethodId(b);
                    MethodInfo* m = basetd->getMethod(disp_id);
                    if( !m ) verifyFailed(kCorruptABCError);
                    MethodSignaturep mms = m->getMethodSignature();
                    resultType = mms->returnTraits();
                }

                emitCheckNull(sp-(n-1));
                coder->writeOp2(state, pc, opcode, imm30, argc, base);
                state->pop_push(n, resultType);

                if (opcode == OP_callsupervoid)
                    state->pop();

                break;
            }

            case OP_getsuper:
            {
                // stack in: obj [ns [name]]
                // stack out: value
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(1, 1, &multiname);
                uint32_t n=1;
                checkPropertyMultiname(n, multiname);

                if (multiname.isAttr())
                    verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname));

                Traits* base = emitCoerceSuper(sp-(n-1));
                Binding b = toplevel->getBinding(base, &multiname);
                Traits* propType = readBinding(base, b);
                emitCheckNull(sp-(n-1));
                coder->writeOp2(state, pc, opcode, imm30, n, base);

                if (AvmCore::hasGetterBinding(b))
                {
                    int disp_id = AvmCore::bindingToGetterId(b);
                    const TraitsBindingsp basetd = base->getTraitsBindings();
                        MethodInfo *f = basetd->getMethod(disp_id);
                    AvmAssert(f != NULL);
                    MethodSignaturep fms = f->getMethodSignature();
                    Traits* resultType = fms->returnTraits();
                    state->pop_push(n, resultType);
                }
                else
                {
                    state->pop_push(n, propType);
                }
                break;
            }

            case OP_setsuper:
            {
                // stack in: obj [ns [name]] value
                Multiname multiname;
                checkConstantMultiname(imm30, multiname);
                checkStackMulti(2, 0, &multiname);
                uint32_t n=2;
                checkPropertyMultiname(n, multiname);

                if (multiname.isAttr())
                    verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname));

                Traits* base = emitCoerceSuper(sp-(n-1));
                emitCheckNull(sp-(n-1));
                coder->writeOp2(state, pc, opcode, imm30, n, base);
                state->pop(n);
                break;
            }

            case OP_constructsuper:
            {
                // stack in: obj, args ...
                const uint32_t argc = imm30;
                checkStack(argc+1, 0);

                int32_t ptrIndex = sp-argc;
                Traits* baseTraits = emitCoerceSuper(ptrIndex); // check receiver

                MethodInfo *f = baseTraits->init;
                AvmAssert(f != NULL);

                emitCoerceArgs(f, argc);
                emitCheckNull(sp-argc);
                coder->writeOp2(state, pc, opcode, 0, argc, baseTraits);
                state->pop(argc+1);
                break;
            }

            case OP_newobject:
            {
                uint32_t argc = imm30;
                checkStack(2*argc, 1);
                int n=0;
                while (argc-- > 0)
                {
                    n += 2;
                    peekType(STRING_TYPE, n); // name; will call intern on it
                }
                coder->write(state, pc, opcode);
                state->pop_push(n, OBJECT_TYPE, true);
                break;
            }

            case OP_newarray:
                checkStack(imm30, 1);
                coder->write(state, pc, opcode);
                state->pop_push(imm30, ARRAY_TYPE, true);
                break;

            case OP_pushscope:
            {
                checkStack(1,0);
                if (state->scopeDepth + 1 > ms->max_scope())
                    verifyFailed(kScopeStackOverflowError);

                Traits* scopeTraits = state->peek().traits;
                const ScopeTypeChain* scope = info->declaringScope();
                if (scope->fullsize > (scope->size+state->scopeDepth))
                {
                    // extra constraints on type of pushscope allowed
                    Traits* requiredType = scope->getScopeTraitsAt(scope->size+state->scopeDepth);
                    if (!scopeTraits || !scopeTraits->subtypeof(requiredType))
                    {
                        verifyFailed(kIllegalOperandTypeError, core->toErrorString(scopeTraits), core->toErrorString(requiredType));
                    }
                }

                emitCheckNull(sp);
                coder->writeOp1(state, pc, opcode, ms->scope_base() + state->scopeDepth);
                state->pop();
                state->setType(ms->scope_base() + state->scopeDepth, scopeTraits, true, false);
                state->scopeDepth++;
                break;
            }

            case OP_pushwith:
            {
                checkStack(1,0);

                if (state->scopeDepth + 1 > ms->max_scope())
                    verifyFailed(kScopeStackOverflowError);

                emitCheckNull(sp);
                coder->writeOp1(state, pc, opcode, ms->scope_base() + state->scopeDepth);

                Traits* scopeTraits = state->peek().traits;
                state->pop();
                state->setType(ms->scope_base() + state->scopeDepth, scopeTraits, true, true);

                if (state->withBase == -1)
                    state->withBase = state->scopeDepth;

                state->scopeDepth++;
                break;
            }

            case OP_newactivation:
            {
                checkStack(0, 1);
                if (!info->needActivation())
                    verifyFailed(kInvalidNewActivationError);
                Traits* atraits = info->activationTraits();
                atraits->resolveSignatures(toplevel);
                coder->write(state, pc, opcode, atraits);
                state->push(atraits, true);
                break;
            }

            case OP_newcatch:
            {
                checkStack(0, 1);
                if (!info->abc_exceptions() || imm30 >= (uint32_t)info->abc_exceptions()->exception_count)
                    verifyFailed(kInvalidNewActivationError);
                    // FIXME better error msg
                ExceptionHandler* handler = &info->abc_exceptions()->exceptions[imm30];
                coder->write(state, pc, opcode);
                state->push(handler->scopeTraits, true);
                break;
            }
            case OP_popscope:
                //checkStack(0,0)
                if (state->scopeDepth-- <= 0)
                    verifyFailed(kScopeStackUnderflowError);

                coder->write(state, pc, opcode);

                if (state->withBase >= state->scopeDepth)
                    state->withBase = -1;
                break;

            case OP_getscopeobject:
                checkStack(0,1);

                // local scope
                if (imm8 >= state->scopeDepth)
                    verifyFailed(kGetScopeObjectBoundsError, core->toErrorString(imm8));

                coder->writeOp1(state, pc, opcode, imm8);

                // this will copy type and all attributes too
                state->push(state->scopeValue(imm8));
                break;

            case OP_getouterscope:
            {
                checkStack(0,1);
                const ScopeTypeChain* scope = info->declaringScope();
                uint32_t index = imm30;
                int captured_depth = scope->size;
                if (captured_depth > 0)
                {
                    // imm30 is unsigned, so can't be < 0
                    if (index >= uint32_t(scope->size))
                        toplevel->throwVerifyError(kGetScopeObjectBoundsError);

                    // enclosing scope
                    Traits* t = scope->getScopeTraitsAt(index);
                    coder->writeOp1(state, pc, opcode, index, t);
                    state->push(t, true);
                }
                else
                {
                    #ifdef _DEBUG
                    if (pool->isBuiltin)
                      core->console << "getouterscope >= depth (" << index << " >= " << state->scopeDepth << ")\n";
                    #endif
                    verifyFailed(kGetScopeObjectBoundsError, core->toErrorString(index));
                }
                break;
            }

            case OP_getglobalscope:
                checkStack(0,1);
                coder->write(state, pc, OP_getglobalscope);
                checkGetGlobalScope(); // after coder->write because mutates stack that coder depends on
                break;

            case OP_getglobalslot:
            {
                checkStack(0,1);
                uint32_t slot = imm30-1;
                Traits* globalTraits = checkGetGlobalScope();
                checkEarlySlotBinding(globalTraits); // sets state->value(sp).traits so CodeWriter can see type
                Traits* slotTraits = checkSlot(globalTraits, slot);
                coder->writeOp1(state, pc, OP_getglobalslot, slot, slotTraits);
                state->pop_push(1, slotTraits);
                break;
            }

            case OP_setglobalslot:
            {
                // FIXME need test case
                const ScopeTypeChain* scope = info->declaringScope();
                if (!state->scopeDepth && !scope->size)
                    verifyFailed(kNoGlobalScopeError);
                Traits *globalTraits = scope->size > 0 ? scope->getScopeTraitsAt(0) : state->scopeValue(0).traits;
                checkStack(1,0);
                checkEarlySlotBinding(globalTraits);
                Traits* slotTraits = checkSlot(globalTraits, imm30-1);
                emitCoerce(slotTraits, state->sp());
                coder->writeOp1(state, pc, opcode, imm30-1, slotTraits);
                state->pop();
                break;
            }

            case OP_getslot:
            {
                checkStack(1,1);
                FrameValue& obj = state->peek();
                checkEarlySlotBinding(obj.traits);
                Traits* slotTraits = checkSlot(obj.traits, imm30-1);
                emitCheckNull(state->sp());
                coder->write(state, pc, opcode);
                state->pop_push(1, slotTraits);
                break;
            }

            case OP_setslot:
            {
                checkStack(2,0);
                FrameValue& obj = state->peek(2); // object
                checkEarlySlotBinding(obj.traits);
                Traits* slotTraits = checkSlot(obj.traits, imm30-1);
                emitCoerce(slotTraits, state->sp());
                emitCheckNull(state->sp()-1);
                coder->write(state, pc, opcode);
                state->pop(2);
                break;
            }

            case OP_pop:
                checkStack(1,0);
                coder->write(state, pc, opcode);
                state->pop();
                break;

            case OP_dup:
            {
                checkStack(1, 2);
                FrameValue& v = state->peek();
                coder->write(state, pc, opcode);
                state->push(v);
                break;
            }

            case OP_swap:
            {
                checkStack(2,2);
                FrameValue v1 = state->peek(1);
                FrameValue v2 = state->peek(2);
                coder->write(state, pc, opcode);
                state->pop(2);
                state->push(v1);
                state->push(v2);
                break;
            }

            case OP_lessthan:
            case OP_greaterthan:
            case OP_lessequals:
            case OP_greaterequals:
            {
                // if either the LHS or RHS is a number type, then we know
                // it will be a numeric comparison.
                checkStack(2,1);
                FrameValue& rhs = state->peek(1);
                FrameValue& lhs = state->peek(2);
                Traits *lhst = lhs.traits;
                Traits *rhst = rhs.traits;

                if(lhst && rhst ){
                    // We assume is that number-to-number comparison yields the same result as float-to-float comparison
                    // Also, float4 will be coerced to number, and this is ok.
                    if (!lhst->isNumeric()  && rhst->isNumeric() )
                    {
                        // convert lhs to Number
                        emitCoerce(NUMBER_TYPE, state->sp()-1);
                    }
                    else if (lhst->isNumeric() && !rhst->isNumeric())
                    {
                        // promote rhs to Number
                        emitCoerce(NUMBER_TYPE, state->sp());
                    }
                }
                coder->write(state, pc, opcode, BOOLEAN_TYPE);
                state->pop_push(2, BOOLEAN_TYPE);
                break;
            }

#ifndef MARK_SECURITY_CHANGE // http://bugzilla.mozilla.org/show_bug.cgi?id=654761
            case OP_in:
                checkStack(2,1);
                emitCheckNull(sp);
                coder->write(state, pc, opcode);
                state->pop_push(2, BOOLEAN_TYPE);
                break;

            case OP_equals:
            case OP_strictequals:
            case OP_instanceof:
                checkStack(2,1);
                coder->write(state, pc, opcode);
                state->pop_push(2, BOOLEAN_TYPE);
                break;
#else
            case OP_equals:
            case OP_strictequals:
            case OP_instanceof:
            case OP_in:
                checkStack(2,1);
                coder->write(state, pc, opcode);
                state->pop_push(2, BOOLEAN_TYPE);
                break;
#endif 

            case OP_not:
                checkStack(1,1);
                emitCoerce(BOOLEAN_TYPE, sp);
                coder->write(state, pc, opcode, BOOLEAN_TYPE);
                state->pop_push(1, BOOLEAN_TYPE);
                break;

            case OP_add:
            {
                checkStack(2,1);

                FrameValue& rhs = state->peek(1);
                FrameValue& lhs = state->peek(2);
                Traits* lhst = lhs.traits;
                Traits* rhst = rhs.traits;
                if ((lhst == STRING_TYPE && lhs.notNull) || (rhst == STRING_TYPE && rhs.notNull))
                {
                    coder->write(state, pc, OP_add, STRING_TYPE);
                    state->pop_push(2, STRING_TYPE, true);
                }
                else if (lhst && lhst->isNumeric() && rhst && rhst->isNumeric())
                {
                    Traits* type = NUMBER_TYPE;
#ifdef VMCFG_FLOAT
                    if(pool->hasFloatSupport())
                    {
                        if(lhst == FLOAT4_TYPE || rhst == FLOAT4_TYPE)
                            type = FLOAT4_TYPE; // float4 + anything numeric gives float4
                        else
                        if(rhst->isNumberType() || lhst->isNumberType())
                            type = NUMBER_TYPE;
                        else 
                        if(rhst == FLOAT_TYPE && lhst == FLOAT_TYPE)
                            type = FLOAT_TYPE;
                        else 
                        {
                            // the result is "NUMERIC" because at least one is NUMERIC and the other is NUMERIC or FLOAT.
                            emitCoerceToNumeric(sp);
                            emitCoerceToNumeric(sp-1);
                            type = OBJECT_TYPE;
                        }
                    }
#endif // VMCFG_FLOAT
                    coder->write(state, pc, OP_add, type);
                    state->pop_push(2, type, true);
                } 
                else
                {
                    coder->write(state, pc, OP_add, OBJECT_TYPE);
                    // NOTE don't know if it will return number or string, but
                    // neither will be null
                    state->pop_push(2, OBJECT_TYPE, true);
                }
                break;
            }

            case OP_modulo:
            case OP_subtract:
            case OP_divide:
            case OP_multiply:
                {
                    checkStack(2,1);
                    Traits* type = NUMBER_TYPE;
                    bool already_coerced = false;
#ifdef VMCFG_FLOAT
                    if(pool->hasFloatSupport())
                    {
                        FrameValue& rhs = state->peek(1);
                        FrameValue& lhs = state->peek(2);
                        BuiltinType lhst = Traits::getBuiltinType(lhs.traits);
                        BuiltinType rhst = Traits::getBuiltinType(rhs.traits);
                        /* If both are floats, operate on float. 
                           If any is float4, operate on float4.
                           If both types are known (and not Object), operate on number.
                           Otherwise - convert to numeric */
                        if(lhst == BUILTIN_float && rhst == BUILTIN_float)
                            type = FLOAT_TYPE;
                        else if(lhst == BUILTIN_float4 || rhst == BUILTIN_float4)
                            type = FLOAT4_TYPE;
                        else if(lhst != BUILTIN_none && lhst != BUILTIN_object && lhst != BUILTIN_any && rhst != BUILTIN_none && rhst != BUILTIN_object && rhst != BUILTIN_any)
                            type = NUMBER_TYPE;
                        else
                        {
                            emitCoerceToNumeric(sp-1);
                            emitCoerceToNumeric(sp);
                            already_coerced = true;
                            type = OBJECT_TYPE;
                        }
                    }
#endif
                    if(!already_coerced)
                    {
                        emitCoerce(type, sp-1);
                        emitCoerce(type, sp);
                    }
                    coder->write(state, pc, opcode, type);
                    state->pop_push(2, type);
                    break;
                }

            case OP_negate:
            case OP_increment:
            case OP_decrement:
                {
                checkStack(1,1);
                Traits* retType = NUMBER_TYPE;
                bool already_coerced = false;
#ifdef VMCFG_FLOAT
                if(pool->hasFloatSupport())
                {
                    FrameValue& v = state->peek(1);
                    BuiltinType bt = Traits::getBuiltinType(v.traits);
                    if(bt == BUILTIN_none || bt == BUILTIN_any || bt == BUILTIN_object)
                    {
                        emitCoerceToNumeric(sp);
                        already_coerced = true;
                        retType = OBJECT_TYPE;
                    }
                    else if (bt == BUILTIN_float || bt == BUILTIN_float4) 
                        retType = v.traits;
                    else
                        retType = NUMBER_TYPE;
                }
#endif
                if(!already_coerced)
                    emitCoerce(retType, sp);
                coder->write(state, pc, opcode, retType);
                break;
                }

            case OP_increment_i:
            case OP_decrement_i:
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                break;

            case OP_add_i:
            case OP_subtract_i:
            case OP_multiply_i:
                checkStack(2,1);
                emitCoerce(INT_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(2, INT_TYPE);
                break;

            case OP_negate_i:
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                break;

            case OP_bitand:
            case OP_bitor:
            case OP_bitxor:
                checkStack(2,1);
                emitCoerce(INT_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(2, INT_TYPE);
                break;

            // ISSUE do we care if shift amount is signed or not?  we mask
            // the result so maybe it doesn't matter.
            // CN says see tests e11.7.2, 11.7.3, 9.6
            case OP_lshift:
            case OP_rshift:
                checkStack(2,1);
                emitCoerce(INT_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(2, INT_TYPE);
                break;

            case OP_urshift:
                checkStack(2,1);
                emitCoerce(INT_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(2, UINT_TYPE);
                break;

            case OP_bitnot:
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                break;

            case OP_typeof:
                checkStack(1,1);
                coder->write(state, pc, opcode);
                state->pop_push(1, STRING_TYPE, true);
                break;

            case OP_nop:
                // those show up but will be ignored
            case OP_bkpt:
            case OP_bkptline:
            case OP_timestamp:
                coder->write(state, pc, OP_nop);
                break;

            case OP_debug:
            {
#ifdef DEBUGGER
                // fixme: bugzilla 552988: remove ifdef.
                uint8_t type = (uint8_t)*(pc + 1);
                if (type == DI_LOCAL) {
                    // see Debugger::scanCode
                    const uint8_t* pc2 = pc + 2;
                    uint32_t index = AvmCore::readU32(pc2);
                    checkStringOperand(index);
                }
#endif
                coder->write(state, pc, opcode);
                break;
            }

            case OP_label:
                coder->write(state, pc, opcode);
                break;

            case OP_debugline:
                coder->write(state, pc, opcode);
                break;

            case OP_nextvalue:
            case OP_nextname:
                checkStack(2,1);
                peekType(INT_TYPE, 1);
                coder->write(state, pc, opcode);
                state->pop_push(2, NULL);
                break;

            case OP_hasnext:
                checkStack(2,1);
                peekType(INT_TYPE,1);
                coder->write(state, pc, opcode);
                state->pop_push(2, INT_TYPE);
                break;

            case OP_hasnext2:
            {
                checkStack(0,1);
                checkLocal(imm30);
                FrameValue& v = checkLocal(imm30b);
                if (imm30 == imm30b)
                    verifyFailed(kInvalidHasNextError);
                if (v.traits != INT_TYPE)
                    verifyFailed(kIllegalOperandTypeError, core->toErrorString(v.traits), core->toErrorString(INT_TYPE));
                coder->write(state, pc, opcode);
                state->setType(imm30, NULL, false);
                state->push(BOOLEAN_TYPE);
                break;
            }

            // sign extends
            case OP_sxi1:
            case OP_sxi8:
            case OP_sxi16:
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(1, INT_TYPE);
                break;

            // loads
            case OP_li8:
            case OP_li16:
                if (pc+1 < code_end &&
                    ((opcode == OP_li8 && pc[1] == OP_sxi8) || (opcode == OP_li16 && pc[1] == OP_sxi16)))
                {
                    checkStack(1,1);
                    emitCoerce(INT_TYPE, sp);
                    coder->write(state, pc, (opcode == OP_li8) ? OP_lix8 : OP_lix16);
                    state->pop_push(1, INT_TYPE);
                    // ++pc; // do not skip the sign-extend; if it's the target
                    // of an implicit label, skipping it would cause verification failure.
                    // instead, just emit it, and rely on LIR to ignore sxi instructions
                    // in these situations.
                    break;
                }
                // else fall thru
            case OP_li32:
            case OP_lf32:
            case OP_lf64:
            {
                Traits* result = (opcode == OP_lf32 || opcode == OP_lf64) ? NUMBER_TYPE : INT_TYPE;
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(1, result);
                break;
            }
#ifdef VMCFG_FLOAT
            case OP_lf32x4: {
                checkStack(1,1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop_push(1, FLOAT4_TYPE);
                break;
            }
#endif

            // stores
            case OP_si8:
            case OP_si16:
            case OP_si32:
            case OP_sf32:
            case OP_sf64:
                checkStack(2,0);
                emitCoerce((opcode == OP_sf32 || opcode == OP_sf64) ? NUMBER_TYPE : INT_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop(2);
                break;
#ifdef VMCFG_FLOAT
            case OP_sf32x4: {
                checkStack(2,0);
                emitCoerce(FLOAT4_TYPE, sp-1);
                emitCoerce(INT_TYPE, sp);
                coder->write(state, pc, opcode);
                state->pop(2);
                break;
            }
#endif
                    
            default:
                // size was nonzero, but no case handled the opcode.  someone asleep at the wheel!
                AvmAssertMsg(false, "Unhandled opcode");
            }

            coder->writeOpcodeVerified(state, pc, opcode);
            #ifdef AVMPLUS_VERBOSE
            if (verbose) {
                StringBuffer buf(core);
                printState(buf, state);
            }
            #endif
        }

        verifyFailed(kCannotFallOffMethodError);
        return code_end;
    }

    void Verifier::checkPropertyMultiname(uint32_t &depth, Multiname &multiname)
    {
        if (multiname.isRtname())
        {
            if (multiname.isQName())
            {
                // a.ns::@[name] or a.ns::[name]
                peekType(STRING_TYPE, depth++);
            }
            else
            {
                // a.@[name] or a[name]
                depth++;
            }
        }

        if (multiname.isRtns())
        {
            peekType(NAMESPACE_TYPE, depth++);
        }
    }

    void Verifier::emitCallproperty(AbcOpcode opcode, int& sp, Multiname& multiname, uint32_t multiname_index, uint32_t argc, const uint8_t* pc)
    {
        uint32_t n = argc+1;
        checkPropertyMultiname(n, multiname);
        Traits* t = state->peek(n).traits;

        if (t)
            t->resolveSignatures(toplevel);
        Binding b = toplevel->getBinding(t, &multiname);

        emitCheckNull(sp-(n-1));

        if (emitCallpropertyMethod(opcode, t, b, multiname, multiname_index, argc, pc))
            return;

        if (emitCallpropertySlot(opcode, sp, t, b, argc, pc))
            return;

        coder->writeOp2(state, pc, opcode, multiname_index, argc);

        // If early binding then the state will have been updated, so this will be skipped
        state->pop_push(n, NULL);
        if (opcode == OP_callpropvoid)
            state->pop();
    }

    bool Verifier::emitCallpropertyMethod(AbcOpcode opcode, Traits* t, Binding b, Multiname& multiname, uint32_t multiname_index, uint32_t argc, const uint8_t* pc)
    {
        (void) multiname_index;  // FIXME remove

        if (!AvmCore::isMethodBinding(b))
            return false;

        uint32_t n = argc+1;
        const TraitsBindingsp tb = t->getTraitsBindings();
        if (t == core->traits.math_ctraits)
            b = findMathFunction(tb, multiname, b, argc);
        else if (t == core->traits.string_itraits)
            b = findStringFunction(tb, multiname, b, argc);

        int disp_id = AvmCore::bindingToMethodId(b);
        MethodInfo* m = tb->getMethod(disp_id);
        MethodSignaturep mms = m->getMethodSignature();

        if (!mms->argcOk(argc))
            return false;

        Traits* resultType = mms->returnTraits();

        emitCoerceArgs(m, argc);
        if (!t->isInterface())
        {
            coder->writeMethodCall(state, pc, OP_callmethod, m, disp_id, argc, resultType);
            if (opcode == OP_callpropvoid)
                coder->write(state, pc, OP_pop);
        }
        else
        {
            // NOTE when the interpreter knows how to dispatch through an
            // interface, we can rewrite this call as a 'writeOp2'.
            coder->writeMethodCall(state, pc, opcode, m, 0, argc, resultType);
        }

        state->pop_push(n, resultType);
        if (opcode == OP_callpropvoid)
        {
            state->pop();
        }

        return true;
    }

    bool Verifier::emitCallpropertySlot(AbcOpcode opcode, int& sp, Traits* t, Binding b, uint32_t argc, const uint8_t *pc)
    {
        if (!AvmCore::isSlotBinding(b))
            return false;
        if ((argc != 1) && (argc != 4))
            return false;

        const TraitsBindingsp tb = t->getTraitsBindings();

        int slot_id = AvmCore::bindingToSlotId(b);
        Traits* slotType = tb->getSlotTraits(slot_id);

        if ((argc == 4) FLOAT_ONLY(&& (slotType != core->traits.float4_ctraits)))
            return false;

        if (slotType == core->traits.int_ctraits)
        {
            coder->write(state, pc, OP_convert_i, INT_TYPE);
            state->setType(sp, INT_TYPE, true);
        }
        else
        if (slotType == core->traits.uint_ctraits)
        {
            coder->write(state, pc, OP_convert_u, UINT_TYPE);
            state->setType(sp, UINT_TYPE, true);
        }
        else
        if (slotType == core->traits.number_ctraits)
        {
            coder->write(state, pc, OP_convert_d, NUMBER_TYPE);
            state->setType(sp, NUMBER_TYPE, true);
        }
        else
#ifdef VMCFG_FLOAT
        if (slotType == core->traits.float_ctraits)
        {
            coder->write(state, pc, OP_convert_f, FLOAT_TYPE);
            state->setType(sp, FLOAT_TYPE, true);
        }
        else
        if (slotType == core->traits.float4_ctraits)
        {
            if(argc == 1) {
                coder->write(state, pc, OP_convert_f4, FLOAT4_TYPE);
                state->setType(sp, FLOAT4_TYPE, true);
            } else {
                AvmAssert(argc == 4);
                AvmAssert(opcode != OP_callpropvoid);
                coder->writeCoerceToFloat4(state, sp);
                state->setType(sp, FLOAT4_TYPE, true);
            }
        }
        else
#endif
        if (slotType == core->traits.boolean_ctraits)
        {
            coder->write(state, pc, OP_convert_b, BOOLEAN_TYPE);
            state->setType(sp, BOOLEAN_TYPE, true);
        }
        else
        if (slotType == core->traits.string_ctraits)
        {
            coder->write(state, pc, OP_convert_s, STRING_TYPE);
            state->setType(sp, STRING_TYPE, true);
        }
        else
        // NOTE the following has been refactored so that both lir and wc coerce. previously
        // wc would be skipped and fall back on the method call the the class converter
        if (slotType && slotType->base == CLASS_TYPE && slotType->getCreateClassClosureProc() == NULL)
        {
            // is this a user defined class?  A(1+ args) means coerce to A
            AvmAssert(slotType->itraits != NULL);
            FrameValue &v = state->value(sp);
            coder->write(state, pc, OP_coerce, slotType->itraits);
            state->setType(sp, slotType->itraits, v.notNull);
        }
        else
        {
            return false;
        }

        if (opcode == OP_callpropvoid)
        {
            AvmAssert(argc == 1);
            coder->write(state, pc, OP_pop);  // result
            coder->write(state, pc, OP_pop);  // function
            state->pop(2);
        }
        else
        {
            FrameValue v = state->stackTop();
            // NOTE writeNip is necessary until lir optimizes the "nip"
            // case to avoid the extra copies that result from swap+pop
            coder->writeNip(state, pc, argc);
            state->pop(argc + 1);
            state->push(v);
        }
        return true;
    }

    void Verifier::emitFindProperty(AbcOpcode opcode, Multiname& multiname, uint32_t imm30, const uint8_t *pc)
    {
        bool skip_translation = false;
        const ScopeTypeChain* scope = info->declaringScope();
        if (multiname.isBinding())
        {
            int scope_base = ms->scope_base();
            int base = scope_base;
            int index = base + state->scopeDepth - 1;
            if (scope->size == 0)
            {
                // if scope->size = 0, then global is a local
                // scope, and we dont want to early bind to global.
                base++;
            }
            for (; index >= base; index--)
            {
                FrameValue& v = state->value(index);
                Binding b = toplevel->getBinding(v.traits, &multiname);
                if (b != BIND_NONE)
                {
                    coder->writeOp1(state, pc, OP_getscopeobject, index - scope_base);
                    state->push(v);
                    return;
                }
                if (v.isWith)
                    break;  // with scope could have dynamic property
            }
            if (index < base)
            {
                // look at captured scope types
                for (index = scope->size-1; index > 0; index--)
                {
                    Traits* t = scope->getScopeTraitsAt(index);
                    Binding b = toplevel->getBinding(t, &multiname);
                    if (b != BIND_NONE)
                    {
                        coder->writeOp1(state, pc, OP_getouterscope, index);
                        state->push(t, true);
                        return;
                    }
                    if (scope->getScopeIsWithAt(index))
                        break;  // with scope could have dynamic property
                }
                if (index <= 0)
                {
                    // look at import table for a suitable script
                    MethodInfo* script = core->domainMgr()->findScriptInPoolByMultiname(pool, multiname);
                    if (script != (MethodInfo*)BIND_NONE && script != (MethodInfo*)BIND_AMBIGUOUS)
                    {
#ifndef MARK_SECURITY_CHANGE // bugzilla.mozilla.org/show_bug.cgi?id=659122
                        // If the script has already failed verification, it may
                        // have a null declaringTraits.
                        if (script->declaringTraits() == NULL)
                            verifyFailed(kCorruptABCError);
#endif
                        if (script == info)
                        {
                            // ISSUE what if there is an ambiguity at runtime? is VT too early to bind?
                            // its defined here, use getscopeobject 0
                            if (scope->size > 0)
                            {
                                coder->writeOp1(state, pc, OP_getouterscope, 0);
                            }
                            else
                            {
                                coder->write(state, pc, OP_getglobalscope);
                            }
                        }
                        else // found a single matching traits
                        {
                            coder->writeOp1(state, pc, OP_finddef, imm30, script->declaringTraits());
                        }
                        state->push(script->declaringTraits(), true);
                        return;
                    }
                    else
                    {
                        switch (opcode) {
                            case OP_findproperty:
                                coder->writeOp1(state, pc, OP_findpropglobal, imm30);
                                break;
                            case OP_findpropstrict:
                                coder->writeOp1(state, pc, OP_findpropglobalstrict, imm30);
                                break;
                            default:
                                AvmAssert(false);
                                break;
                        }
                        skip_translation = true;
                    }
                }
            }
        }
        uint32_t n=1;
        checkPropertyMultiname(n, multiname);
        if (!skip_translation) coder->writeOp1(state, pc, opcode, imm30, OBJECT_TYPE);
        state->pop_push(n-1, OBJECT_TYPE, true);
    }

    void Verifier::emitGetProperty(Multiname &multiname, int n, uint32_t imm30, const uint8_t *pc)
    {
        FrameValue& obj = state->peek(n);

        Binding b = toplevel->getBinding(obj.traits, &multiname);
        Traits* propType = readBinding(obj.traits, b);

        emitCheckNull(state->sp()-(n-1));

        // early bind slot
        if (AvmCore::isSlotBinding(b))
        {
            // Bugzilla 705756: If we can mark known-notnull definitions as 
            // notnull then finddef calls will be removed by the optimizer.
            //
            // *** DO NOT MERGE TO TAMARIN-REDUX WITHOUT CONSULTING FELIX ***
            bool notNull = false;
            if (obj.traits->pool == core->builtinPool)
            {
                // Global object holding the built-in types
                Stringp nm = multiname.getName();
                if (FLOAT_ONLY(nm == core->kfloat4 || nm == core->kfloat ||) nm == core->kMath || nm == core->kNumber)
                {
                    // We know those bindings not to be null values
                    notNull = true;
                }
            }
            coder->writeOp1(state, pc, OP_getslot, AvmCore::bindingToSlotId(b), propType);
            state->pop_push(n, propType, notNull);
            return;
        }

        // early bind accessor
        if (AvmCore::hasGetterBinding(b))
        {
            // Invoke the getter
            int disp_id = AvmCore::bindingToGetterId(b);
            const TraitsBindingsp objtd = obj.traits->getTraitsBindings();
            MethodInfo *f = objtd->getMethod(disp_id);
            AvmAssert(f != NULL);
            emitCoerceArgs(f, 0);
            coder->writeOp2(state, pc, OP_getproperty, imm30, n, propType);
            AvmAssert(propType == f->getMethodSignature()->returnTraits());
            state->pop_push(n, propType);
            return;
        }
        if( !propType )
        {
            if (obj.traits == VECTORINT_TYPE
                || obj.traits == VECTORUINT_TYPE
                || obj.traits == VECTORDOUBLE_TYPE
#ifdef VMCFG_FLOAT
                || obj.traits == VECTORFLOAT_TYPE
                || obj.traits == VECTORFLOAT4_TYPE
#endif
                )
            {
                bool attr = multiname.isAttr();
                Traits* indexType = state->value(state->sp()).traits;
                // NOTE a dynamic name should have the same version as the current pool
                bool maybeIntegerIndex = !attr && multiname.isRtname() && multiname.containsAnyPublicNamespace();
                if( maybeIntegerIndex && (indexType == UINT_TYPE
                                          || indexType == INT_TYPE
                                          || indexType == NUMBER_TYPE
#ifdef VMCFG_FLOAT
                                          || indexType == FLOAT_TYPE
#endif
                                          ) )
                {
                    if(obj.traits == VECTORINT_TYPE)
                        propType = INT_TYPE;
                    else if(obj.traits == VECTORUINT_TYPE)
                        propType = UINT_TYPE;
                    else if(obj.traits == VECTORDOUBLE_TYPE)
                        propType = NUMBER_TYPE;
#ifdef VMCFG_FLOAT
                    else if(obj.traits == VECTORFLOAT_TYPE)
                        propType = FLOAT_TYPE;
                    else if(obj.traits == VECTORFLOAT4_TYPE)
                        propType = FLOAT4_TYPE;
#endif // VMCFG_FLOAT
                }
            }
            else if (obj.traits != NULL && obj.traits->subtypeof(VECTOROBJ_TYPE) && 
                     abc_env->codeContext()->bugCompatibility()->bugzilla678952)
            {
                // We version this because it affects verifier semantics, see comments
                // Bugzilla 678952.  The versioning is static, however: we only want to
                // know the compatibility settings for the code, not for the calling context.
                // If the ABC was compiled for SWFn then it gets SWFn verifier treatment
                // in every context.  Builtin ABC code is compiled with the latest SWF version
                // and always gets the optimization; that that is correct requires some
                // careful argumentation, again see Bugzilla 678952.

                bool attr = multiname.isAttr();
                Traits* indexType = state->value(state->sp()).traits;
                // NOTE a dynamic name should have the same version as the current pool
                bool maybeIntegerIndex = !attr && multiname.isRtname() && multiname.containsAnyPublicNamespace();
                if( maybeIntegerIndex && (indexType == UINT_TYPE
                                          || indexType == INT_TYPE
                                          || indexType == NUMBER_TYPE
#ifdef VMCFG_FLOAT
                                          || indexType == FLOAT_TYPE
#endif
                                          ) )
                {
                    propType = obj.traits->m_paramTraits;
                }
            }
        }

        // default - do getproperty at runtime

        coder->writeOp2(state, pc, OP_getproperty, imm30, n, propType);
        state->pop_push(n, propType);
    }

    Traits* Verifier::checkGetGlobalScope()
    {
        const ScopeTypeChain* scope = info->declaringScope();
        int captured_depth = scope->size;
        if (captured_depth > 0) {
            // enclosing scope
            Traits* t = scope->getScopeTraitsAt(0);
            state->push(t, true);
            return t;
        }
        else {
            // local scope
            if (state->scopeDepth == 0)
                verifyFailed(kGetScopeObjectBoundsError, core->toErrorString(0));
            Traits* t = state->scopeValue(0).traits;
            state->push(state->scopeValue(0));
            return t;
        }
    }

    FrameState *Verifier::getFrameState(const uint8_t* pc) const
    {
        return blockStates ? blockStates->map.get(pc) : NULL;
    }

    bool Verifier::hasFrameState(const uint8_t* pc) const
    {
        return blockStates && blockStates->map.containsKey(pc);
    }

    int Verifier::getBlockCount() const
    {
        return blockStates ? blockStates->map.length() : 0;
    }

    const uint8_t* Verifier::getTryFrom() const
    {
        return tryFrom;
    }

    const uint8_t* Verifier::getTryTo() const
    {
        return tryTo;
    }

    void Verifier::emitCheckNull(int i)
    {
        FrameValue& value = state->value(i);
        if (!value.notNull) {
            coder->writeCheckNull(state, i);
            value.notNull = true;
        }
    }

    void Verifier::checkCallMultiname(AbcOpcode /*opcode*/, Multiname* name) const
    {
        if (name->isAttr())
        {
            StringBuffer sb(core);
            sb << *name;
            verifyFailed(kIllegalOpMultinameError, core->toErrorString(name), sb.toString());
        }
    }

    Traits* Verifier::emitCoerceSuper(int index)
    {
        Traits* base = info->declaringTraits()->base;
        if (base != NULL)
        {
            emitCoerce(base, index);
        }
        else
        {
            verifyFailed(kIllegalSuperCallError, core->toErrorString(info));
        }
        return base;
    }

    void Verifier::emitCoerce(Traits* target, int index)
    {
        FrameValue &v = state->value(index);
        coder->writeCoerce(state, index, target);
        state->setType(index, target, v.notNull);
    }

    void Verifier::emitCoerceToNumeric(int index)
    {
        coder->writeCoerceToNumeric(state, index);
        state->setType(index, OBJECT_TYPE, true);
    }

    Traits* Verifier::peekType(Traits* requiredType, int n)
    {
        Traits* t = state->peek(n).traits;
        if (t != requiredType)
        {
            verifyFailed(kIllegalOperandTypeError, core->toErrorString(t), core->toErrorString(requiredType));
        }
        return t;
    }

    void Verifier::checkEarlySlotBinding(Traits* t)
    {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=652673
        // early slot binding is only allowed if the method in question comes
        // from the same ABC as the Traits being accessed.
        if (!t || t->pool != info->pool() || !t->allowEarlyBinding())
#else
        if (!t || !t->allowEarlyBinding())
#endif
            verifyFailed(kIllegalEarlyBindingError, core->toErrorString(t));
    }

    void Verifier::emitCoerceArgs(MethodInfo* m, int argc)
    {
        if (!m->isResolved())
            m->resolveSignature(toplevel);

        MethodSignaturep mms = m->getMethodSignature();
        if (!mms->argcOk(argc))
        {
            verifyFailed(kWrongArgumentCountError, core->toErrorString(m), core->toErrorString(mms->requiredParamCount()), core->toErrorString(argc));
        }

        // coerce parameter types
        int n=1;
        while (argc > 0)
        {
            Traits* target = (argc <= mms->param_count()) ? mms->paramTraits(argc) : NULL;
            emitCoerce(target, state->sp()-(n-1));
            argc--;
            n++;
        }

        // coerce receiver type
        emitCoerce(mms->paramTraits(0), state->sp()-(n-1));
    }

    void Verifier::checkStack(uint32_t pop, uint32_t push)
    {
        if (uint32_t(state->stackDepth) < pop)
            verifyFailed(kStackUnderflowError);
        if (state->stackDepth - pop + push > uint32_t(ms->max_stack()))
            verifyFailed(kStackOverflowError);
    }

    void Verifier::checkStackMulti(uint32_t pop, uint32_t push, Multiname* m)
    {
        if (m->isRtname()) pop++;
        if (m->isRtns()) pop++;
        checkStack(pop,push);
    }

    FrameValue& Verifier::checkLocal(int local)
    {
        if (local < 0 || local >= ms->local_count())
            verifyFailed(kInvalidRegisterError, core->toErrorString(local));
        return state->value(local);
    }

    Traits* Verifier::checkSlot(Traits *traits, int imm30)
    {
        uint32_t slot = imm30;
        if (traits)
            traits->resolveSignatures(toplevel);
        TraitsBindingsp td = traits ? traits->getTraitsBindings() : NULL;
        const uint32_t count = td ? td->slotCount : 0;
        if (!traits || slot >= count)
        {
            verifyFailed(kSlotExceedsCountError, core->toErrorString(slot+1), core->toErrorString(count), core->toErrorString(traits));
        }
        return td->getSlotTraits(slot);
    }

    Traits* Verifier::readBinding(Traits* traits, Binding b) const
    {
        if (traits)
        {
            traits->resolveSignatures(toplevel);
        }
        else
        {
            AvmAssert(AvmCore::bindingKind(b) == BKIND_NONE);
        }
        return Traits::readBinding(traits, b);
    }

    MethodInfo* Verifier::checkMethodInfo(uint32_t id)
    {
        const uint32_t c = pool->methodCount();
        if (id >= c)
        {
            verifyFailed(kMethodInfoExceedsCountError, core->toErrorString(id), core->toErrorString(c));
        }

        return pool->getMethodInfo(id);
    }

    Traits* Verifier::checkClassInfo(uint32_t id)
    {
        const uint32_t c = pool->classCount();
        if (id >= c)
        {
            verifyFailed(kClassInfoExceedsCountError, core->toErrorString(id), core->toErrorString(c));
        }

        return pool->getClassTraits(id);
    }

    Traits* Verifier::checkTypeName(uint32_t index)
    {
        Multiname name;
        checkConstantMultiname(index, name); // CONSTANT_Multiname
        Traits* t = core->domainMgr()->findTraitsInPoolByMultiname(pool, name);
        if (t == NULL)
            verifyFailed(kClassNotFoundError, core->toErrorString(&name));
        if (t == (Traits*)BIND_AMBIGUOUS)
            toplevel->throwReferenceError(kAmbiguousBindingError, name);
        if (name.isParameterizedType())
        {
            core->stackCheck(toplevel);
            Traits* param_traits = name.getTypeParameter() ? checkTypeName(name.getTypeParameter()) : NULL ;
            t = pool->resolveParameterizedType(toplevel, t, param_traits);
        }
        return t;
    }

    void Verifier::verifyFailed(int errorID, Stringp arg1, Stringp arg2, Stringp arg3) const
    {
        #ifdef AVMPLUS_VERBOSE
        if (!secondTry && !verbose) {
            // capture the verify trace even if verbose is false.
            Verifier v2(info, ms, toplevel, abc_env, true);
            v2.verbose = true;
            v2.tryFrom = tryFrom;
            v2.tryTo = tryTo;
            CodeWriter stubWriter;

            // The second verification pass will presumably always throw an
            // error, which we ignore.  But we /must/ catch it so that we can
            // clean up the verifier resources.  Cleanup happens automatically
            // when execution reaches the end of the block.

            TRY(core, kCatchAction_Ignore) {
                v2.verify(&stubWriter);
            }
            CATCH(Exception *ignored) {
                (void)ignored;
            }
            END_CATCH
            END_TRY
        }
        #endif
        toplevel->throwVerifyError(errorID, arg1, arg2, arg3);

        // This function throws, and should never return.
        AvmAssert(false);
    }

    /**
      * Insert f into list in abc_pc order.  Use a linear search for the
      * right position.  Keeping the worklist sorted avoids pathologically
      * revisiting the same block too many times as long as predecessors
      * generally come before successors in ABC order.
      */
    void requeue(FrameState* f, FrameState** list) {
      while (*list && (*list)->abc_pc < f->abc_pc)
           list = &(*list)->wl_next;
      f->wl_next = *list;
      *list = f;
      f->wl_pending = true;
    }

    // Merge the current FrameState (this->state) with the target
    // FrameState (getFrameState(target)), and report verify errors.
    // Fixme: Bug 558876 - |current| must not be dereferenced, it could point
    // outside the valid range of bytecodes.  Its only for back-edge detection.
    void Verifier::checkTarget(const uint8_t* current, const uint8_t* target, bool isExceptionEdge)
    {
        if (emitPass) {
            AvmAssert(hasFrameState(target));
            return;
        }

        // branches must stay inside code, and back edges must land on an OP_label,
        // or a location already known as a forward-branch target
        if (target < code_pos || target >= code_pos+code_length ||
            (target <= current && !hasFrameState(target) && *target != OP_label)) {
            verifyFailed(kInvalidBranchTargetError);
        }

        FrameState *targetState = getFrameState(target);
        bool targetChanged;
        if (!targetState) {
            if (blockStates != NULL && target > current)
            {
                // If we're jumping forward to an instruction with no FrameState,
                // we need to re-verify the block that contains it to merge FrameStates.
                // This is a conservative approach: roughly,
                //
                // if (target is new) and (branch < nearest < target) and (nearest not queued) then queue(nearest)
                //
                // this is suboptimal in that we may guess wrong and requeue blocks that don't need
                // reverification, but this is theoretically harmless, just extra work.
                //
                int i = blockStates->map.findNear(target);
                // i too large should be impossible, but i < 0 is possible, if the insertion point
                // was before the first block. In that case, just ignore it, since we only
                // need to requeue if it's possible that an existing block has already been
                // verified once; in this case no such block could exist.
                AvmAssert(i < blockStates->map.length());
                // (But let's check anyway...)
                if (i >= 0 && i < blockStates->map.length())
                {
                    // If the block we find in the table is in between the
                    // position of the branch and the position of the target,
                    // requeue. (And also, if it's already pending, don't bother.)
                    FrameState* existingState = blockStates->map.at(i);
                    if (current < existingState->abc_pc &&
                        existingState->abc_pc < target &&
                        !existingState->wl_pending)
                    {
                        #ifdef AVMPLUS_VERBOSE
                        if (verbose) {
                            core->console << "------------------------------------\n";
                            core->console << "RE-QUEUE B" << int(existingState->abc_pc - code_pos) << ":";
                        }
                        #endif
                        existingState->wl_pending = true;
                        existingState->wl_next = worklist;
                        worklist = existingState;
                        // no return: we want to fall thru and create the new blockstate as well.
                    }
                }
            }
            if (!blockStates)
                blockStates = new (core->GetGC()) BlockStatesType(core->GetGC());
            targetState = mmfx_new(FrameState(ms, info));
            targetState->abc_pc = target;
            blockStates->map.put(target, targetState);

            // first time visiting target block
            targetChanged = true;
            AvmAssert(!state->targetOfBackwardsBranch);
            AvmAssert(!state->targetOfExceptionBranch);
            targetState->init(state);

            #ifdef AVMPLUS_VERBOSE
            if (verbose) {
                core->console << "------------------------------------\n";
                StringBuffer buf(core);
                buf << "MERGE FIRST B" << int(targetState->abc_pc - code_pos) << ":";
                printState(buf, targetState);
                core->console << "------------------------------------\n";
            }
            #endif
        } else {
            targetChanged = mergeState(targetState);
        }

        bool targetOfBackwardsBranch = targetState->targetOfBackwardsBranch || target <= current;
        if (targetOfBackwardsBranch != targetState->targetOfBackwardsBranch)
            targetChanged |= true;
        targetState->targetOfBackwardsBranch = targetOfBackwardsBranch;

        bool targetOfExceptionBranch = targetState->targetOfExceptionBranch || isExceptionEdge;
        if (targetOfExceptionBranch != targetState->targetOfExceptionBranch)
            targetChanged |= true;
        targetState->targetOfExceptionBranch = targetOfExceptionBranch;

        if (targetChanged && !targetState->wl_pending)
            requeue(targetState, &worklist);
    }

    bool Verifier::mergeState(FrameState* targetState)
    {
#ifdef AVMPLUS_VERBOSE
        if (verbose) {
            core->console << "------------------------------------\n";
            StringBuffer buf(core);
            buf << "MERGE CURRENT " << int(state->abc_pc - code_pos) << ":";
            printState(buf, state);
            buf.reset();
            buf << "MERGE TARGET B" << int(targetState->abc_pc - code_pos) << ":";
            printState(buf, targetState);
        }
#endif

        // check matching stack depth
        if (state->stackDepth != targetState->stackDepth)
            verifyFailed(kStackDepthUnbalancedError, core->toErrorString((int)state->stackDepth), core->toErrorString((int)targetState->stackDepth));

        // check matching scope chain depth
        if (state->scopeDepth != targetState->scopeDepth)
            verifyFailed(kScopeDepthUnbalancedError, core->toErrorString(state->scopeDepth), core->toErrorString(targetState->scopeDepth));

        // Merge types of locals, scopes, and operands.
        // Merge could preserve common interfaces even when
        // common supertype does not:
        //    class A implements I {}
        //    class B implements I {}
        //    var i:I = b ? new A : new B
        // Doing so would require different specification for verify-time analysis,
        // essentially a differnet ABC spec, yet each abc version needs predictable
        // verifier semantics.
        // On the other hand, later optimization passes are free to be as accurate as
        // they like, if it produces better code.

        bool targetChanged = false;
        const int scopeTop  = ms->scope_base() + targetState->scopeDepth;
        const int stackTop  = ms->stack_base() + targetState->stackDepth;
        for (int i=0, n=stackTop; i < n; i++)
        {
            // ignore empty locations between scopeTop and stackBase
            if (i >= scopeTop && i < ms->stack_base())
                continue;

            const FrameValue& curValue = state->value(i);
            FrameValue& targetValue = targetState->value(i);

            if (curValue.isWith != targetValue.isWith) {
                // failure: pushwith on one edge, pushscope on other edge, cannot merge.
                verifyFailed(kCannotMergeTypesError, core->toErrorString(targetValue.traits), core->toErrorString(curValue.traits));
            }

            Traits* merged_traits = findCommonBase(targetValue.traits, curValue.traits);
            bool merged_notNull = targetValue.notNull && curValue.notNull;

            if (targetValue.traits != merged_traits || targetValue.notNull != merged_notNull)
                targetChanged = true;

#ifdef VMCFG_NANOJIT
            uint16_t merged_sst = targetValue.sst_mask | curValue.sst_mask;

            // String+null and Namespace+null are legal but must be special-cased
            // here.  The SlotStorageType for NULL_TYPE is SST_scriptobject,
            // which will leave two bits set in merged_sst.  Erase the SST_scriptobject bit.
            // Any other type merged with null already works, as does String|Namespace
            // merged with any other type.
            const uint16_t str_or_obj = (1 << SST_string)    | (1 << SST_scriptobject);
            const uint16_t ns_or_obj  = (1 << SST_namespace) | (1 << SST_scriptobject);
            if ((merged_traits == STRING_TYPE    && merged_sst == str_or_obj) ||
                (merged_traits == NAMESPACE_TYPE && merged_sst == ns_or_obj)) {
                AvmAssert(targetValue.traits == NULL_TYPE || curValue.traits == NULL_TYPE);
                merged_sst &= ~(1 << SST_scriptobject);
            }

            if (targetValue.sst_mask != merged_sst)
                targetChanged = true;
            targetValue.sst_mask = merged_sst;
#endif
            targetValue.traits = merged_traits;
            targetValue.notNull = merged_notNull;
        }

#ifdef AVMPLUS_VERBOSE
        if (verbose) {
            StringBuffer buf(core);
            buf << "AFTER MERGE B" << int(targetState->abc_pc - code_pos) << ":";
            printState(buf, targetState);
            core->console << "------------------------------------\n";
        }
#endif
        return targetChanged;
    }

    /**
     * find common base class of these two types
     */
    Traits* Verifier::findCommonBase(Traits* t1, Traits* t2)
    {
        if (t1 == t2)
            return t1;

        if (t1 == NULL) {
            // assume t1 is always non-null
            Traits *temp = t1;
            t1 = t2;
            t2 = temp;
        }

        // okay to merge null with pointer type
        if (Traits::getBuiltinType(t1) == BUILTIN_null && t2 && !t2->isMachineType())
            return t2;
        if (Traits::getBuiltinType(t2) == BUILTIN_null && t1 && !t1->isMachineType())
            return t1;

        // all commonBase flags start out false.  set the cb bits on
        // t1 and its ancestors.
        Traits* t = t1;
        do t->commonBase = true;
        while ((t = t->base) != NULL);

        // now search t2 and its ancestors looking for the first cb=true
        t = t2;
        while (t != NULL && !t->commonBase)
            t = t->base;

        Traits* common = t;

        // finally reset the cb bits to false for next time
        t = t1;
        do t->commonBase = false;
        while ((t = t->base) != NULL);

        return common;
    }

    void Verifier::checkStringOperand(uint32_t index)
    {
        if (!index || index >= pool->constantStringCount)
            verifyFailed(kCpoolIndexRangeError, core->toErrorString(index),
                         core->toErrorString(pool->constantStringCount));
    }

    void Verifier::checkNameOperand(uint32_t index)
    {
        if (!index || index >= pool->cpool_mn_offsets.length())
            verifyFailed(kCpoolIndexRangeError, core->toErrorString(index),
                         core->toErrorString(pool->cpool_mn_offsets.length()));
    }

    void Verifier::checkConstantMultiname(uint32_t index, Multiname& m)
    {
        checkNameOperand(index);
        pool->parseMultiname(m, index);
    }

    Binding Verifier::findMathFunction(TraitsBindingsp math, const Multiname& multiname, Binding b, int argc)
    {
        Stringp newname = core->internString(core->concatStrings(core->kUnderscore, multiname.getName()));
        Binding newb = math->findBinding(newname);
        if (AvmCore::isMethodBinding(newb))
        {
            int disp_id = AvmCore::bindingToMethodId(newb);
            MethodInfo* newf = math->getMethod(disp_id);
            MethodSignaturep newfms = newf->getMethodSignature();
            const int param_count = newfms->param_count();
            if (argc == param_count)
            {
                for (int i=state->stackDepth-argc, n=state->stackDepth; i < n; i++)
                {
                    Traits* t = state->stackValue(i).traits;
                    if (!t || !t->isNumeric())
                        return b;
                }
                b = newb;
            }
        }
        return b;
    }

    Binding Verifier::findStringFunction(TraitsBindingsp str, const Multiname& multiname, Binding b, int argc)
    {
        Stringp newname = core->internString(core->concatStrings(core->kUnderscore, multiname.getName()));
        Binding newb = str->findBinding(newname);
        if (AvmCore::isMethodBinding(newb))
        {
            int disp_id = AvmCore::bindingToMethodId(newb);
            MethodInfo* newf = str->getMethod(disp_id);
            // We have all required parameters but not more than required.
            MethodSignaturep newfms = newf->getMethodSignature();
            const int param_count = newfms->param_count();
            const int optional_count = newfms->optional_count();
            if ((argc >= (param_count - optional_count)) && (argc <= param_count))
            {
                for (int i=state->stackDepth-argc, k = 1, n=state->stackDepth; i < n; i++, k++)
                {
                    Traits* t = state->stackValue(i).traits;
                    if (t != newfms->paramTraits(k))
                        return b;
                }
                b = newb;
            }
        }
        return b;
    }

#ifndef SIZE_T_MAX
#  ifdef SIZE_MAX
#    define SIZE_T_MAX SIZE_MAX
#  else
#    define SIZE_T_MAX UINT_MAX
#  endif
#endif

    void Verifier::parseExceptionHandlers()
    {
        if (info->abc_exceptions()) {
            AvmAssert(tryFrom && tryTo);
            return;
        }

        const uint8_t* pos = code_pos + code_length;
        int exception_count = toplevel->readU30(pos);   // will be nonnegative and less than 0xC0000000

        if (exception_count != 0)
        {
            if (exception_count == 0 || (size_t)(exception_count-1) > SIZE_T_MAX / sizeof(ExceptionHandler))
                verifyFailed(kIllegalExceptionHandlerError);

            ExceptionHandlerTable* table = ExceptionHandlerTable::create(core->GetGC(), exception_count);
            ExceptionHandler *handler = table->exceptions;
            for (int i=0; i < exception_count; i++, handler++)
            {
                handler->from = toplevel->readU30(pos);
                handler->to = toplevel->readU30(pos);
                handler->target = toplevel->readU30(pos);

                const uint8_t* const scopePosInPool = pos;

                int type_index = toplevel->readU30(pos);
                Traits* t = type_index ? checkTypeName(type_index) : NULL;

                Multiname qn;
                int name_index = pool->hasExceptionSupport() ? toplevel->readU30(pos) : 0;
                if (name_index != 0)
                {
                    pool->parseMultiname(qn, name_index);
                    if (!qn.isBinding()) {
                        // abc docs specify that the name is a string but asc generates QNames.
                        // Multinames with no namespaces could be supported, but Tamarin currently can't
                        // handle them in this context.
                        verifyFailed(kCorruptABCError); // the error code could be more precise
                    }
                }

                #ifdef AVMPLUS_VERBOSE
                if (verbose)
                {
                    core->console << "            exception["<<i<<"] from="<< handler->from
                        << " to=" << handler->to
                        << " target=" << handler->target
                        << " type=" << t
                        << " name=";
                    if (name_index != 0)
                        core->console << qn;
                    else
                        core->console << "(none)";
                    core->console << "\n";
                }
                #endif

                if (handler->from < 0 ||
                    handler->to < handler->from ||
                    handler->target < handler->to ||
                    handler->target >= code_length)
                {
                    // illegal range in handler record
                    verifyFailed(kIllegalExceptionHandlerError);
                }

                // save maximum try range
                if (!tryFrom || (code_pos + handler->from) < tryFrom)
                    tryFrom = code_pos + handler->from;
                if (code_pos + handler->to > tryTo)
                    tryTo = code_pos + handler->to;

                // note: since we require (code_len > target >= to >= from >= 0),
                // all implicit exception edges are forward edges.

                // handler->traits = t
                WB(core->GetGC(), table, &handler->traits, t);

                Traits* scopeTraits = name_index == 0 ? OBJECT_TYPE :
                    Traits::newCatchTraits(toplevel, pool, scopePosInPool, qn.getName(), qn.getNamespace());

                // handler->scopeTraits = scopeTraits
                WB(core->GetGC(), table, &handler->scopeTraits, scopeTraits);
            }

            info->set_abc_exceptions(core->GetGC(), table);
        }
        else
        {
            info->set_abc_exceptions(core->GetGC(), NULL);
        }
    }

    #ifdef AVMPLUS_VERBOSE
    void Verifier::printOpcode(const uint8_t* pc, const uint8_t* code_end)
    {
        int offset = int(pc - code_pos);
        core->console << "  " << offset << ':';
        core->formatOpcode(core->console, pc, code_end, (AbcOpcode)*pc, offset, pool);
        core->console << '\n';
    }

    /**
     * display contents of current stack frame only.
     */
    void Verifier::printState(StringBuffer& prefix, FrameState *state)
    {
        PrintWriter& out = core->console;
        if (prefix.length() > 0) {
            char buf[80]; // plenty for currently known prefixes
            VMPI_sprintf(buf, "%-23s[", prefix.c_str());
            out << buf;
        } else {
            out << "                       [";
        }

        // locals
        int scope_base = ms->scope_base();
        for (int i=0, n = scope_base; i < n; i++) {
            printValue(state->value(i));
            if (i+1 < n)
                out << ' ';
        }
        out << "] {";

        // scope chain
        for (int i = scope_base, n = scope_base + state->scopeDepth; i < n; i++) {
            printValue(state->value(i));
            if (i+1 < n)
                out << ' ';
        }
        out << "} (";

        // stack
        int stackStart;
        const int stackLimit = 20; // don't display more than this, to reduce verbosity
        if (state->stackDepth > stackLimit) {
            stackStart = ms->stack_base() + state->stackDepth - stackLimit;
            out << "..." << stackStart << ": ";
        } else {
            stackStart = ms->stack_base();
        }
        for (int i = stackStart, n = ms->stack_base() + state->stackDepth; i < n; i++) {
            printValue(state->value(i));
            if (i+1 < n)
                out << ' ';
        }
        out << ")";

        // branch target info
        if (state->targetOfBackwardsBranch) out << " B";
        if (state->targetOfExceptionBranch) out << " E";

        out << "\n";
    }

    /** display contents of a captured scope chain */
    void Verifier::printScope(const char* title, const ScopeTypeChain* scope)
    {
        PrintWriter& out = core->console;
        out << "  " << title << " = ";
        if (scope && scope->size > 0) {
            out << '[';
            for (int i=0, n=scope->size; i < n; i++) {
                Traits* t = scope->getScopeTraitsAt(i);
                if (!t)
                    out << "*!";
                else
                    out << t;
                if (i+1 < n)
                    out << ' ';
            }
            out << "]\n";
        } else {
            out << "null\n";
        }
    }

    void Verifier::printValue(FrameValue& v)
    {
        Traits* t = v.traits;
        PrintWriter& out = core->console;
        out << t;
        if (!t || (!t->isNumeric() && t != BOOLEAN_TYPE && t != NULL_TYPE && t != VOID_TYPE))
            out << (v.notNull ? "~" : "");

#ifdef VMCFG_NANOJIT
        if (v.sst_mask) {
            out << '[';
            if (v.sst_mask & (1 << SST_atom))            out << 'A';
            if (v.sst_mask & (1 << SST_string))          out << 'S';
            if (v.sst_mask & (1 << SST_namespace))       out << 'N';
            if (v.sst_mask & (1 << SST_scriptobject))    out << 'O';
            if (v.sst_mask & (1 << SST_int32))           out << 'I';
            if (v.sst_mask & (1 << SST_uint32))          out << 'U';
            if (v.sst_mask & (1 << SST_bool32))          out << 'B';
            if (v.sst_mask & (1 << SST_double))          out << 'D';
#ifdef VMCFG_FLOAT
            if (v.sst_mask & (1 << SST_float))           out << 'F';
            if (v.sst_mask & (1 << SST_float4))          out << "F4";
#endif 
            out << ']';
        }
#endif
    }
    #endif /* AVMPLUS_VERBOSE */
}
