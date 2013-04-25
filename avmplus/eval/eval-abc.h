/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

// For unknown reasons these are not in ActionBlockConstants.
enum {
    CONSTANT_ClassSealed = 1,
    CONSTANT_ClassFinal = 2,
    CONSTANT_ClassInterface = 4,
    CONSTANT_ClassProtectedNs = 8
};
    
/* Structures and methods for constructing and emitting ABC code. */

/* An interface implemented by any part of the ABC file */

class ABCChunk {
public:
    virtual ~ABCChunk();
    virtual uint32_t size() = 0;
    virtual uint8_t* serialize(uint8_t* b) = 0;
    
    uint32_t reported_size;     // number of bytes reported by size(), checked by serialize()
};

/* ABCFile container & helper class.
 *
 * Every argument to an addWhatever() method is retained by
 * reference.  When getBytes() is finally called, each object is
 * serialized.  The order of serialization is the order they will
 * have in the ABCFile, and the order among items of the same type
 * is the order in which they were added.
 *
 * Performance ought to be good; nothing is serialized more than
 * once and no data are copied except during serialization.
 */
class ABCFile : public ABCChunk {
public:
    ABCFile(Compiler* compiler);
    
    uint32_t addInt(int32_t i);
    uint32_t addUInt(uint32_t u);
    uint32_t addDouble(double d);
    uint32_t addFloat(float f);
    uint32_t addFloat4(const float4_t& f);
    uint32_t addString(Str* s);
    uint32_t addString(const char* s);
    uint32_t addNamespace(uint8_t kind, uint32_t ns);
    uint32_t addNsset(Seq<uint32_t>* nss);
    uint32_t addQName(uint32_t ns, uint32_t name, bool is_attr=false);
    uint32_t addRTQName(uint32_t name, bool is_attr=false);
    uint32_t addRTQNameL(bool is_attr=false);
    uint32_t addMultiname(uint32_t nsset, uint32_t name, bool is_attr=false);
    uint32_t addMultinameL(uint32_t nsset, bool is_attr=false);
    uint32_t addTypeName(uint32_t parameterizedType, uint32_t parameterType);
    uint32_t addMethod(ABCMethodInfo* m);
    uint32_t addMetadata(ABCMetadataInfo* m);
    uint32_t addClassAndInstance(ABCClassInfo* c, ABCInstanceInfo* i);
    uint32_t addInstance(ABCInstanceInfo* i);
    uint32_t addClass(ABCClassInfo* c);
    uint32_t addScript(ABCScriptInfo* s);
    void addMethodBody(ABCMethodBodyInfo* m);
    bool hasRTNS(uint32_t index);
    bool hasRTName(uint32_t index);
    
    uint32_t size();
    uint8_t* serialize(uint8_t* b);
    
private:
    uint32_t multinameLookup(uint8_t kind, uint32_t ns_or_nsset, uint32_t name);
    ABCMultinameInfo* getMultiname(uint32_t index);

    const uint16_t major_version;
    const uint16_t minor_version;
    const int NO_VALUE;

    Compiler* const compiler;
    Allocator* const allocator;
    
    uint32_t intCount;
    uint32_t uintCount;
    uint32_t doubleCount;
    uint32_t floatCount;
    uint32_t float4Count;
    uint32_t stringCount;
    uint32_t namespaceCount;
    uint32_t nssetCount;
    uint32_t multinameCount;
    uint32_t methodCount;
    uint32_t metadataCount;
    uint32_t instanceCount;
    uint32_t classCount;
    uint32_t scriptCount;
    uint32_t nonemptyMethodBodyCount;

    ByteBuffer intBuf;
    ByteBuffer uintBuf;
    ByteBuffer doubleBuf;
    ByteBuffer floatBuf;
    ByteBuffer float4Buf;
    ByteBuffer stringBuf;
    ByteBuffer namespaceBuf;
    ByteBuffer nssetBuf;
    ByteBuffer multinameBuf;
    
    SeqBuilder<ABCNamespaceInfo*> namespaces;       // few enough of these that a sequentially searchable list is sufficient
    SeqBuilder<ABCNamespaceSetInfo*> namespaceSets; // few enough of these that a sequentially searchable list is sufficient
    SeqBuilder<ABCMultinameInfo*> multinames;       // FIXME - *NOT* few enough of these that a sequential list is sufficient, but OK for now
    SeqBuilder<ABCMethodInfo*> methods;
    SeqBuilder<ABCMetadataInfo*> metadatas;
    SeqBuilder<ABCInstanceInfo*> instances;
    SeqBuilder<ABCClassInfo*> classes;
    SeqBuilder<ABCScriptInfo*> scripts;
    SeqBuilder<ABCMethodBodyInfo*> bodies;
};

class ABCTraitsTable : public ABCChunk {
public:
    ABCTraitsTable(Compiler* compiler);
    uint32_t addTrait(ABCTrait* t);
    
    uint32_t getCount() const { return traitsCount; }

    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
protected:
    uint32_t traitsCount;
    SeqBuilder<ABCTrait*> traits;
};

class ABCScriptInfo : public ABCChunk {
public:
    // init_method is the index of a MethodInfo
    ABCScriptInfo(Compiler* compiler, ABCMethodInfo* init_method, ABCTraitsTable* traits);
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    const uint32_t index;       // script index in compiler->abc
    ABCMethodInfo * const init_method;
    ABCTraitsTable * const traits;
};

class ABCMethodInfo : public ABCChunk {
public:
    ABCMethodInfo(Compiler* compiler,
                  uint32_t name,
                  uint32_t param_count,
                  Seq<uint32_t>* params,
                  uint32_t option_count,
                  Seq<DefaultValue*>* default_values,
                  uint32_t return_type);
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    void setFlags(uint8_t flags);
    
    const uint32_t index;
    const uint32_t name;
    const uint32_t param_count;
    Seq<uint32_t> * const param_types;
    const uint32_t option_count;
    Seq<DefaultValue*> * const default_values;
    const uint32_t return_type;
    
private:
    uint32_t flags;
};

class ABCExceptionInfo : public ABCChunk {
public:
    ABCExceptionInfo(uint32_t from, uint32_t to, uint32_t target, uint32_t exception_type, uint32_t var_name);
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    const uint32_t from;
    const uint32_t to;
    const uint32_t target;
    const uint32_t exception_type;
    const uint32_t var_name;
};

class ABCExceptionTable : public ABCChunk {
public:
    ABCExceptionTable(Compiler* compiler);
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    uint32_t addAtEnd(ABCExceptionInfo* e);

private:
    uint32_t exceptionCount;
    SeqBuilder<ABCExceptionInfo*> exceptions;
};

class ABCMethodBodyInfo : public ABCChunk {
public:
    ABCMethodBodyInfo(Compiler* compiler, ABCMethodInfo* method_info, ABCTraitsTable* traits, uint32_t first_temp, bool is_empty);
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    uint8_t getFlags();
    void clearTraits();     // a hack

    Cogen cogen;
    ABCExceptionTable exceptions;
    ABCMethodInfo * const method_info;
    ABCTraitsTable * traits;
    const bool is_empty;
};

class ABCTrait : public ABCChunk {
public:
    ABCTrait(uint32_t name, uint32_t kind) : name(name), kind(kind) {}
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    virtual uint32_t dataSize() = 0;
    virtual uint8_t* serializeData(uint8_t* b) = 0;
    
    const uint32_t name;
    const uint32_t kind;
};

class ABCSlotTrait : public ABCTrait {
public:
    ABCSlotTrait(uint32_t name, uint32_t type_name, TraitKind kind)
        : ABCTrait(name, kind)
        , type_name(type_name)
    {
        AvmAssert(kind == TRAIT_Slot || kind == TRAIT_Const);
    }
    
    uint32_t type_name;
    virtual uint32_t dataSize();
    virtual uint8_t* serializeData(uint8_t* b);
};

class ABCMethodTrait : public ABCTrait {
public:
    ABCMethodTrait(uint32_t name, uint32_t method_info) : ABCTrait(name, TRAIT_Method), method_info(method_info) {}
    
    virtual uint32_t dataSize();
    virtual uint8_t* serializeData(uint8_t* b);
    
    const uint32_t method_info;
};

class ABCClassTrait : public ABCTrait {
public:
    ABCClassTrait(uint32_t name, uint32_t slot_id, uint32_t index) : ABCTrait(name, TRAIT_Class), slot_id(slot_id), index(index) {}
    
    virtual uint32_t dataSize();
    virtual uint8_t* serializeData(uint8_t* b);
    
    const uint32_t slot_id;
    const uint32_t index;
};

class ABCMetadataInfo : public ABCChunk {
public:
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
};

class ABCInstanceInfo : public ABCChunk {
public:
    ABCInstanceInfo(uint32_t name, uint32_t super_name, uint8_t flags, uint32_t protectedNS,
                    uint32_t interface_count, uint32_t* interfaces,
                    uint32_t iinit,
                    ABCTraitsTable* traits)
        : name(name)
        , super_name(super_name)
        , flags(flags)
        , protectedNS(protectedNS)
        , interface_count(interface_count)
        , interfaces(interfaces)
        , iinit(iinit)
        , traits(traits)
    {
    }

    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    const uint32_t name;
    const uint32_t super_name;
    const uint8_t flags;
    const uint32_t protectedNS;
    const uint32_t interface_count;
    uint32_t* const interfaces;
    const uint32_t iinit;
    ABCTraitsTable * const traits;
};

class ABCClassInfo : public ABCChunk {
public:
    ABCClassInfo(uint32_t cinit, ABCTraitsTable* traits)
        : cinit(cinit)
        , traits(traits)
    {
    }
    
    virtual uint32_t size();
    virtual uint8_t* serialize(uint8_t* b);
    
    const uint32_t cinit;
    ABCTraitsTable * const traits;
};

class ABCNamespaceInfo {
public:
    ABCNamespaceInfo(uint8_t kind, uint32_t name) : kind(kind), name(name) {}
    const uint8_t kind;
    const uint32_t name;
};

class ABCNamespaceSetInfo {
public:
    ABCNamespaceSetInfo(uint32_t length) : length(length) {}
    const uint32_t length;
    uint32_t ns[1];         // really longer than that
};

class ABCMultinameInfo {
public:
    ABCMultinameInfo(uint8_t kind, uint32_t ns_or_nsset, uint32_t name) : kind(kind), ns_or_nsset(ns_or_nsset), name(name) {}
    const uint8_t kind;
    const uint32_t ns_or_nsset;     // For CONSTANT_TypeName this is the parameterizedType
    const uint32_t name;            // For CONSTANT_TypeName this is the parameterType
};

}}
