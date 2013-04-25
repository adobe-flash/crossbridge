/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef AVMSERIALIZER_INCLUDED
#define AVMSERIALIZER_INCLUDED

namespace avmplus
{
    class ByteArrayObject;

    //
    // The following classes are used to serialize Objects :
    //
    // class ClassInfo
    //      for output, it converts a (Traits *) into a list of multinames to output->write(obj->get(mn))
    //      for input, it holds the list of multinames to obj->set(input->read())
    //
    // class ObjectOutput
    //      functionality common to writing avmplus::ScriptObject using AMF0 or AMF3 encoding
    //
    // class AvmPlusObjectOutput : ObjectOutput
    //      functionality for writing avmplus::ScriptObject using AMF3 encoding
    //
    // class ObjectInput
    //      functionality common to reading avmplus::ScriptObject using AMF0 or AMF3 encoding
    //
    // class AvmPlusObjectInput : ObjectInput
    //      functionality for reading avmplus::ScriptObject using AMF3 encoding
    //

    class ClassInfo;
    
    // WARNING: These do not match the AtomConstant types
    enum AtomType {
        kUndefinedAtomType      = 0,
        kNullAtomType           = 1,
        kFalseAtomType          = 2,
        kTrueAtomType           = 3,
        kIntegerAtomType        = 4,
        kDoubleAtomType         = 5,
        kStringAtomType         = 6,
        kAvmMinusXmlAtomType    = 7,
        kDateAtomType           = 8,
        kArrayAtomType          = 9,
        kObjectAtomType         = 10,
        kAvmPlusXmlAtomType     = 11,
        kByteArrayAtomType      = 12,
        kTypedVectorInt         = 13,
        kTypedVectorUint        = 14,
        kTypedVectorDouble      = 15,
        kTypedVectorObject      = 16,
        kDictionaryObject       = 17,
        kTypedVectorFloat       = 18,
        kTypedVectorFloat4      = 19,
        kFloat4AtomType         = 20,
        kFloatAtomType          = 21,
    };


    class ObjectOutput : public DataOutput
    {
    public:
        ObjectOutput(Toplevel *tl);
        ~ObjectOutput();

        virtual void WriteDynamicProperty(Stringp name, Atom value) = 0;

    protected:
        void WriteType(uint8_t type) { WriteU8(type); }

    protected:
        AvmCore* core() const { return toplevel()->core(); }

        void StringTableAdd(String *str)                    { TableAdd(m_htString, str->atom()); }
        void TraitsTableAdd(Traits *traits)                 { TableAdd(m_htTraits, AvmCore::genericObjectToAtom(traits)); }
        void ObjectTableAdd(GCRef<ScriptObject> obj)        { TableAdd(m_htObject, obj->atom()); }
        void ClassInfoListAdd(ClassInfo *info)              { m_listClassInfo.add(info); }

        int32_t StringTableFind(String *str)                { return TableFind(m_htString, str->atom()); }
        int32_t TraitsTableFind(Traits *traits)             { return TableFind(m_htTraits, AvmCore::genericObjectToAtom(traits)); }
        int32_t ObjectTableFind(GCRef<ScriptObject> obj)    { return TableFind(m_htObject, obj->atom()); }
        ClassInfo *ClassInfoListFind(uint32_t i)            { AvmAssert(i < m_listClassInfo.length()); return m_listClassInfo.get(i); }

        void ThrowArgumentError() { toplevel()->argumentErrorClass()->throwError(kInvalidParamError); }

    private:
        void TableAdd(HeapHashtable *ht, Atom atom);
        int32_t TableFind(HeapHashtable *ht, Atom atom); // returns -1 for atom not found

        GCList<ClassInfo>       m_listClassInfo;
        HeapHashtable * const   m_htString;
        HeapHashtable * const   m_htTraits;
        HeapHashtable * const   m_htObject;
    };

    class ObjectInput : public DataInput
    {
    public:
        ObjectInput(Toplevel *tl);
    
    protected:
        uint8_t ReadType() { return ReadU8(); }

    protected:
        void SetObjectProperty(Atom objectAtom, Stringp name, Atom value);

    protected:
        AvmCore* core() const { return toplevel()->core(); }

        void StringListAdd(String *str)             { m_listString.add(str); }
        void ObjectListAdd(GCRef<ScriptObject> obj) { m_listObject.add(obj); }
        void ClassInfoListAdd(ClassInfo *info)      { m_listClassInfo.add(info); }

        String *StringListFind(uint32_t i);
        GCRef<ScriptObject> ObjectListFind(uint32_t i);
        ClassInfo *ClassInfoListFind(uint32_t i);

        void ThrowArgumentError() { toplevel()->argumentErrorClass()->throwError(kInvalidParamError); }

    private:
        RCList<String>          m_listString;
        RCList<ScriptObject>    m_listObject;
        GCList<ClassInfo>       m_listClassInfo;
    };

    class AvmPlusObjectInput : public ObjectInput {
    public:
        AvmPlusObjectInput(Toplevel *tl) : ObjectInput(tl) {}

        String *ReadString();
        uint32_t ReadUint29();
        Atom ReadAtom();

    protected:
        virtual bool handleAdditionalType(uint8_t /*type*/, Atom& /*value*/) { return false; }

        //Helper functions for reading XML
        String* ReadXmlString(uint32_t len);
        uint32_t ReadReference() { return ReadUint29(); }

    private:
        GCRef<ScriptObject> ReadScriptObject(ClassClosure **closure = 0);
        DateObject *ReadDate();
        GCRef<ScriptObject> ReadXML();
        GCRef<ArrayObject> ReadArray();
        GCRef<ByteArrayObject> ReadByteArray();
        GCRef<DictionaryObject> ReadDictionary();
        Atom ReadTypedVector(uint8_t type);
    };

    class AvmPlusObjectOutput : public ObjectOutput
    {
    public:
        AvmPlusObjectOutput(Toplevel *tl) : ObjectOutput(tl) {}

        virtual void WriteDynamicProperty(Stringp name, Atom value)
        {
            toplevel()->checkNull(name, "name");
            WriteString(name);
            WriteAtom(value);
        }

        void WriteAtom(Atom value);
        void WriteString(String *str);
        void WriteUint29(uint32_t i);

    protected:
        virtual bool handleAdditionalAtom(Atom& /*value*/) { return false; }

        void WriteXML(GCRef<ScriptObject> obj, bool isAS3XML);
        void WriteScriptObject(GCRef<ScriptObject> obj);

    private:
        void WriteReference(uint32_t ref) { WriteUint29(ref); }
        void WriteDate(DateObject *obj);
        void WriteArray(GCRef<ArrayObject> obj);
        void WriteByteArray(GCRef<ByteArrayObject> obj);
        void WriteDictionary(GCRef<DictionaryObject> obj);
        void WriteTypedVector(Atom atom);
    };

    class ClassInfo : public MMgc::GCObject
    {
    public:
        // for output, construct information from Traits
        ClassInfo(Toplevel *tl, Traits *t);

        void Write(AvmPlusObjectOutput *output);
        static ClassInfo *Read(Toplevel *tl, AvmPlusObjectInput *input, bool dynamic, bool externalizable, int count);

        Traits *traits() const { return m_traits; }
        bool dynamic() const { return m_dynamic; }
        bool externalizable() const { return ((Binding)m_functionBinding != 0); }
        Binding get_functionBinding() const { return m_functionBinding; }
        String *className() const { return m_name; }
        ClassClosure *closure() const { return m_closure; }
        
        uint32_t SealedPropCount() const { return m_sealed.length(); }
        String *SealedProp(uint32_t i) const { return m_sealed.get(i); }

    private:
        ClassInfo(Toplevel *tl) : m_toplevel(tl), m_sealed(tl->core()->GetGC(), kListInitialCapacity) {} // static Read() will use this, and it initializes members

    private: /* helper methods */

        bool containsTransientMetadata(PoolObject* pool, const uint8_t* meta_pos );
        bool slotContainsTransientMetadata(Traits *traits, int index);
        bool methodContainsTransientMetadata(Traits *traits, int index);
        bool isSerializable(Traits * t, Namespace* ns, Binding b);

    private:
        Toplevel* const         m_toplevel;
        GCMember<Traits>        m_traits;
        GCMember<String>        m_name;
        bool                    m_dynamic;
        Binding                 m_functionBinding;
        
        GCMember<ClassClosure>  m_closure;
        RCList<String>          m_sealed;
    };


    class AvmOutputWrapper : public AvmPlusObjectOutput
    {
    public:
        AvmOutputWrapper(Toplevel *tl, DataOutput *out) : AvmPlusObjectOutput(tl), m_toplevel(tl), m_out(out) {}
        virtual void Write(const void *buffer, uint32_t count) { m_out->Write(buffer, count); }

    protected:
        // override from DataIOBase
        virtual Toplevel* toplevel() const { return m_toplevel; }
        
    private:
        Toplevel* const m_toplevel;
        DataOutput *m_out;
    };

    class AvmInputWrapper : public AvmPlusObjectInput
    {
    public:
        AvmInputWrapper(Toplevel *tl, DataInput *in) : AvmPlusObjectInput(tl), m_toplevel(tl), m_in(in) {}
        virtual uint32_t Available() { return m_in->Available(); }
        virtual void Read(void *buffer, uint32_t count) { m_in->Read(buffer, count); }

    protected:
        // override from DataIOBase
        virtual Toplevel* toplevel() const { return m_toplevel; }
        
    private:
        Toplevel* const m_toplevel;
        DataInput *m_in;
    };

}

#endif /* AVMSERIALIZER_INCLUDED */

