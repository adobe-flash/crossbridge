/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_XMLListObject__
#define __avmplus_XMLListObject__


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

// XMLList.prototype.constructor ??
// XMLList.prototype.attribute
// XMLList.prototype.attributes
// XMLList.prototype.child
// XMLList.prototype.children
// XMLList.prototype.comments
// XMLList.prototype.contains
// XMLList.prototype.copy
// XMLList.prototype.descendants
// XMLList.prototype.elements
// XMLList.prototype.hasOwnProperty
// XMLList.prototype.hasComplexContent
// XMLList.prototype.hasSimpleContent
// XMLList.prototype.length
// XMLList.prototype.normalize
// XMLList.prototype.parent
// XMLList.prototype.processingInstructions
// XMLList.prototype.prependChild
// XMLList.prototype.propertyIsEnumerable
// XMLList.prototype.text
// XMLList.prototype.toString
// XMLList.prototype.toXMLString
// XMLList.prototype.valueOf


namespace avmplus
{
    /**
     * The XMLListObject class is the C++ implementation of the
     * "XMLList" type in the E4X Specification.
     */
    class GC_AS3_EXACT(XMLListObject, ScriptObject)
    {
        friend class XMLObject;
        
    protected:
        XMLListObject(XMLListClass *type, Atom targetObject, const Multiname* targetProperty);

    public:
        REALLY_INLINE static XMLListObject* create(MMgc::GC* gc, XMLListClass* cls, Atom targetObject = nullObjectAtom, const Multiname* targetProperty = 0)
        {
            return new (gc, MMgc::kExact) XMLListObject(cls, targetObject, targetProperty);
        }
        
    private:
        XMLClass* xmlClass() const
        {
            return toplevel()->xmlClass();
        }

    public:
        // Functions that override object version
        // Delete (deleteProperty)
        // Get (getProperty)
        // HasProperty
        // Put (setProperty)
        Stringp toString ();

        Atom callProperty(const Multiname* name, int argc, Atom* argv);

        Atom getAtomProperty(Atom name) const;          // [[Get]]
        void setAtomProperty(Atom name, Atom value);    // [[Put]]
        bool deleteAtomProperty(Atom name);             // [[Delete]

        Atom getMultinameProperty(const Multiname* name) const;
        void setMultinameProperty(const Multiname* name, Atom value);
        bool deleteMultinameProperty(const Multiname* name);

        bool hasMultinameProperty(const Multiname* name) const;
        bool hasAtomProperty(Atom name) const;
        bool hasUintProperty(uint32_t i) const;

        Atom getDescendants(const Multiname* name) const;

        Atom getUintProperty(uint32_t i) const;
        void setUintProperty(uint32_t i, Atom value);
        bool delUintProperty(uint32_t i);

        // private helper functions
        void _appendNode(E4XNode *node);            // [[Append]]
        void _append(Atom child);                   // [[Append]]
        XMLListObject* _deepCopy() const;           // [[DeepCopy]]
        Atom _equals(Atom V) const;                 // [[Equals]]
        Atom _resolveValue();                       // [[ResolveValue]

        uint32_t numChildren()   const { return m_children.length(); }

        // inline version for frequent internal use
        inline uint32_t _length() const { return (numChildren()); } //[[Length]]

        // may convert an E4XNode to an XMLObject despite of being const
        XMLObject* _getAt(uint32_t i) const;
        E4XNode*   _getNodeAt(uint32_t i) const;

        inline void checkCapacity(int c) { m_children.ensureCapacity(c); }

        void __toXMLString(PrintWriter &output, Atom AncestorNamespace, int indentLevel = 0);

        // Iterator support - for in, for each
        Atom nextName(int index);
        Atom nextValue(int index);
        int nextNameIndex(int index);

        // Exposed routines to AS (NATIVE_METHODs)
        XMLListObject* AS3_attribute (Atom arg);
        XMLListObject* AS3_attributes ();
        XMLListObject* AS3_child (Atom propertyName);
        XMLListObject* AS3_children ();
        XMLListObject* AS3_comments ();
        bool AS3_contains (Atom value);
        XMLListObject* AS3_copy ();
        XMLListObject* AS3_descendants (Atom name);
        XMLListObject* AS3_elements (Atom name);
        bool XMLList_AS3_hasOwnProperty (Atom P);
        uint32_t AS3_length () const; // slow version for AS3 glue code
        bool AS3_hasComplexContent ();
        bool AS3_hasSimpleContent ();
        Atom AS3_name();
        XMLListObject* AS3_normalize ();
        Atom AS3_parent ();
        XMLListObject* AS3_processingInstructions (Atom name);
        bool XMLList_AS3_propertyIsEnumerable(Atom P);  // NOT virtual, NOT an override
        XMLListObject* AS3_text ();
        Stringp AS3_toString();
        String *AS3_toXMLString ();

        // The following are not in the spec but work if XMLList has one element
        XMLObject* AS3_addNamespace (Atom _namespace);
        XMLObject* AS3_appendChild (Atom child);
        int AS3_childIndex();
        ArrayObject* AS3_inScopeNamespaces ();
        Atom AS3_insertChildAfter (Atom child1, Atom child2);
        Atom AS3_insertChildBefore (Atom child1, Atom child2);
        Atom _namespace(Atom prefix, int argc); // prefix is optional
        Atom AS3_localName ();
        ArrayObject* AS3_namespaceDeclarations ();
        String *AS3_nodeKind ();
        XMLObject* AS3_prependChild (Atom value);
        XMLObject* AS3_removeNamespace (Atom _namespace);
        XMLObject* AS3_replace (Atom propertyName, Atom value);
        XMLObject* AS3_setChildren (Atom value);
        void AS3_setLocalName (Atom name);
        void AS3_setName (Atom name);
        void AS3_setNamespace (Atom ns);

        // inline wrappers for legacy code
        inline XMLListObject* attribute (Atom arg) { return AS3_attribute (arg); }
        inline XMLListObject* attributes () { return AS3_attributes (); }
        inline XMLListObject* child (Atom propertyName) { return AS3_child (propertyName); }
        inline XMLListObject* children () { return AS3_children (); }
        inline XMLListObject* comments () { return AS3_comments (); }
        inline bool contains (Atom value) { return AS3_contains (value); }
        inline XMLListObject* copy () { return AS3_copy (); }
        inline XMLListObject* descendants (Atom name) { return AS3_descendants (name); }
        inline XMLListObject* elements (Atom name) { return AS3_elements (name); }
        inline bool hasOwnProperty (Atom P) { return XMLList_AS3_hasOwnProperty(P); }
        inline uint32_t length () const { return AS3_length (); }
        inline bool hasComplexContent () { return AS3_hasComplexContent (); }
        inline bool hasSimpleContent () { return AS3_hasSimpleContent (); }
        inline Atom name() { return AS3_name(); }
        inline XMLListObject* normalize () { return AS3_normalize (); }
        inline Atom parent () { return AS3_parent (); }
        inline XMLListObject* processingInstructions (Atom name) { return AS3_processingInstructions (name); }
        inline bool propertyIsEnumerable(Atom P) { return XMLList_AS3_propertyIsEnumerable(P); }
        inline XMLListObject* text () { return AS3_text (); }
        inline String *toXMLString () { return AS3_toXMLString (); }
        inline XMLObject* addNamespace (Atom _namespace) { return AS3_addNamespace (_namespace); }
        inline XMLObject* appendChild (Atom child) { return AS3_appendChild (child); }
        inline int childIndex() { return AS3_childIndex(); }
        inline ArrayObject* inScopeNamespaces () { return AS3_inScopeNamespaces (); }
        inline Atom insertChildAfter (Atom child1, Atom child2) { return AS3_insertChildAfter (child1, child2); }
        inline Atom insertChildBefore (Atom child1, Atom child2) { return AS3_insertChildBefore (child1, child2); }
        inline Atom localName () { return AS3_localName (); }
        inline ArrayObject* namespaceDeclarations () { return AS3_namespaceDeclarations (); }
        inline String *nodeKind () { return AS3_nodeKind (); }
        inline XMLObject* prependChild (Atom value) { return AS3_prependChild (value); }
        inline XMLObject* removeNamespace (Atom _namespace) { return AS3_removeNamespace (_namespace); }
        inline XMLObject* replace (Atom propertyName, Atom value) { return AS3_replace (propertyName, value); }
        inline XMLObject* setChildren (Atom value) { return AS3_setChildren (value); }
        inline void setLocalName (Atom name) { return AS3_setLocalName (name); }
        inline void setName (Atom name) { return AS3_setName (name); }
        inline void setNamespace (Atom ns) { return AS3_setNamespace (ns); }

        // non-E4X extensions
        ScriptObject* getNotification();
        void setNotification(ScriptObject* f);

#ifdef XML_FILTER_EXPERIMENT
        XMLListObject* filter (Atom propertyName, Atom value);
#endif

#ifdef DEBUGGER
        /*override*/ uint64_t bytesUsed() const;
#endif

    private:
        void fixTargetObject() const;
        void setTargetObject(Atom a) const { m_targetObject.set(MMgc::GC::GetGC(this), this, a); }

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(XMLListObject)

    private:
        // These three members are mutable because fixTargetObject may modify them
        mutable HeapMultiname GC_STRUCTURE(m_targetProperty);
        mutable ATOM_WB       GC_ATOM(m_targetObject);
        mutable bool          m_appended;

        // An array of XMLObjects,or E4XNodes; mutable because E4XNodes may be converted to XMLObjects
        mutable AtomList      GC_STRUCTURE(m_children);

        GC_DATA_END(XMLListObject)

        DECLARE_SLOTS_XMLListObject;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_XMLListObject__ */
