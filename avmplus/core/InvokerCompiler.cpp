/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_NANOJIT

#include "CodegenLIR.h"


//
// JIT compiler for invoker stubs
//
namespace avmplus
{
    bool InvokerCompiler::canCompileInvoker(MethodInfo* method)
    {
        MethodSignaturep ms = method->getMethodSignature();
        int32_t rest_offset = ms->rest_offset();
        int32_t param_count = ms->param_count();
        if (rest_offset > int32_t((param_count+1) * sizeof(Atom)) &&
            method->needRestOrArguments()) {
            // situation: natively represented args need more space than provided,
            // and this method uses varargs (rest args or arguments array); this means
            // the declared args must grow, which requires allocating an unknown amount
            // of space since we don't know (at compile time) how many extra varags are present.
            //
            // punt for now.  to better handle this in the future, the cases are:
            //
            // jit function with rest args:
            //   the prolog will create an array with these extra args, so all
            //   we should need is a way to pass in the pointer to them.  (tweak ABI).
            //
            // jit function that needs arguments:
            //   the prolog will create an array with all the args re-boxed as Atom,
            //   plus the extra ones.  (the unboxed copies are still available in the callee).
            //
            // native function with rest args:
            //   the native function abi passes rest args via (argc,Atom*) parameters
            //   which we could adapt to here without shifting or copying, if we could
            //   bypass the normal Gpr/FprMethodProc ABI.
            //
            // native function with arguments:
            //   doesn't happen.  native functions only support rest args.
            //
            // Given the current JIT and native ABI, we can't support shifting and
            // copying the extra unknown number of args.  With changes to the native
            // ABI, we could pass a reference to the extra args without any copying.
            return false;
        }
        return true;
    }

    // compiler driver
    AtomMethodProc InvokerCompiler::compile(MethodInfo* method)
    {
        InvokerCompiler compiler(method);
        compiler.generate_lir();
        return (AtomMethodProc) compiler.assemble();
    }

    InvokerCompiler::InvokerCompiler(MethodInfo* method)
        : LirHelper(method->pool())
        , method(method)
        , ms(method->getMethodSignature())
        , maxargs_br(NULL)
        , minargs_br(NULL)
    {
        this->method = method;
        this->ms = method->getMethodSignature();

        initCodeMgr(method->pool());
        frag = new (*lir_alloc) Fragment(0 verbose_only(, 0));
        LirBuffer* lirbuf = frag->lirbuf = new (*lir_alloc) LirBuffer(*lir_alloc);
        lirbuf->abi = ABI_CDECL;
        LirWriter* lirout = new (*alloc1) LirBufWriter(lirbuf, core->config.njconfig);
        verbose_only(
            if (verbose()) {
                lirbuf->printer = new (*lir_alloc) LInsPrinter(*lir_alloc, TR_NUM_USED_ACCS);
                lirbuf->printer->addrNameMap->addAddrRange(pool->core, sizeof(AvmCore), 0, "core");
            }
        )
        debug_only(
            lirout = validate2 = new (*alloc1) ValidateWriter(lirout, lirbuf->printer, "InvokerCompiler");
        )
        verbose_only(
            if (verbose()) {
                CodeMgr *codeMgr = method->pool()->codeMgr;
                core->console << "compileInvoker " << method << "\n";
                core->console <<
                    " required=" << ms->requiredParamCount() <<
                    " optional=" << (ms->param_count() - ms->requiredParamCount()) << "\n";
                lirout = new (*alloc1) VerboseWriter(*alloc1, lirout, lirbuf->printer, &codeMgr->log);
            }
        )
#if defined(NANOJIT_ARM)
        if (core->config.njconfig.soft_float)
        {
            lirout = new (*alloc1) SoftFloatFilter(lirout);
        }
#endif
        // add other LirWriters here
        this->lirout = lirout;
        emitStart(*alloc1, lirbuf, lirout);
    }

    // recipe for an invoke wrapper:
    //    Atom <generated invoker>(MethodEnv* env, int argc, Atom* argv) {
    //        1. check argc:            env->startCoerce(argc, env->get_ms());
    //        2. unbox args:            unboxCoerceArgs, unrolled for each arg
    //        3. return box(call(...))
    //    }

    // jit-compile an invoker for mi
    void InvokerCompiler::generate_lir()
    {
        // invoker params
        LIns* env_param = param(0, "env");
        LIns* argc_param = p2i(param(1, "argc"));
        LIns* args_param = param(2, "args");
        coreAddr = InsConstPtr(core);

        // if unboxing args will make them expand, allocate more space.
        int32_t rest_offset = ms->rest_offset();
        if (rest_offset > int32_t((ms->param_count()+1)*sizeof(Atom))) {
            AvmAssert(!method->needRestOrArguments());
            args_out = lirout->insAlloc(rest_offset);
        } else {
            // we can do in-place unboxing of args.
            args_out = args_param;
        }

        // 1. check argc
        emit_argc_check(argc_param);

        // 2. unbox & coerce args
        downcast_args(env_param, argc_param, args_param);

        // 3. call, box result, return atom
        call_method(env_param, argc_param);

        // error handler for argc error
        if (minargs_br || maxargs_br) {
            LIns* errlabel = label();
            if (minargs_br) minargs_br->setTarget(errlabel);
            if (maxargs_br) maxargs_br->setTarget(errlabel);
            callIns(FUNCTIONID(argcError), 2, env_param, argc_param);
        }

        // mark the endpoint of generated LIR with an instruction the Assembler allows at the end
        frag->lastIns = livep(env_param);

        // we're done with LIR generation, free up what we can.
        mmfx_delete(alloc1);
        alloc1 = NULL;
    }

    void InvokerCompiler::emit_argc_check(LIns* argc_param)
    {
        int min_argc = ms->requiredParamCount();
        int param_count = ms->param_count();
        if (min_argc == param_count && !ms->argcOk(param_count + 1)) {
            // exactly param_count args required
            // if (argc != param_count) goto error
            maxargs_br = jnei(argc_param, param_count);
        } else {
            if (!ms->argcOk(param_count+1)) {
                // extra params are not allowed, must check for max args
                // if (argc > param_count) goto error
                maxargs_br = jgti(argc_param, param_count);
            }
            if (min_argc > 0) {
                // at least 1 param is required, so check
                // if (argc < min_argc) goto error
                minargs_br = jlti(argc_param, min_argc);
            }
        }
    }

    void InvokerCompiler::downcast_arg(int i, int offset, LIns* env_param, LIns* args_param)
    {
        BuiltinType bt = ms->paramTraitsBT(i);
        if (bt != BUILTIN_any) {
            LIns* atom = ldp(args_param, i*sizeof(Atom), ACCSET_OTHER);
            downcast_and_store(atom, ms->paramTraits(i), env_param, offset);
        } else if (copyArgs()) {
            LIns* atom = ldp(args_param, i*sizeof(Atom), ACCSET_OTHER);
            lirout->insStore(atom, args_out, offset, ACCSET_OTHER);
        }
    }

    void InvokerCompiler::downcast_args(LIns* env_param, LIns* argc_param, LIns* args_param)
    {
        // the receiver arg (arg0) only needs to be unboxed, not coerced
        verbose_only( if (verbose())
            core->console << "unbox arg 0 " << ms->paramTraits(0) << "\n";
        )
        LIns* atom = ldp(args_param, 0, ACCSET_OTHER);
        LIns* native = atomToNative(ms->paramTraitsBT(0), atom);
        if (native != atom || copyArgs())
            lirout->insStore(native, args_out, 0, ACCSET_OTHER);
        int offset = argSize(ms, 0);

        // the required args need to be coerced and unboxed
        int i;
        int required_count = ms->requiredParamCount();
        for (i = 1; i <= required_count; i++) {
            verbose_only( if (verbose())
                core->console << "arg " << i << " " << ms->paramTraits(i) << "\n";
            )
            downcast_arg(i, offset, env_param, args_param);
            offset += argSize(ms, i);
        }

        // optional args also need coercing and unboxing when they're present
        int param_count = ms->param_count();
        if (required_count < param_count) {
            int optional_count = param_count - required_count;
            int branch_count = 0;
            LIns** branches = new (*alloc1) LIns*[optional_count];
            for (; i <= param_count; i++) {
                verbose_only( if (verbose())
                    core->console << "optional arg " << i << " " << ms->paramTraits(i) << "\n";
                )
                // if (argc < i) { goto done }
                branches[branch_count++] = jlti(argc_param, i);
                downcast_arg(i, offset, env_param, args_param);
                offset += argSize(ms, i);
            }
            if (branch_count > 0) {
                // done: patch all the optional-arg branches
                LIns* done_label = label();
                for (i = 0; i < branch_count; i++)
                    branches[i]->setTarget(done_label);
            }
        }
    }

    void* InvokerCompiler::assemble()
    {
        CodeMgr* codeMgr = method->pool()->codeMgr;

        verbose_only(if (pool->isVerbose(LC_Liveness, method)) {
            Allocator live_alloc;
            LirReader in(frag->lastIns);
            nanojit::live(&in, live_alloc, frag, &codeMgr->log);
        })

        // disable hardening features when compiling thunks
        nanojit::Config cfg = core->config.njconfig;
        cfg.harden_function_alignment = false;
        cfg.harden_nop_insertion = false;

        // Use the 'active' log if we are in verbose output mode otherwise sink the output
        LogControl* log = &(codeMgr->log);
        verbose_only(
            SinkLogControl sink;
            log = pool->isVerbose(VB_jit,method) ? log : &sink;
        )

        Assembler *assm = new (*lir_alloc) Assembler(codeMgr->codeAlloc, codeMgr->allocator, *lir_alloc, log, cfg);
        verbose_only( StringList asmOutput(*lir_alloc); )
        verbose_only( if (!pool->isVerbose(VB_raw, method)) assm->_outputCache = &asmOutput; )
        LirReader bufreader(frag->lastIns);
        assm->beginAssembly(frag);
        assm->assemble(frag, &bufreader);
        assm->endAssembly(frag);

        verbose_only(
            assm->_outputCache = 0;
            for (Seq<char*>* p = asmOutput.get(); p != NULL; p = p->tail)
                assm->outputf("%s", p->head);
        );
        if (!assm->error()) {
            if (jit_observer)
                jit_observer->notifyInvokerJITed(method, assm->codeList);
            if (method->isNative()) {
                PERFM_NVPROF("C++ invoker bytes", CodeAlloc::size(assm->codeList));
            } else {
                PERFM_NVPROF("JIT invoker bytes", CodeAlloc::size(assm->codeList));
            }
            return frag->code();
        } else {
            return NULL;
        }
    }

    void InvokerCompiler::call_method(LIns* env_param, LIns* argc_param)
    {
        // We know we've called the method at least once, so method->implGPR pointer is correct.
        CallInfo* call = (CallInfo*) lir_alloc->alloc(sizeof(CallInfo));
        call->_isPure = 0;
        call->_storeAccSet = ACCSET_STORE_ANY;
        call->_abi = ABI_FUNCTION;
        verbose_only( if (verbose()) {
            StUTF8String name(method->getMethodName());
            char *namestr = new (*lir_alloc) char[VMPI_strlen(name.c_str())+1];
            VMPI_strcpy(namestr, name.c_str());
            call->_name = namestr;
        })
        switch (ms->returnTraitsBT()) {
        case BUILTIN_number:
            call->_address = (uintptr_t) method->_implFPR;
            call->_typesig = SIG3(D,P,I,P);
            break;
        case BUILTIN_int: case BUILTIN_uint: case BUILTIN_boolean:
            call->_address = (uintptr_t) method->_implGPR;
            call->_typesig = SIG3(I,P,I,P);
            break;
#ifdef VMCFG_FLOAT
        case BUILTIN_float: 
                call->_address = (uintptr_t) method->_implFPR;
                call->_typesig = SIG3(F,P,I,P);
                break;
        case BUILTIN_float4: 
            call->_address = (uintptr_t) method->_implVECR;
            call->_typesig = SIG3(F4,P,I,P);
            break;
#endif // VMCFG_FLOAT
        default:
            call->_address = (uintptr_t) method->_implGPR;
            call->_typesig = SIG3(A,P,I,P);
            break;
        }
        LIns* result = callIns(call, 3, env_param, argc_param, args_out);
        livep(args_out);
        // box and return the result
        retp(nativeToAtom(result, ms->returnTraits()));
    }

    void InvokerCompiler::downcast_and_store(LIns* atom, Traits* t, LIns* env, int offset)
    {
        LIns *native;
        BuiltinType b = bt(t);
        switch (b) {
        case BUILTIN_object:
            // return (atom == undefinedAtom) ? nullObjectAtom : atom;
            native = choose(eqp(atom, undefinedAtom), nullObjectAtom, atom);
            break;
        case BUILTIN_int:
        case BUILTIN_uint:
            {
                //     int tag = atom & kAtomTypeMask;
                //     if(tag == kIntptrType)
                //        args[i] == atom >> kAtomTypeSize;
                //     else
                //        args[i] = isInt ? integer(atmo) : toUint32(atom);
                LIns* tag = andp(atom, AtomConstants::kAtomTypeMask);
                LIns* slow_br = lirout->insBranch(LIR_jf, eqp(tag, InsConstAtom(AtomConstants::kIntptrType)), NULL);
                LIns* ival = p2i(rshp(atom, AtomConstants::kAtomTypeSize));
                native = BUILTIN_int == b ? i2p(ival) : ui2p(ival);
                stp(native, args_out, offset, ACCSET_OTHER);
                LIns* fast_br = lirout->insBranch(LIR_j, NULL, NULL);
                slow_br->setTarget(label());
                native = BUILTIN_int == b ? i2p(callIns(FUNCTIONID(integer), 1, atom)) : ui2p(callIns(FUNCTIONID(toUInt32), 1, atom));
                stp(native, args_out, offset, ACCSET_OTHER);
                fast_br->setTarget(label());
                return;
            }
        case BUILTIN_number:
            {
                // Based on code from CodegenLIR::coerceToNumber.
                // * -> Number
#ifdef VMCFG_FASTPATH_FROMATOM
                //     double rslt;
                //     intptr_t val = CONVERT_TO_ATOM(arg);
                //     if ((val & kAtomTypeMask) != kIntptrType) goto not_intptr;   # test for kIntptrType tag
                //     # kIntptrType
                //     rslt = double(val >> kAtomTypeSize);                         # extract integer value and convert to double
                //     goto done;
                // not_intptr:
                //     if ((val & kAtomTypeMask) != kDoubleType) goto not_double;   # test for kDoubleType tag
                //     # kDoubleType
                //     rslt = *(val - kDoubleType);                                 # remove tag and dereference
                //     goto done;
                // not_double:
                //     rslt = number(val);                                          # slow path -- call helper
                // done:
                //     result = rslt;
                LIns* tag = andp(atom, AtomConstants::kAtomTypeMask);
                // kIntptrType
                LIns* not_intptr = lirout->insBranch(LIR_jf, eqp(tag, AtomConstants::kIntptrType), NULL);
                // Note that this works on 64bit platforms only if we are careful
                // to restrict the range of intptr values to those that fit within
                // the integer range of the double type.
                std(p2dIns(rshp(atom, AtomConstants::kAtomTypeSize)), args_out, offset, ACCSET_OTHER);
                LIns* fast_path_exit1 = lirout->insBranch(LIR_j, NULL, NULL);
                not_intptr->setTarget(label());
                // kDoubleType
                LIns* not_double = lirout->insBranch(LIR_jf, eqp(tag, AtomConstants::kDoubleType), NULL);
                std(ldd(subp(atom, AtomConstants::kDoubleType), 0, ACCSET_OTHER), args_out, offset, ACCSET_OTHER);
                LIns* fast_path_exit2 = lirout->insBranch(LIR_j, NULL, NULL);
                not_double->setTarget(label());
                std(callIns(FUNCTIONID(number), 1, atom), args_out, offset, ACCSET_OTHER);
                LIns* done = label();
                fast_path_exit1->setTarget(done);
                fast_path_exit2->setTarget(done);
                return;
#else
                native = callIns(FUNCTIONID(number), 1, atom);
#endif
            }            
            break;
#ifdef VMCFG_FLOAT
        case BUILTIN_float:    // TODO: I assume we don't need to inline here since we don't care about untyped performance; check assumption
            native = callIns(FUNCTIONID(singlePrecisionFloat), 1, atom); 
            break;
        case BUILTIN_float4: 
            callIns(FUNCTIONID(float4), 2, lea(offset, args_out), atom);
            return; // not break! float4 does the "store", too.
#endif            
        case BUILTIN_boolean:
            {
                LIns* tag = andp(atom, AtomConstants::kAtomTypeMask);
                LIns* not_bool = lirout->insBranch(LIR_jf, eqp(tag, AtomConstants::kBooleanType), NULL);
                stp(rshp(atom, AtomConstants::kAtomTypeSize), args_out, offset, ACCSET_OTHER);
                LIns* fast_path_exit = lirout->insBranch(LIR_j, NULL, NULL);
                not_bool->setTarget(label());
                stp(ui2p(callIns(FUNCTIONID(boolean), 1, atom)), args_out, offset, ACCSET_OTHER);
                fast_path_exit->setTarget(label());
                return;
            }
            break;
        case BUILTIN_string:
            native = callIns(FUNCTIONID(coerce_s), 2, InsConstPtr(t->core), atom);
            break;
        case BUILTIN_namespace:
            native = andp(callIns(FUNCTIONID(coerce), 3, env, atom, InsConstPtr(t)), ~AtomConstants::kAtomTypeMask);
            break;
        case BUILTIN_void:
        case BUILTIN_null:
        case BUILTIN_any:
            AvmAssert(false);
        default:
            native = downcast_obj(atom, env, t);
            break;
        }
        lirout->insStore(native, args_out, offset, ACCSET_OTHER);
    }

#ifdef NJ_VERBOSE
    bool InvokerCompiler::verbose()
    {
        return method->pool()->isVerbose(VB_jit, method);
    }
#endif

}
#endif // VMCFG_NANOJIT
