/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Multiname__
#define __avmplus_Multiname__


namespace avmplus
{
    #ifdef VMCFG_NANOJIT
    class CodegenLIR;
    #endif

    /**
     * Multiname is a reference to an identifier in 0 or more namespaces.  It consists
     * of the simple name and a list of namespaces.
     *
     * Multinames are always stack allocated.  Namespace and name values are interened, since
     * they are used as hashtable keys.
     */
    class Multiname
    {
        const static int32_t ATTR   = 0x01; // attribute name
        const static int32_t QNAME  = 0x02; // qualified name (size==1, explicit in code)
        const static int32_t RTNS   = 0x04; // runtime namespace
        const static int32_t RTNAME = 0x08; // runtime name
        const static int32_t NSSET  = 0x10;
        const static int32_t TYPEPARAM = 0x20;
        #ifdef VMCFG_NANOJIT
        friend class CodegenLIR;
        #endif
        friend class HeapMultiname;

        Stringp name;
        union
        {
            Namespacep ns;
            NamespaceSetp nsset;
        };
        int32_t flags;
        uint32_t next_index;

    public:
        Multiname();
        Multiname(NamespaceSetp nsset);
        Multiname(const Multiname &other);
        Multiname(Namespacep ns, Stringp name);
        Multiname(Namespacep ns, Stringp name, bool qualified);
        ~Multiname();

        // Utility used by exact tracers of multiname values
        void gcTrace(MMgc::GC* gc);
        
        Stringp getName() const;
        void setName(Stringp _name);
        void setName(const Multiname* other);
        int32_t namespaceCount() const;
        Namespacep getNamespace(int32_t i) const;
        Namespacep getNamespace() const;
        void setNamespace(Namespacep _ns);
        void setNamespace(const Multiname* other);
        NamespaceSetp getNsset() const;
        void setNsset(NamespaceSetp _nsset);
        uint32_t getTypeParameter() const;
        void setTypeParameter(uint32_t index);

        bool contains(Namespacep ns) const;
        bool containsAnyPublicNamespace() const;
        bool isValidDynamicName() const;

        /**
         * return the flags we want to keep when copying a compile-time
         * multiname into a runtime temporary multiname
         */
        int32_t ctFlags() const;

        /**
         * returns true if this multiname could resolve to a binding.  Attributes,
         * wildcards, and runtime parts mean it can't match any binding.
         */
        int32_t isBinding() const;

        int32_t isRuntime() const;
        int32_t isRtns() const;
        int32_t isRtname() const;
        int32_t isQName() const;
        bool isAttr() const;
        bool isAnyName() const;
        bool isAnyNamespace() const;
        int32_t isNsset() const;
        int32_t isParameterizedType() const;

        void setAttr(bool b=true);
        void setQName();
        void setRtns();
        void setRtname();
        void setAnyName();
        void setAnyNamespace();
        bool matches(const Multiname *mn) const;

        // As an optimization a Multiname may be part of a GCRoot.  The following
        // two methods make sure the reference counted dependents of a Multiname
        // stick around (or not, as the case may be).  The reference counts are
        // *not* adjusted by the methods above; multinames on which IncrementRef
        // and DecrementRef are called *must* be considered constant.
    public:
        void IncrementRef();
        void DecrementRef();

//#ifdef AVMPLUS_VERBOSE
    public:
        typedef enum _MultiFormat
        {
            MULTI_FORMAT_FULL = 0,      /* default */
            MULTI_FORMAT_NAME_ONLY,     /* only name */
            MULTI_FORMAT_NS_ONLY        /* only namespaces */
        }
        MultiFormat;

        PrintWriter& print(PrintWriter& prw, MultiFormat form=MULTI_FORMAT_FULL) const;
        PrintWriter& printName(PrintWriter& prw) const;
        static PrintWriter& print(PrintWriter& prw, Namespacep ns, Stringp name, bool attr=false, bool hideNonPublicNamespaces=true);

        // Use 'Format' like this: myPrintWriter << Format(ns, name)
        class Format
        {
        public:
            Format(const Namespace* ns, const String* name) : _ns(ns), _name(name) {}
            Format(const Multiname* mn) : _ns(mn->getNamespace()), _name(mn->getName()) {}
            const Namespace* _ns;
            const String* _name;
        };

        class FormatNameOnly
        {
        public:
            FormatNameOnly(const Multiname* mn) : _mn(mn) {}
            const Multiname* _mn;
        };
//#endif
    private:
        // This isn't implemented which prevents new of any form (new (gc) etc).
        void *operator new(size_t size);
    };

    PrintWriter& operator<<(PrintWriter& prw, const Multiname::Format& mnf);
    PrintWriter& operator<<(PrintWriter& prw, const Multiname::FormatNameOnly& mnf);

    // version of multiname sporting write barriers
    // NOTE NOTE NOTE
    // This is embedded into other GCObjects, it's not a GCObject by itself.
    //
    // NOTE NOTE NOTE
    // Special care must be taken if this object is not embedded within the first 4K of a GC object,
    // because write barriers must be handled differently.  See comments to setMultiname below and
    // comments on the destructor in Multiname.cpp.
    class HeapMultiname : public MMgc::GCInlineObject
    {
    public:

        explicit HeapMultiname();
        explicit HeapMultiname(const Multiname& other);
        ~HeapMultiname();

        operator const Multiname* () const;
        operator const Multiname& () const;
        const HeapMultiname& operator=(const HeapMultiname& that);
        const HeapMultiname& operator=(const Multiname& that);

        // Utility used by exact tracers of multiname values
        void gcTrace(MMgc::GC* gc);
        
        // Use this to set when the location of the HeapMultiname within its parent
        // object is not within the first block of the parent object.  See bugzilla 525875.
        void setMultiname(MMgc::GC* gc, const void* container, const Multiname& other);

        Stringp getName() const;
        int32_t namespaceCount() const;
        Namespacep getNamespace(int32_t i) const;
        Namespacep getNamespace() const;
        NamespaceSetp getNsset() const;
        bool contains(Namespacep ns) const;
        int32_t ctFlags() const;
        int32_t isBinding() const;
        int32_t isRuntime() const;
        int32_t isRtns() const;
        int32_t isRtname() const;
        int32_t isQName() const;
        bool isAttr() const;
        bool isAnyName() const;
        bool isAnyNamespace() const;
        int32_t isNsset() const;
        bool matches(const Multiname *mn) const;

    private:
        Multiname name;
        MMgc::GC* gc() const;

    private:
        void setMultiname(const Multiname& other);

    private:
        explicit HeapMultiname(const HeapMultiname& toCopy); // unimplemented
    };
}

#endif /* __avmplus_Multiname__ */
