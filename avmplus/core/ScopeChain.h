/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{
    /**
     * type descriptor for a captured scope chain
     *
     * Note: ScopeTypeChain is now immutable; once created it cannot be modified
     */
    class GC_CPP_EXACT(ScopeTypeChain, MMgc::GCTraceableObject)
    {
    private:
        ScopeTypeChain(int32_t _size, int32_t _fullsize, Traits* traits);
        static const ScopeTypeChain* create(MMgc::GC* gc, Traits* traits, const ScopeTypeChain* outer, const FrameValue* values, int32_t nValues, Traits* append, Traits* extra);

    public:

        static const ScopeTypeChain* create(MMgc::GC* gc, Traits* traits, const ScopeTypeChain* outer, const FrameState* state, Traits* append, Traits* extra);
#ifdef VMCFG_AOT
        static const ScopeTypeChain* create(MMgc::GC* gc, Traits* traits, const ScopeTypeChain* outer, Traits* const* stateTraits, uint32_t nStateTraits, uint32_t nStateWithTraits, Traits* append, Traits* extra);
#endif
        static const ScopeTypeChain* createEmpty(MMgc::GC* gc, Traits* traits);
        static const ScopeTypeChain* createExplicit(MMgc::GC* gc, Traits* traits, Traits** types);

        const ScopeTypeChain* cloneWithNewTraits(MMgc::GC* gc, Traits* traits) const;
        Traits* traits() const;
        Traits* getScopeTraitsAt(uint32_t i) const;
        bool getScopeIsWithAt(uint32_t i) const;

        PrintWriter& print(PrintWriter& prw) const;

        bool equals(const ScopeTypeChain* that) const;

    private:
        void setScopeAt(uint32_t i, Traits* t, bool w = false);

        // Traits are MMgc-allocated, thus always 8-byte-aligned, so the low 3 bits are available for us to use
        static const uintptr_t ISWITH = 0x01;

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(ScopeTypeChain)
        
    public:
        const int32_t       size;
        const int32_t       fullsize;
    private:
        Traits* const       GC_POINTER(_traits);
        uintptr_t           GC_POINTERS_SMALL(_scopes[1], fullsize); // Tagged pointers to Traits objects.

        GC_DATA_END(ScopeTypeChain)
    // ------------------------ DATA SECTION END
    };

    /**
    * a captured scope chain
    */
    class GC_CPP_EXACT(ScopeChain, MMgc::GCTraceableObject)
    {
#ifdef VMCFG_NANOJIT
        friend class CodegenLIR;
        friend class MopsRangeCheckFilter;
        friend class halfmoon::JitFriend;
#endif
    private:
        ScopeChain(VTable* vtable, AbcEnv* abcEnv, const ScopeTypeChain* scopeTraits, Namespacep dxns);

    public:

        /*
            dxns is modelled as a variable on an activation object.  The activation
            object will be in several scope chains, so we can't store dxns in the SC.
            When it changes, it's new valuable is visible in all closures in scope.
        */

        static ScopeChain* create(MMgc::GC* gc, VTable* vtable, AbcEnv* abcEnv, const ScopeTypeChain* scopeTraits, const ScopeChain* outer, Namespacep dxns);

        ScopeChain* cloneWithNewVTable(MMgc::GC* gc, VTable* vtable, AbcEnv* abcEnv, const ScopeTypeChain* scopeTraits = NULL);

        VTable* vtable() const;
        AbcEnv* abcEnv() const;
        const ScopeTypeChain* scopeTraits() const;
        int32_t getSize() const;
        Atom getScope(int32_t i) const;
        void setScope(MMgc::GC* gc, int32_t i, Atom value);
        Namespacep getDefaultNamespace() const;
        PrintWriter& print(PrintWriter& prw) const;

    // ------------------------ DATA SECTION BEGIN
    GC_DATA_BEGIN(ScopeChain)

    private:
        VTable* const                   GC_POINTER(_vtable);
        AbcEnv* const                   GC_POINTER(_abcEnv);
        const ScopeTypeChain* const     GC_POINTER(_scopeTraits);
        GCMember<Namespace> const       GC_POINTER(_defaultXmlNamespace);
        Atom                            GC_ATOMS_SMALL(_scopes[1], "getSize()");

    GC_DATA_END(ScopeChain)
    // ------------------------ DATA SECTION END
    };

}
