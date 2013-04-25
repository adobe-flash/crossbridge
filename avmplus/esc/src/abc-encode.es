/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace AbcEncode,
    namespace AbcEncode;

use namespace Release;
use namespace Ast;
use namespace Abc;
use namespace Asm;

function indent (n:int) : string {
    let str = "\n";

    for ( ; n > 0; n-- ) {
        str = str + " ";
    }

    return str;
}

function abcFile (nd : ABCFile, verbose = false, nesting : int = 0 ) : string {
    enter ("AbcEncode::abcFile ", nesting);
        
    var encoder = new AbcEncoder(nd, verbose);
        
    var str = encoder.encode(nesting);

    exit ("AbcEncode::abcFile");
    return str;
}
    
class AbcEncoder 
{
    var verbose : Boolean;
    var abc : ABCFile;
    var constants: ABCConstantPoolParser;
        
    function AbcEncoder(a:ABCFile, v:Boolean = false): abc=a, verbose=v {
    }
        
    function encode(nesting:int = 0) : string {
        enter ("AbcEncoder::encode", nesting);
        constants = new ABCConstantPoolParser(abc.constants);
        var str = indent(nesting) + "{ 'abc_class': 'AbcFile'"
            + indent(nesting) + ", 'minor_version': " + abc.minor_version
            + indent(nesting) + ", 'major_version': " + abc.major_version
            + indent(nesting) + ", 'int_pool': [ " + intPool (constants.int_pool,nesting+", 'int_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'uint_pool': [ " + uintPool (constants.uint_pool,nesting+", 'uint_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'double_pool': [ " + doublePool (constants.double_pool,nesting+", 'double_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'utf8_pool': [ " + utf8Pool (constants.utf8_pool,nesting+", 'utf8_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'namespace_pool': [ " + namespacePool (constants.namespace_pool,nesting+", 'namespace_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'nsset_pool': [ " + namespacesetPool (constants.namespaceset_pool,nesting+", 'nsset_pool': [ ".length) + " ]"
            + indent(nesting) + ", 'name_pool': [ " + namePool (constants.name_pool,nesting+", 'name_pool': [ ".length) + " ]"

            + indent(nesting) + ", 'method_infos': [ "   + methodInfos (abc.methods,nesting+", 'method_infos': [ ".length) + " ]"
                
            + indent(nesting) + ", 'metadata_infos': [ " + metadataInfos (abc.metadatas,nesting+", 'metadata_infos': [ ".length) + " ]"
            + indent(nesting) + ", 'instance_infos': [ " + instanceInfos (abc.instances,nesting+", 'instance_infos': [ ".length) + " ]"
            + indent(nesting) + ", 'class_infos': [ "   + classInfos (abc.classes,nesting+", 'class_infos': [ ".length) + " ]"
            + indent(nesting) + ", 'script_infos': [ "   + scriptInfos (abc.scripts,nesting+", 'script_infos': [ ".length) + " ]"
                
            + indent(nesting) + ", 'method_bodys': [ "   + methodBodys (abc.bodies,nesting+", 'method_bodys': [ ".length) + " ]"
            + " }";
        exit("AbcEncoder::encode");
            
        return str;
    }
        
    function intPool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::intPool ", nesting);
        var str = "undefined";
        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + "'" + v + "'"; 
        }

        exit ("AbcEncode::intPool");
        return str;
    }

    function uintPool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::uintPool ", nesting);
        var str = "undefined";
        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + "'" + v + "'" //intConst (nd[i], nesting)
                }

        exit ("AbcEncode::uintPool");
        return str;
    }

    function doublePool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::doublePool ", nesting);
        var str = "undefined";
        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + "'" + v + "'" //intConst (nd[i], nesting)
                }

        exit ("AbcEncode::doublePool");
        return str;
    }

    function utf8Pool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::utf8Pool ", nesting);

        var str = "undefined";
        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + "'" + v + "'" //+ "// " + i//intConst (nd[i], nesting)
                }

        exit ("AbcEncode::utf8Pool");
        return str;
    }

    function namespacePool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::namespacePool ", nesting);

        var str = "undefined";

        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            // v is an array: kind,nd
            str = str
                + indent (nesting-2)
                + ", "
                + namespaceConst (v, nesting)
                }

        exit ("AbcEncode::namespacePool");
        return str;
    }

    function namespaceConst (nd, nesting : int = 0) : string {
        enter ("AbcEncode::namespaceConst ", nesting);
        var str = "{ 'kind': " + namespaceKind (nd.kind,nesting+"{ 'kind': ".length)
            + indent(nesting) + ", 'utf8': " + utf8(nd.utf8)
            + " }";

        exit ("AbcEncode::abcFile");
        return str;
    }

    function namespaceKind (nd, nesting : int = 0) : string {
        enter ("AbcEncode::namespaceKind ", nesting);
        var str = "";

        switch (nd) {
        case CONSTANT_Namespace:
            str = "'Namespace'";
            break;
        case CONSTANT_PackageNamespace:
            str = "'PackageNamespace'";
            break;
        case CONSTANT_PrivateNamespace:
            str = "'PrivateNamespace'";
            break;
        case CONSTANT_PackageInternalNS:
            str = "'PackageInternalNamespace'";
            break;
        case CONSTANT_ProtectedNamespace:
            str = "'ProtectedNamespace'";
            break;
        case CONSTANT_StaticProtectedNS:
            str = "'StaticProtectedNamespace'";
            break;
        default:
            str = "'Other'";
            break;
        }

        exit ("AbcEncode::namespaceKind");
        return str;
    }

    function namespacesetPool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::namespacesetPool ", nesting);

        var str = "undefined";

        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + "[ " + namespacesetConst (v, nesting+"[ ".length)
                + " ]";
        }

        exit ("AbcEncode::namespacesetPool");
        return str;
    }

    function namespacesetConst (nd, nesting : int = 0) : string {
        enter ("AbcEncode::namespacesetConst ", nesting);

        var str = "";
        //        for each (var elt in nd) {
        for (var i=0; i<nd.length; ++i) {
            var elt = nd[i];
            str = str
                + elt
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::namespacesetConst");
        return str;
    }

    function namePool (pool, nesting : int = 0) : string {
        enter ("AbcEncode::namePool ", nesting);

        var str = "undefined";
        //        for each (var v in pool.slice(1)) {   // FIXME esc bug
        for (var p=pool.slice(1), i=0; i<p.length; ++i) {
            var v = p[i]
            str = str
                + indent (nesting-2)
                + ", "
                + nameConst (v, nesting);
        }

        exit ("AbcEncode::namePool");
        return str;
    }

    function nameConst (nd, nesting : int = 0)
        : string {
        enter ("AbcEncode::nameConst ", nesting);

        var str = "";
            
        var kind = nd.kind;

        switch (kind) {
        case CONSTANT_QName:
            str = "{ 'kind': 'QName'"
                + indent(nesting)
                + ", 'ns': " + this.namespace(nd.ns)
                + indent(nesting) 
                + ", 'utf8': " + utf8(nd.utf8)
                + " }";
            break;
        case CONSTANT_RTQName:
            str = "{ 'kind': 'RTQName'"
                + indent(nesting) 
                + ", 'utf8': " + utf8(nd.utf8)
                + " }";
            break;
        case CONSTANT_RTQNameL:
            str = "{ 'kind': 'RTQName'"
                + " }";
            break;
        case CONSTANT_Multiname:
            str = "{ 'kind': 'Multiname'"
                + indent(nesting) 
                + ", 'utf8': " + utf8(nd.utf8)
                + indent(nesting) 
                + ", 'nsset': " + nd.nsset
                + " }";
            break;
        case CONSTANT_MultinameL:
            str = "{ 'kind': 'MultinameL'"
                + indent(nesting) 
                + ", 'nss': " + nd.nss
                + " }";
            break;
        case CONSTANT_QNameA:
        case CONSTANT_RTQNameA:
        case CONSTANT_RTQNameLA:
        case CONSTANT_MultinameA:
        case CONSTANT_MultinameLA:
        default:
            str = "Unsupported Name constant";
            break;
        }

        exit ("AbcEncode::nameConst");
        return str;
    }

    function methodInfos (nd, nesting : int = 0) : string {
        enter ("AbcEncode::methodInfos ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + methodInfo (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::methodInfos");
        return str;
    }

    function methodInfo (nd, nesting: int = 0) : string {
        enter ("AbcEncode::methodInfo");

        var str =
            "{ 'ret_type': " + name(nd.return_type, nesting+"{ 'ret_type': ".length)
            + indent(nesting) + ", 'param_types': [" + names(nd.param_types, nesting+", 'param_types': [".length) + "]"
            + indent(nesting) + ", 'name': " + utf8(nd.name, nesting+", 'name': ".length)
            + indent(nesting) + ", 'flags': " + nd.flags
            + indent(nesting) + ", 'optional_count': " + (nd.options != null ? nd.options.length : 0)
            + indent(nesting) + ", 'optionals': [" + optionals(nd.options, nesting+", 'optionals': [".length) + "]"
            + indent(nesting) + ", 'param_names': [ " + "" + " ]"
            + " }";

        exit ("AbcEncode:methodInfo ",str);
        return str;
    }
        
    function optionals(nd, nesting: int = 0) : string {
        enter("AbcEncode::optionals");
            
        var str = "";
        if( nd ) {
            for( var i:uint = 0, limit:uint = nd.length; i < limit; ++i ) {
                str = str
                    + optional(nd[i], nesting)
                    + indent(nesting-2)
                    + ", ";
            }
        }
        exit("AbcEncode::optionals ", str);
        return str;
    }
        
    function optional(nd, nesting: int = 0) : string {
        enter("AbcEncode::optional");
        var str = "";
            
        var val = "";
        var kind;
            
        switch(nd.kind) {
        case CONSTANT_Integer:
            val = integer(nd.val, nesting);
            kind = "'CONSTANT_Integer'";
            break;
        case CONSTANT_UInt:
            val = uinteger(nd.val, nesting);
            kind = "'CONSTANT_UInt'";
            break;
        case CONSTANT_Utf8:
            val = utf8(nd.val, nesting);
            kind = "'CONSTANT_Utf8'";
            break;
        case CONSTANT_Namespace:
            val = this.namespace(nd.val, nesting);
            kind = "'CONSTANT_Namespace'";
            break;
        case CONSTANT_Double:
            val = number(nd.val, nesting);
            kind = "'CONSTANT_Double'";
            break;
        case CONSTANT_Null:
            val = 0;
            kind = "'CONSTANT_Null'";
            break;
        case 0:
            val = 0;
            kind = 0;
            break;
        case CONSTANT_True:
            val = 0;
            kind = "'CONSTANT_True'";
            break;
        case CONSTANT_False:
            val = 0;
            kind = "'CONSTANT_False'";
            break;
        }
        str = str
            + "{ 'val': " + val
            + indent(nesting) + ", 'kind': " + kind + " }";
            
        exit("AbcEncode::optional ", str);
        return str;
                
    }

    function metadataInfos (nd, nesting : int = 0) : string {
        enter ("AbcEncode::metadataInfos ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + metadataInfo (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::metadataInfos");
        return str;
    }

    function metadataInfo (nd, nesting: int = 0) : string {
        enter ("AbcEncode::metadataInfo");

        var str =
            "{ 'name': " + utf8(nd.name)
            + indent(nesting) + ", 'items': [" + metadataItems(nd.items, nesting+", 'items': [".length) + "]"
            + " }";

        exit ("AbcEncode:metadataInfo ",str);
        return str;
    }

    function metadataItems(nd, nesting: int = 0) : string {
        enter("AbcEncode::metadataItems");
        var str = "";
            
        for(var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + "{ 'key':" + utf8(nd[i].key) + " 'value':"+utf8(nd[i].value) + " }"
                + indent(nesting-2)
                + ", ";
        }
        exit("AbcEncode::metadataItems");
    }
        
    function instanceInfos (nd, nesting : int = 0) : string {
        enter ("AbcEncode::instanceInfos ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + instanceInfo (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::instanceInfos");
        return str;
    }

    function instanceInfo (nd, nesting: int = 0) : string {
        enter ("AbcEncode::instanceInfo");

        var str =
            "{ 'name': " + name(nd.name, nesting+"{ 'name': ".length)
            + indent(nesting) + ", 'super_name': " + name(nd.super_name, nesting+", 'super_name': ".length)
            + indent(nesting) + ", 'flags': " + nd.flags
            + indent(nesting) + ", 'protected_namespace': " + nd.protectedNS
            + indent(nesting) + ", 'interface_count': " + nd.interfaces.length
            + indent(nesting) + ", 'interfaces': [" + names(nd.interfaces, nesting+", 'interfaces': [".length) + "]"
            + indent(nesting) + ", 'iinit': " + nd.iinit
            + indent(nesting) + ", 'traits': [ " + traits(nd.traits, nesting+", 'traits': { ".length) + " ]"
            + " }";

        exit ("AbcEncode:instanceInfo ",str);
        return str;
    }

    function classInfos (nd, nesting : int = 0) : string {
        enter ("AbcEncode::classInfos ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + classInfo (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::classInfos");
        return str;
    }

    function classInfo (nd, nesting: int = 0) : string {
        enter ("AbcEncode::classInfo");

        var str =
            "{ 'cinit': " + nd.cinit
            + indent(nesting) + ", 'traits': [ " + traits(nd.traits, nesting+", 'traits': { ".length) + " ]"
            + " }";

        exit ("AbcEncode:classInfo ",str);
        return str;
    }

    function scriptInfos (nd, nesting : int = 0) : string {
        enter ("AbcEncode::scriptInfos ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + scriptInfo (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::scriptInfos");
        return str;
    }

    function scriptInfo (nd, nesting: int = 0) : string {
        enter ("AbcEncode::scriptInfo");

        var str =
            "{ 'init': " + nd.init
            + indent(nesting) + ", 'traits': [ " + traits(nd.traits, nesting+", 'traits': { ".length) + " ]" 
            + " }";

        exit ("AbcEncode:scriptInfo ",str);
        return str;
    }
        
    function names(nd, nesting : int = 0) : string {
        enter ("AbcEncode::names ", nesting);
            
        var str = "";
            
        for(var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + name(nd[i], nesting)
                + indent(nesting-2)
                + ", ";
        }
            
        exit ("AbcEncode::names ", nesting);
            
        return str;
    }

    function methodBodys (nd, nesting : int = 0) : string {
        enter ("AbcEncode::methodBodys ", nesting);

        var str = "";

        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + methodBody (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::methodBodys");
        return str;
    }

    function methodBody (nd, nesting: int = 0) : string {
        enter ("AbcEncode::methodBody");

        var str =
            "{ 'method_info': " + nd.method
            + indent(nesting) + ", 'max_stack': " + nd.max_stack
            + indent(nesting) + ", 'max_regs': " + nd.local_count
            + indent(nesting) + ", 'scope_depth': " + nd.init_scope_depth
            + indent(nesting) + ", 'max_scope': " + nd.max_scope_depth
            + indent(nesting) + ", 'code': [ " + code (nd.code, nesting+", 'code': [ ".length) + " ]"
            + indent(nesting) + ", 'exceptions': [ " + "" + " ]"
            + indent(nesting) + ", 'fixtures': [ " + "" + " ]"
            + indent(nesting) + ", 'traits': [ " + traits(nd.traits, nesting+", 'traits': { ".length) + " ]"
            + " }";

        exit ("AbcEncode:methodBody ",str);
        return str;
    }

    function traits(nd, nesting : int = 0) : string {
            
        var str = "";
        for (var i = 0, len = nd.length; i < len; ++i) {
            str = str
                + trait (nd[i], nesting)
                + indent (nesting-2)
                + ", ";
        }
        return str;
    }
        
    function trait(nd, nesting : int = 0) : string {
            
        var kind = (nd.kind&0x0F);
        var str = 
            "{ 'name': " + name(nd.name, nesting+"{ 'name': ".length)
            + indent(nesting) + ", 'kind': " + slotKind(nd.kind&0x0F)
            + indent(nesting) + ", 'attrs': " + attrs((nd.kind>>4)&0x0F);
                
        switch(kind) {
        case TRAIT_Slot:
        case TRAIT_Const:
            str =  str + indent(nesting) + ", 'slot_id': " + nd.slot_id
                +  indent(nesting) + ", 'type_name': " + name(nd.type_name, nesting+", 'type_name': ".length)
                +  indent(nesting) + ", 'val_index': " + nd.vindex;
            if( nd.vindex != 0 )
                str = str + indent(nesting) + ", 'val_kind': " + nd.vkind;
            break;
        case TRAIT_Method:
        case TRAIT_Getter:
        case TRAIT_Setter:
            str =  str + indent(nesting) + ", 'disp_id': " + nd.id
                +  indent(nesting) + ", 'method': " + nd.val;
            break;
        case TRAIT_Class:
            str =  str + indent(nesting) + ", 'slot_id': " + nd.id
                +  indent(nesting) + ", 'class': " + nd.val;
            break;
        }
            
        str = str + " }";        
            
        return str;
    }
        
    function slotKind(kind:uint) : string {
        return kind;
    }
        
    function attrs(a:uint) :string {
        return a;
    }
        
    function code (nd, nesting : int = 0) : string {
        enter ("AbcEncode::code ", nesting);

        var str = "";
        var bytes = nd;
        bytes.position = 0;

        //        for (var ip = 0, len = bytes.length; ip < len; ip=ip+width)
        while (bytes.bytesAvailable > 0) {
            var [bs,width] = instruction ();
            str = str
                + bs
                + indent (nesting-2)
                + ", ";
        }

        exit ("AbcEncode::code");
        return str;

        function instruction () : [int,string] {
            enter ("AbcEncode::instruction ");
                
            var str = "";
            var op = bytes.readByte();
            var width = opcodes [op][1];
            switch (width) {
            case 0:
                str = "invalid opcode " + opcodes[op][0];
                width = 1;
                break;
            case 1:
                str = "[ '" + opcodes[op][0] + "' ]";
            default:
                str = "[ '" + opcodes[op][0] + "'" + operands (nesting + 4 + opcodes[op][0].length) + " ]";
                break;
            }
                
            exit ("AbcEncode::code");
            return [str,width];
            
            function operands (nesting:int = 0) {
                var s = "";

                switch(op) {
                case OP_debugfile:
                case OP_pushstring:
                    s = s + ", " + utf8(bytes.readU32(), nesting+2);// + 
                    break;
                case OP_pushnamespace:
                    s = s + ", " + this.namespace(bytes.readU32(), nesting+2);
                    break;
                case OP_pushint:
                    s = s + ", " + integer(bytes.readU32(), nesting+2);
                    break;
                case OP_pushuint:
                    s = s + ", " + uinteger(bytes.readU32(), nesting+2);
                    break;
                case OP_pushdouble:
                    s = s + ", " + number(bytes.readU32(), nesting+2);
                    break;
                case OP_getsuper: 
                case OP_setsuper: 
                case OP_getproperty: 
                case OP_initproperty: 
                case OP_setproperty: 
                case OP_getlex: 
                case OP_findpropstrict: 
                case OP_findproperty:
                case OP_getouterscope:
                case OP_finddef:
                case OP_deleteproperty: 
                case OP_istype: 
                case OP_coerce: 
                case OP_astype: 
                case OP_getdescendants:
                    s = s + ", " + name(bytes.readU32(), nesting+2);
                    break;
                case OP_constructprop:
                case OP_callproperty:
                case OP_callproplex:
                case OP_callsuper:
                case OP_callsupervoid:
                case OP_callpropvoid:
                    s = s + ", " + name(bytes.readU32(), nesting+2);
                    s = s + ", " + bytes.readU32();
                    break;
                case OP_newfunction: {
                    s = s + ", " + method(bytes.readU32(), nesting+2);
                    break;
                }
                case OP_callstatic:
                    s = s + ", " + method(bytes.readU32(), nesting+2);
                    s = s + ", " + bytes.readU32();
                    break;
                case OP_newclass: 
                    s = s + ", " + clas(bytes.readU32(), nesting+2);
                    break;
                case OP_lookupswitch:
                    var pos = bytes.position-1;
                    s = s + ", " + bytes.readS24();
                    //var target = pos + readS24()
                    var maxindex = bytes.readU32();
                    //s += "default:" + labels.labelFor(target) // target + "("+(target-pos)+")"
                    //s += " maxcase:" + maxindex
                    s = s + ", " + maxindex;
                    for (var i:int=0; i <= maxindex; i++) {
                        //target = pos + readS24();
                        s = s +  ", " + bytes.readS24();
                        //labels.labelFor(target) // target + "("+(target-pos)+")"
                    }
                    break;
                case OP_jump:
                case OP_iftrue:		case OP_iffalse:
                case OP_ifeq:		case OP_ifne:
                case OP_ifge:		case OP_ifnge:
                case OP_ifgt:		case OP_ifngt:
                case OP_ifle:		case OP_ifnle:
                case OP_iflt:		case OP_ifnlt:
                case OP_ifstricteq:	case OP_ifstrictne:
                    var offset = bytes.readS24();
                    //var target = code.position+offset
                    //s += target + " ("+offset+")"
                    s = s + ", " + offset;//labels.labelFor(target)
                    //if (!((code.position) in labels))
                    //	s += "\n"
                    break;
                case OP_inclocal:
                case OP_declocal:
                case OP_inclocal_i:
                case OP_declocal_i:
                case OP_getlocal:
                case OP_kill:
                case OP_setlocal:
                case OP_debugline:
                case OP_getglobalslot:
                case OP_getslot:
                case OP_setglobalslot:
                case OP_setslot:
                case OP_pushshort:
                case OP_newcatch:
                case OP_newobject:
                case OP_newarray:
                    s = s + ", " + bytes.readU32();
                    break;
                case OP_debug:
                    s = s + ", " + bytes.readByte(); 
                    s = s + ", " + bytes.readU32();
                    s = s + ", " + bytes.readByte();
                    s = s + ", " + bytes.readU32();
                    break;
                case OP_call:
                case OP_construct:
                case OP_constructsuper:
                    s = s + ", " + bytes.readU32();
                    break;
                case OP_pushbyte:
                case OP_getscopeobject:
                    s = s + ", " + bytes.readByte();
                    break;
                case OP_hasnext2: {
                    let b1 = bytes.readU32();
                    let b2 = bytes.readU32();
                    s = s + ", " + b1 + ", " + b2;
                    break;
                }
                default:
                    break;
                }

                return s;
            }
        }
    }

    function utf8(index : int , nesting : int = 0) : string {
        var str = index;
        if( verbose ) { 
            let utf8_pool = abc.constants.utf8_pool;
            str = "'" + constants.getUtf8(index) + "'";
            //str = str.replace(/\n/g,"\\n");
            //str = str.replace(/\t/g,"\\t"); 
        }
        return str;
    }
        
    function namespace(index : int, nesting : int = 0) : string {
        var str = index;
        if( verbose ) {
            let val = constants.namespace_pool[index];
            str = "{ 'utf8': " + utf8(val.utf8) + ", 'kind': " + namespaceKind(val.kind) + " }";
        }
        return str;
    }
        
    function integer(index: int, nesting : int = 0 ) : string {
        var str = index;
        if( verbose ) {
            str = "'" + constants.getInt(index) + "'";
        }
        return str;
    }

    function uinteger(index: int, nesting : int = 0 ) : string {
        var str = index;
        if( verbose ) {
            str = "'" + constants.getUint(index) + "'";
        }
        return str;
    }

    function number(index: int, nesting : int = 0 ) : string {
        var str = index;
        if( verbose ) {
            str = "'" + constants.getDouble(index) + "'";
        }
        return str;
    }
        
    function name(index: int, nesting : int = 0 ) : string {
        var str = index;
            
        if( verbose && index > 0) {
            let name = constants.name_pool[index];
            switch(name.kind) {
            case CONSTANT_QName:
                str = "{ 'kind': 'QName'"
                    + indent(nesting) + ", 'utf8': " + utf8(name.utf8) 
                    + indent(nesting) + ", 'ns': " + this.namespace(name.ns) + " }";
                break;
            case CONSTANT_RTQName:
                str = "{ 'kind': 'RTQName'"
                    + indent(nesting) +  ", utf8': " + utf8(name.utf8) + " }";
                break;
            case CONSTANT_RTQNameL:
                str = "{ 'kind': 'RTQNameL' }";
                break;
            case CONSTANT_Multiname:
                str = "{ 'kind': 'Multiname'"
                    + indent(nesting) + ", 'utf8': " + utf8(name.utf8) 
                    + indent(nesting) + ", 'nsset': " + name.ns + " }";
                break;
            case CONSTANT_MultinameL:
                str = "{ 'kind': 'MultinameL'" + ", 'nss': " + name.nss + " }";
                break;
            }
        }
        return str;
    }
        
    function method(index: int, nesting : int = 0 ) : string {
        var str = index;
        if( verbose ) {
        }
        return str;
    }

    function clas(index: int, nesting : int = 0 ) : string {
        var str = index;
        if( verbose ) {
        }
        return str;
    }
}

// negative widths mean variable length. -2 for each variable length operand. zero width
// means no implemented or deprecated and should never be used.

var opcodes = [ ["OP_0x00", 0]
                , ["bkpt", 1]
                , ["nop", 1]
                , ["throw", 1]
                , ["getsuper", 1-2]
                , ["setsuper", 1-2]
                , ["dxns", 1-2]
                , ["dxnslate", 1]
                , ["kill", 1-2]
                , ["label", 1]
                , ["OP_0x0A", 0]
                , ["OP_0x0B", 0]
                , ["ifnlt", 1+3]
                , ["ifnle", 1+3]
                , ["ifngt", 1+3]
                , ["ifnge", 1+3]
                , ["jump", 1+3]
                , ["iftrue", 1+3]
                , ["iffalse", 1+3]
                , ["ifeq", 1+3]
                , ["ifne", 1+3]
                , ["iflt", 1+3]
                , ["ifle", 1+3]
                , ["ifgt", 1+3]
                , ["ifge", 1+3]
                , ["ifstricteq", 1+3]
                , ["ifstrictne", 1+3]
                , ["lookupswitch", 1+3+2+0]
                , ["pushwith", 1]
                , ["popscope", 1]
                , ["nextname", 1]
                , ["hasnext", 1]
                , ["pushnull", 1]
                , ["pushundefined", 1]
                , ["OP_0x22", 0]
                , ["nextvalue", 1]
                , ["pushbyte", 1+1]
                , ["pushshort", 1-2]
                , ["pushtrue", 1]
                , ["pushfalse", 1]
                , ["pushnan", 1]
                , ["pop", 1]
                , ["dup", 1]
                , ["swap", 1]
                , ["pushstring", 1-2]
                , ["pushint", 1-2]
                , ["pushuint", 1-2]
                , ["pushdouble", 1-2]
                , ["pushscope", 1]
                , ["pushnamespace", 1-2]
                , ["hasnext2", 1]
                , ["OP_0x33", 0]
                , ["OP_0x34", 0]
                , ["OP_0x35", 0]
                , ["OP_0x36", 0]
                , ["OP_0x37", 0]
                , ["OP_0x38", 0]
                , ["OP_0x39", 0]
                , ["OP_0x3A", 0]
                , ["OP_0x3B", 0]
                , ["OP_0x3C", 0]
                , ["OP_0x3D", 0]
                , ["OP_0x3E", 0]
                , ["OP_0x3F", 0]
                , ["newfunction", 1-2]
                , ["call", 1-2]
                , ["construct", 1-2]
                , ["callmethod", 1-2-2]
                , ["callstatic", 1-2-2]
                , ["callsuper", 1-2-2]
                , ["callproperty", 1-2-2]
                , ["returnvoid", 1]
                , ["returnvalue", 1]
                , ["constructsuper", 1-2]
                , ["constructprop", 1-2-2]
                , ["callsuperid", 0]
                , ["callproplex", 1-2-2]
                , ["callinterface", 0]
                , ["callsupervoid", 1-2-2]
                , ["callpropvoid", 1-2-2]
                , ["OP_0x50", 0]
                , ["OP_0x51", 0]
                , ["OP_0x52", 0]
                , ["OP_0x53", 0]
                , ["OP_0x54", 0]
                , ["newobject", 1-2]
                , ["newarray", 1-2]
                , ["newactivation", 1]
                , ["newclass", 1-2]
                , ["getdescendants", 1-2]
                , ["newcatch", 1-2]
                , ["OP_0x5B", 0]
                , ["OP_0x5C", 0]
                , ["findpropstrict", 1-2]  // width depends on size of index
                , ["findproperty", 1-2]
                , ["finddef", 1-2]
                , ["getlex", 1-2]
                , ["setproperty", 1-2]
                , ["getlocal", 1-2]
                , ["setlocal", 1-2]
                , ["getglobalscope", 1]
                , ["getscopeobject", 1-2]
                , ["getproperty", 1-2]
                , ["getouterscope", 1-2]
                , ["initproperty", 1-2]
                , ["OP_0x69", 0]
                , ["deleteproperty", 1-2]
                , ["OP_0x6B", 0]
                , ["getslot", 1-2]
                , ["setslot", 1-2]
                , ["getglobalslot", 0]
                , ["setglobalslot", 0]
                , ["convert_s", 1]
                , ["esc_xelem", 1]
                , ["esc_xattr", 1]
                , ["convert_i", 1]
                , ["convert_u", 1]
                , ["convert_d", 1]
                , ["convert_b", 1]
                , ["convert_o", 1]
                , ["checkfilter", 1]
                , ["OP_0x79", 0]
                , ["OP_0x7A", 0]
                , ["OP_0x7B", 0]
                , ["OP_0x7C", 0]
                , ["OP_0x7D", 0]
                , ["OP_0x7E", 0]
                , ["OP_0x7F", 0]
                , ["coerce", 1-2]
                , ["coerce_b", 0]
                , ["coerce_a", 1]
                , ["coerce_i", 0]
                , ["coerce_d", 0]
                , ["coerce_s", 1]
                , ["astype", 1-2]
                , ["astypelate", 1]
                , ["coerce_u", 0]
                , ["coerce_o", 1]
                , ["OP_0x8A", 0]
                , ["OP_0x8B", 0]
                , ["OP_0x8C", 0]
                , ["OP_0x8D", 0]
                , ["OP_0x8E", 0]
                , ["OP_0x8F", 0]
                , ["negate", 1]
                , ["increment", 1]
                , ["inclocal", 1-2]
                , ["decrement", 1]
                , ["declocal", 1-2]
                , ["typeof", 1]
                , ["not", 1]
                , ["bitnot", 1]
                , ["OP_0x98", 0]
                , ["OP_0x99", 0]
                , ["concat", 0]
                , ["add_d", 0]
                , ["OP_0x9C", 0]
                , ["OP_0x9D", 0]
                , ["OP_0x9E", 0]
                , ["OP_0x9F", 0]
                , ["add", 1]
                , ["subtract", 1]
                , ["multiply", 1]
                , ["divide", 1]
                , ["modulo", 1]
                , ["lshift", 1]
                , ["rshift", 1]
                , ["urshift", 1]
                , ["bitand", 1]
                , ["bitor", 1]
                , ["bitxor", 1]
                , ["equals", 1]
                , ["strictequals", 1]
                , ["lessthan", 1]
                , ["lessequals", 1]
                , ["greaterthan", 1]
                , ["greaterequals", 1]
                , ["instanceof", 1]
                , ["istype", 1-2]
                , ["istypelate", 1]
                , ["in", 1]
                , ["OP_0xB5", 0]
                , ["OP_0xB6", 0]
                , ["OP_0xB7", 0]
                , ["OP_0xB8", 0]
                , ["OP_0xB9", 0]
                , ["OP_0xBA", 0]
                , ["OP_0xBB", 0]
                , ["OP_0xBC", 0]
                , ["OP_0xBD", 0]
                , ["OP_0xBE", 0]
                , ["OP_0xBF", 0]
                , ["increment_i", 1]
                , ["decrement_i", 1]
                , ["inclocal_i", 1-2]
                , ["declocal_i", 1-2]
                , ["negate_i", 1]
                , ["add_i", 1]
                , ["subtract_i", 1]
                , ["multiply_i", 1]
                , ["OP_0xC8", 0]
                , ["OP_0xC9", 0]
                , ["OP_0xCA", 0]
                , ["OP_0xCB", 0]
                , ["OP_0xCC", 0]
                , ["OP_0xCD", 0]
                , ["OP_0xCE", 0]
                , ["OP_0xCF", 0]
                , ["getlocal0", 1]
                , ["getlocal1", 1]
                , ["getlocal2", 1]
                , ["getlocal3", 1]
                , ["setlocal0", 1]
                , ["setlocal1", 1]
                , ["setlocal2", 1]
                , ["setlocal3", 1]
                , ["OP_0xD8", 0]
                , ["OP_0xD9", 0]
                , ["OP_0xDA", 0]
                , ["OP_0xDB", 0]
                , ["OP_0xDC", 0]
                , ["OP_0xDD", 0]
                , ["OP_0xDE", 0]
                , ["OP_0xDF", 0]
                , ["OP_0xE0", 0]
                , ["OP_0xE1", 0]
                , ["OP_0xE2", 0]
                , ["OP_0xE3", 0]
                , ["OP_0xE4", 0]
                , ["OP_0xE5", 0]
                , ["OP_0xE6", 0]
                , ["OP_0xE7", 0]
                , ["OP_0xE8", 0]
                , ["OP_0xE9", 0]
                , ["OP_0xEA", 0]
                , ["OP_0xEB", 0]
                , ["OP_0xEC", 0]
                , ["OP_0xED", 0]
                , ["abs_jump", 0]
                , ["debug", 1-2-2-2-2]  // this isn't quite right, some operands are unsigned bytes
                , ["debugline", 1-2]
                , ["debugfile", 1-2]
                , ["bkptline", 1-2]
                , ["timestamp", 1]
                , ["OP_0xF4", 0]
                , ["verifypass", 0]
                , ["alloc", 0]
                , ["mark", 0]
                , ["wb", 0]
                , ["prologue", 0]
                , ["sendenter", 0]
                , ["doubletoatom", 0]
                , ["sweep", 0]
                , ["codegenop", 0]
                , ["verifyop", 0]
                , ["decode", 0]
                , ];
    
var slotKinds = [ "TRAIT_Slot"
                  , "TRAIT_Method"
                  , "TRAIT_Getter"
                  , "TRAIT_Setter"
                  , "TRAIT_Class"
                  , "TRAIT_Function"
                  , "TRAIT_Const"
                  , ];

