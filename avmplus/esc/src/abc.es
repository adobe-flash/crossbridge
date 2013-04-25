/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Abc,
    namespace Abc;

use namespace Asm,
    namespace Util;

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

class ABCFile
{
    const major_version = 46;
    const minor_version = 16;

    const methods = [];
    const metadatas = [];
    const instances = [];
    const classes = [];
    const scripts = [];
    const bodies = [];
    var constants : ABCConstantPool;

    function getBytes(): * /* same type as ABCByteStream.getBytes() */ {
        function emitArray(a, len) {
            if (len)
                bytes.uint30(a.length);
            for ( let i=0, limit=a.length ; i < limit ; i++ )
                a[i].serialize(bytes);
        }

        let bytes = new ABCByteStream;

        Util::assert(constants);
        Util::assert(scripts.length != 0);
        Util::assert(methods.length != 0);
        Util::assert(bodies.length != 0);
        Util::assert(classes.length == instances.length);

        bytes.uint16(minor_version);
        bytes.uint16(major_version);
        constants.serialize(bytes);
        emitArray(methods,true);
        emitArray(metadatas,true);
        emitArray(instances,true);
        emitArray(classes, false);
        emitArray(scripts,true);
        emitArray(bodies,true);
        return bytes.getBytes();
    }

    function addConstants(cpool: ABCConstantPool): void {
        constants = cpool;
    }

    function addMethod(m: ABCMethodInfo)/*: uint*/ {
        return methods.push(m)-1;
    }

    function addMetadata(m: ABCMetadataInfo)/*: uint*/ {
        return metadatas.push(m)-1;
    }

    function addClassAndInstance(cls, inst)/*: uint*/ {
        let x = addClass(cls);
        let y = addInstance(inst);
        Util::assert( x == y );
        return x;
    }

    function addInstance(i: ABCInstanceInfo)/*: uint*/ {
        return instances.push(i)-1;
    }

    function addClass(c: ABCClassInfo)/*: uint*/ {
        return classes.push(c)-1;
    }

    function addScript(s: ABCScriptInfo)/*: uint*/ {
        return scripts.push(s)-1;
    }

    function addMethodBody(b: ABCMethodBodyInfo)/*: uint*/ {
        return bodies.push(b)-1;
    }
        
}

class ABCConstantPool
{
    function ABCConstantPool() {
        function eq_numbers(n1, n2)
            n1 == n2;

        function eq_strings(s1, s2)
            s1 == s2; 

        function hash_namespace(ns)
            ns.kind ^ ns.name;          // Fairly arbitrary

        function eq_namespaces(ns1, ns2)
            ns1.kind == ns2.kind && ns1.name == ns2.name;

        function hash_multiname(m)
            m.kind ^ m.name ^ m.ns;     // Fairly arbitrary

        function eq_multinames(m1, m2)
            m1.kind == m2.kind && m1.ns == m2.ns && m1.name == m2.name;

        function hash_namespaceset(nss) {
            let hash = nss.length;
            for ( let i=0, limit=nss.length ; i < limit ; i++ )
                hash = ((hash << 5) + hash) + nss[i];
            return hash >>> 0;
        }

        function eq_namespacesets(nss1, nss2) {
            if (nss1.length != nss2.length)
                return false;
            for (let i=0, limit=nss1.length ; i < limit ; i++)
                if (nss1[i] != nss2[i])
                    return false;
            return true;
        }

        function hash_name(n: Token::Tok)
            n.hash;

        function eq_names(n1: Token::Tok, n2: Token::Tok)
            n1 === n2;

        // All pools and counts start at 1.  Counts are
        // initialized in the property definitions.

        multiname_pool.length = 1;

        int_map = new Hashtable(Util::hash_number, eq_numbers, 0);
        uint_map = new Hashtable(Util::hash_number, eq_numbers, 0);
        double_map = new Hashtable(Util::hash_number, eq_numbers, 0);
        utf8_map = new Hashtable(hash_name, eq_names, 0);
        namespace_map = new Hashtable(hash_namespace, eq_namespaces, 0);
        namespaceset_map = new Hashtable(hash_namespaceset, eq_namespacesets, 0);
        multiname_map = new Hashtable(hash_multiname, eq_multinames, 0);
    }

    function int32(n:int):uint {
        let probe = int_map.read(n);
        if (probe == 0) {
            probe = int_count++;
            int_map.write(n, probe);
            int_bytes.int32(n);
        }
        return probe;
    }

    function uint32(n:uint):uint {
        let probe = uint_map.read(n);
        if (probe == 0) {
            probe = uint_count++;
            uint_map.write(n, probe);
            uint_bytes.uint32(n);
        }
        return probe;
    }

    function float64(n: Number):uint {
        let probe = double_map.read(n);
        if (probe == 0) {
            probe = double_count++;
            double_map.write(n, probe);
            double_bytes.float64(n);
        }
        return probe;
    }

    function symbolUtf8(s) {
        if (!(s is Token::Tok))
            throw new Error("Not a token: <" + s + ">: " + (s is String) + " " + (s is Number));
        let probe = utf8_map.read(s);
        if (probe == 0) {
            probe = utf8_count++;
            utf8_map.write(s, probe);
            utf8_bytes.uint30(utf8length(s.text));
            utf8_bytes.utf8(s.text);
        }
        return probe;
    }

    function stringUtf8(s) {
        if (!(s is String))
            throw new Error("Not a string: <" + s + ">");
        return symbolUtf8(Token::intern(s));
    }

    // The virtue of this solution is that it caters to the common
    // case and does not need to know anything about utf8 apart
    // from the ASCII range.  The uncommon case can be optimized,
    // if we care.

    function utf8length(s) {
        let i;
        let limit=s.length; 

        for ( i=0 ; i < limit && s.charCodeAt(i) < 128 ; i++ )
            ;
        if (i == limit)
            return limit;

        let bs = new ABCByteStream;
        bs.utf8(s);
        return bs.length;
    }

    var tmp_namespace = {"kind": 0, "name": 0 };  // avoids allocation when we don't need it

    function namespace(kind/*:uint*/, name/*:uint*/) {
        tmp_namespace.kind = kind;
        tmp_namespace.name = name;
        let probe = namespace_map.read(tmp_namespace);
        if (probe == 0) {
            probe = namespace_count++;
            namespace_map.write({"kind": tmp_namespace.kind, "name": tmp_namespace.name}, probe);
            namespace_bytes.uint8(tmp_namespace.kind);
            namespace_bytes.uint30(tmp_namespace.name);
        }
        return probe;
    }

    function namespaceset(namespaces:Array) {
        let probe = namespaceset_map.read(namespaces);
        if (probe == 0) {
            probe = namespaceset_count++;
            // Not necessary to copy here at the moment, as the array is a fresh
            // copy created from flattening a NamespaceSetList.
            namespaceset_map.write(/*Util::copyArray*/(namespaces), probe);
            namespaceset_bytes.uint30(namespaces.length);
            for ( let i=0, limit=namespaces.length ; i < limit ; i++ )
                namespaceset_bytes.uint30(namespaces[i]);
        }
        return probe;
    }

    /* Look up a multiname entry with kind, name, and namespace
     * set.  Allocate an entry for it if it does not exist.  If an
     * entry were allocated, then the negative of the entry index
     * is returned (and the caller should emit data to the
     * multiname_bytes stream), otherwise the entry index is
     * returned.  (If this seems a little contorted, it reduces
     * allocation of closures in the caller.)
     */

    // Temporary structure (avoids allocation of structures that already exist).
    var tmp_multiname = { "kind": 0, "name": 0, "ns": 0 };

    function multinameLookup(kind, name, ns) {
        tmp_multiname.kind = kind;
        tmp_multiname.name = name;
        tmp_multiname.ns = ns;
        let probe = multiname_map.read(tmp_multiname);
        if (probe != 0)
            return probe;

        // Allocate
        probe = multiname_pool.length;
        let entry = {"kind":tmp_multiname.kind, "name":tmp_multiname.name, "ns":tmp_multiname.ns}
        multiname_pool.push(entry);           // need "kind" for later, could optimize here --
        multiname_map.write(entry, probe);    //   but need to save the whole entry anyway
        return -probe;
    }

    function QName(ns/*: uint*/, name/*: uint*/, is_attr: Boolean) {
        let kind = is_attr ? CONSTANT_QNameA : CONSTANT_QName;
        let idx = multinameLookup( kind, name, ns );
        if (idx < 0) {
            multiname_bytes.uint8(kind);
            multiname_bytes.uint30(ns);
            multiname_bytes.uint30(name);
            idx = -idx;
        }
        return idx;
    }

    function RTQName(name/*: uint*/, is_attr: Boolean) {
        let kind = is_attr ? CONSTANT_RTQNameA : CONSTANT_RTQName;
        let idx = multinameLookup( kind, name, 0 );
        if (idx < 0) {
            multiname_bytes.uint8(kind);
            multiname_bytes.uint30(name); 
            idx = -idx;
        }
        return idx;
    }

    function RTQNameL(is_attr: Boolean) {
        let kind = is_attr ? CONSTANT_RTQNameLA : CONSTANT_RTQNameL;
        let idx = multinameLookup( kind, 0, 0 );
        if (idx < 0) {
            multiname_bytes.uint8(kind);
            idx = -idx;
        }
        return idx;
    }

    function Multiname(nsset/*: uint*/, name/*: uint*/, is_attr: Boolean) {
        let kind = is_attr ? CONSTANT_MultinameA : CONSTANT_Multiname;
        let idx = multinameLookup(kind, name, nsset );
        if (idx < 0) {
            multiname_bytes.uint8(kind);
            multiname_bytes.uint30(name);
            multiname_bytes.uint30(nsset); 
            idx = -idx;
        }
        return idx;
    }

    function MultinameL(nsset/*: uint*/, is_attr: Boolean) {
        let kind = is_attr ? CONSTANT_MultinameLA : CONSTANT_MultinameL;
        let idx = multinameLookup(kind, 0, nsset);
        if (idx < 0) {
            multiname_bytes.uint8(kind);
            multiname_bytes.uint30(nsset);
            idx = -idx;
        }
        return idx;
    }

    function hasRTNS(index) {
        let kind = multiname_pool[index].kind;
        let result;
        switch (kind) {
        case CONSTANT_RTQName:
        case CONSTANT_RTQNameA:
        case CONSTANT_RTQNameL:
        case CONSTANT_RTQNameLA:
            result = true;
        default:
            result = false;
        }
        return result;
    }

    function hasRTName(index) {
        let kind = multiname_pool[index].kind;
        let result;
        switch (multiname_pool[index].kind) {
        case CONSTANT_RTQNameL:
        case CONSTANT_RTQNameLA:
        case CONSTANT_MultinameL:
        case CONSTANT_MultinameLA:
            result = true;
        default:
            result = false;
        }
        return result;
    }

    function printPoolStats() {
        print("  Ints: n=" + int_count + ", bytes=" + int_bytes.length);
        print("  Uints: n=" + uint_count + ", bytes=" + uint_bytes.length);
        print("  Doubles: n=" + double_count + ", bytes=" + double_bytes.length);
        print("  Strings: n=" + utf8_count + ", bytes=" + utf8_bytes.length);
        print("  Namespaces: n=" + namespace_count + ", bytes=" + namespace_bytes.length);
        print("  Namespace sets: n=" + namespaceset_count + ", bytes=" + namespaceset_bytes.length);
        print("  Multinames: n=" + multiname_pool.length + ", bytes=" + multiname_bytes.length);
    }

    function serialize(bs) {
        bs.uint30(int_count);
        bs.byteStream(int_bytes);

        bs.uint30(uint_count);
        bs.byteStream(uint_bytes);

        bs.uint30(double_count);
        bs.byteStream(double_bytes);

        bs.uint30(utf8_count);
        bs.byteStream(utf8_bytes);

        bs.uint30(namespace_count);
        bs.byteStream(namespace_bytes);

        bs.uint30(namespaceset_count);
        bs.byteStream(namespaceset_bytes);

        bs.uint30(multiname_pool.length);
        bs.byteStream(multiname_bytes);

        //printPoolStats();

        return bs;
    }

    // abc-parse.es grubs around here.

    /*private*/ var   int_count = 1;
    /*private*/ var   uint_count = 1;
    /*private*/ var   double_count = 1;
    /*private*/ var   utf8_count = 1;
    /*private*/ var   namespace_count = 1;
    /*private*/ var   namespaceset_count = 1;
    /*private*/ const multiname_pool = new Array; // its length is the count

    /*private*/ const int_bytes = new ABCByteStream;
    /*private*/ const uint_bytes = new ABCByteStream;
    /*private*/ const double_bytes = new ABCByteStream;
    /*private*/ const utf8_bytes = new ABCByteStream;
    /*private*/ const namespace_bytes = new ABCByteStream;
    /*private*/ const namespaceset_bytes = new ABCByteStream;
    /*private*/ const multiname_bytes = new ABCByteStream;

    var utf8_map;
    var multiname_map;
    var namespace_map;
    var namespaceset_map;
}

class ABCMethodInfo
{
    /* \param name         string index
     * \param param_types  array of multiname indices.  May not be null.
     * \param return_type  multiname index.
     * \param flags        bitwise or of NEED_ARGUMENTS, NEED_ACTIVATION, HAS_REST, SET_DXNS
     * \param options      [{val:uint, kind:uint}], if present.
     * \param param_names  array of param_info structures, if present.
     */
    function ABCMethodInfo(name/*:uint*/, param_types:Array, return_type/*:uint*/, flags/*:uint*/,
                           options:Array, param_names:Array) {
        this.name = name;
        if (flags & METHOD_Needrest) {
            this.param_types = copyArray(param_types);
            this.param_types.pop();
        }
        else
            this.param_types = param_types;
        this.return_type = return_type;
        this.flags = flags;
        this.options = options;
        this.param_names = param_names;
    }

    function setFlags(flags) {
        this.flags = flags;
    }

    function serialize(bs) {
        let i, limit;
        bs.uint30(param_types.length);
        bs.uint30(return_type);
        for ( i=0, limit=param_types.length ; i < limit ; i++ )
            bs.uint30(param_types[i]);
        bs.uint30(name);
        if (options != null) {
            flags = flags | METHOD_HasOptional;
        }
        if (param_names != null) {
            flags = flags | METHOD_HasParamNames;
        }
        bs.uint8(flags);
        if (options != null) {
            bs.uint30(options.length);
            for ( i=0, limit=options.length ; i < limit ; i++ ) {
                bs.uint30(options[i].val);
                bs.uint8(options[i].kind);
            }
        }
        if (param_names != null) {
            Util::assert( param_names.length == param_types.length );
            for ( i=0, limit=param_names.length ; i < limit ; i++ )
                bs.uint30(param_names[i]);
        }
    }

    // abc-parse.es grubs around here.
    /*private*/ var name, param_types, return_type, flags, options, param_names;
}

class ABCMetadataInfo
{
    function ABCMetadataInfo( name/*: uint*/, items: Array ) {
        Util::assert( name != 0 );
        this.name = name;
        this.items = items;
    }

    function serialize(bs) {
        bs.uint30(name);
        bs.uint30(items.length);
        for ( let i=0, limit=items.length ; i < limit ; i++ ) {
            bs.uint30(items[i].key);
            bs.uint30(items[i].value);
        }
    }

    // abc-parse.es grubs around here.
    /*private*/ var name, items;
}

class ABCInstanceInfo
{
    function ABCInstanceInfo(name, super_name, flags, protectedNS, interfaces) {
        this.name = name;
        this.super_name = super_name;
        this.flags = flags;
        this.protectedNS = protectedNS;
        this.interfaces = interfaces;
        this.traits = [];
    }

    function setIInit(x) {
        iinit = x;
    }

    function addTrait(t) {
        return traits.push(t)-1;
    }

    function serialize(bs) {
        let i, limit;

        Util::assert( iinit != undefined || (flags & CONSTANT_ClassInterface) != 0);

        bs.uint30(name);
        bs.uint30(super_name);
        bs.uint8(flags);
        if (flags & CONSTANT_ClassProtectedNs)
            bs.uint30(protectedNS);
        bs.uint30(interfaces.length);
        for ( i=0, limit=interfaces.length ; i < limit ; i++ ) {
            Util::assert( interfaces[i] != 0 );
            bs.uint30(interfaces[i]);
        }
        bs.uint30(iinit);
        bs.uint30(traits.length);
        for ( i=0, limit=traits.length ; i < limit ; i++ )
            traits[i].serialize(bs);
    }

    // abc-parse.es grubs around here.
    /*private*/ var name, super_name, flags, protectedNS, interfaces, iinit, traits;
}

class ABCTrait
{
    /* FIXME #101: super not implemented; subclasses must do implementation themselves;
       the constructor must not be defined here (for the sake of AS3).  */
    /*
      function ABCTrait(name, kind) {
      this.name = name;
      this.kind = kind;
      }
    */

}

class ABCSlotTrait /// extends ABCTrait
{
    function ABCSlotTrait(name, attrs, is_const, slot_id, type_name, vindex, vkind) {
        /*FIXME #101: super not implemented*/
        //super(name, (attrs << 4) | TRAIT_Slot);
        this.name = name;
        this.kind = (attrs << 4) | (is_const ? TRAIT_Const : TRAIT_Slot);
        this.metadata = [];
        //End of fixme
        this.slot_id = slot_id;
        this.type_name = type_name;
        this.vindex = vindex;
        this.vkind = vkind;
    }

    function inner_serialize(bs) {
        bs.uint30(slot_id);
        bs.uint30(type_name);
        bs.uint30(vindex);
        if (vindex != 0)
            bs.uint8(vkind);
    }

    // abc-parse.es grubs around here.
    /*private*/ var slot_id, type_name, vindex, vkind;

    // from ABCTrait

    function addMetadata(n) {
        return metadata.push(n)-1;
    }

    function serialize(bs) {
        if (metadata.length > 0)
            kind = kind | ATTR_Metadata;
        bs.uint30(name);
        bs.uint30(kind);
        inner_serialize(bs);
        if (metadata.length > 0) {
            bs.uint30(metadata.length);
            for ( let i=0, limits=metadata.length ; i < limit ; i++ )
                bs.uint30(metadata[i]);
        }
    }

    var name, kind, metadata;
}

class ABCOtherTrait /// extends ABCTrait  // removed for esc
{
    /* TAG is one of the TRAIT_* values, except TRAIT_Slot */
    function ABCOtherTrait(name, attrs, tag, id, val) {
        /*FIXME #101: super not implemented*/
        //super(name, (attrs << 4) | tag);
        this.name = name;
        this.kind = (attrs << 4) | tag;
        this.metadata = [];
        //End of fixme
        this.id = id;
        this.val = val;
    }

    // esc doesn't support override yet
    function inner_serialize(bs) {
        bs.uint30(id);
        bs.uint30(val);
    }

    // abc-parse.es grubs around here.
    /*private*/ var id, val;

    // from ABCTrait

    function addMetadata(n) {
        return metadata.push(n)-1;
    }

    function serialize(bs) {
        if (metadata.length > 0)
            kind = kind | ATTR_Metadata;
        bs.uint30(name);
        bs.uint30(kind);
        inner_serialize(bs);
        if (metadata.length > 0) {
            bs.uint30(metadata.length);
            for ( let i=0, limit=metadata.length ; i < limit ; i++ )
                bs.uint30(metadata[i]);
        }
    }

    var name, kind, metadata;


}

class ABCClassInfo
{
    function setCInit(cinit) {
        this.cinit = cinit;
    }

    function addTrait(t) {
        return traits.push(t)-1;
    }

    function serialize(bs) {
        Util::assert( cinit != undefined );
        bs.uint30(cinit);
        bs.uint30(traits.length);
        for ( let i=0, limit=traits.length ; i < limit ; i++ )
            traits[i].serialize(bs);
    }

    // abc-parse.es grubs around here.
    /*private*/ var cinit, traits = [];
}

class ABCScriptInfo
{
    function ABCScriptInfo(init) {
        this.init = init;
    }

    function setInit(init) {
        this.init = init;
    }

    function addTrait(t) {
        return traits.push(t)-1;
    }

    function serialize(bs) {
        Util::assert( init != undefined );
        bs.uint30(init);
        bs.uint30(traits.length);
        for ( let i=0, limit=traits.length ; i < limit ; i++ )
            traits[i].serialize(bs);
    }

    // abc-parse.es grubs around here.
    /*private*/ var init, traits = [];
}

class ABCMethodBodyInfo
{
    function ABCMethodBodyInfo(method) {
        this.method = method;
    }
    function setMaxStack(ms) { max_stack = ms }
    function setLocalCount(lc) { local_count = lc }
    function setInitScopeDepth(sd) { init_scope_depth = sd }
    function setMaxScopeDepth(msd) { max_scope_depth = msd }
    function setCode(insns) { code = insns }

    function addException(exn) {
        return exceptions.push(exn)-1;
    }

    function addTrait(t) {
        return traits.push(t)-1;
    }

    function serialize(bs) {
        Util::assert( max_stack != undefined && local_count != undefined );
        Util::assert( init_scope_depth != undefined && max_scope_depth != undefined );
        Util::assert( code != undefined );

        bs.uint30(method);
        bs.uint30(max_stack);
        bs.uint30(local_count);
        bs.uint30(init_scope_depth);
        bs.uint30(max_scope_depth);
        bs.uint30(code.length);
        code.serialize(bs);
        bs.uint30(exceptions.length);
        for ( let i=0, limit=exceptions.length ; i < limit ; i++ )
            exceptions[i].serialize(bs);
        bs.uint30(traits.length);
        for ( let i=0, limit=traits.length ; i < limit ; i++ )
            traits[i].serialize(bs);
    }

    // abc-parse.es grubs around here.
    /*private*/ var init_scope_depth = 0, exceptions = [], traits = [];
    /*private*/ var method, max_stack, local_count, max_scope_depth, code;
}

class ABCException
{
    function ABCException(first_pc, last_pc, target_pc, exc_type, var_name) {
        this.first_pc = first_pc;
        this.last_pc = last_pc;
        this.target_pc = target_pc;
        this.exc_type = exc_type;
        this.var_name = var_name;
    }

    function serialize(bs) {
        bs.uint30(first_pc);
        bs.uint30(last_pc);
        bs.uint30(target_pc);
        bs.uint30(exc_type);
        bs.uint30(var_name);
    }

    // abc-parse.es grubs around here.
    /*private*/ var first_pc, last_pc, target_pc, exc_type, var_name;
}
