/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Abc,
    namespace Abc;

use namespace Asm;

// Construct an ABCFile instance from a bytestream representing an abc block.
function parseAbcFile(b : ABCByteStream) : ABCFile {
    b.position = 0;
    magic = b.readInt();

    if (magic != (46<<16|16))
        throw new Error("not an abc file.  magic=" + magic.toString(16));
        
    var abc : ABCFile = new ABCFile();

    abc.constants = parseCpool(b);
        
    var i;
    var n;
    // MethodInfos
    n = b.readU32();
    for(i = 0; i < n; i++) {
        abc.addMethod(parseMethodInfo(b));
    }

    // MetaDataInfos
    n = b.readU32();
    for(i = 0; i < n; i++) {
        abc.addMetadata(parseMetadata(b));
    }

    // InstanceInfos
    n = b.readU32();
    for(i = 0; i < n; i++) {
        abc.addInstance(parseInstanceInfo(b));
    }

    // ClassInfos
    for(i = 0; i < n; i++) {
        abc.addClass(parseClassInfo(b));
    }

    // ScriptInfos
    n = b.readU32();
    for(i = 0; i < n; i++) {
        abc.addScript(parseScriptInfo(b));
    }

    // MethodBodies
    n = b.readU32();
    for(i = 0; i < n; i++) {
        abc.addMethodBody(parseMethodBody(b));
    }

    return abc;            
}

function parseCpool(b : ABCByteStream) : ABCConstantPool {
    var i:int;
    var n:int;
        
    var pool : ABCConstantPool = new ABCConstantPool;
        
    // ints
    n = b.readU30();
    for (i=1; i < n; i++)
        pool.int32(b.readS32());
        
    // uints
    n = b.readU30();
    for (i=1; i < n; i++)
        pool.uint32(b.readU32());
        
    // doubles
    n = b.readU30();
    doubles = [NaN];
    for (i=1; i < n; i++)
        pool.float64(b.readDouble());

    // strings
    n = b.readU30();
    for (i=1; i < n; i++)
        pool.stringUtf8(b.readUTFBytes(b.readU32()));
        
    // namespaces
    n = b.readU30()
		for (i=1; i < n; i++)
            {
                var nskind = b.readByte();
                var uri = b.readU32();
                pool.namespace(nskind, uri);
            }
        
    // namespace sets
    n = b.readU30();
    for (i=1; i < n; i++)
		{
			var count:int = b.readU30();
			var nsset = [];
			for (j=0; j < count; j++)
				nsset[j] = b.readU30();
            pool.namespaceset(nsset);
		}
        
    // multinames
    n = b.readU30();
    for (i=1; i < n; i++) {
        var kind = b.readByte();
        switch (kind) {
        case CONSTANT_QName:
        case CONSTANT_QNameA:
            pool.QName(b.readU30(), b.readU30(), kind==CONSTANT_QNameA);
            break;
			
        case CONSTANT_RTQName:
        case CONSTANT_RTQNameA:
            pool.RTQName(b.readU30(), kind==CONSTANT_RTQNameA);
            break;
			
        case CONSTANT_RTQNameL:
        case CONSTANT_RTQNameLA:
            pool.RTQNameL(kind==CONSTANT_RTQNameLA);
            names[i] = null;
            break;
			
        case CONSTANT_Multiname:
        case CONSTANT_MultinameA:
            var name = b.readU30();
            pool.Multiname(b.readU30(), name, kind==CONSTANT_MultinameA);
            break;

        case CONSTANT_MultinameL:
        case CONSTANT_MultinameLA:
            pool.MultinameL(b.readU30(), kind==CONSTANT_MultinameLA);
            break;
        }
    }
        
    return pool;
}

function parseMethodInfo(b : ABCByteStream) : ABCMethodInfo {
        
    var paramcount = b.readU32();
    var returntype = b.readU32();
    var params = [];
    for(let i = 0; i < paramcount; ++i) {
        params[i] = b.readU32();
    }
        
    var name = b.readU32();
    var flags = b.readByte();
        
    var optionalcount = 0;
    var optionals = null;
    if( flags & METHOD_HasOptional ) {
        optionalcount = b.readU32();
        optionals = [];
        for(let i = 0; i < optionalcount; ++i ) {
            optionals[i] = { val:b.readU32(), kind:b.readByte() };
        }
    }
        
    var paramnames = null;
    if( flags & METHOD_HasParamNames )        {
        paramnames=[];
        for(let i = 0; i < paramcount; ++i)
            paramnames[i] = b.readU32();
    }    
        
    return new ABCMethodInfo(name, params, returntype, flags, optionals, paramnames);
}
    
function parseMetadataInfo(b : ABCByteStream) : ABCMetadataInfo {
    var name = b.readU32();
    var itemcount = b.readU32();
        
    var items = [];
    for( let i = 0; i < itemcount; i++ ) {
        let key = b.readU32();
        let value = b.readU32();
        items[i] = { key:key, value:value };
    }
        
    return new ABCMetadataInfo(name, items);
}
    
function parseInstanceInfo(b : ABCByteStream) : ABCInstanceInfo {
    var name = b.readU32();
    var superclass = b.readU32();
    var flags = b.readByte();
    var protectedNS = 0;
    if( flags & 8 ) 
        protectedNS = b.readU32();
        
    var interfacecount = b.readU32();
    var interfaces = [];
    for(let i = 0; i < interfacecount; ++i) {
        interfaces[i] = b.readU32();
    }
    var iinit = b.readU32();
        
    var instance_info = new ABCInstanceInfo(name, superclass, flags, protectedNS, interfaces);
        
    instance_info.setIInit(iinit);
        
    parseTraits(instance_info, b);
        
    return instance_info;
}
    
function parseClassInfo(b : ABCByteStream) : ABCClassInfo {
    var cinit = b.readU32();

    var class_info = new ABCClassInfo();
    class_info.setCInit(cinit);
        
    parseTraits(class_info, b);
        
    return class_info;
}
    
function parseScriptInfo(b : ABCByteStream) : ABCScriptInfo {
        
    var script = new ABCScriptInfo(b.readU32());
    parseTraits(script, b);
    return script;
}
    
function parseMethodBody(b : ABCByteStream) : ABCMethodBodyInfo {
    var mb:ABCMethodBodyInfo = new ABCMethodBodyInfo(b.readU32());
        
    mb.setMaxStack(b.readU32());
    mb.setLocalCount(b.readU32());
    mb.setInitScopeDepth(b.readU32());
    mb.setMaxScopeDepth(b.readU32());
        
    let code_len = b.readU32();
    let code = new ABCByteStream;
    mb.setCode(code);
    for(let i = 0; i < code_len; ++i) {
        code.uint8(b.readByte());
    }
        
    var excount = b.readU32();
    for( let i = 0; i < excount; ++i ) {
        mb.addException(parseException(b));
    }
        
    parseTraits(mb, b);
        
    return mb;
}
    
function parseException(b : ABCByteStream) : ABCException {
    var start = b.readU32();
    var end = b.readU32();
    var target = b.readU32();
    var typename = b.readU32();
    var name = b.readU32();
        
    // WTF is wrong with this????
    var ex;
    ex = new ABCException(start, end, target, typename, name);
    return ex;
}
    
function parseTraits(target, b : ABCByteStream) {
    var traitcount = b.readU32();
    for(let i =0 ; i < traitcount; ++i) {
        target.addTrait(parseTrait(b));
    }
}

function parseTrait(b : ABCByteStream) { //: ABCTrait should be ABCTrait once inheritance is supported

    var name = b.readU32();
        
    var tag = b.readByte();
    var kind = tag & 0x0F;
    var attrs = (tag>>4) & 0x0F;
        
    var trait;
        
    switch(kind) {
    case TRAIT_Slot:
    case TRAIT_Const:
        let slotid = b.readU32();
        let typename = b.readU32();
        let value = b.readU32();
        let kind = null;
        if( value != 0 )
            kind = b.readByte();
        trait = new ABCSlotTrait(name, attrs, kind==TRAIT_Const, slotid, typename, value, kind);
        break;
    case TRAIT_Method:
    case TRAIT_Setter:
    case TRAIT_Getter:
        let dispid = b.readU32();
        let methinfo = b.readU32();
        trait = new ABCOtherTrait(name, attrs, kind, dispid, methinfo);
        break;
    case TRAIT_Class:
        let slotid = b.readU32();
        let classinfo = b.readU32();
        trait = new ABCOtherTrait(name, attrs, kind, slotid, classinfo);
        break;
    case TRAIT_Function: // TODO:
        b.readU32();
        b.readU32();
        break;
    }
        
    if( attrs & ATTR_Metadata ) {
        let metadatacount = b.readU32();
        for(let i = 0; i < metadatacount; ++i) {
            trait.addMetadata(b.readU32());
        }
    }
        
    return trait;
}

// This is a helper for parsing the ABCConstantPool from its
// bytes and storing the resulting values in arrays.  This allows
// easy access to the values referenced in the pools.

public class ABCConstantPoolParser {
    // the pools themselves are public for people who need the
    // raw data.
    public var int_pool, uint_pool, double_pool;
    public var utf8_pool, namespace_pool, namespaceset_pool, name_pool;

    public function ABCConstantPoolParser(pool: ABCConstantPool) {
        // load the constants from an existing abc object
        // setup our arrays, reserving element 0 which remains undefined
        int_pool = new Array(1);
        uint_pool = new Array(1);
        double_pool = new Array(1);
        utf8_pool = new Array(1);
        namespace_pool = new Array(1);
        namespaceset_pool = new Array(1);
        name_pool = new Array(1);

        // and read the bytes from the streams.
        intPool(pool.int_bytes);
        uintPool(pool.uint_bytes);
        doublePool(pool.double_bytes);
        utf8Pool(pool.utf8_bytes);
        namespacePool(pool.namespace_bytes);
        namespacesetPool(pool.namespaceset_bytes);
        namePool(pool.multiname_bytes);
    }

    // We also provide a couple of helper functions take an index/ices
    // and indirects through the various pools returning the value.
    public function getInt(offset: uint) : int {
        return int_pool[offset];
    }
    public function getUint(offset: uint) : uint {
        return uint_pool[offset];
    }
    public function getDouble(offset: uint) : double {
        return double_pool[offset];
    }
    public function getUtf8(offset: uint) : string {
        return utf8_pool[offset];
    }
    public function getNamespace(offset: uint) {
        // returns [kind:uint, value:string]
        var entry = namespace_pool[offset]
            return [entry.kind, getUtf8(entry.utf8)]
            }
    public function getConstantName(offset: uint) {
        // returns [namespace:string, name:string]
        var entry = name_pool[offset]
            Util::assert(entry.kind == CONSTANT_QName); // not a constant string.
        var [ns_kind, ns_name] = getNamespace(entry.ns)
            return [ns_name, getUtf8(entry.utf8)]
            }
    public function getConstantNameType(offset: uint) {
        var entry = name_pool[offset]
            Util::assert(entry.kind == CONSTANT_QName); // not a constant string.
        var [ns_kind, ns_name] = getNamespace(entry.ns)
            return ns_kind;
    }
    // any others?

    // ******************************************************
    // Loading/parsing of the pools
    // prepare a pool's stream for re-reading
    function preparePool (nd) : uint
    {
        var orig = nd.position;
        var pos = 0;
        nd.position = (pos);
        return orig;
    }

    // load the various pools
    function intPool (nd) : void {
        var orig = preparePool(nd);
        while (nd.bytesAvailable) 
            int_pool.push(nd.readS32());
        nd.position = orig;
    }

    function uintPool (nd) : void {
        var orig = preparePool(nd);
        while (nd.bytesAvailable) 
            uint_pool.push(nd.readU32());
        nd.position = orig;
    }

    function doublePool (nd) : void {
        var orig = preparePool(nd);nd.position;
        while (nd.bytesAvailable) 
            double_pool.push(nd.readDouble());
        nd.position = orig;
    }

    function utf8Pool (nd) : void {
        var orig = preparePool(nd);
        while (nd.bytesAvailable) {
            var len = nd.readU32();
            utf8_pool.push(nd.readUTFBytes(len));
        }
        nd.position = orig;
    }

    function namespacePool (nd) : void {
        var orig = preparePool(nd);
        while(nd.bytesAvailable)
            namespace_pool.push( {kind: nd.readByte(), utf8: nd.readU32()} )
                nd.position = orig;
    }

    function namespacesetPool (nd) : void {
        var orig = preparePool(nd);
        while(nd.bytesAvailable) {
            // read a const - each one is variable length.
            var len = nd.readU32();
            var new_val = new Array(len);
            for (var i = 0 ; i < len; ++i) 
                new_val[i] = nd.readU32();

            namespaceset_pool.push(new_val);
        }
        nd.position = orig;
    }

    function namePool (nd) : void {
        var orig = preparePool(nd);
        while(nd.bytesAvailable) {
            // read a name-const
            var kind = nd.readByte();
            var new_val = {kind: kind}

            switch (kind) {
            case CONSTANT_QName:
                new_val.ns = nd.readU32(); // ns offset
                new_val.utf8 = nd.readU32(); // utf8 index
                break;
            case CONSTANT_RTQName:
                new_val.utf8 = nd.readU32(); // utf8 index.
                break;
            case CONSTANT_RTQNameL:
                break;
            case CONSTANT_Multiname:
                new_val.utf8 = nd.readU32(); // utf8 offset
                new_val.nsset = nd.readU32(); // nsset offset
                break;
            case CONSTANT_MultinameL:
                new_val.nss = nd.readU32() // nss
                    break;
            case CONSTANT_QNameA:
            case CONSTANT_RTQNameA:
            case CONSTANT_RTQNameLA:
            case CONSTANT_MultinameA:
            case CONSTANT_MultinameLA:
            default:
                // we must fail here - we don't know how many bytes to
                // read, screwing up attempts to parse ones which follow
                throw("Unsupported Name constant");
            }
            name_pool.push(new_val);
        }
        nd.position = orig;
    }
}

