/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_E4XNode__
#define __avmplus_E4XNode__


// E4X 9.1.1 Internal Properties and Methods
// Name
// Parent
// Attributes
// InScopeNamespaces
// Length
// Delete (overrides Object version)
// Get (overrides Object version)
// HasProperty (overrides Object version)
// Put (overrides Object version)
// DeleteByIndex (PropertyName)
// DeepCopy
// ResolveValue
// Descendants (PropertyName)
// Equals (Value)
// Insert (PropertyName, Value)
// Replace (PropertyName, Value)
// AddInScopeNamespace (Namespace)

// These are static objects on the XML type
// E4X: The XML constructor has the following properties
// XML.ignoreComments
// XML.ignoreProcessingInstructions
// XML.ignoreWhitespace
// XML.prettyPrinting
// XML.prettyIndent
// XML.settings
// XML.setSettings ([settings])
// XML.defaultSettings()

// XML.prototype.constructor ??
// XML.prototype.addNamespace
// XML.prototype.appendChild
// XML.prototype.attribute
// XML.prototype.attributes
// XML.prototype.child
// XML.prototype.childIndex
// XML.prototype.children
// XML.prototype.comments
// XML.prototype.contains
// XML.prototype.copy
// XML.prototype.descendants
// XML.prototype.elements
// XML.prototype.hasOwnProperty
// XML.prototype.hasComplexContent
// XML.prototype.hasSimpleContent
// XML.prototype.inScopeNamespaces
// XML.prototype.insertChildAfter
// XML.prototype.insertChildBefore
// XML.prototype.length
// XML.prototype.localName
// XML.prototype.name
// XML.prototype.namespace
// XML.prototype.namespaceDeclarations
// XML.prototype.nodeKind
// XML.prototype.normalize
// XML.prototype.parent
// XML.prototype.processingInstructions
// XML.prototype.prependChild
// XML.prototype.propertyIsEnumerable
// XML.prototype.removeNamespace
// XML.prototype.replace
// XML.prototype.setChildren
// XML.prototype.setLocalName
// XML.prototype.setName
// XML.prototype.setNamespace
// XML.prototype.text
// XML.prototype.toString
// XML.prototype.toXMLString
// XML.prototype.valueOf

namespace avmplus
{
    class GC_CPP_EXACT(E4XNodeAux, MMgc::GCTraceableObject)
    {
        friend class E4XNode;
        friend class ElementE4XNode;

    private:
        E4XNodeAux(Stringp s, Namespace* ns, FunctionObject* notify = NULL);

    public:
        REALLY_INLINE static E4XNodeAux* create(MMgc::GC* gc, Stringp name, Namespace *ns, FunctionObject* notify = NULL)
        {
            return new (gc, MMgc::kExact) E4XNodeAux (name, ns, notify);
        }

#ifdef DEBUGGER
        uint64_t bytesUsed() const;
#endif

    private:
        GC_DATA_BEGIN(E4XNodeAux)

        GCMember<String>         GC_POINTER(m_name);
        GCMember<Namespace>      GC_POINTER(m_ns);

        /** callback on changes to children, attribute, name or namespace */
        GCMember<FunctionObject> GC_POINTER(m_notification);

        GC_DATA_END(E4XNodeAux)
    };

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    /**
     * E4XNode is the C++ implementation of the XML class
     * in the E4X Specification.
     */
    // Currently this is 12-bytes in size (4 bytes wasted by GC)
    // Element nodes are 24 bytes
    // All other nodes are 16 bytes
    class GC_CPP_EXACT(E4XNode, MMgc::GCTraceableObject)
    {
        friend class ElementE4XNode;

    protected:
        GC_DATA_BEGIN(E4XNode)
        
        /** Either null or an E4XNode, valid for all node types */
        GCMember<E4XNode> GC_POINTER(m_parent);

        // If this is a simple name with no namespace or notification function,
        // we just have a string pointer.  Otherwise, we're a E4XNodeAux value
        // containing a name + namespace as well as a notification function.
        // E4XNodeAux *
        // String *
        uintptr_t GC_POINTER(m_nameOrAux);
        #define AUXBIT 0x1

        GC_DATA_END(E4XNode)

    protected:
        E4XNode(E4XNode* parent) : m_parent(parent), m_nameOrAux(0) { }
        
    public:
        
        typedef GCList<E4XNode> E4XNodeList;
        typedef ExactHeapList<E4XNodeList> HeapE4XNodeList;

    public:
        bool getQName (Multiname *mn, Namespacep publicNS) const;
        bool getQNameForeign (AvmCore* core, Multiname *mn, Namespacep publicNS) const;
        void setQName (AvmCore *core, Stringp name, Namespace *ns = 0);
        void setQName (AvmCore *core, const Multiname *mn);

        virtual Stringp getValue() const = 0;
        virtual void setValue (String *s) = 0;

        E4XNode* getParent() const { return m_parent; }
        void setParent(E4XNode* n) { m_parent = n; }

        // Not used as bit fields (only one bit at a time can be set) but
        // used for fast multi-type compares.
        enum NodeTypes
        {
            kUnknown                = 0x0001,
            kAttribute              = 0x0002,
            kText                   = 0x0004,
            kCDATA                  = 0x0008, // same as text but no string conversion on output
            kComment                = 0x0010,
            kProcessingInstruction  = 0x0020,
            kElement                = 0x0040
        };

        virtual int getClass() const = 0;

        virtual uint32_t numAttributes() const { return 0; }
        virtual HeapE4XNodeList* getAttributes() const { return 0; }
        virtual E4XNode *getAttribute(uint32_t /*index*/) const { return NULL; }

        virtual uint32_t numNamespaces() const { return 0; }
        virtual HeapNamespaceList* getNamespaces() const { return 0; }

        virtual uint32_t numChildren()   const { return 0; }
        virtual E4XNode *_getAt(uint32_t /*i*/) const { return 0; }

        virtual void clearChildren() {}
        virtual void setChildAt (uint32_t /*i*/, E4XNode* /*x*/) {}
        virtual void insertChild (uint32_t /*i*/, E4XNode* /*x*/) {}
        virtual void removeChild (uint32_t /*i*/) {}
        virtual void convertToE4XNodeList() {}

        bool hasSimpleContent() const;
        bool hasComplexContent() const;
        int32_t childIndex() const;
        String* nodeKind(Toplevel* toplevel) const;

        virtual void addAttribute (E4XNode *x);

        // Should this silently fail or assert?
        virtual void setNotification(AvmCore* /*core*/, FunctionObject* /*f*/, Namespacep /*publicNS*/) { return; }
        virtual FunctionObject* getNotification() const { return NULL; }

        // The following routines are E4X support routines

        // Private functions not exposed to AS
        // DeleteByIndex (PropertyName)
        // DeepCopy
        // ResolveValue
        // Descendants (PropertyName)
        // Equals (Value)
        // Insert (PropertyName, Value)
        // Replace (PropertyName, Value)
        // AddInScopeNamespace (Namespace)

        // Corresponds to [[Length]] in the docs
        virtual uint32_t _length() const { return 0; }

        bool _equals(Toplevel* toplevel, AvmCore *core, E4XNode *value) const;

        void _deleteByIndex (uint32_t entry);
        E4XNode *_deepCopy (AvmCore *core, Toplevel *toplevel, Namespacep publicNS) const;
        E4XNode *_deepCopyForeign (AvmCore *core, Toplevel *toplevel, Namespacep publicNS) const;
        virtual void _insert (AvmCore *core, Toplevel *toplevel, uint32_t entry, Atom value);
        virtual E4XNode* _replace (AvmCore *core, Toplevel *toplevel, uint32_t entry, Atom value, Atom pastValue = 0);
        virtual void _addInScopeNamespace (AvmCore *core, Namespace *ns, Namespacep publicNS);
        virtual void _append (E4XNode* /*childNode*/) { AvmAssert(0); }
        // Extract a namespace from a tag name, and return the new tag name in tagName
        Namespace *FindNamespace(AvmCore *core, Toplevel *toplevel, Stringp& tagName, bool bAttribute);
        int FindMatchingNamespace(AvmCore *core, Namespace *ns);

        void BuildInScopeNamespaceList(AvmCore *core, NamespaceList& list) const;
        void dispose();

        MMgc::GC *gc() const { return MMgc::GC::GetGC(this); }

#ifdef DEBUGGER
    public:
        uint64_t bytesUsed() const;

    protected:
        // Internal helper function called by bytesUsed(), which calculates how much
        // memory is used from this node on down to the bottom of the tree of E4XNodes;
        // will not look "up" the tree at m_parent.
        virtual uint64_t bytesUsedDown() const;
#endif
    };

    class GC_CPP_EXACT(TextE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(TextE4XNode)
        GCMember<String> GC_POINTER(m_value);
        GC_DATA_END(TextE4XNode)

        TextE4XNode (E4XNode *parent, String *value);
    public:
        REALLY_INLINE static TextE4XNode* create(MMgc::GC* gc, E4XNode *parent, String *value)
        {
            return new (gc, MMgc::kExact) TextE4XNode(parent, value);
        }

        int getClass() const { return kText; }
        Stringp getValue() const { return m_value; }
        void setValue (String *s) { m_value = s; }

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const { return E4XNode::bytesUsedDown() + m_value->bytesUsed(); }
#endif
    };

    class GC_CPP_EXACT(CommentE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(CommentE4XNode)
        GCMember<String> GC_POINTER(m_value);
        GC_DATA_END(CommentE4XNode)

        CommentE4XNode (E4XNode *parent, String *value);
    public:
        REALLY_INLINE static CommentE4XNode* create(MMgc::GC* gc, E4XNode *parent, String *value)
        {
            return new (gc, MMgc::kExact) CommentE4XNode(parent, value);
        }

        int getClass() const { return kComment; }
        Stringp getValue() const { return m_value; }
        void setValue (String *s) { m_value = s; }

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const { return E4XNode::bytesUsedDown() + m_value->bytesUsed(); }
#endif
    };

    class GC_CPP_EXACT(AttributeE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(AttributeE4XNode)
        GCMember<String> GC_POINTER(m_value);
        GC_DATA_END(AttributeE4XNode)

        AttributeE4XNode (E4XNode *parent, String *value);
        
    public:
        REALLY_INLINE static AttributeE4XNode* create(MMgc::GC* gc, E4XNode *parent, String *value)
        {
            return new (gc, MMgc::kExact) AttributeE4XNode(parent, value);
        }

        int getClass() const { return kAttribute; }
        Stringp getValue() const { return m_value; }
        void setValue (String *s) { m_value = s; }

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const { return E4XNode::bytesUsedDown() + m_value->bytesUsed(); }
#endif
    };

    class GC_CPP_EXACT(CDATAE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(CDATAE4XNode)
        GCMember<String> GC_POINTER(m_value);
        GC_DATA_END(CDATAE4XNode)

        CDATAE4XNode (E4XNode *parent, String *value);
    public:
        REALLY_INLINE static CDATAE4XNode* create(MMgc::GC* gc, E4XNode *parent, String *value)
        {
            return new (gc, MMgc::kExact) CDATAE4XNode(parent, value);
        }

        int getClass() const { return kCDATA; }
        Stringp getValue() const { return m_value; }
        void setValue (String *s) { m_value = s; }

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const { return E4XNode::bytesUsedDown() + m_value->bytesUsed(); }
#endif
    };

    class GC_CPP_EXACT(PIE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(PIE4XNode)
        GCMember<String> GC_POINTER(m_value); // only when m_class != kElement
        GC_DATA_END(PIE4XNode)

        PIE4XNode (E4XNode *parent, String *value);
    public:
        REALLY_INLINE static PIE4XNode* create(MMgc::GC* gc, E4XNode *parent, String *value)
        {
            return new (gc, MMgc::kExact) PIE4XNode(parent, value);
        }

        int getClass() const { return kProcessingInstruction; }
        Stringp getValue() const { return m_value; }
        void setValue (String *s) { m_value = s; }

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const { return E4XNode::bytesUsedDown() + m_value->bytesUsed(); }
#endif
    };

    // Currently this is 24-bytes in size
    class GC_CPP_EXACT(ElementE4XNode, E4XNode)
    {
        GC_DATA_BEGIN(ElementE4XNode)

        GCMember<HeapE4XNodeList>   GC_POINTER(m_attributes);
        GCMember<HeapNamespaceList> GC_POINTER(m_namespaces);

        // If the low bit of this integer is set, this value points directly
        // to a single child (one E4XNode *).  If there are multiple children,
        // this points to a HeapE4XNodeList
        uintptr_t        GC_POINTER(m_children);        // Must write with WB, GCMember<> does not work.
        #define SINGLECHILDBIT 0x1

        GC_DATA_END(ElementE4XNode)

        friend class E4XNode;

        ElementE4XNode (E4XNode *parent);
    public:
        REALLY_INLINE static ElementE4XNode* create(MMgc::GC* gc, E4XNode *parent)
        {
            return new (gc, MMgc::kExact) ElementE4XNode(parent);
        }

        int getClass() const { return kElement; }

        uint32_t numAttributes() const;
        HeapE4XNodeList* getAttributes() const;
        E4XNode *getAttribute(uint32_t index) const;
        void addAttribute (E4XNode *x);

        uint32_t numNamespaces() const;
        HeapNamespaceList* getNamespaces() const;

        uint32_t numChildren()   const;

        void clearChildren();
        void setChildAt (uint32_t i, E4XNode *x);
        #define SINGLECHILDBIT 0x1
        void insertChild (uint32_t i, E4XNode *x);
        void removeChild (uint32_t i);
        void convertToE4XNodeList();

        Stringp getValue() const { return 0; }
        void setValue (String *s) { (void)s; AvmAssert(s == 0); }

        void setNotification(AvmCore* core, FunctionObject* f, Namespacep publicNS);
        FunctionObject* getNotification() const;

        // E4X support routines below
        uint32_t _length() const { return numChildren(); }
        E4XNode *_getAt(uint32_t i) const;

        void _append (E4XNode *childNode);

        void _addInScopeNamespace (AvmCore *core, Namespace *ns, Namespacep publicNS);
        void _insert (AvmCore *core, Toplevel *toplevel, uint32_t entry, Atom value);
        E4XNode* _replace (AvmCore *core, Toplevel *toplevel, uint32_t entry, Atom value, Atom pastValue = 0);

        void CopyAttributesAndNamespaces(AvmCore *core, Toplevel *toplevel, XMLTag& tag, Namespacep publicNS);

#ifdef DEBUGGER
    protected:
        virtual uint64_t bytesUsedDown() const;
#endif
    };
}
#endif /* __avmplus_E4XNode__ */
