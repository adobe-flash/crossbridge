/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Namespace__
#define __avmplus_Namespace__


namespace avmplus
{
    /**
     * a namespace is a primitive value in the system, similar to String
     */
    class GC_CPP_EXACT(Namespace, AvmPlusScriptableObject)
    {
    private:
        friend class AvmCore;
        template <class VALUE_TYPE, class VALUE_WRITER> friend class MultinameHashtable;
        template <class VALUE_TYPE> friend class Quad;

        GC_DATA_BEGIN(Namespace);
    private:
        AtomWB              GC_ATOM(m_prefix);
        uintptr_t           GC_POINTER(m_uriAndType);          // Uses 3 bits for flags, but otherwise is really a Stringp
        ApiVersion  const   m_apiVersion;
        GC_DATA_END(Namespace);

    public:
        enum NamespaceType
        {
            NS_Public = 0,
            NS_Protected = 1,
            NS_PackageInternal = 2,
            NS_Private = 3,
            NS_Explicit = 4,
            NS_StaticProtected = 5
        };
        
    private:
        Namespace(Atom prefix, Stringp uri, NamespaceType type, ApiVersion apiVersion);
        
    public:
        static REALLY_INLINE Namespace* create(MMgc::GC* gc, Atom prefix, Stringp uri, NamespaceType type, ApiVersion apiVersion)
        {
            return new (gc, MMgc::kExact) Namespace(prefix, uri, type, apiVersion);
        }

#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~Namespace();
#endif

        // AS3 native functions
        REALLY_INLINE Atom get_prefix() const { return m_prefix; }
        REALLY_INLINE Stringp get_uri() const { return (Stringp)atomPtr(m_uriAndType); }

        REALLY_INLINE Atom getPrefix() const { return get_prefix(); }
        REALLY_INLINE Stringp getURI() const { return get_uri(); }
        REALLY_INLINE ApiVersion getApiVersion() { return m_apiVersion; }

        Atom  atom() const { return AtomConstants::kNamespaceType | (Atom)this; }

        /**
         * virtual version of atom():
         */
        virtual Atom toAtom() const { return atom(); }

        bool hasPrefix () const;

        bool isPublic() const;

        bool EqualTo(const Namespace* other) const;

        REALLY_INLINE bool isPrivate() const
        {
            return ISNULL(m_prefix);
        }

        REALLY_INLINE NamespaceType getType() const
        {
            return (NamespaceType)(m_uriAndType & 7);
        }

        // Iterator support - for in, for each
        Atom nextName(const AvmCore *core, int index);
        Atom nextValue(int index);
        int nextNameIndex(int index);

#ifdef DEBUGGER
        uint64_t bytesUsed() const { return sizeof(Namespace); }

        // Like bytesUsed(), but also includes memory taken by the
        // prefix and the uri.
        uint64_t bytesUsedDeep() const;
#endif

//#ifdef AVMPLUS_VERBOSE
    public:
        PrintWriter& print(PrintWriter& prw) const;
//#endif
    private:
        void setUri(Stringp uri, NamespaceType flags);
    };
}

#endif /* __avmplus_Namespace__ */

