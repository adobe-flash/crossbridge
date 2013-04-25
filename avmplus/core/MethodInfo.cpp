/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "pcre.h"

//#define DOPROF
//#include "../vprof/vprof.h"

namespace avmplus
{
    using namespace MMgc;

    /**
     * MethodInfo wrapper around a system-generated init method.  Used when
     * there is no init method defined in the abc; this only occurs for activation
     * object traits and catch-block activation traits.
     */
    MethodInfo::MethodInfo(InitMethodStub, Traits* declTraits) :
        _msref(declTraits->pool->core->GetGC()->emptyWeakRef),
        _declarer(declTraits),
        _activation(NULL),
        _pool(declTraits->pool),
        _abc_info_pos(NULL),
        _method_id(-1),
        _hasMethodBody(1),
        _isResolved(1)
#ifdef VMCFG_FLOAT
        ,_has128bitLocals(0)
#endif // VMCFG_FLOAT
    {
        declTraits->core->exec->init(this, NULL);
    }

#ifdef VMCFG_AOT
    MethodInfo::MethodInfo(InitMethodStub, Traits* declTraits, const NativeMethodInfo* native_info, const AvmThunkNativeHandler& handler, int32_t method_id) :
        MethodInfoProcHolder(),
        _msref(declTraits->pool->core->GetGC()->emptyWeakRef),
        _declarer(declTraits),
        _activation(NULL),
        _pool(declTraits->pool),
        _abc_info_pos(NULL),
        _method_id(method_id),
        _hasMethodBody(1),
        _isResolved(1)
#ifdef VMCFG_FLOAT
        ,_has128bitLocals(0)
#endif // VMCFG_FLOAT
    {
        AvmAssert(native_info != NULL);
        this->_native.thunker = native_info->thunker;
        this->setAotCompiled(handler);

        declTraits->core->exec->init(this, native_info);
    }
#endif

    /**
     * ordinary MethodInfo for abc or native method.
     */
    MethodInfo::MethodInfo(int32_t method_id,
                            PoolObject* pool,
                            const uint8_t* abc_info_pos,
                            uint8_t abcFlags,
                            const NativeMethodInfo* native_info) :
        _msref(pool->core->GetGC()->emptyWeakRef),
        _declarer(NULL),
        _activation(NULL),
        _pool(pool),
        _abc_info_pos(abc_info_pos),
        _method_id(method_id),
        _needArguments((abcFlags & abcMethod_NEED_ARGUMENTS) != 0),
        _needActivation((abcFlags & abcMethod_NEED_ACTIVATION) != 0),
        _needRest((abcFlags & abcMethod_NEED_REST) != 0),
        _hasOptional((abcFlags & abcMethod_HAS_OPTIONAL) != 0),
        _ignoreRest((abcFlags & abcMethod_IGNORE_REST) != 0),
        _isNative((abcFlags & abcMethod_NATIVE) != 0),
        _setsDxns((abcFlags & abcMethod_SETS_DXNS) != 0),
        _hasParamNames((abcFlags & abcMethod_HAS_PARAM_NAMES) != 0),
        _hasMethodBody(1) // assume we have bytecode body until set otherwise
#ifdef VMCFG_FLOAT
        ,_has128bitLocals(0)
#endif // VMCFG_FLOAT
    {
        AvmAssert(method_id >= 0);

        if (native_info)
        {
            _needsCodeContext = 1;
            _needsDxns = 1;
            _hasMethodBody = 0;
        }
        pool->core->exec->init(this, native_info);
    }

    void MethodInfo::gcTraceHook_MethodInfo(MMgc::GC* gc)
    {
        if (_isNative) {
            // _native - nothing to do
        }
        else {
            gc->TraceLocation(&_abc.exceptions);
#ifdef VMCFG_WORDCODE
            gc->TraceLocation(&_abc.word_code.translated_code);
            gc->TraceLocation(&_abc.word_code.exceptions);
#endif
        }
    }

    void MethodInfo::init_activationTraits(Traits* t)
    {
        AvmAssert(_activation.getTraits() == NULL);
        _activation.setTraits(pool()->core->GetGC(), this, t);
    }

    void MethodInfo::init_activationScope(const ScopeTypeChain* scope)
    {
        AvmAssert(!_activation.hasScope());
        _activation.setScope(pool()->core->GetGC(), this, scope);
    }

#ifdef VMCFG_FLOAT
    REALLY_INLINE float unpack_float(const void* src)
    {
        return *(const float*)src;
    }

    REALLY_INLINE float4_t unpack_float4(const void* src){
        return AvmThunkUnbox_FLOAT4(float4_t,*(const Atom*)src);
    }
#endif // VMCFG_FLOAT

    REALLY_INLINE
    void MethodInfo::_installRefToResolvedMethodSignature(MethodSignature* ms)
    {
        AvmAssert(isResolved());
        AvmAssert(_msref->isNull());
        _msref = ms->GetWeakRef();
        PoolObject* pool = this->pool();
        AvmCore* core = pool->core;
        core->msCache()->add(ms);
    }

    // note that the "local" can be a true local (0..local_count-1)
    // or an entry on the scopechain (local_count...(local_count+max_scope)-1)
    // FIXME: this is exactly the same as makeatom() in jit-calls.h,
    // except for the decoding of unaligned doubles
    Atom nativeLocalToAtom(AvmCore* core, void* src, SlotStorageType sst)
    {
        switch (sst)
        {
            case SST_double:
                return core->doubleToAtom(unpack_double(src));
#ifdef VMCFG_FLOAT
            case SST_float:
                return core->floatToAtom(unpack_float(src));
            case SST_float4:
                return core->float4ToAtom(unpack_float4(src));
#endif // VMCFG_FLOAT
            default:
                AvmAssert(false);
                // fall through.
            case SST_int32:
                return core->intToAtom(*(const int32_t*)src);
            case SST_uint32:
                return core->uintToAtom(*(const uint32_t*)src);
            case SST_bool32:
                return *(const int32_t*)src ? trueAtom : falseAtom;
            case SST_atom:
                return *(const Atom*)src;
            case SST_string:
                return (*(const Stringp*)src)->atom();
            case SST_namespace:
                return (*(const Namespacep*)src)->atom();
            case SST_scriptobject:
                return (*(ScriptObject**)src)->atom();
        }
    }

    // this looks deceptively similar to nativeLocalToAtom, but is subtly different:
    // for locals, int/uint/bool are always stored as a 32-bit value in the 4 bytes
    // of the memory slot (regardless of wordsize and endianness);
    // for args, int/uint/bool are always expanded to full intptr size. In practice
    // this only makes a difference on big-endian 64-bit systems (eg PPC64) which makes
    // it a hard bug to notice.
    static Atom nativeArgToAtom(AvmCore* core, void* src, BuiltinType bt)
    {
        switch (bt)
        {
#ifdef VMCFG_FLOAT
            case BUILTIN_float:
                return core->floatToAtom(unpack_float(src));
            case BUILTIN_float4:
                return core->float4ToAtom(unpack_float4(src));
#endif // VMCFG_FLOAT
            case BUILTIN_number:
            {
                return core->doubleToAtom(unpack_double(src));
            }
            case BUILTIN_int:
            {
                return core->intToAtom((int32_t)*(const intptr_t*)src);
            }
            case BUILTIN_uint:
            {
                return core->uintToAtom((uint32_t)*(const uintptr_t*)src);
            }
            case BUILTIN_boolean:
            {
                return *(const intptr_t*)src ? trueAtom : falseAtom;
            }
            case BUILTIN_any:
            case BUILTIN_object:
            case BUILTIN_void:
            {
                return *(const Atom*)src;
            }
            case BUILTIN_string:
            {
                return (*(const Stringp*)src)->atom();
            }
            case BUILTIN_namespace:
            {
                return (*(const Namespacep*)src)->atom();
            }
            default:
            {
                return (*(ScriptObject**)src)->atom();
            }
        }
    }

#ifdef DEBUGGER

    /*static*/ DebuggerMethodInfo* DebuggerMethodInfo::create(AvmCore* core, int32_t local_count, uint32_t codeSize, int32_t max_scopes)
    {
        MMgc::GC* gc = core->GetGC();
        const uint32_t extra = (local_count <= 1) ? 0 : (sizeof(Stringp)*(local_count-1));

        DebuggerMethodInfo* dmi = new (gc, MMgc::kExact, extra) DebuggerMethodInfo(local_count, codeSize, max_scopes);
        const Stringp undef = core->kundefined;
        for (int32_t i=0; i<local_count; i++)
        {
            WBRC(gc, dmi, &dmi->localNames[i], undef);
        }
        return dmi;
    }

    AbcFile* MethodInfo::file() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? (AbcFile*)(dmi->file) : NULL;
    }

    int32_t MethodInfo::firstSourceLine() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->firstSourceLine : 0;
    }

    int32_t MethodInfo::lastSourceLine() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->lastSourceLine : 0;
    }

    int32_t MethodInfo::offsetInAbc() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->offsetInAbc : 0;
    }

    uint32_t MethodInfo::codeSize() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->codeSize : 0;
    }

    int32_t MethodInfo::local_count() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->local_count : 0;
    }

    int32_t MethodInfo::max_scopes() const
    {
        DebuggerMethodInfo* dmi = this->dmi();
        return dmi ? dmi->max_scopes : 0;
    }

    void MethodInfo::setFile(AbcFile* file)
    {
        DebuggerMethodInfo* dmi = this->dmi();
        if (dmi)
            dmi->file = file;
    }

    Stringp MethodInfo::getArgName(int32_t index)
    {
        return getRegName(index);
    }

    Stringp MethodInfo::getLocalName(int32_t index)
    {
        return getRegName(index+getMethodSignature()->param_count());
    }

    void MethodInfo::updateSourceLines(int32_t linenum, int32_t offset)
    {
        DebuggerMethodInfo* dmi = this->dmi();
        if (dmi)
        {
            if (dmi->firstSourceLine == 0 || linenum < dmi->firstSourceLine)
                dmi->firstSourceLine = linenum;

            if (dmi->offsetInAbc == 0 || offset < dmi->offsetInAbc)
                dmi->offsetInAbc = offset;

            if (dmi->lastSourceLine == 0 || linenum > dmi->lastSourceLine)
                dmi->lastSourceLine = linenum;
        }
    }

    // Note: dmi() can legitimately return NULL (for MethodInfo's that were synthesized by Traits::genInitBody,
    // or for MethodInfo's that have no body, e.g. interface methods).
    DebuggerMethodInfo* MethodInfo::dmi() const
    {
        // rely on the fact that not-in-pool MethodInfo returns -1,
        // which will always be > methodCount as uint32_t
        const uint32_t method_id = uint32_t(this->method_id());
        AvmAssert(_pool->core->debugger() != NULL);
        // getDebuggerMethodInfo quietly returns NULL for out-of-range.
        return _pool->getDebuggerMethodInfo(method_id);
    }

    Stringp MethodInfo::getRegName(int32_t slot) const
    {
        DebuggerMethodInfo* dmi = this->dmi();

        if (dmi && slot >= 0 && slot < dmi->local_count)
            return dmi->localNames[slot];

        return this->pool()->core->kundefined;
    }

    void MethodInfo::setRegName(int32_t slot, Stringp name)
    {
        DebuggerMethodInfo* dmi = this->dmi();

        if (!dmi || slot < 0 || slot >= dmi->local_count)
            return;

        AvmCore* core = this->pool()->core;

        // [mmorearty 5/3/05] temporary workaround for bug 123237: if the register
        // already has a name, don't assign a new one
        if (dmi->localNames[slot] != core->kundefined)
            return;

        WBRC(core->GetGC(), dmi, &dmi->localNames[slot], core->internString(name));
    }

    // note that the "local" can be a true local (0..local_count-1)
    // or an entry on the scopechain (local_count...(local_count+max_scope)-1)
    Atom MethodInfo::boxOneLocal(FramePtr src, int32_t srcPos, const uint8_t* sstArr)
    {
        // if we are running jit then the types are native and we need to box them.
        if (_isJitImpl)
        {
            src = FramePtr(uintptr_t(src) + (srcPos << VARSHIFT(this)));
            AvmAssert(sstArr != NULL);
            return nativeLocalToAtom(this->pool()->core, src, (SlotStorageType)sstArr[srcPos]);
        }
        else
        {
            AvmAssert(sstArr == NULL);
            src = FramePtr(uintptr_t(src) + srcPos*sizeof(Atom));
            return *(const Atom*)src;
        }
    }


    /**
     * convert ap[start]...ap[start+count-1] entries from their native types into
     * Atoms.  The result is placed into out[to]...out[to+count-1].
     *
     * The traitArr is used to determine the type of conversion that should take place.
     * traitArr[start]...traitArr[start+count-1] are used.
     *
     * If the method is interpreted then we just copy the Atom, no conversion is needed.
     */
    void MethodInfo::boxLocals(FramePtr src, int32_t srcPos, const uint8_t* sstArr, Atom* dest, int32_t destPos, int32_t length)
    {
        for (int32_t i = srcPos, n = srcPos+length; i < n; i++)
        {
#ifdef VMCFG_AOT
            AvmAssert(i >= 0 && (dmi() == NULL || i < local_count()));
#else
            AvmAssert(i >= 0 && i < getMethodSignature()->local_count());
#endif
            dest[destPos++] = boxOneLocal(src, i, sstArr);
        }
    }


    // note that the "local" can be a true local (0..local_count-1)
    // or an entry on the scopechain (local_count...(local_count+max_scope)-1)
    void MethodInfo::unboxOneLocal(Atom src, FramePtr dst, int32_t dstPos, const uint8_t* sstArr)
    {
        if (_isJitImpl)
        {
            AvmAssert(sstArr != NULL);
            dst = FramePtr(uintptr_t(dst) + (dstPos << VARSHIFT(this)));
            switch ((SlotStorageType)sstArr[dstPos])
            {
                case SST_double:
                {
                    *(double*)dst = AvmCore::number_d(src);
                    break;
                }
#ifdef VMCFG_FLOAT
                case SST_float:
                {
                    *(float*)dst =  AvmCore::atomToFloat(src);
                    break;
                }
                case SST_float4:
                {
                    AvmAssert( (((uintptr_t)dst) & 0xf) == 0); // should be 16-byte aligned.
                    *(float4_t*)dst =  AvmCore::atomToFloat4(src);
                    break;
                }
#endif // VMCFG_FLOAT
                case SST_int32:
                {
                    *(int32_t*)dst = AvmCore::integer_i(src);
                    break;
                }
                case SST_uint32:
                {
                    *(uint32_t*)dst = AvmCore::integer_u(src);
                    break;
                }
                case SST_bool32:
                {
                    *(int32_t*)dst = (int32_t)atomGetBoolean(src);
                    break;
                }
                case SST_atom:
                {
                    *(Atom*)dst = src;
                    break;
                }
                default: // SST_string, SST_namespace, SST_scriptobject
                {
                    // ScriptObject, String, Namespace, or Null
                    *(void**)dst = atomPtr(src);
                    break;
                }
            }
        }
        else
        {
            AvmAssert(sstArr == NULL);
            dst = FramePtr(uintptr_t(dst) + dstPos*sizeof(Atom));
            *(Atom*)dst = src;
        }
    }

    /**
     * convert in[0]...in[count-1] entries from Atoms to native types placing them
     * in ap[start]...out[start+count-1].
     *
     * The traitArr is used to determine the type of conversion that should take place.
     * traitArr[start]...traitArr[start+count-1] are used.
     *
     * If the method is interpreted then we just copy the Atom, no conversion is needed.
     */
    void MethodInfo::unboxLocals(const Atom* src, int32_t srcPos, const uint8_t* sstArr, FramePtr dest, int32_t destPos, int32_t length)
    {
        for (int32_t i = destPos, n = destPos+length; i < n; i++)
        {
#ifdef VMCFG_AOT
            AvmAssert(i >= 0 && (dmi() == NULL || i < local_count()));
#else
            AvmAssert(i >= 0 && i < getMethodSignature()->local_count());
#endif
            unboxOneLocal(src[srcPos++], dest, i, sstArr);
        }
    }

#endif //DEBUGGER

    PrintWriter& MethodInfo::print(PrintWriter& prw) const
    {
        String* n = getMethodName();
        return n ? prw << n << "()"
                 : prw << "?()";
    }

    bool MethodInfo::makeMethodOf(Traits* traits)
    {
        AvmAssert(!isResolved());
// begin AVMPLUS_UNCHECKED_HACK
        AvmAssert(!_isProtoFunc);
// end AVMPLUS_UNCHECKED_HACK

        _hasMakeMethodOfBeenCalled = 1;

        if (_declarer.getTraits() == NULL)
        {
            _declarer.setTraits(pool()->core->GetGC(), this, traits);
            _needClosure = 1;
            return true;
        }
        else
        {
            #ifdef AVMPLUS_VERBOSE
            if (pool()->isVerbose(VB_parse))
                pool()->core->console << "WARNING: method " << this << " was already bound to " << declaringTraits() << "\n";
            #endif

            return false;
        }
    }

    void MethodInfo::makeIntoPrototypeFunction(const Toplevel* toplevel, const ScopeTypeChain* fscope)
    {
        // Once a MethodInfo has been made a method of a particular class,
        // it is an error to attempt to use it for OP_newfunction; we may have
        // jitted code with the method and made early-binding assumptions about the
        // receiver type that this would invalidate (by allowing us to call the method
        // with a different type of object).
        if (_hasMakeMethodOfBeenCalled)
            toplevel->throwVerifyError(kCorruptABCError);

        AvmAssert(_declarer.getScope() == NULL);

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=706473
        // Set scope after resolveSignature; latter may fail to verify
        // and such failures should be effect-free (as much as possible).
#else
        _declarer.setScope(toplevel->core()->GetGC(), this, fscope);
#endif

// begin AVMPLUS_UNCHECKED_HACK
        this->_isProtoFunc = 1;
// end AVMPLUS_UNCHECKED_HACK

        // make sure param & return types are fully resolved.
        // this does not set the verified flag, so real verification will
        // still happen before the function runs the first time.
        resolveSignature(toplevel);

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=706473
        // Signature resolved successfully; now safe to set declaring scope.
        _declarer.setScope(toplevel->core()->GetGC(), this, fscope);
#endif
    }

    /**
     * convert native args to atoms.  argc is the number of
     * args, not counting the instance which is arg[0].  the
     * layout is [instance][arg1..argN]
     */
    void MethodSignature::boxArgs(AvmCore* core, int32_t argc, const uint32_t* ap, Atom* out) const
    {
        MMGC_STATIC_ASSERT(sizeof(Atom) == sizeof(void*));  // if this ever changes, this function needs smartening
        typedef const Atom* ConstAtomPtr;
        // box the typed args, up to param_count
        const int32_t param_count = this->param_count();
        for (int32_t i=0; i <= argc; i++)
        {
            const BuiltinType bt = (i <= param_count) ? this->paramTraitsBT(i) : BUILTIN_any;
            out[i] = nativeArgToAtom(core, (void*)ap, bt);
            ap += (bt == BUILTIN_number ? sizeof(double) : sizeof(Atom)) / sizeof(int32_t);
        }
    }

    MethodSignature* FASTCALL MethodInfo::_buildMethodSignature(const Toplevel* toplevel)
    {
        PoolObject* pool = this->pool();
        AvmCore* core = pool->core;
        GC* gc = core->GetGC();

        const uint8_t* pos = this->_abc_info_pos;
        const uint32_t param_count = pos ? AvmCore::readU32(pos) : 0;
        uint32_t optional_count = 0;
        uint32_t rest_offset = 0;
        Traits* returnType;
        Traits* receiverType;

        // we only need param_count+optional_count extra, but we don't know
        // optional_count yet, so over-allocate for the worst case. (we know optional_count<=param_count).
        // this wastes space, but since we only cache a few dozen of these, it's preferable to walking the data twice
        // to get an exact count.
        //
        // It is /critical/ to correct exact tracing of the MethodSignature that param_count is written
        // into the object before any types are written into _args, and that optional_count is written
        // into the object before any default values are written into _args.

        const uint32_t extra = sizeof(MethodSignature::AtomOrType) * (param_count + (hasOptional() ? param_count : 0));
        MethodSignature* ms = MethodSignature::create(gc, extra, param_count);

        if (pos)
        {
            returnType = pool->resolveTypeName(pos, toplevel, /*allowVoid=*/true);
            receiverType = _needClosure ? declaringTraits() : core->traits.object_itraits;
            rest_offset = argSize(receiverType);
// begin AVMPLUS_UNCHECKED_HACK
            uint32_t untyped_args = 0;
// end AVMPLUS_UNCHECKED_HACK
            for (uint32_t i=1; i <= param_count; i++)
            {
                Traits* argType = pool->resolveTypeName(pos, toplevel);
                WB(gc, ms, &ms->_args[i].paramType, argType);
                rest_offset += argSize(argType);
// begin AVMPLUS_UNCHECKED_HACK
                untyped_args += (argType == NULL);
// end AVMPLUS_UNCHECKED_HACK
            }
            AvmCore::skipU32(pos); // name_index;
            pos++; // abcFlags;
// begin AVMPLUS_UNCHECKED_HACK
            if (untyped_args == param_count)
                _onlyUntypedParameters = 1;
            // toplevel!=NULL check is so we only check when resolveSignature calls us (not subsequently)
            if (toplevel != NULL && _isProtoFunc)
            {
                // HACK - compiler should do this, and only to toplevel functions
                // that meet the E4 criteria for being an "unchecked function"
                // the tests below are too loose

                // if all params and return types are Object then make all params optional=undefined
                if (param_count == 0)
                    _ignoreRest = 1;
                if (!hasOptional() && returnType == NULL && param_count > 0 && untyped_args == param_count)
                {
                    _hasOptional = 1;
                    _ignoreRest = 1;
                    _isUnchecked = 1;
                    // oops -- the ms we allocated is too small, has no space for optional values
                    // but it's easy to re-create, especially since we know the types are all null
                    const uint32_t extra = sizeof(MethodSignature::AtomOrType) * (param_count + param_count);
                    ms = MethodSignature::create(gc, extra, param_count);
                    // don't need to re-set paramTypes: they are inited to NULL which is the right value
                }
            }
            if (_isUnchecked)
            {
                optional_count = param_count;
                ms->_optional_count = optional_count;
                for (uint32_t j=0; j < optional_count; j++)
                {
                    //WBATOM(gc, ms, &ms->_args[param_count+1+j].defaultValue, undefinedAtom);
                    // since we know we're going from NULL->undefinedAtom we can skip the WBATOM call
                    ms->_args[param_count+1+j].defaultValue = undefinedAtom;
                }
            }
            else
// end AVMPLUS_UNCHECKED_HACK
            if (hasOptional())
            {
                optional_count = AvmCore::readU32(pos);
                ms->_optional_count = optional_count;
                for (uint32_t j=0; j < optional_count; j++)
                {
                    const int32_t param = param_count-optional_count+1+j;
                    const int32_t index = AvmCore::readU32(pos);
                    CPoolKind kind = (CPoolKind)*pos++;

                    // check that the default value is legal for the param type
                    Traits* argType = ms->_args[param].paramType;
                    const Atom value = pool->getLegalDefaultValue(toplevel, index, kind, argType);
                    WBATOM(gc, ms, &ms->_args[param_count+1+j].defaultValue, value);
                }
            }

            if (!isNative())
            {
                const uint8_t* body_pos = this->abc_body_pos();
                if (body_pos)
                {
                    ms->_max_stack = AvmCore::readU32(body_pos);
                    ms->_local_count = AvmCore::readU32(body_pos);
                    const int32_t init_scope_depth = AvmCore::readU32(body_pos);
                    ms->_max_scope = AvmCore::readU32(body_pos) - init_scope_depth;
                #ifdef VMCFG_WORDCODE
                #else
                    ms->_abc_code_start = body_pos;
                    AvmCore::skipU32(ms->_abc_code_start); // code_length
                #endif
                }
            }
        }
        else
        {
            // this is a synthesized MethodInfo from genInitBody
            AvmAssert(param_count == 0);
            rest_offset = sizeof(Atom);
            returnType = pool->core->traits.void_itraits;
            receiverType = declaringTraits();
            // values derived from Traits::genInitBody()
            const int32_t max_stack = 2;
            const int32_t local_count = 1;
            const int32_t init_scope_depth = 1;
            const int32_t max_scope_depth = 1;
            ms->_max_stack = max_stack;
            ms->_local_count = local_count;
            ms->_max_scope = max_scope_depth - init_scope_depth;
        #if defined(VMCFG_WORDCODE) || defined(VMCFG_AOT)
        #else
            ms->_abc_code_start = this->abc_body_pos();
            AvmCore::skipU32(ms->_abc_code_start, 5);
        #endif
        }
        ms->_frame_size = ms->_local_count + ms->_max_scope + ms->_max_stack;
        #ifndef AVMPLUS_64BIT
        // The interpreter wants this to be padded to a doubleword boundary because
        // it allocates two objects in a single alloca() request - the frame and
        // auxiliary storage, in that order - and wants the second object to be
        // doubleword aligned.
        ms->_frame_size = (ms->_frame_size + 1) & ~1;
        #endif
        // _param_count and _optional_count were written earlier for reasons of GC correctness
        //ms->_param_count = param_count;
        //ms->_optional_count = optional_count;
        ms->_rest_offset = rest_offset;
        ms->_isNative = this->_isNative;
        ms->_allowExtraArgs = this->_needRest || this->_needArguments || this->_ignoreRest;
        WB(gc, ms, &ms->_returnTraits, returnType);
        WB(gc, ms, &ms->_args[0].paramType, receiverType);

        return ms;
    }

    MethodSignature* FASTCALL MethodInfo::_getMethodSignature()
    {
        AvmAssert(this->isResolved());
        // note: MethodSignaturep are always built the first time in resolveSignature; this is only
        // executed for subsequent re-buildings. Thus we pass NULL for toplevel (it's only used
        // for verification errors, but those will have been caught prior to this) and for
        // abcGen and imtBuilder (since those only need to be done once).
        MethodSignature* ms = _buildMethodSignature(NULL);

        _installRefToResolvedMethodSignature(ms);

        return ms;
    }

    void MethodInfo::update_max_stack(int32_t max_stack)
    {
        MethodSignature* ms = (MethodSignature*)_msref->get();
        if (ms)
        {
            ms->_max_stack = max_stack;
        }
    }

    uint32_t MethodInfo::parse_code_length() const
    {
        // Parse the code length out of ABC sice it is not stored elsewhere.
        const uint8_t* pos = abc_body_pos();
        AvmCore::skipU32(pos, 4);
        return AvmCore::readU32(pos);
    }

    void MethodInfo::resolveSignature(const Toplevel* toplevel)
    {
        if (!isResolved())
        {
            MethodSignature* ms = _buildMethodSignature(toplevel);

            if (!isNative())
            {
                if (ms->frame_size() < 0 || ms->local_count() < 0 || ms->max_scope() < 0)
                    toplevel->throwVerifyError(kCorruptABCError);
            }

            if (ms->paramTraits(0) != NULL && ms->paramTraits(0)->isInterface())
                _hasMethodBody = 0;

            _isResolved = 1;
            _installRefToResolvedMethodSignature(ms);

            pool()->core->exec->notifyMethodResolved(this, ms);
        }
    }

#ifdef DEBUGGER
    uint64_t MethodInfo::bytesUsed()
    {
        uint64_t size = sizeof(MethodInfo);
        size += getMethodSignature()->param_count() * 2 * sizeof(Atom);
        size += codeSize();
        return size;
    }
#endif

    bool MethodInfo::usesCallerContext() const
    {
        return pool()->isBuiltin && (!_isNative || _needsCodeContext);
    }

    // Builtin + non-native functions always need the dxns code emitted
    // Builtin + native functions have flags to specify if they need the dxns code
    bool MethodInfo::usesDefaultXmlNamespace() const
    {
        return pool()->isBuiltin && (!_isNative || _needsDxns);
    }

    bool MethodInfo::isTrivial()
    {
        if (_isTrivial)
            return true;
        if (_isNonTrivial)
            return false;
        bool trivial = computeIsTrivial();
        if (trivial)
            _isTrivial = 1;
        else
            _isNonTrivial = 1;
        return trivial;
    }

    // Compute a conservative approximation to triviality.  It is possible to
    // do better.  For example, IFTRUE and IFFALSE don't throw exceptions but
    // termination is assured only if all branches are forward; also, RETURNVALUE
    // is acceptable if data flow and type analyses shows that the returned value
    // will not trigger a coercion exception.  (That's true for many instructions
    // generally.)

    bool MethodInfo::computeIsTrivial()
    {
#ifdef VMCFG_AOT
        return false;
#else
        // Initial attribute guard:
        //
        // _needRest and _needArguments are here because they invoke
        // the Array constructor, ie, they may update system state.
        //
        // _isNative and _hasMethodBody are here because those methods have
        // no bytecode, and this computation is bytecode-based.
        //
        // _setsDxns is here because the "default xml namespace" semantics
        // are wild & crazy and some testing suggests that all bets are
        // off when using it.  (Belt and suspenders, belt and suspenders...)
        //
        // _needActivation is here mostly as a shortcut; it will usually pair
        // with NEWACTIVATION and GETPROPERTY and so on, and so the method will
        // be tossed out anyway.
        if (_needActivation ||
            _needRest ||
            _needArguments ||
            _isNative ||
            _setsDxns ||
            !_hasMethodBody)
            return false;

        const uint8_t* pc = _abc.body_pos;
        AvmCore::skipU32(pc, 4);
        uint32_t code_length = AvmCore::readU32(pc);
        const uint8_t* code_end = pc + code_length;

        while (pc < code_end) {
            const uint8_t* nextpc = pc;
            uint32_t imm30=0, imm30b=0;
            int32_t imm8=0, imm24=0;

            AbcOpcode opcode = (AbcOpcode) *pc;
            AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);

            // We may want to fold this logic into the table in ActionBlockConstants.cpp
            // rather than having a dedicated switch.

            switch (opcode) {
                case OP_nop:
                case OP_kill:
                case OP_label:
                case OP_popscope:
                case OP_pushnull:
                case OP_pushundefined:
                case OP_pushbyte:
                case OP_pushshort:
                case OP_pushtrue:
                case OP_pushfalse:
                case OP_pushnan:
                case OP_pop:
                case OP_dup:
                case OP_swap:
                case OP_pushstring:
                case OP_pushint:
                case OP_pushuint:
                case OP_pushdouble:
                case OP_pushscope:
                case OP_pushnamespace:
                case OP_returnvoid:
                case OP_getlocal:
                case OP_getlocal0:
                case OP_getlocal1:
                case OP_getlocal2:
                case OP_getlocal3:
                case OP_setlocal:
                case OP_setlocal0:
                case OP_setlocal1:
                case OP_setlocal2:
                case OP_setlocal3:
                case OP_getglobalscope:
                case OP_getscopeobject:
                case OP_getouterscope:
                case OP_getglobalslot:
                case OP_typeof:
                case OP_not:
                    pc = nextpc;
                    break;
                case OP_bkpt:
                case OP_debug:
                case OP_debugline:
                case OP_debugfile:
                case OP_bkptline:
                case OP_timestamp:
#ifdef DEBUGGER
                    // Debugging instructions are nontrivial if the debugger
                    // is present, we don't want to confuse the programmer.
                    if (pool()->core->debugger() != NULL)
                        return false;
#endif
                    pc = nextpc;
                    break;
                default:
                    return false;
            }
        }
        return true;
#endif
    }

    bool MethodInfo::isConstructor() const
    {
        return declaringTraits()->init == this;
    }

    Stringp MethodInfo::getMethodName(bool includeAllNamespaces) const
    {
        Stringp methodName = NULL;

#ifdef AVMPLUS_SAMPLER
        // We cache method names, because the profiler requests them over and
        // over.  (Bug 2547382)
        methodName = _methodName;
#endif

        if (!methodName)
        {
            Traits* declaringTraits = this->declaringTraits();

            methodName = getMethodNameWithTraits(declaringTraits, includeAllNamespaces);

#ifdef AVMPLUS_SAMPLER
            Sampler* sampler = declaringTraits ? declaringTraits->core->get_sampler() : NULL;
            if (sampler && sampler->sampling())
                _methodName = methodName;
#endif
        }

        return methodName;
    }

    Stringp MethodInfo::getMethodNameWithTraits(Traits* t, bool includeAllNamespaces) const
    {
        Stringp name = NULL;
        const int32_t method_id = this->method_id();

        PoolObject* pool = this->pool();
        AvmCore* core = pool->core;
        if (core->config.methodNames)
        {
            name = pool->getMethodInfoName(method_id);
            if (name && name->length() == 0)
            {
                name = core->kanonymousFunc;
            }

            if (t)
            {
                StringBuffer sb(core);
                t->print(sb, includeAllNamespaces);
                Stringp tname = sb.toString();
                if (core->config.oldVectorMethodNames)
                {
                    // Tamarin used to incorrectly return the internal name of these
                    // Vector types rather than the "official" name due to initialization
                    // order. Names on the left are "more correct" but old builds might
                    // require the "classic" name for compatibility purposes, so check.
                    struct NameMapRec { const char* n; const char* o; };
                    static const NameMapRec kNameMap[4] =
                    {
                        { "Vector.<Number>", "Vector$double" },
                        { "Vector.<int>", "Vector$int" },
                        { "Vector.<uint>", "Vector$uint" },
                        { "Vector.<*>", "Vector$object" },
                    };
                    for (int32_t i = 0; i < 4; ++i)
                    {
                        if (tname->equalsLatin1(kNameMap[i].n))
                        {
                            tname = core->newConstantStringLatin1(kNameMap[i].o);
                            break;
                        }
                    }
                };

                if (this == t->init)
                {
                    // careful, name could be null, that's ok for init methods
                    if (t->posType() == TRAITSTYPE_SCRIPT)
                    {
                        name = tname->appendLatin1("$init");
                    }
                    else if (t->posType() == TRAITSTYPE_CLASS)
                    {
                        name = tname->appendLatin1("cinit");
                    }
                    else
                    {
                        AvmAssert(t->isInstanceType() || t->isActivationTraits());
                        name = tname;
                    }
                }
                else if (name)
                {
                    const char* sep;
                    if (_isGetter)
                        sep = "/get ";
                    else if (_isSetter)
                        sep = "/set ";
                    else
                        sep = "/";
                    name = tname->appendLatin1(sep)->append(name);
                }
            }
        }

        // if config.methodNames isn't set, might as well still return a non-null result
        if (name == NULL)
            name = core->concatStrings(core->newConstantStringLatin1("MethodInfo-"), core->intToString(method_id));

        return name;
    }

    void MethodSignature::gcTraceHook_MethodSignature(MMgc::GC* gc)
    {
        int32_t numitems = 1+_param_count+_optional_count;
        for ( int32_t i=0 ; i < numitems ; i++ ) {
            if (atomKind(_args[i].defaultValue) == 0)
                gc->TraceLocation(&_args[i].paramType);
        }
    }

    /**
     * MethodRecognizer and the associated helper functions
     */
    #ifdef AVMPLUS_VERBOSE
    // PrinterWriter interface
    PrintWriter& MethodRecognizer::print(PrintWriter& prw) const { AvmAssert(0); return prw; }

    PrintWriter& MethodNameRecognizer::print(PrintWriter& prw) const
    {
        prw << "methods with a name that exactly matches '";
        prw.writeN(_pattern, _length);
        prw << "'";
        return prw;
    }

    PrintWriter& MethodNameRegExRecognizer::print(PrintWriter& prw) const
    {
        prw << "methods with names matching '";
        prw.write(_pattern);
        prw << "'";
        return prw;
    }

    PrintWriter& MethodIdRecognizer::print(PrintWriter& prw) const
    {
        prw << "methods processed in the range ";
        if (_low) prw << (uint64_t)_low;
        prw << "-";
        if (_high) prw << (uint64_t)_high;
        return prw;
    }
    #endif

    #if defined(VMCFG_COMPILEPOLICY) || defined(AVMPLUS_VERBOSE)
    // Exact string match
    MethodNameRecognizer::MethodNameRecognizer(const char* pattern, size_t length)
        : _pattern(pattern)
        , _length(length)
    {}

    bool MethodNameRecognizer::matches(const MethodInfo* m) const
    {
        String* nm = m->getMethodName();
        bool match = nm->equalsLatin1(_pattern, (int32_t)_length);
        return match;
    }

    // RegEx string match on the name
    MethodNameRegExRecognizer::MethodNameRegExRecognizer(const char* pattern, size_t length)
    {
        char* str = new char[length + 1];
        VMPI_strncpy(str, pattern, length);
        str[length]='\0';
        _pattern = str;
        _invalid = false;

        int options = PCRE_UTF8;
        int error, errOffset;
        const char* errStr;
        _regex = (void*)pcre_compile2(_pattern, options, &error, &errStr, &errOffset, NULL);

        // effective but not so pretty error handling.
        if (_regex == NULL)
        {
            delete _pattern;

            size_t errLen = (errStr == NULL) ? 0 : VMPI_strlen(errStr);
            const char* msg = "*** REGULAR EXPRESSION PARSE ERROR *** : ";
            const char* msgcont = " in : ";
            str = new char[VMPI_strlen(msg) + errLen + VMPI_strlen(msgcont) + length + 1];
            VMPI_strcpy(str, msg);
            (errLen) ? VMPI_strcat(str, errStr) : 0;
            VMPI_strcat(str, msgcont);
            VMPI_strncat(str, pattern, length);
            _pattern = str;
            _invalid = true;
        }
    }

    MethodNameRegExRecognizer::~MethodNameRegExRecognizer()
    {
        delete _pattern;
        _pattern = NULL;
        (pcre_free)((pcre*)_regex);
        _regex = NULL;
    }

    bool MethodNameRegExRecognizer::matches(const MethodInfo* m) const
    {
        StUTF8String nm( m->getMethodName() );
        int matches = (_invalid) ? -1 : pcre_exec((pcre*)_regex, NULL, nm.c_str(), nm.length(), 0, PCRE_NO_UTF8_CHECK, NULL, 0);
        return (matches >= 0);
    }

    // Unique id compare
    MethodIdRecognizer::MethodIdRecognizer(size_t low, size_t high)
        : _low(low)
        , _high(high)
    {}

    bool MethodIdRecognizer::matches(const MethodInfo* m) const
    {
        uint32_t currentId = m->unique_method_id();
        bool match = false;
        if (_low == 0)
            match = currentId <= _high;
        else if (_high == 0)
            match = currentId >= _low;
        else
            match = currentId >= _low && currentId <= _high;
        return match;
    }

    static const char* parseDigit(const char* c, size_t* val)
    {
        *val = VMPI_atoi(c);
        while( VMPI_isdigit(*c) )
            c++;
        return c;
    }

    /**
     * Read a method specification from the given string and return
     * the appropriate MethodSpecifier.  The original ptr to the string
     * is updated to reflect the location where the parsing stopped.
     */
    MethodRecognizer* MethodRecognizer::parse(const char** ptr, char separator)
    {
        MethodRecognizer* r = NULL;
        const char* s = *ptr;

        // check for numeric ranges of the form
        //   'n', 'n-' , '-m' , 'n-m'
        if (*s == '-')
        {
            size_t high = 0;
            s = parseDigit(&s[1], &high);
            r = mmfx_new( MethodIdRecognizer(0, high) );
        }
        else if (VMPI_isdigit(*s))
        {
            size_t low = 0;
            size_t high = 0;
            s = parseDigit(s, &low);
            if (*s && *s == '-')
                s = parseDigit(&s[1], &high);
            else if (*s)
                high = low; // single #
            r = mmfx_new( MethodIdRecognizer(low, high) );
        }
        else if (*s == '%')
        {
            // regex case; read until '%'
            const char* c = s+1;
            while( *c && *c != '%')
                c++;
            r = mmfx_new( MethodNameRegExRecognizer(s+1,c-(s+1)) );
            s = c;
        }
        else
        {
            // string case; read until ','
            const char* c = s;
            while( *c && *c != separator)
                c++;
            r = mmfx_new( MethodNameRecognizer(s,c-s) );
            s = c;
        }
        *ptr = s;
        return r;
    }
    #endif /* VMCFG_COMPILEPOLICY || AVMPLUS_VERBOSE */
}
