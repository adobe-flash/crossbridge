/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* machine generated file -- do not edit */

#ifndef _H_nativegen_classes_shell_toplevel
#define _H_nativegen_classes_shell_toplevel

namespace avmplus {

//-----------------------------------------------------------
// avmplus::ITest
//-----------------------------------------------------------
class ITestInterface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::ITestClass;
    REALLY_INLINE explicit ITestInterface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit ITestInterface(const ITestInterface&); // unimplemented
    void operator=(const ITestInterface&); // unimplemented
};

//-----------------------------------------------------------
// avmplus::ITest$
//-----------------------------------------------------------
class ITestClass : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::ITestInterface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::ITestInterface>((avmplus::ITestInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ITestInterface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::ITestInterface>((avmplus::ITestInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ITestInterface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::ITestInterface>((avmplus::ITestInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ITestInterface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::ITestInterface>((avmplus::ITestInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit ITestClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit ITestClass(const ITestClass&); // unimplemented
    void operator=(const ITestClass&); // unimplemented
};

//-----------------------------------------------------------
// avmplus::CTest
//-----------------------------------------------------------
class CTestObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::CTestClass;
    REALLY_INLINE explicit CTestObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit CTestObject(const CTestObject&); // unimplemented
    void operator=(const CTestObject&); // unimplemented
};

#define avmplus_CTestObject_isExactInterlock 1
//-----------------------------------------------------------
// avmplus::CTest$
//-----------------------------------------------------------
class CTestClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::CTestObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::CTestObject>((avmplus::CTestObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::CTestObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::CTestObject>((avmplus::CTestObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CTestObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::CTestObject>((avmplus::CTestObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CTestObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::CTestObject>((avmplus::CTestObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CTestObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::CTestObject>((avmplus::CTestObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit CTestClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit CTestClass(const CTestClass&); // unimplemented
    void operator=(const CTestClass&); // unimplemented
};

#define avmplus_CTestClass_isExactInterlock 1
//-----------------------------------------------------------
// avmplus::System
//-----------------------------------------------------------
class SystemObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmshell::SystemClass;
    REALLY_INLINE explicit SystemObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit SystemObject(const SystemObject&); // unimplemented
    void operator=(const SystemObject&); // unimplemented
};

#define avmplus_SystemObject_isExactInterlock 1
//-----------------------------------------------------------
// avmplus::File
//-----------------------------------------------------------
class FileObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmshell::FileClass;
    REALLY_INLINE explicit FileObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit FileObject(const FileObject&); // unimplemented
    void operator=(const FileObject&); // unimplemented
};

#define avmplus_FileObject_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseAS3
//-----------------------------------------------------------
class NativeBaseAS3Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::NativeBaseAS3Class;
    REALLY_INLINE explicit NativeBaseAS3Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit NativeBaseAS3Object(const NativeBaseAS3Object&); // unimplemented
    void operator=(const NativeBaseAS3Object&); // unimplemented
};

#define avmplus_NativeBaseAS3Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseAS3$
//-----------------------------------------------------------
class NativeBaseAS3Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::NativeBaseAS3Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct_native(avmplus::NativeBaseAS3Class::createInstanceProc, 0, args);
        return GCRef<avmplus::NativeBaseAS3Object>((avmplus::NativeBaseAS3Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseAS3Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::NativeBaseAS3Object>((avmplus::NativeBaseAS3Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseAS3Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseAS3Object>((avmplus::NativeBaseAS3Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseAS3Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::NativeBaseAS3Object>((avmplus::NativeBaseAS3Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseAS3Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseAS3Object>((avmplus::NativeBaseAS3Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit NativeBaseAS3Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit NativeBaseAS3Class(const NativeBaseAS3Class&); // unimplemented
    void operator=(const NativeBaseAS3Class&); // unimplemented
};

#define avmplus_NativeBaseAS3Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::ShellCoreFriend1
//-----------------------------------------------------------
class ShellCoreFriend1Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
    friend class avmshell::ShellCore;
protected:
    friend class avmplus::ShellCoreFriend1Class;
    REALLY_INLINE explicit ShellCoreFriend1Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit ShellCoreFriend1Object(const ShellCoreFriend1Object&); // unimplemented
    void operator=(const ShellCoreFriend1Object&); // unimplemented
};

#define avmplus_ShellCoreFriend1Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::ShellCoreFriend1$
//-----------------------------------------------------------
class ShellCoreFriend1Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::ShellCoreFriend1Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::ShellCoreFriend1Object>((avmplus::ShellCoreFriend1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend1Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::ShellCoreFriend1Object>((avmplus::ShellCoreFriend1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend1Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::ShellCoreFriend1Object>((avmplus::ShellCoreFriend1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend1Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::ShellCoreFriend1Object>((avmplus::ShellCoreFriend1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend1Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::ShellCoreFriend1Object>((avmplus::ShellCoreFriend1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
    friend class avmshell::ShellCore;
protected:
    REALLY_INLINE int32_t get_foo() const { return m_slots_ShellCoreFriend1Class.m_private_foo; }
    REALLY_INLINE void set_foo(int32_t newVal) { m_slots_ShellCoreFriend1Class.m_private_foo = newVal; }
private:
    avmplus::NativeID::avmplus_ShellCoreFriend1ClassSlots m_slots_ShellCoreFriend1Class;
protected:
    inline explicit ShellCoreFriend1Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit ShellCoreFriend1Class(const ShellCoreFriend1Class&); // unimplemented
    void operator=(const ShellCoreFriend1Class&); // unimplemented
};

#define avmplus_ShellCoreFriend1Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::ShellCoreFriend2
//-----------------------------------------------------------
class ShellCoreFriend2Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
    friend class avmshell::ShellCore;
    friend class avmshell::ShellToplevel;
protected:
    friend class avmplus::ShellCoreFriend2Class;
    REALLY_INLINE explicit ShellCoreFriend2Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit ShellCoreFriend2Object(const ShellCoreFriend2Object&); // unimplemented
    void operator=(const ShellCoreFriend2Object&); // unimplemented
};

#define avmplus_ShellCoreFriend2Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::ShellCoreFriend2$
//-----------------------------------------------------------
class ShellCoreFriend2Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::ShellCoreFriend2Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::ShellCoreFriend2Object>((avmplus::ShellCoreFriend2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend2Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::ShellCoreFriend2Object>((avmplus::ShellCoreFriend2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend2Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::ShellCoreFriend2Object>((avmplus::ShellCoreFriend2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend2Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::ShellCoreFriend2Object>((avmplus::ShellCoreFriend2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ShellCoreFriend2Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::ShellCoreFriend2Object>((avmplus::ShellCoreFriend2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
    friend class avmshell::ShellCore;
    friend class avmshell::ShellToplevel;
protected:
    REALLY_INLINE int32_t get_bar() const { return m_slots_ShellCoreFriend2Class.m_private_bar; }
    REALLY_INLINE void set_bar(int32_t newVal) { m_slots_ShellCoreFriend2Class.m_private_bar = newVal; }
private:
    avmplus::NativeID::avmplus_ShellCoreFriend2ClassSlots m_slots_ShellCoreFriend2Class;
protected:
    inline explicit ShellCoreFriend2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit ShellCoreFriend2Class(const ShellCoreFriend2Class&); // unimplemented
    void operator=(const ShellCoreFriend2Class&); // unimplemented
};

#define avmplus_ShellCoreFriend2Class_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::Capabilities
//-----------------------------------------------------------
class CapabilitiesObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::CapabilitiesClass;
    REALLY_INLINE explicit CapabilitiesObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit CapabilitiesObject(const CapabilitiesObject&); // unimplemented
    void operator=(const CapabilitiesObject&); // unimplemented
};

#define avmplus_CapabilitiesObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::Capabilities$
//-----------------------------------------------------------
class CapabilitiesClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::CapabilitiesObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::CapabilitiesObject>((avmplus::CapabilitiesObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::CapabilitiesObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::CapabilitiesObject>((avmplus::CapabilitiesObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CapabilitiesObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::CapabilitiesObject>((avmplus::CapabilitiesObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CapabilitiesObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::CapabilitiesObject>((avmplus::CapabilitiesObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CapabilitiesObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::CapabilitiesObject>((avmplus::CapabilitiesObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit CapabilitiesClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit CapabilitiesClass(const CapabilitiesClass&); // unimplemented
    void operator=(const CapabilitiesClass&); // unimplemented
};

#define avmplus_CapabilitiesClass_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class
//-----------------------------------------------------------
class public_classObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE bool get_public_var() const { return m_slots_public_classObject.m_public_var != 0; }
    REALLY_INLINE void set_public_var(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_0() const { return m_slots_public_classObject.m_public_var_AIR_1_0 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_0 = newVal; }
public:
    REALLY_INLINE bool get_public_var_FP_10_0() const { return m_slots_public_classObject.m_public_var_FP_10_0 != 0; }
    REALLY_INLINE void set_public_var_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_FP_10_0 = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_5() const { return m_slots_public_classObject.m_public_var_AIR_1_5 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_5(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_5 = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_5_1() const { return m_slots_public_classObject.m_public_var_AIR_1_5_1 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_5_1(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_5_1 = newVal; }
public:
    REALLY_INLINE bool get_public_var_FP_10_0_32() const { return m_slots_public_classObject.m_public_var_FP_10_0_32 != 0; }
    REALLY_INLINE void set_public_var_FP_10_0_32(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_FP_10_0_32 = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_5_2() const { return m_slots_public_classObject.m_public_var_AIR_1_5_2 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_5_2(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_5_2 = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_0_FP_10_0() const { return m_slots_public_classObject.m_public_var_AIR_1_0_FP_10_0 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_0_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_0_FP_10_0 = newVal; }
public:
    REALLY_INLINE bool get_public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2() const { return m_slots_public_classObject.m_public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2 != 0; }
    REALLY_INLINE void set_public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_AIR_1_5_1_FP_10_0_AIR_1_5_2 = newVal; }
public:
    REALLY_INLINE bool get_public_var_FP_10_0_32_AIR_1_0_FP_10_0() const { return m_slots_public_classObject.m_public_var_FP_10_0_32_AIR_1_0_FP_10_0 != 0; }
    REALLY_INLINE void set_public_var_FP_10_0_32_AIR_1_0_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_var_FP_10_0_32_AIR_1_0_FP_10_0 = newVal; }
public:
    REALLY_INLINE bool get_public_const() const { return m_slots_public_classObject.m_public_const != 0; }
    REALLY_INLINE void setconst_public_const(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_0() const { return m_slots_public_classObject.m_public_const_AIR_1_0 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_0 = newVal; }
public:
    REALLY_INLINE bool get_public_const_FP_10_0() const { return m_slots_public_classObject.m_public_const_FP_10_0 != 0; }
    REALLY_INLINE void setconst_public_const_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_FP_10_0 = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_5() const { return m_slots_public_classObject.m_public_const_AIR_1_5 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_5(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_5 = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_5_1() const { return m_slots_public_classObject.m_public_const_AIR_1_5_1 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_5_1(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_5_1 = newVal; }
public:
    REALLY_INLINE bool get_public_const_FP_10_0_32() const { return m_slots_public_classObject.m_public_const_FP_10_0_32 != 0; }
    REALLY_INLINE void setconst_public_const_FP_10_0_32(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_FP_10_0_32 = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_5_2() const { return m_slots_public_classObject.m_public_const_AIR_1_5_2 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_5_2(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_5_2 = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_0_FP_10_0() const { return m_slots_public_classObject.m_public_const_AIR_1_0_FP_10_0 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_0_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_0_FP_10_0 = newVal; }
public:
    REALLY_INLINE bool get_public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2() const { return m_slots_public_classObject.m_public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2 != 0; }
    REALLY_INLINE void setconst_public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_AIR_1_5_1_FP_10_0_AIR_1_5_2 = newVal; }
public:
    REALLY_INLINE bool get_public_const_FP_10_0_32_AIR_1_0_FP_10_0() const { return m_slots_public_classObject.m_public_const_FP_10_0_32_AIR_1_0_FP_10_0 != 0; }
    REALLY_INLINE void setconst_public_const_FP_10_0_32_AIR_1_0_FP_10_0(avmplus::bool32 newVal) { m_slots_public_classObject.m_public_const_FP_10_0_32_AIR_1_0_FP_10_0 = newVal; }
private:
    avmplus::NativeID::avmplus_public_classObjectSlots m_slots_public_classObject;
protected:
    friend class avmplus::public_classClass;
    REALLY_INLINE explicit public_classObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_classObject(const public_classObject&); // unimplemented
    void operator=(const public_classObject&); // unimplemented
};

#define avmplus_public_classObject_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class$
//-----------------------------------------------------------
class public_classClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_classObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_classObject>((avmplus::public_classObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_classObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_classObject>((avmplus::public_classObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_classObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_classObject>((avmplus::public_classObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_classObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_classObject>((avmplus::public_classObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_classObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_classObject>((avmplus::public_classObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_classClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_classClass(const public_classClass&); // unimplemented
    void operator=(const public_classClass&); // unimplemented
};

#define avmplus_public_classClass_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_0
//-----------------------------------------------------------
class public_class_AIR_1_0Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_0Class;
    REALLY_INLINE explicit public_class_AIR_1_0Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_0Object(const public_class_AIR_1_0Object&); // unimplemented
    void operator=(const public_class_AIR_1_0Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_0Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_0$
//-----------------------------------------------------------
class public_class_AIR_1_0Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_0Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_0Object>((avmplus::public_class_AIR_1_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_0Object>((avmplus::public_class_AIR_1_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_0Object>((avmplus::public_class_AIR_1_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_0Object>((avmplus::public_class_AIR_1_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_0Object>((avmplus::public_class_AIR_1_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_0Class(const public_class_AIR_1_0Class&); // unimplemented
    void operator=(const public_class_AIR_1_0Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_0Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0
//-----------------------------------------------------------
class public_class_FP_10_0Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_FP_10_0Class;
    REALLY_INLINE explicit public_class_FP_10_0Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_FP_10_0Object(const public_class_FP_10_0Object&); // unimplemented
    void operator=(const public_class_FP_10_0Object&); // unimplemented
};

#define avmplus_public_class_FP_10_0Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0$
//-----------------------------------------------------------
class public_class_FP_10_0Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_FP_10_0Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_FP_10_0Object>((avmplus::public_class_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0Object>((avmplus::public_class_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0Object>((avmplus::public_class_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0Object>((avmplus::public_class_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0Object>((avmplus::public_class_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_FP_10_0Class(const public_class_FP_10_0Class&); // unimplemented
    void operator=(const public_class_FP_10_0Class&); // unimplemented
};

#define avmplus_public_class_FP_10_0Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5
//-----------------------------------------------------------
class public_class_AIR_1_5Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_5Class;
    REALLY_INLINE explicit public_class_AIR_1_5Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_5Object(const public_class_AIR_1_5Object&); // unimplemented
    void operator=(const public_class_AIR_1_5Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_5Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5$
//-----------------------------------------------------------
class public_class_AIR_1_5Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_5Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_5Object>((avmplus::public_class_AIR_1_5Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5Object>((avmplus::public_class_AIR_1_5Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5Object>((avmplus::public_class_AIR_1_5Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5Object>((avmplus::public_class_AIR_1_5Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5Object>((avmplus::public_class_AIR_1_5Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_5Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_5Class(const public_class_AIR_1_5Class&); // unimplemented
    void operator=(const public_class_AIR_1_5Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_5Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_1
//-----------------------------------------------------------
class public_class_AIR_1_5_1Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_5_1Class;
    REALLY_INLINE explicit public_class_AIR_1_5_1Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_5_1Object(const public_class_AIR_1_5_1Object&); // unimplemented
    void operator=(const public_class_AIR_1_5_1Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_1Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_1$
//-----------------------------------------------------------
class public_class_AIR_1_5_1Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_5_1Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_5_1Object>((avmplus::public_class_AIR_1_5_1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_1Object>((avmplus::public_class_AIR_1_5_1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_1Object>((avmplus::public_class_AIR_1_5_1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_1Object>((avmplus::public_class_AIR_1_5_1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_1Object>((avmplus::public_class_AIR_1_5_1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_5_1Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_5_1Class(const public_class_AIR_1_5_1Class&); // unimplemented
    void operator=(const public_class_AIR_1_5_1Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_1Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0_32
//-----------------------------------------------------------
class public_class_FP_10_0_32Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_FP_10_0_32Class;
    REALLY_INLINE explicit public_class_FP_10_0_32Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_FP_10_0_32Object(const public_class_FP_10_0_32Object&); // unimplemented
    void operator=(const public_class_FP_10_0_32Object&); // unimplemented
};

#define avmplus_public_class_FP_10_0_32Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0_32$
//-----------------------------------------------------------
class public_class_FP_10_0_32Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_FP_10_0_32Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_FP_10_0_32Object>((avmplus::public_class_FP_10_0_32Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0_32Object>((avmplus::public_class_FP_10_0_32Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0_32Object>((avmplus::public_class_FP_10_0_32Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0_32Object>((avmplus::public_class_FP_10_0_32Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0_32Object>((avmplus::public_class_FP_10_0_32Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_FP_10_0_32Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_FP_10_0_32Class(const public_class_FP_10_0_32Class&); // unimplemented
    void operator=(const public_class_FP_10_0_32Class&); // unimplemented
};

#define avmplus_public_class_FP_10_0_32Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_2
//-----------------------------------------------------------
class public_class_AIR_1_5_2Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_5_2Class;
    REALLY_INLINE explicit public_class_AIR_1_5_2Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_5_2Object(const public_class_AIR_1_5_2Object&); // unimplemented
    void operator=(const public_class_AIR_1_5_2Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_2Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_2$
//-----------------------------------------------------------
class public_class_AIR_1_5_2Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_5_2Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_2Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_2Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_2Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_2Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_5_2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_5_2Class(const public_class_AIR_1_5_2Class&); // unimplemented
    void operator=(const public_class_AIR_1_5_2Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_2Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_0_FP_10_0
//-----------------------------------------------------------
class public_class_AIR_1_0_FP_10_0Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_0_FP_10_0Class;
    REALLY_INLINE explicit public_class_AIR_1_0_FP_10_0Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_0_FP_10_0Object(const public_class_AIR_1_0_FP_10_0Object&); // unimplemented
    void operator=(const public_class_AIR_1_0_FP_10_0Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_0_FP_10_0Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_0_FP_10_0$
//-----------------------------------------------------------
class public_class_AIR_1_0_FP_10_0Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object>((avmplus::public_class_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object>((avmplus::public_class_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object>((avmplus::public_class_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object>((avmplus::public_class_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_0_FP_10_0Object>((avmplus::public_class_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_0_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_0_FP_10_0Class(const public_class_AIR_1_0_FP_10_0Class&); // unimplemented
    void operator=(const public_class_AIR_1_0_FP_10_0Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_0_FP_10_0Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2
//-----------------------------------------------------------
class public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class;
    REALLY_INLINE explicit public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object(const public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object&); // unimplemented
    void operator=(const public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2$
//-----------------------------------------------------------
class public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object>((avmplus::public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class(const public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class&); // unimplemented
    void operator=(const public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class&); // unimplemented
};

#define avmplus_public_class_AIR_1_5_1_FP_10_0_AIR_1_5_2Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0_32_AIR_1_0_FP_10_0
//-----------------------------------------------------------
class public_class_FP_10_0_32_AIR_1_0_FP_10_0Object : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Class;
    REALLY_INLINE explicit public_class_FP_10_0_32_AIR_1_0_FP_10_0Object(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_class_FP_10_0_32_AIR_1_0_FP_10_0Object(const public_class_FP_10_0_32_AIR_1_0_FP_10_0Object&); // unimplemented
    void operator=(const public_class_FP_10_0_32_AIR_1_0_FP_10_0Object&); // unimplemented
};

#define avmplus_public_class_FP_10_0_32_AIR_1_0_FP_10_0Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_class_FP_10_0_32_AIR_1_0_FP_10_0$
//-----------------------------------------------------------
class public_class_FP_10_0_32_AIR_1_0_FP_10_0Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object>((avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object>((avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object>((avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object>((avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object>((avmplus::public_class_FP_10_0_32_AIR_1_0_FP_10_0Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_class_FP_10_0_32_AIR_1_0_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_class_FP_10_0_32_AIR_1_0_FP_10_0Class(const public_class_FP_10_0_32_AIR_1_0_FP_10_0Class&); // unimplemented
    void operator=(const public_class_FP_10_0_32_AIR_1_0_FP_10_0Class&); // unimplemented
};

#define avmplus_public_class_FP_10_0_32_AIR_1_0_FP_10_0Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::public_interface
//-----------------------------------------------------------
class public_interfaceInterface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interfaceClass;
    REALLY_INLINE explicit public_interfaceInterface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interfaceInterface(const public_interfaceInterface&); // unimplemented
    void operator=(const public_interfaceInterface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface$
//-----------------------------------------------------------
class public_interfaceClass : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interfaceInterface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interfaceInterface>((avmplus::public_interfaceInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interfaceInterface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interfaceInterface>((avmplus::public_interfaceInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interfaceInterface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interfaceInterface>((avmplus::public_interfaceInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interfaceInterface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interfaceInterface>((avmplus::public_interfaceInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interfaceClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interfaceClass(const public_interfaceClass&); // unimplemented
    void operator=(const public_interfaceClass&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_0
//-----------------------------------------------------------
class public_interface_AIR_1_0Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_0Class;
    REALLY_INLINE explicit public_interface_AIR_1_0Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_0Interface(const public_interface_AIR_1_0Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_0Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_0$
//-----------------------------------------------------------
class public_interface_AIR_1_0Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_0Interface>((avmplus::public_interface_AIR_1_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_0Interface>((avmplus::public_interface_AIR_1_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_0Interface>((avmplus::public_interface_AIR_1_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_0Interface>((avmplus::public_interface_AIR_1_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_0Class(const public_interface_AIR_1_0Class&); // unimplemented
    void operator=(const public_interface_AIR_1_0Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0
//-----------------------------------------------------------
class public_interface_FP_10_0Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_FP_10_0Class;
    REALLY_INLINE explicit public_interface_FP_10_0Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_FP_10_0Interface(const public_interface_FP_10_0Interface&); // unimplemented
    void operator=(const public_interface_FP_10_0Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0$
//-----------------------------------------------------------
class public_interface_FP_10_0Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0Interface>((avmplus::public_interface_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0Interface>((avmplus::public_interface_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0Interface>((avmplus::public_interface_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0Interface>((avmplus::public_interface_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_FP_10_0Class(const public_interface_FP_10_0Class&); // unimplemented
    void operator=(const public_interface_FP_10_0Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5
//-----------------------------------------------------------
class public_interface_AIR_1_5Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_5Class;
    REALLY_INLINE explicit public_interface_AIR_1_5Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_5Interface(const public_interface_AIR_1_5Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_5Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5$
//-----------------------------------------------------------
class public_interface_AIR_1_5Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5Interface>((avmplus::public_interface_AIR_1_5Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5Interface>((avmplus::public_interface_AIR_1_5Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5Interface>((avmplus::public_interface_AIR_1_5Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5Interface>((avmplus::public_interface_AIR_1_5Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_5Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_5Class(const public_interface_AIR_1_5Class&); // unimplemented
    void operator=(const public_interface_AIR_1_5Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_1
//-----------------------------------------------------------
class public_interface_AIR_1_5_1Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_5_1Class;
    REALLY_INLINE explicit public_interface_AIR_1_5_1Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_5_1Interface(const public_interface_AIR_1_5_1Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_5_1Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_1$
//-----------------------------------------------------------
class public_interface_AIR_1_5_1Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_1Interface>((avmplus::public_interface_AIR_1_5_1Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_1Interface>((avmplus::public_interface_AIR_1_5_1Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_1Interface>((avmplus::public_interface_AIR_1_5_1Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_1Interface>((avmplus::public_interface_AIR_1_5_1Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_5_1Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_5_1Class(const public_interface_AIR_1_5_1Class&); // unimplemented
    void operator=(const public_interface_AIR_1_5_1Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0_32
//-----------------------------------------------------------
class public_interface_FP_10_0_32Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_FP_10_0_32Class;
    REALLY_INLINE explicit public_interface_FP_10_0_32Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_FP_10_0_32Interface(const public_interface_FP_10_0_32Interface&); // unimplemented
    void operator=(const public_interface_FP_10_0_32Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0_32$
//-----------------------------------------------------------
class public_interface_FP_10_0_32Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0_32Interface>((avmplus::public_interface_FP_10_0_32Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0_32Interface>((avmplus::public_interface_FP_10_0_32Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0_32Interface>((avmplus::public_interface_FP_10_0_32Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0_32Interface>((avmplus::public_interface_FP_10_0_32Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_FP_10_0_32Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_FP_10_0_32Class(const public_interface_FP_10_0_32Class&); // unimplemented
    void operator=(const public_interface_FP_10_0_32Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_2
//-----------------------------------------------------------
class public_interface_AIR_1_5_2Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_5_2Class;
    REALLY_INLINE explicit public_interface_AIR_1_5_2Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_5_2Interface(const public_interface_AIR_1_5_2Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_5_2Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_2$
//-----------------------------------------------------------
class public_interface_AIR_1_5_2Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_2Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_2Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_2Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_2Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_5_2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_5_2Class(const public_interface_AIR_1_5_2Class&); // unimplemented
    void operator=(const public_interface_AIR_1_5_2Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_0_FP_10_0
//-----------------------------------------------------------
class public_interface_AIR_1_0_FP_10_0Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_0_FP_10_0Class;
    REALLY_INLINE explicit public_interface_AIR_1_0_FP_10_0Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_0_FP_10_0Interface(const public_interface_AIR_1_0_FP_10_0Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_0_FP_10_0Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_0_FP_10_0$
//-----------------------------------------------------------
class public_interface_AIR_1_0_FP_10_0Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_0_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_0_FP_10_0Class(const public_interface_AIR_1_0_FP_10_0Class&); // unimplemented
    void operator=(const public_interface_AIR_1_0_FP_10_0Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2
//-----------------------------------------------------------
class public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class;
    REALLY_INLINE explicit public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface(const public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface&); // unimplemented
    void operator=(const public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2$
//-----------------------------------------------------------
class public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface>((avmplus::public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class(const public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class&); // unimplemented
    void operator=(const public_interface_AIR_1_5_1_FP_10_0_AIR_1_5_2Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0_32_AIR_1_0_FP_10_0
//-----------------------------------------------------------
class public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class;
    REALLY_INLINE explicit public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface(const public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface&); // unimplemented
    void operator=(const public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::public_interface_FP_10_0_32_AIR_1_0_FP_10_0$
//-----------------------------------------------------------
class public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface>((avmplus::public_interface_FP_10_0_32_AIR_1_0_FP_10_0Interface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class(const public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class&); // unimplemented
    void operator=(const public_interface_FP_10_0_32_AIR_1_0_FP_10_0Class&); // unimplemented
};

//-----------------------------------------------------------
// avmshell::SubclassOfAbstractBase
//-----------------------------------------------------------
class SubclassOfAbstractBaseObject : public avmshell::AbstractBaseObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::SubclassOfAbstractBaseClass;
    REALLY_INLINE explicit SubclassOfAbstractBaseObject(VTable* ivtable, ScriptObject* delegate) : avmshell::AbstractBaseObject(ivtable, delegate) {}
private:
    explicit SubclassOfAbstractBaseObject(const SubclassOfAbstractBaseObject&); // unimplemented
    void operator=(const SubclassOfAbstractBaseObject&); // unimplemented
};

#define avmplus_SubclassOfAbstractBaseObject_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::SubclassOfAbstractBase$
//-----------------------------------------------------------
class SubclassOfAbstractBaseClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::SubclassOfAbstractBaseObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::SubclassOfAbstractBaseObject>((avmplus::SubclassOfAbstractBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractBaseObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::SubclassOfAbstractBaseObject>((avmplus::SubclassOfAbstractBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractBaseObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfAbstractBaseObject>((avmplus::SubclassOfAbstractBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractBaseObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::SubclassOfAbstractBaseObject>((avmplus::SubclassOfAbstractBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractBaseObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfAbstractBaseObject>((avmplus::SubclassOfAbstractBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit SubclassOfAbstractBaseClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit SubclassOfAbstractBaseClass(const SubclassOfAbstractBaseClass&); // unimplemented
    void operator=(const SubclassOfAbstractBaseClass&); // unimplemented
};

#define avmplus_SubclassOfAbstractBaseClass_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::SubclassOfRestrictedBase
//-----------------------------------------------------------
class SubclassOfRestrictedBaseObject : public avmshell::RestrictedBaseObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::SubclassOfRestrictedBaseClass;
    REALLY_INLINE explicit SubclassOfRestrictedBaseObject(VTable* ivtable, ScriptObject* delegate) : avmshell::RestrictedBaseObject(ivtable, delegate) {}
private:
    explicit SubclassOfRestrictedBaseObject(const SubclassOfRestrictedBaseObject&); // unimplemented
    void operator=(const SubclassOfRestrictedBaseObject&); // unimplemented
};

#define avmplus_SubclassOfRestrictedBaseObject_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::SubclassOfRestrictedBase$
//-----------------------------------------------------------
class SubclassOfRestrictedBaseClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::SubclassOfRestrictedBaseObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::SubclassOfRestrictedBaseObject>((avmplus::SubclassOfRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfRestrictedBaseObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::SubclassOfRestrictedBaseObject>((avmplus::SubclassOfRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfRestrictedBaseObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfRestrictedBaseObject>((avmplus::SubclassOfRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfRestrictedBaseObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::SubclassOfRestrictedBaseObject>((avmplus::SubclassOfRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfRestrictedBaseObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfRestrictedBaseObject>((avmplus::SubclassOfRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit SubclassOfRestrictedBaseClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit SubclassOfRestrictedBaseClass(const SubclassOfRestrictedBaseClass&); // unimplemented
    void operator=(const SubclassOfRestrictedBaseClass&); // unimplemented
};

#define avmplus_SubclassOfRestrictedBaseClass_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::SubclassOfAbstractRestrictedBase
//-----------------------------------------------------------
class SubclassOfAbstractRestrictedBaseObject : public avmshell::AbstractRestrictedBaseObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::SubclassOfAbstractRestrictedBaseClass;
    REALLY_INLINE explicit SubclassOfAbstractRestrictedBaseObject(VTable* ivtable, ScriptObject* delegate) : avmshell::AbstractRestrictedBaseObject(ivtable, delegate) {}
private:
    explicit SubclassOfAbstractRestrictedBaseObject(const SubclassOfAbstractRestrictedBaseObject&); // unimplemented
    void operator=(const SubclassOfAbstractRestrictedBaseObject&); // unimplemented
};

#define avmplus_SubclassOfAbstractRestrictedBaseObject_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::SubclassOfAbstractRestrictedBase$
//-----------------------------------------------------------
class SubclassOfAbstractRestrictedBaseClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject>((avmplus::SubclassOfAbstractRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject>((avmplus::SubclassOfAbstractRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject>((avmplus::SubclassOfAbstractRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject>((avmplus::SubclassOfAbstractRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::SubclassOfAbstractRestrictedBaseObject>((avmplus::SubclassOfAbstractRestrictedBaseObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit SubclassOfAbstractRestrictedBaseClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit SubclassOfAbstractRestrictedBaseClass(const SubclassOfAbstractRestrictedBaseClass&); // unimplemented
    void operator=(const SubclassOfAbstractRestrictedBaseClass&); // unimplemented
};

#define avmplus_SubclassOfAbstractRestrictedBaseClass_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseExtender1
//-----------------------------------------------------------
class NativeBaseExtender1Object : public avmshell::NativeBaseObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    REALLY_INLINE int32_t get_foo() const { return m_slots_NativeBaseExtender1Object.m_private_foo; }
    REALLY_INLINE void set_foo(int32_t newVal) { m_slots_NativeBaseExtender1Object.m_private_foo = newVal; }
private:
    avmplus::NativeID::avmplus_NativeBaseExtender1ObjectSlots m_slots_NativeBaseExtender1Object;
protected:
    friend class avmplus::NativeBaseExtender1Class;
    REALLY_INLINE explicit NativeBaseExtender1Object(VTable* ivtable, ScriptObject* delegate) : avmshell::NativeBaseObject(ivtable, delegate) {}
private:
    explicit NativeBaseExtender1Object(const NativeBaseExtender1Object&); // unimplemented
    void operator=(const NativeBaseExtender1Object&); // unimplemented
};

#define avmplus_NativeBaseExtender1Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseExtender1$
//-----------------------------------------------------------
class NativeBaseExtender1Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::NativeBaseExtender1Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::NativeBaseExtender1Object>((avmplus::NativeBaseExtender1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender1Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::NativeBaseExtender1Object>((avmplus::NativeBaseExtender1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender1Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseExtender1Object>((avmplus::NativeBaseExtender1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender1Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::NativeBaseExtender1Object>((avmplus::NativeBaseExtender1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender1Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseExtender1Object>((avmplus::NativeBaseExtender1Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit NativeBaseExtender1Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit NativeBaseExtender1Class(const NativeBaseExtender1Class&); // unimplemented
    void operator=(const NativeBaseExtender1Class&); // unimplemented
};

#define avmplus_NativeBaseExtender1Class_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseExtender2
//-----------------------------------------------------------
class NativeBaseExtender2Object : public avmshell::NativeBaseObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    REALLY_INLINE int32_t get_foo() const { return m_slots_NativeBaseExtender2Object.m_private_foo; }
    REALLY_INLINE void set_foo(int32_t newVal) { m_slots_NativeBaseExtender2Object.m_private_foo = newVal; }
protected:
    REALLY_INLINE int32_t get_foo1() const { return m_slots_NativeBaseExtender2Object.m_private_foo1; }
    REALLY_INLINE void set_foo1(int32_t newVal) { m_slots_NativeBaseExtender2Object.m_private_foo1 = newVal; }
protected:
    REALLY_INLINE int32_t get_foo2() const { return m_slots_NativeBaseExtender2Object.m_private_foo2; }
    REALLY_INLINE void set_foo2(int32_t newVal) { m_slots_NativeBaseExtender2Object.m_private_foo2 = newVal; }
private:
    avmplus::NativeID::avmplus_NativeBaseExtender2ObjectSlots m_slots_NativeBaseExtender2Object;
protected:
    friend class avmplus::NativeBaseExtender2Class;
    REALLY_INLINE explicit NativeBaseExtender2Object(VTable* ivtable, ScriptObject* delegate) : avmshell::NativeBaseObject(ivtable, delegate) {}
private:
    explicit NativeBaseExtender2Object(const NativeBaseExtender2Object&); // unimplemented
    void operator=(const NativeBaseExtender2Object&); // unimplemented
};

#define avmplus_NativeBaseExtender2Object_isExactInterlock 1
//-----------------------------------------------------------
// avmshell::NativeBaseExtender2$
//-----------------------------------------------------------
class NativeBaseExtender2Class : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::NativeBaseExtender2Object> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::NativeBaseExtender2Object>((avmplus::NativeBaseExtender2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender2Object> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::NativeBaseExtender2Object>((avmplus::NativeBaseExtender2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender2Object> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseExtender2Object>((avmplus::NativeBaseExtender2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender2Object> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::NativeBaseExtender2Object>((avmplus::NativeBaseExtender2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::NativeBaseExtender2Object> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::NativeBaseExtender2Object>((avmplus::NativeBaseExtender2Object*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit NativeBaseExtender2Class(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit NativeBaseExtender2Class(const NativeBaseExtender2Class&); // unimplemented
    void operator=(const NativeBaseExtender2Class&); // unimplemented
};

#define avmplus_NativeBaseExtender2Class_isExactInterlock 1
//-----------------------------------------------------------
// flash.utils::IKernel
//-----------------------------------------------------------
class IKernelInterface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::IKernelClass;
    REALLY_INLINE explicit IKernelInterface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit IKernelInterface(const IKernelInterface&); // unimplemented
    void operator=(const IKernelInterface&); // unimplemented
};

//-----------------------------------------------------------
// flash.utils::IKernel$
//-----------------------------------------------------------
class IKernelClass : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::IKernelInterface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::IKernelInterface>((avmplus::IKernelInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IKernelInterface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::IKernelInterface>((avmplus::IKernelInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IKernelInterface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::IKernelInterface>((avmplus::IKernelInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IKernelInterface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::IKernelInterface>((avmplus::IKernelInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit IKernelClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit IKernelClass(const IKernelClass&); // unimplemented
    void operator=(const IKernelClass&); // unimplemented
};

//-----------------------------------------------------------
// flash.sampler::StackFrame
//-----------------------------------------------------------
class StackFrameObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE uint32_t get_line() const { return m_slots_StackFrameObject.m_line; }
    REALLY_INLINE void setconst_line(uint32_t newVal) { m_slots_StackFrameObject.m_line = newVal; }
public:
    REALLY_INLINE avmplus::String* get_name() const { return m_slots_StackFrameObject.m_name; }
    REALLY_INLINE void setconst_name(avmplus::String* newVal) { m_slots_StackFrameObject.m_name = newVal; }
public:
    REALLY_INLINE avmplus::String* get_file() const { return m_slots_StackFrameObject.m_file; }
    REALLY_INLINE void setconst_file(avmplus::String* newVal) { m_slots_StackFrameObject.m_file = newVal; }
public:
    REALLY_INLINE double get_scriptID() const { return m_slots_StackFrameObject.m_scriptID; }
    REALLY_INLINE void setconst_scriptID(double newVal) { m_slots_StackFrameObject.m_scriptID = newVal; }
private:
    avmplus::NativeID::avmplus_StackFrameObjectSlots m_slots_StackFrameObject;
protected:
    friend class avmplus::StackFrameClass;
    REALLY_INLINE explicit StackFrameObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit StackFrameObject(const StackFrameObject&); // unimplemented
    void operator=(const StackFrameObject&); // unimplemented
};

#define avmplus_StackFrameObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::StackFrame$
//-----------------------------------------------------------
class StackFrameClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::StackFrameObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::StackFrameObject>((avmplus::StackFrameObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::StackFrameObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::StackFrameObject>((avmplus::StackFrameObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::StackFrameObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::StackFrameObject>((avmplus::StackFrameObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::StackFrameObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::StackFrameObject>((avmplus::StackFrameObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::StackFrameObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::StackFrameObject>((avmplus::StackFrameObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit StackFrameClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit StackFrameClass(const StackFrameClass&); // unimplemented
    void operator=(const StackFrameClass&); // unimplemented
};

#define avmplus_StackFrameClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::Sample
//-----------------------------------------------------------
class SampleObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::ArrayObject* get_stack() const { return m_slots_SampleObject.m_stack; }
    REALLY_INLINE void setconst_stack(avmplus::ArrayObject* newVal) { m_slots_SampleObject.m_stack = newVal; }
public:
    REALLY_INLINE double get_time() const { return m_slots_SampleObject.m_time; }
    REALLY_INLINE void setconst_time(double newVal) { m_slots_SampleObject.m_time = newVal; }
private:
    avmplus::NativeID::avmplus_SampleObjectSlots m_slots_SampleObject;
protected:
    friend class avmplus::SampleClass;
    REALLY_INLINE explicit SampleObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit SampleObject(const SampleObject&); // unimplemented
    void operator=(const SampleObject&); // unimplemented
};

#define avmplus_SampleObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::Sample$
//-----------------------------------------------------------
class SampleClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::SampleObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::SampleObject>((avmplus::SampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::SampleObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::SampleObject>((avmplus::SampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SampleObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::SampleObject>((avmplus::SampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SampleObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::SampleObject>((avmplus::SampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::SampleObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::SampleObject>((avmplus::SampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit SampleClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit SampleClass(const SampleClass&); // unimplemented
    void operator=(const SampleClass&); // unimplemented
};

#define avmplus_SampleClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::ClassFactory
//-----------------------------------------------------------
class ClassFactoryObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::ClassFactoryClass;
    REALLY_INLINE explicit ClassFactoryObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit ClassFactoryObject(const ClassFactoryObject&); // unimplemented
    void operator=(const ClassFactoryObject&); // unimplemented
};

#define avmplus_ClassFactoryObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::ClassFactory$
//-----------------------------------------------------------
class ClassFactoryClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::ClassFactoryObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::ClassFactoryObject>((avmplus::ClassFactoryObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::ClassFactoryObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::ClassFactoryObject>((avmplus::ClassFactoryObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ClassFactoryObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::ClassFactoryObject>((avmplus::ClassFactoryObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ClassFactoryObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::ClassFactoryObject>((avmplus::ClassFactoryObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::ClassFactoryObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::ClassFactoryObject>((avmplus::ClassFactoryObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::ClassClosure* get_StackFrameClass() const { return m_slots_ClassFactoryClass.m_StackFrameClass; }
    REALLY_INLINE void setconst_StackFrameClass(avmplus::ClassClosure* newVal) { m_slots_ClassFactoryClass.m_StackFrameClass = newVal; }
public:
    REALLY_INLINE avmplus::ClassClosure* get_SampleClass() const { return m_slots_ClassFactoryClass.m_SampleClass; }
    REALLY_INLINE void setconst_SampleClass(avmplus::ClassClosure* newVal) { m_slots_ClassFactoryClass.m_SampleClass = newVal; }
public:
    REALLY_INLINE avmplus::ClassClosure* get_DeleteObjectSampleClass() const { return m_slots_ClassFactoryClass.m_DeleteObjectSampleClass; }
    REALLY_INLINE void setconst_DeleteObjectSampleClass(avmplus::ClassClosure* newVal) { m_slots_ClassFactoryClass.m_DeleteObjectSampleClass = newVal; }
public:
    REALLY_INLINE avmplus::ClassClosure* get_NewObjectSampleClass() const { return m_slots_ClassFactoryClass.m_NewObjectSampleClass; }
    REALLY_INLINE void setconst_NewObjectSampleClass(avmplus::ClassClosure* newVal) { m_slots_ClassFactoryClass.m_NewObjectSampleClass = newVal; }
private:
    avmplus::NativeID::avmplus_ClassFactoryClassSlots m_slots_ClassFactoryClass;
protected:
    inline explicit ClassFactoryClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit ClassFactoryClass(const ClassFactoryClass&); // unimplemented
    void operator=(const ClassFactoryClass&); // unimplemented
};

#define avmplus_ClassFactoryClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::DeleteObjectSample
//-----------------------------------------------------------
class DeleteObjectSampleObject : public avmplus::SampleObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE double get_id() const { return m_slots_DeleteObjectSampleObject.m_id; }
    REALLY_INLINE void setconst_id(double newVal) { m_slots_DeleteObjectSampleObject.m_id = newVal; }
public:
    REALLY_INLINE double get_size() const { return m_slots_DeleteObjectSampleObject.m_size; }
    REALLY_INLINE void setconst_size(double newVal) { m_slots_DeleteObjectSampleObject.m_size = newVal; }
private:
    avmplus::NativeID::avmplus_DeleteObjectSampleObjectSlots m_slots_DeleteObjectSampleObject;
protected:
    friend class avmplus::DeleteObjectSampleClass;
    REALLY_INLINE explicit DeleteObjectSampleObject(VTable* ivtable, ScriptObject* delegate) : avmplus::SampleObject(ivtable, delegate) {}
private:
    explicit DeleteObjectSampleObject(const DeleteObjectSampleObject&); // unimplemented
    void operator=(const DeleteObjectSampleObject&); // unimplemented
};

#define avmplus_DeleteObjectSampleObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.sampler::DeleteObjectSample$
//-----------------------------------------------------------
class DeleteObjectSampleClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::DeleteObjectSampleObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::DeleteObjectSampleObject>((avmplus::DeleteObjectSampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::DeleteObjectSampleObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::DeleteObjectSampleObject>((avmplus::DeleteObjectSampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::DeleteObjectSampleObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::DeleteObjectSampleObject>((avmplus::DeleteObjectSampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::DeleteObjectSampleObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::DeleteObjectSampleObject>((avmplus::DeleteObjectSampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::DeleteObjectSampleObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::DeleteObjectSampleObject>((avmplus::DeleteObjectSampleObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit DeleteObjectSampleClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit DeleteObjectSampleClass(const DeleteObjectSampleClass&); // unimplemented
    void operator=(const DeleteObjectSampleClass&); // unimplemented
};

#define avmplus_DeleteObjectSampleClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.trace::Trace
//-----------------------------------------------------------
class TraceObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::TraceClass;
    REALLY_INLINE explicit TraceObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit TraceObject(const TraceObject&); // unimplemented
    void operator=(const TraceObject&); // unimplemented
};

#define avmplus_TraceObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.utils::Endian
//-----------------------------------------------------------
class EndianObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::EndianClass;
    REALLY_INLINE explicit EndianObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit EndianObject(const EndianObject&); // unimplemented
    void operator=(const EndianObject&); // unimplemented
};

#define avmplus_EndianObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.utils::Endian$
//-----------------------------------------------------------
class EndianClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::EndianObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::EndianObject>((avmplus::EndianObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::EndianObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::EndianObject>((avmplus::EndianObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EndianObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::EndianObject>((avmplus::EndianObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EndianObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::EndianObject>((avmplus::EndianObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EndianObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::EndianObject>((avmplus::EndianObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::String* get_BIG_ENDIAN() const { return m_slots_EndianClass.m_BIG_ENDIAN; }
    REALLY_INLINE void setconst_BIG_ENDIAN(avmplus::String* newVal) { m_slots_EndianClass.m_BIG_ENDIAN = newVal; }
public:
    REALLY_INLINE avmplus::String* get_LITTLE_ENDIAN() const { return m_slots_EndianClass.m_LITTLE_ENDIAN; }
    REALLY_INLINE void setconst_LITTLE_ENDIAN(avmplus::String* newVal) { m_slots_EndianClass.m_LITTLE_ENDIAN = newVal; }
private:
    avmplus::NativeID::avmplus_EndianClassSlots m_slots_EndianClass;
protected:
    inline explicit EndianClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit EndianClass(const EndianClass&); // unimplemented
    void operator=(const EndianClass&); // unimplemented
};

#define avmplus_EndianClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::WorkerState
//-----------------------------------------------------------
class WorkerStateObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::WorkerStateClass;
    REALLY_INLINE explicit WorkerStateObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit WorkerStateObject(const WorkerStateObject&); // unimplemented
    void operator=(const WorkerStateObject&); // unimplemented
};

#define avmplus_WorkerStateObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::WorkerState$
//-----------------------------------------------------------
class WorkerStateClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::WorkerStateObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::WorkerStateObject>((avmplus::WorkerStateObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::WorkerStateObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::WorkerStateObject>((avmplus::WorkerStateObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerStateObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::WorkerStateObject>((avmplus::WorkerStateObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerStateObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::WorkerStateObject>((avmplus::WorkerStateObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerStateObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::WorkerStateObject>((avmplus::WorkerStateObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::String* get_NEW() const { return m_slots_WorkerStateClass.m_NEW; }
    REALLY_INLINE void setconst_NEW(avmplus::String* newVal) { m_slots_WorkerStateClass.m_NEW = newVal; }
public:
    REALLY_INLINE avmplus::String* get_RUNNING() const { return m_slots_WorkerStateClass.m_RUNNING; }
    REALLY_INLINE void setconst_RUNNING(avmplus::String* newVal) { m_slots_WorkerStateClass.m_RUNNING = newVal; }
public:
    REALLY_INLINE avmplus::String* get_TERMINATED() const { return m_slots_WorkerStateClass.m_TERMINATED; }
    REALLY_INLINE void setconst_TERMINATED(avmplus::String* newVal) { m_slots_WorkerStateClass.m_TERMINATED = newVal; }
public:
    REALLY_INLINE avmplus::String* get_FAILED() const { return m_slots_WorkerStateClass.m_FAILED; }
    REALLY_INLINE void setconst_FAILED(avmplus::String* newVal) { m_slots_WorkerStateClass.m_FAILED = newVal; }
public:
    REALLY_INLINE avmplus::String* get_ABORTED() const { return m_slots_WorkerStateClass.m_ABORTED; }
    REALLY_INLINE void setconst_ABORTED(avmplus::String* newVal) { m_slots_WorkerStateClass.m_ABORTED = newVal; }
public:
    REALLY_INLINE avmplus::String* get_EXCEPTION() const { return m_slots_WorkerStateClass.m_EXCEPTION; }
    REALLY_INLINE void setconst_EXCEPTION(avmplus::String* newVal) { m_slots_WorkerStateClass.m_EXCEPTION = newVal; }
private:
    avmplus::NativeID::avmplus_WorkerStateClassSlots m_slots_WorkerStateClass;
protected:
    inline explicit WorkerStateClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit WorkerStateClass(const WorkerStateClass&); // unimplemented
    void operator=(const WorkerStateClass&); // unimplemented
};

#define avmplus_WorkerStateClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::WorkerEvent
//-----------------------------------------------------------
class WorkerEventObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    REALLY_INLINE avmplus::String* get_m_previousState() const { return m_slots_WorkerEventObject.m_private_m_previousState; }
    REALLY_INLINE void set_m_previousState(avmplus::String* newVal) { m_slots_WorkerEventObject.m_private_m_previousState = newVal; }
protected:
    REALLY_INLINE avmplus::String* get_m_currentState() const { return m_slots_WorkerEventObject.m_private_m_currentState; }
    REALLY_INLINE void set_m_currentState(avmplus::String* newVal) { m_slots_WorkerEventObject.m_private_m_currentState = newVal; }
protected:
    REALLY_INLINE avmplus::Atom get_m_target() const { return m_slots_WorkerEventObject.m_flash_system_m_target; }
    REALLY_INLINE void set_m_target(avmplus::Atom newVal) { m_slots_WorkerEventObject.m_flash_system_m_target = newVal; }
private:
    avmplus::NativeID::avmplus_WorkerEventObjectSlots m_slots_WorkerEventObject;
protected:
    friend class avmplus::WorkerEventClass;
    REALLY_INLINE explicit WorkerEventObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit WorkerEventObject(const WorkerEventObject&); // unimplemented
    void operator=(const WorkerEventObject&); // unimplemented
};

#define avmplus_WorkerEventObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.system::WorkerEvent$
//-----------------------------------------------------------
class WorkerEventClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::WorkerEventObject> constructObject(GCRef<avmplus::String> arg1, GCRef<avmplus::String> arg2)
    {
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom(), arg2->atom() };
        avmplus::Atom const result = this->construct(2, args);
        return GCRef<avmplus::WorkerEventObject>((avmplus::WorkerEventObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::WorkerEventObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::WorkerEventObject>((avmplus::WorkerEventObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerEventObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::WorkerEventObject>((avmplus::WorkerEventObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerEventObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::WorkerEventObject>((avmplus::WorkerEventObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WorkerEventObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::WorkerEventObject>((avmplus::WorkerEventObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::String* get_WORKER_STATE() const { return m_slots_WorkerEventClass.m_WORKER_STATE; }
    REALLY_INLINE void setconst_WORKER_STATE(avmplus::String* newVal) { m_slots_WorkerEventClass.m_WORKER_STATE = newVal; }
private:
    avmplus::NativeID::avmplus_WorkerEventClassSlots m_slots_WorkerEventClass;
protected:
    inline explicit WorkerEventClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit WorkerEventClass(const WorkerEventClass&); // unimplemented
    void operator=(const WorkerEventClass&); // unimplemented
};

#define avmplus_WorkerEventClass_isExactInterlock 1
}

#endif // _H_nativegen_classes_shell_toplevel
