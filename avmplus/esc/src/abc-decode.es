/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace AbcDecode;
use default namespace AbcDecode,
    namespace AbcDecode;

use namespace Release;
use namespace Ast;
use namespace Abc;
use namespace Asm;

function abcFile (asm) : ABCFile {
    enter ("AbcDecode::abcFile");
    var decoder = new AbcDecoder(asm);
    var abc = decoder.decode();
    exit("AbcDecode::abcFile");
    return abc;
}

class AbcDecoder {
    var asm;
    var abc;
    function AbcDecoder(a) 
        : asm=a { }
        
    function decode() :ABCFile {
            
        enter ("AbcDecoder::decode");

        if( asm.abc_class == undefined )
            throw "AbcDecoder::abcFile, not a valid asm";
            
        abc = new ABCFile;
            
        abc.addConstants( constantPool(asm) );
            
        var i;
        var n;
            
        for(i = 0, n = asm.method_infos.length; i < n; ++i ) {
            abc.addMethod(methodInfo(asm.method_infos[i]));
        }

        for(i = 0, n = asm.metadata_infos.length; i < n; ++i ) {
            abc.addMetadata(metadataInfo(asm.metadata_infos[i]));
        }
            
        for(i = 0, n = asm.instance_infos.length; i < n; ++i ) {
            abc.addInstance(instanceInfo(asm.instance_infos[i]));
        }

        for(i = 0, n = asm.class_infos.length; i < n; ++i ) {
            abc.addClass(classInfo(asm.class_infos[i]));
        }

        for(i = 0, n = asm.script_infos.length; i < n; ++i ) {
            abc.addScript(scriptInfo(asm.script_infos[i]));
        }

        for(i = 0, n = asm.method_bodys.length; i < n; ++i ) {
            abc.addMethodBody(methodBodyInfo(asm.method_bodys[i]));
        }

        exit ("AbcDecoder::abcFile");
        return abc;
    }

    function constantPool(asm) : ABCConstantPool {
            
        var cpool = new ABCConstantPool;
            
        if(asm.int_pool == undefined)
            throw "AbcDecoder::constantPool, not a valid asm - missing int_pool";

        var i;
        var n;
            
        for( i=1, n=asm.int_pool.length; i < n; ++i)
            cpool.int32(asm.int_pool[i]);

        for( i=1, n=asm.uint_pool.length; i < n; ++i)
            cpool.uint32(asm.uint_pool[i]);
                
        for( i=1, n=asm.double_pool.length; i < n; ++i)
            cpool.float64(asm.double_pool[i]);

        for( i=1, n=asm.utf8_pool.length; i < n; ++i)
            cpool.stringUtf8(asm.utf8_pool[i]);
            
        for( i=1, n=asm.namespace_pool.length; i < n; ++i) {
            let ns = asm.namespace_pool[i];
            let nskind = namespaceKind(ns.kind);
            let uri = ns.utf8;
                
            cpool.namespace(nskind, uri);
        }
            
        for( i=1, n=asm.nsset_pool.length; i < n; ++i) {
            let entry = asm.nsset_pool[i];
            let nsset = [];
            let count = entry.length;
            for(let j=0; j < count; ++j)
                nsset[j] = entry[j];
                
            cpool.namespaceset(nsset);
        }
            
        for (i=1, n=asm.name_pool.length; i < n; i++) {
            let name = asm.name_pool[i];
            var kind = name.kind;
            switch (kind) {
            case "QName":
            case "QNameA":
                cpool.QName(name.ns, name.utf8, kind=="QNameA");
                break;
                
            case "RTQName":
            case "RTQNameA":
                cpool.RTQName(name.utf8, kind=="RTQNameA");
                break;
                
            case "RTQNameL":
            case "RTQNameLA":
                cpool.RTQNameL(kind=="RTQNameLA");
                break;
                
            case "Multiname":
            case "MultinameA":
                cpool.Multiname(name.nsset, name.utf8, kind=="MultinameA");
                break;

            case "MultinameL":
            case "MultinameLA":
                cpool.MultinameL(name.nsset, kind=="MultinameLA");
                break;
                
            default:
                throw "AbcDecode::abcConstanPool, unknown name type " + kind;
            }
        }
            
        return cpool;
            
    }
        
    function namespaceKind(nskind) : uint {
        switch (nskind) {
        case 'Namespace':
            return CONSTANT_Namespace;
        case 'PackageNamespace':
            return CONSTANT_PackageNamespace;
        case 'PrivateNamespace':
            return CONSTANT_PrivateNamespace;
        case 'PackageInternalNamespace':
            return CONSTANT_PackageInternalNS;
        case 'ProtectedNamespace':
            return CONSTANT_ProtectedNamespace;
        case 'StaticProtectedNamespace':
            return CONSTANT_StaticProtectedNS;
        default:
            throw "AbcDecode::namespaceKind, unknown namespace kind " + nskind;
        }
    }
        
    function methodInfo(mi) : ABCMethodInfo {
        return new ABCMethodInfo(name(mi.name), names(mi.param_types), name(mi.ret_type), mi.flags, optionals(mi.optionals), mi.paramnames);
    }

    function optionals(ops) : [{val:uint, kind:uint}] {
        var optionals = [];
            
        if(ops.length == 0 )
            return null;
            
        for(var i:uint = 0, limit:uint = ops.length; i < limit; ++i ) {
            optionals.push(optional(ops[i]));
        }
        return optionals;
    }
    function optional(op) : {val:uint, kind:uint} {
        switch(op.kind) {
        case "CONSTANT_Integer":
            val = integer(op.val);
            kind = CONSTANT_Integer;
            break;
        case "CONSTANT_UInt":
            val = uinteger(op.val);
            kind = CONSTANT_UInt;
            break;
        case "CONSTANT_Utf8":
            val = utf8(op.val);
            kind = CONSTANT_Utf8;
            break;
        case "CONSTANT_Namespace":
            val = this.namespace(op.val);
            kind = CONSTANT_Namespace;
            break;
        case "CONSTANT_Double":
            val = number(op.val);
            kind = CONSTANT_Double;
            break;
        case "CONSTANT_Null":
            val = 0;
            kind = CONSTANT_Null;
            break;
        case "CONSTANT_True":
            val = 0;
            kind = CONSTANT_True;
            break;
        case "CONSTANT_False":
            val = 0;
            kind = CONSTANT_False;
            break;
        default:
            val = 0;
            kind =0;
            break;
        }
        return{ val:val, kind:kind};
    }

    function metadataInfo(mi) : ABCMethodInfo {
            
        //TODO: implement
        return null;
    }
        
    function instanceInfo(ii) : ABCInstanceInfo {
            
        var instance = new ABCInstanceInfo(name(ii.name), name(ii.super_name), ii.flags, ii.protected_namespace, ii.interfaces);
            
        instance.setIInit(ii.iinit);
        traits(instance, ii.traits);
        return instance;
    }
        
    function classInfo(ci) : ABCClassInfo {
        var clas = new ABCClassInfo();
        clas.setCInit(ci.cinit);
        traits(clas, ci.traits);
        return clas;
    }
        
    function scriptInfo(si) : ABCScriptInfo {
        var script = new ABCScriptInfo(si.init);
        traits(script, si.traits);
        return script;
    }
        
    function methodBodyInfo(mb) : ABCMethodBodyInfo {
            
        var method_body = new ABCMethodBodyInfo(mb.method_info);
            
        method_body.max_stack = mb.max_stack;
        method_body.local_count = mb.max_regs;
        method_body.max_scope_depth = mb.max_scope;
        method_body.init_scope_depth = mb.scope_depth;
            
        method_body.code = code(mb.code);
            
        traits(method_body, mb.traits);
            
        return method_body;
    }
        
    function traits(target, t) {
        for( var i = 0, n = t.length; i < n; ++i ) {
            if( t[i] != undefined)
                target.addTrait(trait(t[i]));
        }
    }
        
    function trait(t) {  //Should return ABCTrait once inheritance is supported
        
        var trait = null;
        switch(t.kind) {
        case TRAIT_Slot:
        case TRAIT_Const:
            trait = new ABCSlotTrait(name(t.name), t.attrs, t.kind==TRAIT_Const, t.slot_id, name(t.type_name), t.val_index, t.val_kind);
            break;
        case TRAIT_Method:
        case TRAIT_Getter:
        case TRAIT_Setter:
            trait = new ABCOtherTrait(name(t.name), t.attrs, t.kind, t.disp_id, t.method);
            break;
        case TRAIT_Class:
            trait = new ABCOtherTrait(name(t.name), t.attrs, t.kind, t.slot_id, t["class"]);
            break;
        }
        return trait;
    }
        
    function utf8(s) : uint {
        if(s is Number)
            return uint(s);
        return abc.constants.stringUtf8(s);
    }

    function namespace(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            var kind = namespaceKind(s.kind);
            var str = utf8(s.utf8);
            return abc.constants.namespace(kind, str);
        }
    }

    function integer(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            return abc.constants.int32(s);
        }
    }
    function uinteger(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            return abc.constants.uint32(s);
        }
    }
    function number(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            return abc.constants.float64(s);
        }
    }

    function clas(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            // find cpool int entry
            return -1;
        }
    }
    function method(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            // find cpool uint entry
            return -1;
        }
    }
        
    function names(n) {
        var names = [];
        for(var i = 0; i < n.length; ++i ) {
            names.push(name(n[i]));
        }
        return names;
    }
        
    function name(s) : uint {
        if(s is Number){
            return uint(s);
        }
        else {
            switch(s.kind) {
            case "QName":
                var ns = this.namespace(s.ns);
                var name = utf8(s.utf8);
                return abc.constants.QName(ns, name, false);
            case "RTQName":
                var name = utf8(s.utf8);
                return abc.constants.RTQName(name, false);
            case "RTQNameL":
                return abc.constants.RTQNameL(false);
            case "Multiname":
                var name = utf8(s.utf8);
                var nss = namespaceset(s.nsset);
                return abc.constants.Multiname(nss, name, false);
            case "MultinameL":
                var nss = namespaceset(s.nss);
                return abc.constants.MulinameL(nss, false);
            }
            // find cpool number entry
            return -1;
        }
    }
        
    function namespaceset(s) : uint {
        if( s is Number) {
            return uint(s);
        }
        else {
        }
                
    }

    function code(code) {
        var bytes = new ABCByteStream;
            
        for( var i = 0, n = code.length; i < n; ++i) {
            let ins = code[i];
            let op = opcode(ins[0]);
            bytes.uint8(op);
            switch(op) {
            case OP_debugfile:
            case OP_pushstring:
                bytes.uint30(utf8(ins[1]));
                break;
            case OP_pushnamespace:
                bytes.uint30(this.namespace(ins[1]));
                break;
            case OP_pushint:
                bytes.uint30(integer(ins[1]));
                break;
            case OP_pushuint:
                bytes.uint30(uinteger(ins[1]));
                break;
            case OP_pushdouble:
                bytes.uint30(number(ins[1]));
                break;
            case OP_getsuper: 
            case OP_setsuper: 
            case OP_getproperty: 
            case OP_initproperty: 
            case OP_setproperty: 
            case OP_getlex: 
            case OP_findpropstrict: 
            case OP_findproperty:
            case OP_getglobalscope:
            case OP_getouterscope:
            case OP_finddef:
            case OP_deleteproperty: 
            case OP_istype: 
            case OP_coerce: 
            case OP_astype: 
            case OP_getdescendants:
                bytes.uint30(name(ins[1]));
                break;
            case OP_constructprop:
            case OP_callproperty:
            case OP_callproplex:
            case OP_callsuper:
            case OP_callsupervoid:
            case OP_callpropvoid:
                bytes.uint30(name(ins[1]));
                bytes.uint30(ins[2]);
                break;
            case OP_newfunction: {
                bytes.uint30(method(ins[1]));
                break;
            }
            case OP_callstatic:
                bytes.uint30(method(ins[1]));
                bytes.uint30(ins[2]);
                break;
            case OP_newclass: 
                bytes.uint30(clas(ins[1]));
                break;
            case OP_lookupswitch:
                var pos = bytes.position-1;
                bytes.int24(ins[1]);
                //var target = pos + readS24()
                var maxindex = ins[2];
                //s += "default:" + labels.labelFor(target) // target + "("+(target-pos)+")"
                //s += " maxcase:" + maxindex
                bytes.uint30(maxindex);
                for (var i:int=3; i-3 <= maxindex; i++) {
                    //target = pos + readS24();
                    bytes.int24(ins[i]);
                    //labels.labelFor(target) // target + "("+(target-pos)+")"
                }
                break;
            case OP_jump:
            case OP_iftrue:     case OP_iffalse:
            case OP_ifeq:       case OP_ifne:
            case OP_ifge:       case OP_ifnge:
            case OP_ifgt:       case OP_ifngt:
            case OP_ifle:       case OP_ifnle:
            case OP_iflt:       case OP_ifnlt:
            case OP_ifstricteq: case OP_ifstrictne:
                bytes.int24(ins[1]);
                //var target = code.position+offset
                //s += target + " ("+offset+")"
                //s = s + ", " + offset;//labels.labelFor(target)
                //if (!((code.position) in labels))
                //  s += "\n"
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
                bytes.uint30(ins[1]);
                break;
            case OP_debug:
                bytes.uint8(ins[1]);
                bytes.uint30(ins[2]);
                bytes.uint8(ins[3]);
                bytes.uint30(ins[4]);
                break;
            case OP_call:
            case OP_construct:
            case OP_constructsuper:
                bytes.uint30(ins[1]);
                break;
            case OP_pushbyte:
            case OP_getscopeobject:
                bytes.uint8(ins[1]);
                break;
            case OP_hasnext2:
                bytes.uint30(ins[1]);
                bytes.uint30(ins[2]);
            default:
                for( let j = 1, len = ins.length; j < len; ++j ) {
                    bytes.uint8(ins[j]);
                }
                break;
            }
        }
        return bytes;
    }
    //esc doesn't support statics yet
    var ops = new opcodes;
        
    function opcode(op) : uint {
            
        var opcode = ops[op];
        if(opcode == undefined) {
            opcode = -1;
            if( op is Number )
                opcode = uint(op);
        }
        return opcode;
    }
}
        
// Map op names to the actual opcode
dynamic class opcodes
{
    function opcodes() {
        this["throw"] = OP_throw;
        this["instanceof"] = OP_instanceof;
        this["in"] = OP_in;
        this["typeof"] = OP_typeof;
    }
    const bkpt = OP_bkpt;
    const nop = OP_nop;
    //const throw = OP_throw;
    const getsuper = OP_getsuper;
    const setsuper = OP_setsuper;
    const dxns = OP_dxns;
    const dxnslate = OP_dxnslate;
    const kill = OP_kill;
    const label = OP_label;
    const ifnlt = OP_ifnlt;
    const ifnle = OP_ifnle;
    const ifngt = OP_ifngt;
    const ifnge = OP_ifnge;
    const jump = OP_jump;
    const iftrue = OP_iftrue;
    const iffalse = OP_iffalse;
    const ifeq = OP_ifeq;
    const ifne = OP_ifne;
    const iflt = OP_iflt;
    const ifle = OP_ifle;
    const ifgt = OP_ifgt;
    const ifge = OP_ifge;
    const ifstricteq = OP_ifstricteq;
    const ifstrictne = OP_ifstrictne;
    const lookupswitch = OP_lookupswitch;
    const pushwith = OP_pushwith;
    const popscope = OP_popscope;
    const nextname = OP_nextname;
    const hasnext = OP_hasnext;
    const pushnull = OP_pushnull;
    const pushundefined = OP_pushundefined;
    const nextvalue = OP_nextvalue;
    const pushbyte = OP_pushbyte;
    const pushshort = OP_pushshort;
    const pushtrue = OP_pushtrue;
    const pushfalse = OP_pushfalse;
    const pushnan = OP_pushnan;
    const pop = OP_pop;
    const dup = OP_dup;
    const swap = OP_swap;
    const pushstring = OP_pushstring;
    const pushint = OP_pushint;
    const pushuint = OP_pushuint;
    const pushdouble = OP_pushdouble;
    const pushscope = OP_pushscope;
    const pushnamespace = OP_pushnamespace;
    const hasnext2 = OP_hasnext2;
    const newfunction = OP_newfunction;
    const call = OP_call;
    const construct = OP_construct;
    const callmethod = OP_callmethod;
    const callstatic = OP_callstatic;
    const callsuper = OP_callsuper;
    const callproperty = OP_callproperty;
    const returnvoid = OP_returnvoid;
    const returnvalue = OP_returnvalue;
    const constructsuper = OP_constructsuper;
    const constructprop = OP_constructprop;
    const callproplex = OP_callproplex;
    const callsupervoid = OP_callsupervoid;
    const callpropvoid = OP_callpropvoid;
    const newobject = OP_newobject;
    const newarray = OP_newarray;
    const newactivation = OP_newactivation;
    const newclass = OP_newclass;
    const getdescendants = OP_getdescendants;
    const newcatch = OP_newcatch;
    const findpropstrict = OP_findpropstrict;
    const findproperty = OP_findproperty;
    const finddef = OP_finddef;
    const getlex = OP_getlex;
    const setproperty = OP_setproperty;
    const getlocal = OP_getlocal;
    const setlocal = OP_setlocal;
    const getglobalscope = OP_getglobalscope;
    const getscopeobject = OP_getscopeobject;
    const getouterscope = OP_getouterscope;
    const getproperty = OP_getproperty;
    const initproperty = OP_initproperty;
    const deleteproperty = OP_deleteproperty;
    const getslot = OP_getslot;
    const setslot = OP_setslot;
    const convert_s = OP_convert_s;
    const esc_xelem = OP_esc_xelem;
    const esc_xattr = OP_esc_xattr;
    const convert_i = OP_convert_i;
    const convert_u = OP_convert_u;
    const convert_d = OP_convert_d;
    const convert_b = OP_convert_b;
    const convert_o = OP_convert_o;
    const checkfilter = OP_checkfilter;
    const coerce = OP_coerce;
    const coerce_a = OP_coerce_a;
    const coerce_s = OP_coerce_s;
    const astype = OP_astype;
    const astypelate = OP_astypelate;
    const coerce_o = OP_coerce_o;
    const negate = OP_negate;
    const increment = OP_increment;
    const inclocal = OP_inclocal;
    const decrement = OP_decrement;
    const declocal = OP_declocal;
    //const typeof = OP_typeof;
    const not = OP_not;
    const bitnot = OP_bitnot;
    const add = OP_add;
    const subtract = OP_subtract;
    const multiply = OP_multiply;
    const divide = OP_divide;
    const modulo = OP_modulo;
    const lshift = OP_lshift;
    const rshift = OP_rshift;
    const urshift = OP_urshift;
    const bitand = OP_bitand;
    const bitor = OP_bitor;
    const bitxor = OP_bitxor;
    const equals = OP_equals;
    const strictequals = OP_strictequals;
    const lessthan = OP_lessthan;
    const lessequals = OP_lessequals;
    const greaterthan = OP_greaterthan;
    const greaterequals = OP_greaterequals;
    //const instanceof = OP_instanceof;
    const istype = OP_istype;
    const istypelate = OP_istypelate;
    //const in = OP_in;
    const increment_i = OP_increment_i;
    const decrement_i = OP_decrement_i;
    const inclocal_i = OP_inclocal_i;
    const declocal_i = OP_declocal_i;
    const negate_i = OP_negate_i;
    const add_i = OP_add_i;
    const subtract_i = OP_subtract_i;
    const multiply_i = OP_multiply_i;
    const getlocal0 = OP_getlocal0;
    const getlocal1 = OP_getlocal1;
    const getlocal2 = OP_getlocal2;
    const getlocal3 = OP_getlocal3;
    const setlocal0 = OP_setlocal0;
    const setlocal1 = OP_setlocal1;
    const setlocal2 = OP_setlocal2;
    const setlocal3 = OP_setlocal3;
    const debug = OP_debug;
    const debugline = OP_debugline;
    const debugfile = OP_debugfile;
    const bkptline = OP_bkptline;
    const timestamp = OP_timestamp;
}
