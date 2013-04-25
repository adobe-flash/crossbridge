/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"
#include "TypeDescriber.h"

using namespace MMgc;

namespace avmplus
{
    TypeDescriber::TypeDescriber(Toplevel* toplevel) :
        m_toplevel(toplevel)
    {
        VMPI_memset(m_strs, 0, sizeof(m_strs));
    }

    Stringp TypeDescriber::describeClassName(Traitsp traits)
    {
        if (!traits)
            return str(kstrid_asterisk);

        Stringp name = traits->formatClassName();

        // no need to explicitly escape XML chars (eg > into &gt;)
        // since we're returning as JSON and converting to XML in AS3...
        // the escaping will happen automatically by E4X infrastructure
        return name;
    }

    ScriptObject* TypeDescriber::new_object()
    {
        return m_toplevel->objectClass->newInstance();
    }

    ArrayObject* TypeDescriber::new_array()
    {
        return (ArrayObject*)m_toplevel->arrayClass()->newInstance();
    }

    void TypeDescriber::setpropmulti(ScriptObject* o, const TypeDescriber::KVPair* kv, uint32_t count)
    {
        while (count--)
        {
            o->setStringProperty(str(kv->keyid), kv->value);
            ++kv;
        }
    }

    Stringp TypeDescriber::str(StringId i)
    {
        static const char* const k_strs[maxStringId] =
        {
            "access",
            "accessors",
            "API",
            "*",
            "bases",
            "constructor",
            "declaredBy",
            "",
            "interfaces",
            "isDynamic",
            "isFinal",
            "isStatic",
            "key",
            "metadata",
            "method",
            "methods",
            "name",
            "native",
            "optional",
            "parameters",
            "readonly",
            "readwrite",
            "returnType",
            "traits",
            "type",
            "uri",
            "value",
            "variables",
            "writeonly"
        };
        if (!m_strs[i])
            m_strs[i] = m_toplevel->core()->internConstantStringLatin1(k_strs[i]);
        return m_strs[i];
    }

    #define elem_count(o) (sizeof(o)/sizeof((o)[0]))

    static void pushstr(ArrayObject* a, Stringp s)
    {
        a->setUintProperty(a->getLength(), s->atom());
    }

    static void pushobj(ArrayObject* a, ScriptObject* o)
    {
        a->setUintProperty(a->getLength(), o->atom());
    }

    static inline Atom strAtom(Stringp s)
    {
        return s ? s->atom() : nullStringAtom;
    }

    static inline Atom objAtom(ScriptObject* o)
    {
        return o ? o->atom() : nullObjectAtom;
    }

    static inline Atom boolAtom(bool b)
    {
        return b ? trueAtom : falseAtom;
    }

    ArrayObject* TypeDescriber::describeParams(MethodInfo* /*mi*/, MethodSignaturep ms)
    {
        ArrayObject* a = new_array();
        const int requiredParamCount = ms->requiredParamCount();
        for (int i = 1, n = ms->param_count(); i <= n; ++i)
        {
            ScriptObject* v = new_object();
            const KVPair props[] = {
                { kstrid_type, strAtom(describeClassName(ms->paramTraits(i))) },
                { kstrid_optional, boolAtom(i > requiredParamCount) },
            };
            setpropmulti(v, props, elem_count(props));
            pushobj(a, v);
        }
        return a;
    }

    void TypeDescriber::addBindings(AvmCore* core, MultinameBindingHashtable* bindings, TraitsBindingsp tb, uint32_t flags)
    {
        if (!tb) return;
        if ((flags & TypeDescriber::HIDE_OBJECT) && !tb->base && !tb->owner->isInterface()) return;
        addBindings(core, bindings, tb->base, flags);
        StTraitsBindingsIterator iter(tb);
        ApiVersion const curapi = core->getApiVersionFromCallStack();
        while (iter.next())
        {
            if (!iter.key()) continue;
            // Only add names if they are visible to the current active API
            if (iter.apiVersion() > curapi) continue;
            bindings->add(iter.key(), iter.ns(), iter.value());
        }
    }

    ScriptObject* TypeDescriber::describeTraits(Traitsp traits, uint32_t flags, Toplevel* toplevel)
    {
        if (!(flags & INCLUDE_TRAITS))
            return NULL;

        AvmCore* core = m_toplevel->core();
        GC* gc = core->GetGC();
        TraitsBindingsp tb = traits->getTraitsBindings();
        TraitsMetadatap tm = traits->getTraitsMetadata();

        ScriptObject* o = new_object();

        ArrayObject* bases = NULL;
        ArrayObject* metadata = NULL;
        ArrayObject* interfaces = NULL;
        ArrayObject* methods = NULL;
        ArrayObject* accessors = NULL;
        ArrayObject* variables = NULL;
        ScriptObject* constructor = NULL;

        if (flags & INCLUDE_METADATA)
        {
            metadata = new_array();
            PoolObject* class_mdpool;
            const uint8_t* class_md = tm->getMetadataPos(class_mdpool);
            if (class_md)
                addDescribeMetadata(metadata, class_mdpool, class_md);
        }

        if (flags & INCLUDE_BASES)
        {
            bases = new_array();
            for (Traitsp b = traits->base; b; b = b->base)
                pushstr(bases, describeClassName(b));
        }

        if (flags & INCLUDE_INTERFACES)
        {
            interfaces = new_array();
            for (InterfaceIterator iter(traits); iter.hasNext();)
            {
                Traits* ti = iter.next();
                pushstr(interfaces, describeClassName(ti));
            }
        }

        // constructor
        if (flags & INCLUDE_CONSTRUCTOR)
        {
            MethodInfo* initMethod = traits->init;
            if (initMethod)
            {
                initMethod->resolveSignature(toplevel);
                MethodSignaturep ms = initMethod->getMethodSignature();
                if (ms->param_count() > 0)
                {
                    constructor = describeParams(initMethod, ms);
                }
            }
        }

        if (flags & (INCLUDE_ACCESSORS | INCLUDE_METHODS | INCLUDE_VARIABLES))
        {
            // recover slot/method metadata and method-declarer information.

            // make a flattened set of bindings so we don't have to check for overrides as we go.
            // This is not terribly efficient, but doesn't need to be.
            MultinameBindingHashtable* mybind = MultinameBindingHashtable::create(gc);
            addBindings(m_toplevel->core(), mybind, tb, flags);

            // Don't want interface methods, so post-process and wipe out any
            // bindings that were added.
            for (InterfaceIterator ifc_iter(traits); ifc_iter.hasNext();)
            {
                Traitsp ti = ifc_iter.next();
                TraitsBindingsp tbi = ti->getTraitsBindings();
                StTraitsBindingsIterator iter(tbi);
                while (iter.next())
                {
                    if (!iter.key()) continue;
                    mybind->add(iter.key(), iter.ns(), BIND_NONE);
                }
            }

            // yuck, replicate buggy behavior in FP9/10
            RCList<Namespace> nsremoval(gc, kListInitialCapacity);
            if (flags & HIDE_NSURI_METHODS)
            {
                for (TraitsBindingsp tbi = tb->base; tbi; tbi = tbi->base)
                {
                    StTraitsBindingsIterator iter(tbi);
                    while (iter.next())
                    {
                        if (!iter.key()) continue;
                        Namespacep ns = iter.ns();
                        if (ns->getURI()->length() > 0 && nsremoval.indexOf(ns) < 0)
                        {
                            nsremoval.add(ns);
                        }
                    }
                }
            }

            StMNHTBindingIterator iter(mybind);
            while (iter.next())
            {
                if (!iter.key()) continue;
                Stringp name = iter.key();
                Namespacep ns = iter.ns();
                Binding binding = iter.value();
                Stringp nsuri = ns->getURI();
                TraitsMetadata::MetadataPtr md1 = NULL;
                TraitsMetadata::MetadataPtr md2 = NULL;
                PoolObject* md1pool = NULL;
                PoolObject* md2pool = NULL;

                // We only display public members -- exposing private namespaces could compromise security.
                if (ns->getType() != Namespace::NS_Public) {
                    continue;
                }

                if ((flags & HIDE_NSURI_METHODS) && nsremoval.indexOf(ns) >= 0) {
                    continue;
                }
                ScriptObject* v = new_object();

                const BindingKind bk = AvmCore::bindingKind(binding);
                switch (bk)
                {
                    case BKIND_CONST:
                    case BKIND_VAR:
                    {
                        if (!(flags & INCLUDE_VARIABLES))
                            continue;

                        const uint32_t slotID = AvmCore::bindingToSlotId(binding);
                        const KVPair props[] = {
                            { kstrid_access, strAtom(str(bk == BKIND_CONST ? kstrid_readonly : kstrid_readwrite)) },
                            { kstrid_type, strAtom(describeClassName(tb->getSlotTraits(slotID))) },
                        };
                        setpropmulti(v, props, elem_count(props));
                        if (!variables) variables = new_array();
                        pushobj(variables, v);
                        md1 = tm->getSlotMetadataPos(slotID, md1pool);
                        break;
                    }

                    case BKIND_METHOD:
                    {
                        if (!(flags & INCLUDE_METHODS))
                            continue;

                        const uint32_t methodID = AvmCore::bindingToMethodId(binding);
                        MethodInfo* mi = tb->getMethod(methodID);
                        mi->resolveSignature(toplevel);
                        MethodSignaturep ms = mi->getMethodSignature();

                        Traitsp declaringTraits = mi->declaringTraits();

                        const KVPair props[] = {
                            { kstrid_declaredBy, strAtom(describeClassName(declaringTraits)) },
                            { kstrid_returnType, strAtom(describeClassName(ms->returnTraits())) },
                            { kstrid_parameters, objAtom(describeParams(mi, ms)) },
                        };
                        setpropmulti(v, props, elem_count(props));
                        if (!methods) methods = new_array();
                        pushobj(methods, v);
                        md1 = tm->getMethodMetadataPos(methodID, md1pool);
                        break;
                    }

                    case BKIND_GET:
                    case BKIND_SET:
                    case BKIND_GETSET:
                    {
                        if (!(flags & INCLUDE_ACCESSORS))
                            continue;

                        const uint32_t methodID = AvmCore::hasGetterBinding(binding) ?
                                                    AvmCore::bindingToGetterId(binding) :
                                                    AvmCore::bindingToSetterId(binding);

                        MethodInfo* mi = tb->getMethod(methodID);
                        mi->resolveSignature(toplevel);
                        MethodSignaturep ms = mi->getMethodSignature();

                        Traitsp declaringTraits = mi->declaringTraits();

#ifdef SECURITYFIX_ELLIS
                        // https://bugzilla.mozilla.org/show_bug.cgi?id=786528
                        // The verifier does not check the signature of a setter
                        // except when it is invoked.  We must be prepared for the
                        // case in which it has no arguments.
                        Traitsp accessorType;

                        if (AvmCore::hasGetterBinding(binding)) {
                            accessorType = ms->returnTraits();
                        } else {
                            // If setter is malformed, just use '*' as a placeholder.
                            accessorType = (ms->param_count() < 1) ? NULL : ms->paramTraits(1);
                        }
#else /*SECURITYFIX_ELLIS*/
                        Traitsp accessorType = AvmCore::hasGetterBinding(binding) ?
                                                    ms->returnTraits() :
                                                    ms->paramTraits(1);
#endif /*SECURITYFIX_ELLIS*/

                        static const uint8_t bk2str[8] =
                        {
                            uint8_t(kstrid_emptyString),    // BKIND_NONE
                            uint8_t(kstrid_emptyString),    // BKIND_METHOD
                            uint8_t(kstrid_emptyString),    // BKIND_VAR
                            uint8_t(kstrid_emptyString),    // BKIND_CONST
                            uint8_t(kstrid_emptyString),    // unused
                            uint8_t(kstrid_readonly),       // BKIND_GET
                            uint8_t(kstrid_writeonly),      // BKIND_SET
                            uint8_t(kstrid_readwrite)       // BKIND_GETSET
                        };
                        const KVPair props[] = {
                            { kstrid_declaredBy, strAtom(describeClassName(declaringTraits)) },
                            { kstrid_access, strAtom(str(StringId(bk2str[bk]))) },
                            { kstrid_type, strAtom(describeClassName(accessorType)) },
                        };
                        setpropmulti(v, props, elem_count(props));
                        if (AvmCore::hasGetterBinding(binding))
                            md1 = tm->getMethodMetadataPos(AvmCore::bindingToGetterId(binding), md1pool);
                        if (AvmCore::hasSetterBinding(binding))
                            md2 = tm->getMethodMetadataPos(AvmCore::bindingToSetterId(binding), md2pool);
                        if (!accessors) accessors = new_array();
                        pushobj(accessors, v);
                        break;
                    }
                    case BKIND_NONE:
                        break;
                }

                ArrayObject* vm = NULL;
                if ((flags & INCLUDE_METADATA) && (md1 || md2))
                {
                    vm = new_array();
                    addDescribeMetadata(vm, md1pool, md1);
                    addDescribeMetadata(vm, md2pool, md2);
                }
                const KVPair props[] = {
                    { kstrid_name, strAtom(name) },
                    { kstrid_uri, strAtom(nsuri->length() == 0 ? NULL : nsuri) },
                    { kstrid_metadata, objAtom(vm) },
                };
                setpropmulti(v, props, elem_count(props));
            }
        }

        const KVPair props[] = {
            { kstrid_bases, objAtom(bases) },
            { kstrid_interfaces, objAtom(interfaces) },
            { kstrid_metadata, objAtom(metadata) },
            { kstrid_accessors, objAtom(accessors) },
            { kstrid_methods, objAtom(methods) },
            { kstrid_variables, objAtom(variables) },
            { kstrid_constructor, objAtom(constructor) },
        };
        setpropmulti(o, props, elem_count(props));

        return o;
    }

    void TypeDescriber::addDescribeMetadata(ArrayObject* a, PoolObject* pool, const uint8_t* meta_pos)
    {
        if (meta_pos)
        {
            uint32_t metadata_count = AvmCore::readU32(meta_pos);
            while (metadata_count--)
            {
                const uint32_t metadata_index = AvmCore::readU32(meta_pos);
                ScriptObject* md = describeMetadataInfo(pool, metadata_index);
                if (md)
                    pushobj(a, md);
            }
        }
    }

    static void read_u30_list(DataList<uint32_t>& list, uint32_t val_count, const uint8_t*& pos)
    {
        list.ensureCapacity(val_count);
        while (val_count--)
        {
            list.add(AvmCore::readU32(pos));
        }
    }

    ScriptObject* TypeDescriber::describeMetadataInfo(PoolObject* pool, uint32_t metadata_index)
    {
        AvmCore* core = m_toplevel->core();
        const uint8_t* metadata_pos = pool->metadata_infos[metadata_index];

        const uint32_t name_index = (metadata_pos) ? AvmCore::readU32(metadata_pos) : 0;
        // A bit of a hack: if the pool is builtin, always omit metadata chunks with names of "Version"
        // or "native" or "API", since these are used for reserved purposes internally.
        Stringp name = poolstr(pool, name_index);
        AvmAssert(name->isInterned() && core->kVersion->isInterned() && str(kstrid_native)->isInterned() && str(kstrid_api)->isInterned());
        if (pool->isBuiltin && (name == core->kVersion || name == str(kstrid_native) || name == str(kstrid_api)))
            return NULL;

        const uint32_t val_count = (metadata_pos) ? AvmCore::readU32(metadata_pos) : 0;

        ScriptObject* o = new_object();
        ArrayObject* a = new_array();

        if (val_count > 0)
        {
            MMgc::GC* gc = pool->core->GetGC();
            DataList<uint32_t> key_indexes(gc, kListInitialCapacity);
            DataList<uint32_t> val_indexes(gc, kListInitialCapacity);

            read_u30_list(key_indexes, val_count, metadata_pos);
            read_u30_list(val_indexes, val_count, metadata_pos);

            for (uint32_t i = 0; i < val_count; ++i)
            {
                ScriptObject* v = new_object();
                const KVPair props[] = {
                    { kstrid_key, strAtom(poolstr(pool, key_indexes.get(i))) },
                    { kstrid_value, strAtom(poolstr(pool, val_indexes.get(i))) },
                };
                setpropmulti(v, props, elem_count(props));
                pushobj(a, v);
            }
        }

        const KVPair props[] = {
            { kstrid_name, strAtom(name) },
            { kstrid_value, objAtom(a) },
        };
        setpropmulti(o, props, elem_count(props));

        return o;
    }

    Stringp TypeDescriber::poolstr(PoolObject* pool, uint32_t index)
    {
        return index < pool->constantStringCount ? pool->getString(index) : str(kstrid_emptyString);
    }

    // bug comaptibility note: FP10 and earlier sniffed the Atom type and return 'int' for
    // all integer atoms. This is wrong, but we should preserve the behavior. Note that we
    // compare the numeric range: doesn't matter for 32-bit builds, but 64-builds have larger
    // possible int atoms, so this preserves the existing behavior of "29-bit int -> int" even
    // on 64-bit systems.
    static bool isIntAtom29Bit(Atom value)
    {
        if (atomIsIntptr(value))
        {
            intptr_t const i = atomGetIntptr(value);
            int32_t const i32 = (int32_t(i)<<3)>>3;
            return i == intptr_t(i32);
        }
        return false;
    }

    Traits* TypeDescriber::chooseTraits(Atom value, uint32_t flags)
    {
        Traitsp traits;
        if (value == undefinedAtom)
            traits = m_toplevel->core()->traits.void_itraits;
        else if (ISNULL(value))
            traits = m_toplevel->core()->traits.null_itraits;
        else if (isIntAtom29Bit(value))
            traits = m_toplevel->core()->traits.int_itraits;
        else
            traits = m_toplevel->toTraits(value);

        if (flags & USE_ITRAITS)
            traits = traits->itraits;

        return traits;
    }

    ScriptObject* TypeDescriber::describeType(Atom value, uint32_t flags)
    {
        Traitsp traits = chooseTraits(value, flags);

        if (!traits)
            return NULL;

        ScriptObject* o = new_object();

        const KVPair props[] = {
            { kstrid_name, strAtom(describeClassName(traits)) },
            { kstrid_isDynamic, boolAtom(traits->needsHashtable()) },
            { kstrid_isFinal, boolAtom(traits->final) },
            { kstrid_isStatic, boolAtom(traits->itraits != NULL) },
            { kstrid_traits, objAtom(describeTraits(traits, flags, m_toplevel)) },
        };
        setpropmulti(o, props, elem_count(props));

        return o;
    }

    Stringp TypeDescriber::getQualifiedClassName(Atom value)
    {
        Traitsp traits = chooseTraits(value, 0);
        return traits ? describeClassName(traits) : NULL;
    }

    Stringp TypeDescriber::getQualifiedSuperclassName(Atom value)
    {
        // getQualifiedSuperclassName explicitly allows us to pass Class or Instance,
        // and either should resolve to super of Instance
        Traitsp traits = chooseTraits(value, 0);
        if (!traits)
            return NULL;

        if (traits->itraits)
            traits = traits->itraits;

        return traits->base && traits->base != m_toplevel->core()->traits.class_itraits ?
                describeClassName(traits->base) :
                NULL;
    }
}
