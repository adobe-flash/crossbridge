/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{

#define BUILTIN_TRAITS_(class_id) toplevel->core()->builtinPool->getClassTraits(avmplus::NativeID::abcclass_##class_id)->itraits

    static const char kTransient[] = "Transient";
    static const char kWriteExternal[] = "writeExternal";
    static const char kReadExternal[] = "readExternal";

//---------------------------------------------------------------------
// class ObjectOutput is the base class for classes ClassicObjectOutput
// and AvmPlusObjectOutput
//

    ObjectOutput::ObjectOutput(Toplevel *tl)
        : m_listClassInfo(tl->core()->GetGC(), 64)
        , m_htString(HeapHashtable::create(tl->core()->GetGC()))
        , m_htTraits(HeapHashtable::create(tl->core()->GetGC()))
        , m_htObject(HeapHashtable::create(tl->core()->GetGC()))
    {
    }

    ObjectOutput::~ObjectOutput()
    {
        HeapHashtable* temp;
        
        temp = m_htString;
        *(HeapHashtable **)&m_htString = NULL;
        delete temp;

        temp = m_htTraits;
        *(HeapHashtable **)&m_htTraits = NULL;
        delete temp;

        temp = m_htObject;
        *(HeapHashtable **)&m_htObject = NULL;
        delete temp;
    }

    void ObjectOutput::TableAdd(HeapHashtable *ht, Atom atom)
    {
        // this may realloc *ppHashtable
        // just store the raw int value.  we encapsulate access to these tables so it's
        // okay that the keys are atoms and the values are raw ints.
        ht->add(atom, core()->intToAtom(ht->getSize()));
    }

    int32_t ObjectOutput::TableFind(HeapHashtable *ht, Atom atom)
    {
        if(!ht->contains(atom))
            return -1;

        return core()->integer(ht->get(atom));
    }
// end class ObjectOutput
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// class ObjectInput is the base class for classes ClassicObjectInput
// and AvmPlusObjectInput
//

    ObjectInput::ObjectInput(Toplevel *tl)
        : m_listString(tl->core()->GetGC(), 64),
          m_listObject(tl->core()->GetGC(), 64),
          m_listClassInfo(tl->core()->GetGC(), 64)
    {
    }


    void ObjectInput::SetObjectProperty(Atom objectAtom, Stringp name, Atom value)
    {
        TRY(core(), kCatchAction_Ignore)
        {
            Toplevel* toplevel = this->toplevel();
            toplevel->setpropname(objectAtom, name, value);
        }
        CATCH(Exception *exception)
        {
            // http://flashqa.macromedia.com/bugapp/detail.asp?ID=144290
            //
            // Flex has requested read suppresses exceptions thrown while adding members
            // to the object being created (for instance, if the class is sealed and
            // does not contain member name).

            // output the error to the console so it's not entirely lost.
            core()->console << core()->string(exception->atom) << "\n";
        }
        END_CATCH
        END_TRY
    }


    String *ObjectInput::StringListFind(uint32_t i)
    {
        if (i >= m_listString.length())
        {
            ThrowRangeError();
        }

        return m_listString.get(i);
    }

    GCRef<ScriptObject> ObjectInput::ObjectListFind(uint32_t i)
    {
        if (i >= m_listObject.length())
        {
            ThrowRangeError();
        }

        return GCREF_CASTFROMVOID(ScriptObject, m_listObject.get(i));
    }

    ClassInfo *ObjectInput::ClassInfoListFind(uint32_t i)
    {
        if (i >= m_listClassInfo.length())
        {
            ThrowRangeError();
        }

        return m_listClassInfo.get(i);
    }

// end class ObjectInput
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// BEGIN class AvmPlusObjectOutput
//

    void AvmPlusObjectOutput::WriteUint29(uint32_t ref)
    {
        // Represent smaller integers with fewer bytes using the most
        // significant bit of each byte. The worst case uses 32-bits
        // to represent a 29-bit number, which is what we would have
        // done with no compression.

        // 0x00000000 - 0x0000007F : 0xxxxxxx
        // 0x00000080 - 0x00003FFF : 1xxxxxxx 0xxxxxxx
        // 0x00004000 - 0x001FFFFF : 1xxxxxxx 1xxxxxxx 0xxxxxxx
        // 0x00200000 - 0x3FFFFFFF : 1xxxxxxx 1xxxxxxx 1xxxxxxx xxxxxxxx
        // 0x40000000 - 0xFFFFFFFF : throw range exception

        if (ref < 0x80) {
            // 0x00000000 - 0x0000007F : 0xxxxxxx
            WriteU8((uint8_t)ref);
        } else if (ref < 0x4000) {
            // 0x00000080 - 0x00003FFF : 1xxxxxxx 0xxxxxxx
            WriteU8((uint8_t)(((ref >> 7) & 0x7F) | 0x80));
            WriteU8((uint8_t)(ref & 0x7F));
        } else if (ref < 0x200000) {
            // 0x00004000 - 0x001FFFFF : 1xxxxxxx 1xxxxxxx 0xxxxxxx
            WriteU8((uint8_t)(((ref >> 14) & 0x7F) | 0x80));
            WriteU8((uint8_t)(((ref >>  7) & 0x7F) | 0x80));
            WriteU8((uint8_t)(ref & 0x7F));
        } else if (ref < 0x40000000) {
            // 0x00200000 - 0x3FFFFFFF : 1xxxxxxx 1xxxxxxx 1xxxxxxx xxxxxxxx
            WriteU8((uint8_t)(((ref >> 22) & 0x7F) | 0x80));
            WriteU8((uint8_t)(((ref >> 15) & 0x7F) | 0x80));
            WriteU8((uint8_t)(((ref >>  8) & 0x7F) | 0x80));
            WriteU8((uint8_t)(ref & 0xFF));
        } else {
            // 0x40000000 - 0xFFFFFFFF : throw range exception
            ThrowRangeError();
        }
    }

    void AvmPlusObjectOutput::WriteString(String *str) {

        if (str->length() == 0)
        {
            // don't create a reference for the empty string,
            // as it's represented by the one byte value 1
            // len = 0, ((len << 1) | 1).
            WriteReference(1);
            return;
        }

        // intern so (str1 == str2) is true if the strings are the same.
        str = core()->internString(str);

        int32_t ref = StringTableFind(str);

        if (ref >= 0)
        {
            // this is a duplicate of a previous String, so write a reference.
            WriteReference(ref << 1);
        }
        else
        {
            StringTableAdd(str);

            StUTF8String utf8(str);
            uint32_t len = utf8.length();

            WriteReference((len << 1) | 1);
            Write(utf8.c_str(), len);
        }
    }


    void AvmPlusObjectOutput::WriteAtom(Atom value)
    {
        if (handleAdditionalAtom(value))
            return; //Handled by a sub-class implementation. Nothing to be done here
            
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<AvmCore> core = toplevel->core();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();

        if (AvmCore::isUndefined(value))
        {
            WriteType(kUndefinedAtomType);
        }
        else if (AvmCore::isNull(value))
        {
            WriteType(kNullAtomType);
        }
        else if (AvmCore::isBoolean(value))
        {
            if (value == trueAtom)
            {
                WriteType(kTrueAtomType);
            }
            else
            {
                WriteType(kFalseAtomType);
            }

        }
        else if (atomIsIntptr(value))
        {
            intptr_t const val = atomGetIntptr(value);
            // Does our integer fit into 29 bits?
            if (atomCanBeInt32(value) && ((int32_t(val)<<3)>>3) == val)
            {
                // 32-bit: AVM+ integers are int29, so use reference
                // 64-bit: Our check above protects large integers
                WriteType(kIntegerAtomType);
                WriteUint29(val & 0x1fffffff);
            }
            else
            {
                WriteType(kDoubleAtomType);
                WriteDouble((double)val);
            }
        }
        else if (AvmCore::isDouble(value))
        {
            WriteType(kDoubleAtomType);
            WriteDouble(AvmCore::number(value));
        } 
#ifdef VMCFG_FLOAT
        else if(AvmCore::isFloat(value))
        {
            WriteType(kFloatAtomType);
            WriteFloat(AvmCore::atomToFloat(value));
        }
        else if(AvmCore::isFloat4(value))
        {
            float4_t floatVal = AvmCore::atomToFloat4(value);
            WriteType(kFloat4AtomType);
            WriteFloat4(floatVal);
        }
#endif// VMCFG_FLOAT
        else if (AvmCore::isString(value)) 
        {
            WriteType(kStringAtomType);
            WriteString(core->string(value));
        }
        else if (core->isFunction(value))
        {
            // don't serialize functions
            WriteType(kUndefinedAtomType);
        }
        else if (!AvmCore::isObject(value))
        {
            // We've enumerated everything we know but object,
            // so if it isn't an object, it's something we cannot serialize.
            ThrowArgumentError();
        }
        else
        {
            GCRef<ScriptObject> obj = AvmCore::atomToScriptObject(value);

            // cn: Note we ignore XMLList type here, which is actually different from the XML type.
            //     AMF is a published format, however, so we can't add a new datatype to correctly
            //     handle XMLList.  We'll write a generic Object type in that case (see default below),
            //     workaround is to wrap your XMLList value in an XML wrapper.

            if (AvmCore::istype(value, core->traits.xml_itraits))
            {
                WriteType(kAvmPlusXmlAtomType);
                WriteXML(obj,true);
            }
            else if (AvmCore::istype(value, core->traits.date_itraits))
            {
                WriteType(kDateAtomType);
                WriteDate(obj.staticCast<DateObject>());
            }
            else if (AvmCore::istype(value, core->traits.array_itraits))
            {
                WriteType(kArrayAtomType);
                WriteArray(obj.staticCast<ArrayObject>());
            }
            else if (builtinClasses->get_ByteArrayClass()->isType(value))
            {
                WriteType(kByteArrayAtomType);
                WriteByteArray(obj.staticCast<ByteArrayObject>());
            }
            else if (builtinClasses->get_DictionaryClass()->isType(value))
            {
                WriteDictionary(obj.staticCast<DictionaryObject>());
            }
            else if (AvmCore::istype(value, VECTORINT_TYPE) ||
                        AvmCore::istype(value, VECTORUINT_TYPE) ||
                        AvmCore::istype(value, VECTORDOUBLE_TYPE) ||
#ifdef VMCFG_FLOAT
                        AvmCore::istype(value, VECTORFLOAT_TYPE) ||
                        AvmCore::istype(value, VECTORFLOAT4_TYPE) ||
#endif
                        AvmCore::istype(value, VECTOROBJ_TYPE)) 
            {
                // We assume vectors can only be created from within SWF10 files.
                WriteTypedVector(value);
            }
            else
            {
                WriteType(kObjectAtomType);
                WriteScriptObject(obj);
            }
        }
    }

    void AvmPlusObjectOutput::WriteXML(GCRef<ScriptObject> obj, bool isAS3XML)
    {
        int32_t ref = ObjectTableFind(obj);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }

        ObjectTableAdd(obj);

        String* str = NULL;
        // cn:  XML's toString() will skip the tags if the content is simple (i.e. <f>hi</f>.toString() == "hi")
        //        use toXMLString instead.
        if (isAS3XML)
        {
            XMLObject* x = obj.staticCast<XMLObject>();
            str = x->toXMLString();
        }
        else
        {
            str = obj->toString();
        }

        StUTF8String utf8(str);
        uint32_t len = utf8.length();

        // Store length in the surplus bits of the invalid reference
        WriteReference((len << 1) | 1);
        Write(utf8.c_str(), len);
    }

    void AvmPlusObjectOutput::WriteDate(DateObject *obj)
    {
        int32_t ref = ObjectTableFind(obj);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        // write an invalid reference, followed by the date.
        // As the date is only a double, it's hard to re-use the surplus bits of the reference.
        WriteReference(1);
        WriteDouble(obj->date.getTime());
    }

    void AvmPlusObjectOutput::WriteByteArray(GCRef<ByteArrayObject> obj)
    {
        int32_t ref = ObjectTableFind(obj);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        //
        // obj might be the ByteArray we are writing to, such as
        //  o.ba = ba;
        //  ba.writeObject(o);
        // so play it safe and duplicate the ByteArray before copying.
        //
        // ByteArray ba(obj->GetByteArray());
        
        // actually, it's been legal for a ByteArray to write to itself for quite
        // a while, so duplicating this data is unnecessary
        ByteArray& ba = obj->GetByteArray();

        int len = ba.GetLength();
        const uint8_t* buf = ba.GetReadableBuffer();

        // write an invalid reference, but re-use the surplus bits to store the length
        WriteReference((len << 1) | 1);
        Write(buf, len);
    }

    void AvmPlusObjectOutput::WriteArray(GCRef<ArrayObject> obj)
    {
        int32_t ref = ObjectTableFind(obj);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        // Arrays could be written just like Objects, as an undefined terminated
        // list of {name, value} pairs, but we'd like to avoid writing the name's
        // if they are all integers. So, we calculate the smallest range that has no
        // holes, and emit that as a list, with any extra properties getting emitted as a map.
        // (Array used to have a "getDenseLength" call that would we'd use, but that relied
        // on the underlying representation, which has changed in the name of efficiency;
        // therefore we calculate it manually here. Note that this could cause some arrays
        // to be written in slightly different form (ie with more name/value pairs), but that's
        // OK, just suboptimal.)
        uint32_t denseLen = obj->getLength();
        for (uint32_t i = 0, n = denseLen; i < n; ++i)
        {
            if (!obj->hasUintProperty(i))
            {
                denseLen = i;
                break;
            }
        }

        uint32_t len;
        int index;
        uint32_t i;

        // This is a bit of a pest, but we don't write out functions,
        // and we don't want any holes in the dense portion. AVM-
        // wrote an undefined in this case, but we don't want read to
        // create an undefined element, and so we scan this dense portion
        // searching for functions, and contract the length to exclude them.

        for (i = 0, index = 0, len = denseLen; (i < len); i++)
        {
            index = obj->nextNameIndex(index);
            Atom value = obj->nextValue(index);

            if (AvmCore::istype(value, core()->traits.function_itraits))
            {
                len = i;
                break;
            }
        }

        // Write an invalid reference. Repurpose the unused bits to store the number of dense elements.
        WriteReference((len << 1) | 1);

        // When the entire Array is dense, the server will create a List.
        // When the Array has holes, or contains elements indexed with non-numeric values,
        // the server will create a Map.
        //
        // Therefore, it's easier for the server if we write out the non-dense portion first.
        while ((index = obj->nextNameIndex(index)) != 0)
        {
            Atom name = obj->nextName(index);
            Atom value = obj->nextValue(index);

            if (AvmCore::istype(value, core()->traits.function_itraits))
            {
                continue;
            }

            WriteString(core()->string(name));
            WriteAtom(value);
        }

        // Terminate the non-dense portion with an empty string (which is invalid as a property name)
        WriteString(core()->kEmptyString);

        // Write out the dense portion of the array
        for (i = 0, index = 0; i < len; i++)
        {
            index = obj->nextNameIndex(index);
            WriteAtom(obj->nextValue(index));
        }
    }

    void AvmPlusObjectOutput::WriteDictionary(GCRef<DictionaryObject> obj)
    {
        int32_t ref = ObjectTableFind(obj);

        WriteType(kDictionaryObject);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        InlineHashtable *h = obj->getTable();

        WriteReference((h->getSize()<<1)|1);
        
        WriteBoolean(obj->isUsingWeakKeys());
        
        for (uint32_t c = h->next(0); c ; c = h->next(c))
        {
            Atom key = h->keyAt(c);
            // The dictionary class converts keys to integers if possible
            // so convert it back to a string if neccessary.
            if (atomIsIntptr(key))
            {
                key = MathUtils::convertIntegerToStringRadix(core(), atomGetIntptr(key), 10, MathUtils::kTreatAsSigned)->atom();
            }
            WriteAtom(key);
            WriteAtom(h->valueAt(c));
        }
    }

    void AvmPlusObjectOutput::WriteTypedVector(Atom atom)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        GCRef<ScriptObject> obj =  core->atomToScriptObject(atom);
        int32_t ref = ObjectTableFind(obj);

        uint8_t vec_type = 0;
        // Write out the type - have to do this before we write out the reference
        if (AvmCore::istype(atom, VECTORINT_TYPE))
        {
            vec_type = kTypedVectorInt;
        }
        else if (AvmCore::istype(atom, VECTORUINT_TYPE))
        {
            vec_type = kTypedVectorUint;
        }
        else if (AvmCore::istype(atom, VECTORDOUBLE_TYPE))
        {
            vec_type = kTypedVectorDouble;
        } 
#ifdef VMCFG_FLOAT
        else if(AvmCore::istype(atom, VECTORFLOAT_TYPE))
        {
            vec_type = kTypedVectorFloat;
        }
        else if(AvmCore::istype(atom, VECTORFLOAT4_TYPE))
        {
            vec_type = kTypedVectorFloat4;
        }
#endif// VMCFG_FLOAT
        else 
        {
            vec_type = kTypedVectorObject;
        }
        
        WriteType(vec_type);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        if (vec_type == kTypedVectorInt)
        {
            IntVectorObject *v = obj.staticCast<IntVectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            IntVectorAccessor va(v);
            const int32_t* data = va.addr();
            uint32_t len = va.length();
            for (uint32_t c=0; c<len; c++)
            {
                WriteU32(uint32_t(data[c]));
            }
            
        }
        else if (vec_type == kTypedVectorUint)
        {
            UIntVectorObject *v = obj.staticCast<UIntVectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            UIntVectorAccessor va(v);
            const uint32_t* data = va.addr();
            uint32_t len = va.length();
            for (uint32_t c=0; c<len; c++)
            {
                WriteU32(data[c]);
            }
            
        }
        else if (vec_type == kTypedVectorDouble)
        {
            DoubleVectorObject *v = obj.staticCast<DoubleVectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            DoubleVectorAccessor va(v);
            const double* data = va.addr();
            uint32_t len = va.length();
            for (uint32_t c=0; c<len; c++)
            {
                WriteDouble(data[c]);
            }
        } 
#ifdef VMCFG_FLOAT
        else if (vec_type == kTypedVectorFloat) 
        {
            FloatVectorObject *v = obj.staticCast<FloatVectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            FloatVectorAccessor va(v);
            const float* data = va.addr();
            uint32_t len = va.length();
            for (uint32_t c=0; c<len; c++) 
            {
                WriteFloat(data[c]);
            }
        } 
        else if (vec_type == kTypedVectorFloat4) 
        {
            Float4VectorObject *v = obj.staticCast<Float4VectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            Float4VectorAccessor va(v);
            const float4_t* data = va.addr();
            uint32_t len = va.length();
            for (uint32_t c=0; c<len; c++) 
            {
                WriteFloat4(data[c]);
            }
        } 
#endif // VMCFG_FLOAT
        else 
        {
            ObjectVectorObject *v = obj.staticCast<ObjectVectorObject>();
            WriteReference((v->get_length()<<1)|1);
            WriteBoolean(v->get_fixed());

            // Write prototype
            Traits* traits = v->getTypeTraits();
            Stringp classname;
            if (traits)
            {
                classname = toplevel->getAliasFromTraits(traits);
            }
            else
            {
                classname = core->kAsterisk;
            }
            WriteString(classname);

            // Write contents
            uint32_t len = v->get_length();
            for (uint32_t c=0; c<len; c++)
            {
                WriteAtom(v->getUintProperty(c));
            }
        }
    }

    void AvmPlusObjectOutput::WriteScriptObject(GCRef<ScriptObject> obj)
    {
        int32_t ref = ObjectTableFind(obj);

        if (ref >= 0)
        {
            // output already contains object. Save a reference to previous instance.
            WriteReference(ref << 1);
            return;
        }
        
        ObjectTableAdd(obj);

        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        //
        // Start an object with its class information
        //
        Traits *traits = obj->traits();
        ClassInfo *info;
        int i, count;
        bool dynamic;
        bool externalizable;

        ref = TraitsTableFind(traits);
        if (ref >= 0)
        {
            // output already contains class info. Save a reference to previous instance.
            WriteReference((ref << 2) | 1);
            info = ClassInfoListFind(ref);
            count = info->SealedPropCount();
            dynamic = info->dynamic();
            externalizable = info->externalizable();
        }
        else
        {
            // Create class info
            info = new (core->GetGC()) ClassInfo(toplevel, traits);

            // Add class info to relate tables
            ClassInfoListAdd(info);
            TraitsTableAdd(traits);
            AvmAssert(ClassInfoListFind(TraitsTableFind(traits)) == info);

            // write out class info, using the surplas bits of the reference to
            // store dynamic and the number of sealed properties
            count = info->SealedPropCount();
            dynamic = info->dynamic();
            externalizable = info->externalizable();

            WriteReference(3 | (externalizable ? 4 : 0) | (dynamic ? 8 : 0) | (count << 4));
            info->Write(this);
        }

        if (externalizable)
        {
            // ClassInfo's constructor has already verified this, so we don't need to repeat the error handling here
            AvmAssert(obj->traits()->containsInterface(BUILTIN_TRAITS_(flash_utils_IExternalizable)));

            // !!@ Is it a worthwhile performance optimization to :
            //  1. create only one ObjectOutputObject for the stream?

            // invoke writeExternal()
            const int argc = 1;
            Atom argv[argc + 1];
            argv[0] = obj->atom();

            GCRef<ObjectOutputObject> output = toplevel->builtinClasses()->get_ObjectOutputClass()->constructObject();
            output->m_out = this;
            argv[1] = output->atom();

            MethodEnv* method = obj->vtable->methods[AvmCore::bindingToMethodId(info->get_functionBinding())];
            method->coerceEnter(argc, argv);

        }
        else
        {
            // for each sealed property in the class info, write out its value
            for (i = 0; i < count; i++)
            {
                WriteAtom(toplevel->getpropname(obj->atom(), info->SealedProp(i)));
            }

            if (dynamic)
            {
                GCRef<ScriptObject> writer = toplevel->builtinClasses()->get_ObjectEncodingClass()->m_writer;

                if (!writer)
                {
                    for (int index = 0; ((index = obj->nextNameIndex(index)) != 0);)
                    {
                        Atom name = obj->nextName(index);
                        Atom value = obj->nextValue(index);

                        if (AvmCore::istype(value, core->traits.function_itraits))
                        {
                            continue;
                        }

                        // Empty string is sentinel for end-of-data, so we CANNOT serialize it.
                        String *nameStr = core->string(name);
                        if (nameStr->length() == 0)
                        {
                            continue;
                        }
                        WriteString(nameStr);
                        WriteAtom(value);
                    }
                }
                else
                {
                    // invoke ObjectEncoding.dynamicPropertyWriter
                    GCRef<ScriptObject> func =  AvmCore::atomToScriptObject(toplevel->getpropname(writer->atom(), core->internConstantStringLatin1("writeDynamicProperties")));
                    GCRef<DynamicPropertyOutputObject> output = toplevel->builtinClasses()->get_DynamicPropertyOutputClass()->constructObject();
                    output->m_out = this;

                    const int argc = 2;
                    Atom argv[argc + 1];
                    argv[0] = writer->atom();
                    argv[1] = obj->atom();
                    argv[2] = output->atom();

                    func->call(argc, argv);
                }

                // Write out the empty string as a terminator
                WriteString(core->kEmptyString);
            }
        }
    }



// END class AvmPlusObjectOutput
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// BEGIN class AvmPlusObjectInput
//
    uint32_t AvmPlusObjectInput::ReadUint29()
    {
        // Represent smaller integers with fewer bytes using the most
        // significant bit of each byte. The worst case uses 32-bits
        // to represent a 29-bit number, which is what we would have
        // done with no compression.

        // 0x00000000 - 0x0000007F : 0xxxxxxx
        // 0x00000080 - 0x00003FFF : 1xxxxxxx 0xxxxxxx
        // 0x00004000 - 0x001FFFFF : 1xxxxxxx 1xxxxxxx 0xxxxxxx
        // 0x00200000 - 0x3FFFFFFF : 1xxxxxxx 1xxxxxxx 1xxxxxxx xxxxxxxx
        // 0x40000000 - 0xFFFFFFFF : throw range exception

        uint8_t byte;
        uint32_t ref;

        byte = ReadU8();

        if (byte < 128)
        {
            return byte;
        }

        ref = (byte & 0x7F) << 7;
        byte = ReadU8();

        if (byte < 128)
        {
            return (ref | byte);
        }
    
        ref = (ref | (byte & 0x7F)) << 7;
        byte = ReadU8();

        if (byte < 128)
        {
            return (ref | byte);
        }

        ref = (ref | (byte & 0x7F)) << 8;
        byte = ReadU8();

        return (ref | byte);
    }

    Atom AvmPlusObjectInput::ReadAtom()
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        uint8_t type = ReadType();

        Atom retVal;
        if (handleAdditionalType(type, retVal))
            return retVal;      // Handled by subclass. Nothing to be done here

        switch (type)
        {
            case kUndefinedAtomType:
                return undefinedAtom;
            case kNullAtomType:
                return nullObjectAtom;
            case kFalseAtomType:
                return falseAtom;
            case kTrueAtomType:
                return trueAtom;
            case kIntegerAtomType:
                // symmetric with writing an integer to fix sign bits for negative values
                return core->intToAtom(((int)ReadUint29() << 3) >> 3);
            case kDoubleAtomType:
                return core->doubleToAtom(ReadDouble());
#ifdef VMCFG_FLOAT
            case kFloatAtomType:
                return core->floatToAtom(ReadFloat());
            case kFloat4AtomType:
                return core->float4ToAtom(ReadFloat4());
#endif //VMCFG_FLOAT           
            case kStringAtomType:
                return ReadString()->atom();
            case kAvmMinusXmlAtomType:  //Even XML minus atoms are read back as XML objects. However handleAdditionalType can override this behavior.
            case kAvmPlusXmlAtomType:
                return ReadXML()->atom();
            case kDateAtomType:
                return ReadDate()->atom();
            case kArrayAtomType:
                return ReadArray()->atom();
            case kByteArrayAtomType:
                return ReadByteArray()->atom();
            case kObjectAtomType:
                return ReadScriptObject()->atom();
            case kDictionaryObject:
                return ReadDictionary()->atom();
            case kTypedVectorInt:
            case kTypedVectorUint:
            case kTypedVectorDouble:
            case kTypedVectorFloat:
            case kTypedVectorFloat4:
            case kTypedVectorObject:
                return ReadTypedVector(type);
            default:
                this->ThrowRangeError();
                return undefinedAtom;
        }
    }

    DateObject* AvmPlusObjectInput::ReadDate()
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1).staticCast<DateObject>();
        }

        Toplevel* toplevel = this->toplevel();
        DateClass* dateClass = toplevel->dateClass();
        DateObject* obj = DateObject::create(dateClass->ivtable()->gc(), dateClass, Date(ReadDouble()));
        ObjectListAdd(obj);

        return obj;
    }

    GCRef<ByteArrayObject> AvmPlusObjectInput::ReadByteArray()
    {
        Toplevel* toplevel = this->toplevel();
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1).staticCast<ByteArrayObject>();
        }

        GCRef<ByteArrayObject> obj =  toplevel->byteArrayClass()->constructObject();
        ObjectListAdd(obj);

        int len = ref >> 1;

        ByteArray& ba = obj->GetByteArray();
        ba.SetLength(len);
        Read(ba.GetWritableBuffer(), len);

        return obj;
    }

    String *AvmPlusObjectInput::ReadString()
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return StringListFind(ref >> 1);
        }

        // Read the string in
        uint32_t len = (ref >> 1);

        if (len == 0)
        {
            // WriteString() special cases the empty string to avoid creating a reference.
            return core()->kEmptyString;
        }

        char *buffer = mmfx_new_array_opt(char, len+1, MMgc::kCanFail);
        if (!buffer)
        {
            ThrowMemoryError();
        }

        Read(buffer, len);
        buffer[len] = 0;
        
        String *str = core()->newStringUTF8(buffer, len);
        mmfx_delete_array(buffer);

        StringListAdd(str);
        return str;
    }

    String* AvmPlusObjectInput::ReadXmlString(uint32_t len)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();
        char *buffer = mmfx_new_array_opt(char, len+1, MMgc::kCanFail);
        if (!buffer)
        {
            ThrowMemoryError();
        }

        Read(buffer, len);
        buffer[len] = 0;
        
        String *str = core->newStringUTF8(buffer, len);
        mmfx_delete_array(buffer);
        return str;
    }

    GCRef<ScriptObject> AvmPlusObjectInput::ReadXML()
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1);
        }

        Toplevel* toplevel = this->toplevel();

        // Read the toString() version of the XML object in
        uint32_t len = (ref >> 1);

        String* str = ReadXmlString(len);

        // construct an XML object from the string
        GCRef<ScriptObject> obj;
        obj = toplevel->xmlClass()->constructObject(str->atom());

        ObjectListAdd(obj);

        return obj;
    }

    GCRef<DictionaryObject> AvmPlusObjectInput::ReadDictionary()
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1).staticCast<DictionaryObject>();
        }

        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        uint32_t len = ref >> 1;
        bool weakKeys = ReadBoolean();
        GCRef<DictionaryObject> obj = toplevel->builtinClasses()->get_DictionaryClass()->constructObject(weakKeys);
        ObjectListAdd(obj);

        for (uint32_t c = 0; c<len ; c++)
        {
            // Key can be either an object or an interned string
            Atom key = ReadAtom();
            // Value can be anything
            Atom value = ReadAtom();
            if (AvmCore::isString(key))
            {
                // Intern the string if not done already
                key = core->internString(key)->atom();
            }
            else if (!AvmCore::isObject(key))
            {
                // Keys can not be anything else
                ThrowArgumentError();
            }
            obj->setAtomProperty(key, value);
        }
        return obj;
    }

    Atom AvmPlusObjectInput::ReadTypedVector(uint8_t type)
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1)->atom();
        }

        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        uint32_t len = ref >> 1;
        bool fixed = ReadBoolean();
        
        switch (type)
        {
            case kTypedVectorInt:
            {
                GCRef<IntVectorObject> v = toplevel->intVectorClass()->newVector(len);
                ObjectListAdd(v);
                v->set_fixed(fixed);
                IntVectorAccessor va(v);
                int32_t* data = va.addr();
                for (uint32_t c=0; c<len; c++)
                {
                    data[c] = int32_t(ReadU32());
                }
                return v->atom();
            }
            case kTypedVectorUint:
            {
                GCRef<UIntVectorObject> v = toplevel->uintVectorClass()->newVector(len);
                ObjectListAdd(v);
                v->set_fixed(fixed);
                UIntVectorAccessor va(v);
                uint32_t* data = va.addr();
                for (uint32_t c=0; c<len; c++)
                {
                    data[c] = ReadU32();
                }
                return v->atom();
            }
            case kTypedVectorDouble:
            {
                GCRef<DoubleVectorObject> v = toplevel->doubleVectorClass()->newVector(len);
                ObjectListAdd(v);
                v->set_fixed(fixed);
                DoubleVectorAccessor va(v);
                double* data = va.addr();
                for (uint32_t c=0; c<len; c++)
                {
                    data[c] = ReadDouble();
                }
                return v->atom();
            }
#ifdef VMCFG_FLOAT
            case kTypedVectorFloat: 
            {
                GCRef<FloatVectorObject> v = toplevel->floatVectorClass()->newVector(len);
                ObjectListAdd(v);
                v->set_fixed(fixed);
                FloatVectorAccessor va(v);
                float* data = va.addr();
                for (uint32_t c=0; c<len; c++) 
                {
                    data[c] = ReadFloat();
                }
                return v->atom();
            }
            case kTypedVectorFloat4: 
            {
                GCRef<Float4VectorObject> v = toplevel->float4VectorClass()->newVector(len);
                ObjectListAdd(v);
                v->set_fixed(fixed);
                Float4VectorAccessor va(v);
                float4_t* data = va.addr();
                for (uint32_t c=0; c<len; c++) 
                {
                    data[c] = ReadFloat4();
                }
                return v->atom();
            }
#endif // VMCFG_FLOAT
            case kTypedVectorObject: 
            {
                // Read prototype
                Stringp classname = core->internString(ReadString());
                ClassClosure* closure = NULL;
                if (classname != core->kAsterisk)
                    closure = toplevel->getClassClosureFromAlias(classname);
                GCRef<ObjectVectorObject> v =  toplevel->vectorClass()->newVector(closure, len);
                ObjectListAdd(v);
                v->set_fixed(fixed);

                // Read contents
                for (uint32_t c=0; c<len; c++)
                {
                    v->setUintProperty(c,ReadAtom());
                }
                return v->atom();
            }
        }

        ThrowArgumentError();

        return 0;
    }

    GCRef<ScriptObject> AvmPlusObjectInput::ReadScriptObject(ClassClosure** closure)
    {
        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This object has already been in input, so (ref >> 1) is an index
            return ObjectListFind(ref >> 1);
        }

        // The object has not previously been in input, so we must read it in.
        // It begins with its class info

        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        ClassInfo *info;

        if ((ref & 3) == 1)
        {
            // This ClassInfo has already been in input, so (ref >> 2) is an index
            info = ClassInfoListFind(ref >> 2);
        }
        else
        {
            // The ClassInfo has not previously been in input, so we must read it in.
            // dynamic, and the number of multinames to follow have been encoded
            // in the surplus bits of ref.
            bool externalizable = (ref & 4) ? true : false;
            bool dynamic = (ref & 8) ? true : false;
            int count = (ref >> 4);

            info = ClassInfo::Read(toplevel, this, dynamic, externalizable, count);
            ClassInfoListAdd(info);
        }

        // Create an object and add it to the list of objects already read in
        GCRef<ScriptObject> obj = info->closure()->constructObject();
        ObjectListAdd(obj);

        if (closure)
        {
            *closure = info->closure();
        }

        if (info->externalizable())
        {
            // ClassInfo::Read() has already verified that the class closure registered for this
            // class's alias implements IExternalizable, so there's no need to repeat the error handling here.
            AvmAssert(obj->traits()->containsInterface(BUILTIN_TRAITS_(flash_utils_IExternalizable)));

            // invoke readExternal()

            const int argc = 1;
            Atom argv[argc + 1];
            argv[0] = obj->atom();

            GCRef<ObjectInputObject> input = toplevel->builtinClasses()->get_ObjectInputClass()->constructObject();
            input->m_in = this;
            argv[1] = input->atom();

            MethodEnv* method = obj->vtable->methods[AvmCore::bindingToMethodId(info->get_functionBinding())];
            method->coerceEnter(argc, argv);
        }
        else
        {
            // read in the value of each sealed property, and then assign it to the object's member
            for (int i = 0, len = info->SealedPropCount(); i < len; i++)
            {
                SetObjectProperty(obj->atom(), info->SealedProp(i), ReadAtom());
            }
            
            // if the class is dynamic, read in the remaining properties (if any)
            if (info->dynamic())
            {
                for (;;)
                {
                    String* name = ReadString();
                    if (name->length() == 0)
                        break;

                    Atom value = ReadAtom();
                    SetObjectProperty(obj->atom(), core->internString(name), value);
                }
            }
        }

        return obj;
    }

    GCRef<ArrayObject> AvmPlusObjectInput::ReadArray()
    {
        AvmCore *core = this->core(); // this->core() is necessary when there is a local variable called core.

        uint32_t ref = ReadReference();

        if ((ref & 1) == 0)
        {
            // This is a reference
            return ObjectListFind(ref >> 1).staticCast<ArrayObject>();
        }

        uint32_t len = (ref >> 1);
        uint32_t i;

        Toplevel* toplevel = this->toplevel();
        GCRef<ArrayObject> obj =  toplevel->arrayClass()->newArray(len);
        ObjectListAdd(obj);

        // read the non-dense portion first, terminated by the empty string
        for (;;)
        {
            String* name = ReadString();
            if (name->length() == 0)
                break;

            Atom value = ReadAtom();
            SetObjectProperty(obj->atom(), core->internString(name), value);
        }

        // now read the dense portion.
        for (i = 0; i < len; i ++)
        {
            obj->setUintProperty(i, ReadAtom());
        }

        return obj;
    }
// END class AvmPlusObjectInput
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// BEGIN class ClassInfo is used by class ObjectInput and ObjectOutput

    // for output, construct information from Traits

    bool ClassInfo::containsTransientMetadata(PoolObject* pool, const uint8_t* meta_pos)
    {
        AvmCore* core = m_toplevel->core();
        return meta_pos ? pool->hasMetadataName(meta_pos, core->internConstantStringLatin1(kTransient)) : false;
    }

    bool ClassInfo::slotContainsTransientMetadata(Traits *traits, int index)
    {
        PoolObject* pool = NULL;
        const uint8_t* pos = traits->getTraitsMetadata()->getSlotMetadataPos(index, pool);
        return containsTransientMetadata(pool, pos);
    }

    bool ClassInfo::methodContainsTransientMetadata(Traits *traits, int index)
    {
        PoolObject* pool = NULL;
        const uint8_t* pos = traits->getTraitsMetadata()->getMethodMetadataPos(index, pool);
        return containsTransientMetadata(pool, pos);
    }

    bool ClassInfo::isSerializable(Traits * t, Namespace* ns, Binding b)
    {
        // ignore members that are not public and not compatible with the current version
        if (!ns->isPublic() || !m_toplevel->core()->isNamespaceVisibleToApiVersionFromCallStack(ns))
        {
            return false;
        }

        if (AvmCore::isVarBinding(b))
        {
            uint32_t slot = AvmCore::bindingToSlotId(b);
            if (slotContainsTransientMetadata(t, slot))
            {
                // exclude slots contain metadata transient
                return false;
            }

        }
        else if (AvmCore::isAccessorBinding(b))
        {
            if (!AvmCore::hasGetterBinding(b) || methodContainsTransientMetadata(t, AvmCore::bindingToGetterId(b)))
            {
                // must implement a non-transient get accessor
                return false;
            }

            if (!AvmCore::hasSetterBinding(b) || methodContainsTransientMetadata(t, AvmCore::bindingToSetterId(b)))
            {
                // must implement a non-transient set accessor
                return false;
            }

        }
        else
        {
            // ignore members that aren't variables or accessors
            return false;
        }

        return true;
    }


    ClassInfo::ClassInfo(Toplevel* toplevel, Traits *t) :
        m_toplevel(toplevel),
        m_traits(t),
        m_dynamic(t->needsHashtable()),
        m_sealed(toplevel->core()->GetGC(), 64)
    {
        // initialize m_name
        m_name = toplevel->getAliasFromTraits(t);

        if (t->containsInterface(BUILTIN_TRAITS_(flash_utils_IExternalizable)))
        {
            if (m_name->isEmpty())
            {
                // readExternal() does not exist on class Object,
                // so it is meaningless to writeExternal() without a class name,
                // as an instance of Object will be created when we decode the data.

                // !!@ FIXME : Create a new exception for this?
                toplevel->argumentErrorClass()->throwError(kInvalidParamError);
            }
            
            AvmCore* core = toplevel->core();
            Multiname mn(core->getPublicNamespace(t->pool), core->internConstantStringLatin1(kWriteExternal));
            m_functionBinding = toplevel->getBinding(t, &mn);
            return;
        }

        TraitsIterator iterator(t);
        Stringp name;
        Namespace *ns;
        Binding b;

        // Add the name:String of all public variables to m_sealed.
        // For our purposes, a getter/setter is considered a variable too.
        while (iterator.getNext(name, ns, b))
        {
            if (isSerializable(t, ns, b))
            {
                m_sealed.add(name);
            }
        }
    }

    void ClassInfo::Write(AvmPlusObjectOutput *output)
    {
        int len = SealedPropCount();

        output->WriteString(m_name);

        for (int i = 0; i < len; i++)
        {
            // write the name:String for each property (SealedPropCount() was encoded in the invalid ref id
            output->WriteString(SealedProp(i));
        }
    }

    // for input, construct information from DataInput
    ClassInfo *ClassInfo::Read(Toplevel *toplevel, AvmPlusObjectInput *input, bool dynamic, bool externalizable, int count)
    {
        AvmCore* core = toplevel->core();
        ClassInfo *info = new (core->GetGC()) ClassInfo(toplevel);
        int i;

        info->m_dynamic = dynamic;
        info->m_name = core->internString(input->ReadString());

        // initialize m_closure
        info->m_closure = toplevel->getClassClosureFromAlias(info->m_name);

        if (externalizable)
        {
            if (!info->m_closure->traits()->itraits->containsInterface(BUILTIN_TRAITS_(flash_utils_IExternalizable)))
            {
                // object was encoded with writeExternal, but current class does not implement readExternal,
                // so data is meaningless to us.
                toplevel->argumentErrorClass()->throwError(kReadExternalNotImplementedError, info->m_name);
            }

            Multiname mn(core->findPublicNamespace(), core->internConstantStringLatin1(kReadExternal));
            info->m_functionBinding = toplevel->getBinding(info->m_closure->traits()->itraits, &mn);
        }

        info->m_traits = NULL; // unused when reading

        for (i = 0; i < count; i++)
        {
            String *name = input->ReadString();
            info->m_sealed.add(core->internString(name));
        }

        return info;
    }

// end class ClassInfo
//---------------------------------------------------------------------

}

