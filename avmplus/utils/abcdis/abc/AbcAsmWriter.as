/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc
{
    import avmplus.File
    
    import flash.utils.ByteArray
    
    import abc.Constants;
    import abc.Types.ABCFile
    import abc.Types.Exception
    import abc.Types.Pool
    import abc.Types.MethodInfo
    import abc.Types.MethodBody
    import abc.Types.ScriptInfo
    import abc.Types.Traits
    import abc.Types.Trait
    import abc.Types.ConstantMultiname
    import abc.Types.ConstantNamespace
    import abc.Types.ConstantNamespaceSet
    import IndentingPrinter;
    import ByteArrayPrinter;
    import TablePrinter;
    
    public final class AbcAsmWriter
    {
        public function AbcAsmWriter(abcfile:ABCFile, file:String)
        {
            this.abcfile = abcfile
            this.file = file
        }
        
        private var abcfile:ABCFile
        private var file:String
        private var data:ByteArray
        private var printer:IndentingPrinter
        
        static private var identifierRegEx : RegExp = /^[$_a-zA-Z][$_a-zA-Z0-9]*$/;
        
        public function write():void {
            data = new ByteArray()
            var byteArrayPrinter : ByteArrayPrinter = new ByteArrayPrinter(data);
            printer = new IndentingPrinter(byteArrayPrinter, 0, 4);
            printer.println("// minor_version " + abcfile.minor_version);
            printer.println("// major_version " + abcfile.major_version);
            
            writeArray(abcfile.scriptInfos,    writeScriptInfo)
            
            
            writeArray(abcfile.methodInfos,    writeMethodInfo)
            
            if (abcfile.metadataInfos.getNumItems() > 0)
                printer.println("#error abcasm does not support meta-data info");
            
            if (abcfile.instanceInfos.getNumItems() > 0)
                printer.println("#error abcasm does not support instance info");
                
            if (abcfile.classInfos.getNumItems() > 0)
                printer.println("#error abcasm does not support class info");
            
            writeArray(abcfile.methodBodies,   writeMethodBody)
            
            File.writeByteArray(file, data)
        }
        
        private function writeScriptInfo(si:ScriptInfo, id:int) : void {
            printer.println("// Script " + id)
            printer.println(".script_info {")
            printer.indent();
            printer.println("init : " + si.init_index);
            printer.println("traits : {");
            printer.indent();
            writeTraits(si.traits)
            printer.unindent();
            printer.println("}");
            printer.unindent();
            printer.println("}");
        }
        
        private function writeMethodInfo(mi:MethodInfo, id:int) : void {
            printer.println("// MethodInfo " + id)
            printer.println(".method_info {")
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(3, 2);
            if (mi.name_index != 0)
            {
                var methodName : String = abcfile.getString(mi.name_index);
                if (!isIdentifier(methodName))
                    throw new UnsupportedConstructError("abcasm only supports method names that are identifiers: " + methodName);
                tablePrinter.addRow(["name", ":", methodName])
            }
            try
            {
                tablePrinter.addRow(["return_type", ":", nameToString(abcfile.getMultiname(mi.ret_type))]);
            }
            catch (e:UnsupportedConstructError)
            {
                tablePrinter.addRow(["#error" , "", e.message]);
            }
            var flags : Array = [];
            if (mi.needsArguments)
                flags.push("NEED_ARGUMENTS");
            if (mi.needsActivation)
                flags.push("NEED_ACTIVATION");
            if (mi.needsRest)
                flags.push("NEED_REST");
            if (mi.hasOptional)
                flags.push("HAS_OPTIONAL")
            if (mi.setDXNS)
                flags.push("SET_DXNS")
            if (mi.hasParamNames)
                flags.push("HAS_PARAM_NAMES")
            if (flags.length > 0)
                tablePrinter.addRow(["flags", ":", flags.join(" | ")]);
            
            tablePrinter.print(printer);
            
            if (mi.param_count > 0)
            {
                printer.println("param_type : {");
                printer.indent();
                var paramTypeStrings : Vector.<String> = new Vector.<String>(mi.param_count, true);
                try
                {
                    for (var paramIndex : uint = 0; paramIndex < mi.param_count; ++paramIndex)
                        paramTypeStrings[paramIndex] = nameToString(abcfile.getMultiname(mi.param_types[paramIndex]))
                    printer.println(paramTypeStrings.join(", "));
                }
                catch (e:UnsupportedConstructError)
                {
                    printer.println("#error " + e.message);
                }
                printer.unindent();
                printer.println("}");
            }
            
            printer.unindent();
            printer.println("}");
        }
        
        private function findBasicBlockStartAtOffset(offset : int, blocks : Vector.<BasicBlock>) : BasicBlock
        {
            for each ( var bb : BasicBlock in blocks )
            {
                if (bb.getInstrs()[0].getOffset() == offset)
                    return bb;
            }
            return null;
        }
        
        private function writeMethodBody(mb : MethodBody, id:int) : void {
            printer.println("// MethodBody " + id + " code_offset=" + mb.code_offset);
            printer.println(".method_body_info {")
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(3, 2);
            tablePrinter.addRow(["method", ":", mb.method_info])
            tablePrinter.addRow(["local_count", ":", mb.max_regs])
            tablePrinter.addRow(["max_stack", ":", mb.max_stack])
            tablePrinter.addRow(["max_scope_depth", ":", mb.max_scope])
            tablePrinter.addRow(["init_scope_depth", ":", mb.scope_depth])
            tablePrinter.print(printer);
            printer.println("code : {");
            printer.indent();
            for(var i:int =0; i<mb.blocks.length; i++) {
                var block : BasicBlock = mb.blocks[i];
                printer.println("bb" + i + ":");
                printer.indent();
                try
                {
                    for(var j:int =0; j<block.getInstrs().length; j++) {
                        var inst:Instruction = mb.blocks[i].getInstrs()[j]
                        switch(inst.getOpcode()) {
                            case Constants.OP_ifnlt:
                            case Constants.OP_ifnle:
                            case Constants.OP_ifngt:
                            case Constants.OP_ifnge:
                            case Constants.OP_iftrue:
                            case Constants.OP_iffalse:
                            case Constants.OP_ifeq:
                            case Constants.OP_ifne:
                            case Constants.OP_iflt:
                            case Constants.OP_ifle:
                            case Constants.OP_ifgt:
                            case Constants.OP_ifge:
                            case Constants.OP_ifstricteq:
                            case Constants.OP_ifstrictne:
                            case Constants.OP_jump:
                                printer.println(inst.getOpcodeName() + " bb" + mb.blocks.indexOf(inst.jumpTarget))
                                break
                            /* case Constants.OP_lookupswitch:
                                addJumpTarget(jumpTargetMap, cur.getOffset() + cur.getImm(0), cur.getOffset())

                                var n:int = inst.getImm(1)
                                for(var j:int=0; j<=n; j++)
                                    addJumpTarget(jumpTargetMap, inst.getOffset() + inst.getImm(2 + j), cur.getOffset())
                                break*/
                            case Constants.OP_call:
                                printer.println(inst.getOpcodeName() + " (" + inst.getImm(0) + ")");
                                break;
                            
                            // Instructions with a immediate name id and an immediate int.
                            case Constants.OP_callproplex:
                            case Constants.OP_callproperty:
                            case Constants.OP_callpropvoid:
                            case Constants.OP_callsuper:
                            case Constants.OP_callsupervoid:
                            case Constants.OP_constructprop:
                                printer.println(inst.getOpcodeName() + " " + nameToString(abcfile.getMultiname(inst.getImm(0))) + "(" + inst.getImm(1) + ")");
                                break;
                                
                            case Constants.OP_callstatic:
                                printer.println(inst.getOpcodeName() + " " + inst.getImm(0) + " " + inst.getImm(1));
                                break;
                            
                            // Instructions with a single immediate name id.
                            case Constants.OP_astype:
                            case Constants.OP_coerce:
                            case Constants.OP_deleteproperty:
                            case Constants.OP_finddef:
                            case Constants.OP_findproperty:
                            case Constants.OP_findpropstrict:
                            case Constants.OP_getlex:
                            case Constants.OP_getproperty:
                            case Constants.OP_getsuper:
                            case Constants.OP_istype:
                            case Constants.OP_setproperty:
                            case Constants.OP_getdescendants:
                                printer.println(inst.getOpcodeName() + " " + nameToString(abcfile.getMultiname(inst.getImm(0))));
                                break;
                            
                            // opcodes with list of integer immediates.
                            case Constants.OP_applytype:
                            case Constants.OP_debug:
                            case Constants.OP_debugline:
                            case Constants.OP_declocal:
                            case Constants.OP_declocal_i:
                            case Constants.OP_getlocal:
                            case Constants.OP_getscopeobject:
                            case Constants.OP_getouterscope:
                            case Constants.OP_hasnext2:
                            case Constants.OP_inclocal:
                            case Constants.OP_inclocal_i:
                            case Constants.OP_kill:
                            case Constants.OP_newarray:
                            case Constants.OP_newcatch:
                            case Constants.OP_newobject:
                            case Constants.OP_pushbyte:
                            case Constants.OP_pushshort:
                            case Constants.OP_setlocal:
                            case Constants.OP_bkptline:
                            case Constants.OP_newfunction:
                            
                            case Constants.OP_getslot:
                            case Constants.OP_getglobalslot:
                            case Constants.OP_setslot:
                            case Constants.OP_setglobalslot:
                            
                            case Constants.OP_constructsuper:
                            case Constants.OP_construct:
                                printer.println(inst.getOpcodeName() + " " + inst.getImms().join(", ") + "")
                                break;
                            
                            case Constants.OP_pushint:
                                printer.println(inst.getOpcodeName() + " " + abcfile.intPool.get(inst.getImm(0)));
                                break;
                            case Constants.OP_pushuint:
                                printer.println(inst.getOpcodeName() + " 0x" + abcfile.uintPool.get(inst.getImm(0)).toString(16));
                                break;
                            case Constants.OP_pushdouble:
                                var doubleStr : String = abcfile.numberPool.get(inst.getImm(0)).toString();
                                if (doubleStr.indexOf(".") == -1)
                                    doubleStr += ".0";
                                printer.println(inst.getOpcodeName() + " " + doubleStr);
                                break;
                            
                            case Constants.OP_pushnamespace:
                                printer.println(inst.getOpcodeName() + " " + namespaceToString(ConstantNamespace(abcfile.nsPool.get(inst.getImm(0)))));
                                break;
                            
                            // opcodes with a string immediate
                            case Constants.OP_debugfile:
                            case Constants.OP_dxns:
                            case Constants.OP_pushstring:
                                printer.println(inst.getOpcodeName() + " \"" + stringToEscapedString(abcfile.getString(inst.getImm(0))) + "\"")
                                break;
                                
                            // opcodes with no operands.
                            case Constants.OP_add:
                            case Constants.OP_add_i:
                            case Constants.OP_astypelate:
                            case Constants.OP_bitand:
                            case Constants.OP_bitnot:
                            case Constants.OP_bitor:
                            case Constants.OP_bitxor:
                            case Constants.OP_checkfilter:
                            case Constants.OP_coerce_a:
                            case Constants.OP_coerce_b:
                            case Constants.OP_coerce_d:
                            case Constants.OP_coerce_i:
                            case Constants.OP_coerce_s:
                            case Constants.OP_convert_b:
                            case Constants.OP_convert_i:
                            case Constants.OP_convert_d:
                            case Constants.OP_convert_o:
                            case Constants.OP_convert_u:
                            case Constants.OP_convert_s:
                            case Constants.OP_decrement:
                            case Constants.OP_decrement_i:
                            case Constants.OP_divide:
                            case Constants.OP_dup:
                            case Constants.OP_dxnslate:
                            case Constants.OP_equals:
                            case Constants.OP_strictequals:
                            case Constants.OP_esc_xattr:
                            case Constants.OP_esc_xelem:
                            case Constants.OP_getglobalscope:
                            case Constants.OP_getlocal0:
                            case Constants.OP_getlocal1:
                            case Constants.OP_getlocal2:
                            case Constants.OP_getlocal3:
                            case Constants.OP_greaterequals:
                            case Constants.OP_greaterthan:
                            case Constants.OP_hasnext:
                            case Constants.OP_in:
                            case Constants.OP_increment:
                            case Constants.OP_increment_i:
                            case Constants.OP_instanceof:
                            case Constants.OP_istypelate:
                            case Constants.OP_label:
                            case Constants.OP_lessequals:
                            case Constants.OP_lessthan:
                            case Constants.OP_lshift:
                            case Constants.OP_modulo:
                            case Constants.OP_multiply:
                            case Constants.OP_multiply_i:
                            case Constants.OP_negate:
                            case Constants.OP_negate_i:
                            case Constants.OP_newactivation:
                            case Constants.OP_nextname:
                            case Constants.OP_nextvalue:
                            case Constants.OP_nop:
                            case Constants.OP_not:
                            case Constants.OP_pop:
                            case Constants.OP_popscope:
                            case Constants.OP_pushfalse:
                            case Constants.OP_pushtrue:
                            case Constants.OP_pushnan:
                            case Constants.OP_pushnull:
                            case Constants.OP_pushscope:
                            case Constants.OP_pushundefined:
                            case Constants.OP_pushwith:
                            case Constants.OP_returnvalue:
                            case Constants.OP_returnvoid:
                            case Constants.OP_rshift:
                            case Constants.OP_setlocal0:
                            case Constants.OP_setlocal1:
                            case Constants.OP_setlocal2:
                            case Constants.OP_setlocal3:
                            case Constants.OP_subtract:
                            case Constants.OP_subtract_i:
                            case Constants.OP_swap:
                            case Constants.OP_throw:
                            case Constants.OP_typeof:
                            case Constants.OP_urshift:
                            case Constants.OP_bkpt:
                            case Constants.OP_timestamp:
                            case Constants.OP_li8:
                            case Constants.OP_li16:
                            case Constants.OP_li32:
                            case Constants.OP_lf32:
                            case Constants.OP_lf64:
                            case Constants.OP_si8:
                            case Constants.OP_si16:
                            case Constants.OP_si32:
                            case Constants.OP_sf32:
                            case Constants.OP_sf64:
                            case Constants.OP_sxi1:
                            case Constants.OP_sxi8:
                            case Constants.OP_sxi16:
                                printer.println(inst.getOpcodeName());
                                break;
                            default:
                                printer.println("#error " + inst.getOpcodeName())
                        }
                    }
                }
                catch (e : UnsupportedConstructError)
                {
                    printer.println("#error " + inst.toString() + ": " + e.message);
                }
                printer.unindent();
            }
            
            printer.unindent();
            printer.println("}");
            if (mb.exceptions.length > 0)
            {
                printer.println("exception : {");
                printer.indent();
                tablePrinter = new TablePrinter(18, 2);
                for each (var exception : Exception in mb.exceptions)
                {
                    var exceptionFromBlock : BasicBlock = mb.findBasicBlockWithStartOffset(exception.start);
                    var exceptionToBlock : BasicBlock = mb.findBasicBlockWithStartOffset(exception.end);
                    var exceptionTarget : BasicBlock = mb.findBasicBlockWithStartOffset(exception.target);
                    var r : Array = [".exception", "{"];
                    r.push("from", ":", "bb" + mb.blocks.indexOf(exceptionFromBlock))
                    r.push("to", ":", "bb" + mb.blocks.indexOf(exceptionToBlock))
                    r.push("target", ":", "bb" + mb.blocks.indexOf(exceptionTarget))
                    if (exception.type_index != 0)
                        r.push("type", ":", nameToString(abcfile.getMultiname(exception.type_index)))
                    else
                        r.push("", "", "")
                    
                    if (exception.name_index != 0)
                        r.push("type", ":", abcfile.getMultiname(exception.type_index).nameString)
                    else
                        r.push("", "", "")
                    r.push("}");
                    tablePrinter.addRow(r);
                }
                tablePrinter.print(printer);
                printer.unindent();
                printer.println("}");
            }
            
            if (mb.activation_traits.ts.length > 0)
            {
                printer.println("traits : {");
                printer.indent();
                writeTraits(mb.activation_traits)
                printer.unindent();
                printer.println("}");
            }
            printer.unindent();
            printer.println("}");
        }
        
        private function writeTraits(traits : Traits) : void
        {
            for each (var trait : Trait in traits.ts) {
                writeTrait(trait);
            }
        }
        
        private function isIdentifier(s : String) : Boolean
        {
            return s.match(identifierRegEx) != null;
        }
        
        private function isPackageName(s : String) : Boolean
        {
            var components : Array = s.split(".");
            var f : Function = function (s : String, i : int, a : Array) : Boolean
            {
                return isIdentifier(s);
            }
            
            return components.every(f);
        }
        
        private function namespaceToString(ns : ConstantNamespace) : String
        {
            switch (ns.kind)
            {
            case Constants.CONSTANT_PrivateNs:
                return "private";
            case Constants.CONSTANT_PackageNs:
                if (ns.uriString.length == 0)
                    return "package";
                break;
            case Constants.CONSTANT_Namespace:
                if (isPackageName(ns.uriString))
                    return ns.uriString;
            /*
            case Constants.CONSTANT_PackageInternalNs:
            case Constants.CONSTANT_ProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs2:
            */
            }
            throw new UnsupportedConstructError("abcasm does not support this namespace: " + ns.kind + " " + ns.toString());
            return null;
        }
        
        private function nameToString(n : ConstantMultiname) : String
        {
            var baseName : String = n.nameString;
            var result : String = null;
            var constantNS : ConstantNamespace;
            if (n.kind == 0)
                return "*";
            if (!isIdentifier(baseName))
                throw new UnsupportedConstructError("abcasm requires base names to be identifiers: " + n.toString())
            switch(n.kind) {
                case Constants.CONSTANT_Qname:
                    constantNS = abcfile.getNamespace(n.ns_index)
                    if ((constantNS.kind == Constants.CONSTANT_PackageNs) && (constantNS.uriString.length == 0))
                    {
                        result = baseName
                    }
                    else
                    {
                        var ns:String = namespaceToString(constantNS)
                        if (ns == null)
                            result = baseName;
                        else
                            result = "{ " + ns + " }::" + baseName;
                    }
                    break
                case Constants.CONSTANT_Multiname:
                    var nsset : Vector.<ConstantNamespace> = n.nsSet.constantNamespaces
                    var nssetStrings : Vector.<String> = new Vector.<String>();
                    for each (constantNS in nsset)
                        nssetStrings.push(namespaceToString(constantNS));
                    
                    if (nssetStrings.indexOf(null) != -1)
                        throw new UnsupportedConstructError("abcasm does not support empty URI in namespace set: " + n.toString())
                    result = "{ " + nssetStrings.join(", ") + " }::" + baseName;
                    break
                case Constants.CONSTANT_QnameA:
                case Constants.CONSTANT_MultinameA:
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                case Constants.CONSTANT_MultinameL:
                    throw new UnsupportedConstructError("abcasm only supports CONSTANT_RTQname or CONSTANT_Multiname: " + n.toString())
                    break
            }
            return result;
        }
        
        
        
        private function stringToEscapedString(s : String) : String
        {
            const charsToEscape : String = "\b\t\n\f\r\"\'\\";
            const escapeChars : String = "btnfr\"\'\\";
            var escapeIndex : int;
            var currChar : String;
            var result : String = "";
            for (var i:uint = 0; i < s.length; ++i)
            {
                currChar = s.charAt(i)
                escapeIndex = charsToEscape.indexOf(currChar);
                if (escapeIndex != -1)
                    result += "\\" + escapeChars.charAt(escapeIndex);
                else
                    result += currChar;
            }
            return result;
        }
        
        private function traitToKindString(t : Trait) : String
        {
            var result : String;
            switch (t.kind)
            {
            case Constants.TRAIT_Slot:
                result = "var";
                break;
            case Constants.TRAIT_Const:
                result = "const";
                throw new UnsupportedConstructError("abcasm does not support TRAIT_Const: " + t.multiname.toString());
                break;
            case Constants.TRAIT_Method:
                result = "method";
                break;
            case Constants.TRAIT_Getter:
                result = "get";
                break;
            case Constants.TRAIT_Setter:
                result = "set";
                break;
            case Constants.TRAIT_Function:
                result = "function";
                break;
            case Constants.TRAIT_Class:
                result = "class"
                throw new UnsupportedConstructError("abcasm does not support TRAIT_Class: " + t.multiname.toString());
                break;
            }
            if (t.isFinal)
                result += " | ATTR_Final";
            if (t.isOverride)
                result += " | ATTR_Override";
            if (t.hasMetadata)
                result += " | ATTR_Metadata";
            return result;
        }
        
        private function writeTrait(trait : Trait) : void
        {
            printer.println(".trait {");
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(3, 2);
            try
            {
                var kindStr : String = traitToKindString(trait);
                switch(trait.kind)
                {
                case Constants.TRAIT_Slot:
                    tablePrinter.addRow(["kind", ":", kindStr]);
                    tablePrinter.addRow(["name", ":", nameToString(trait.multiname)]);
                    if (trait.slot_id != 0)
                        tablePrinter.addRow(["slot_id", ":", trait.slot_id])
                    tablePrinter.addRow(["type_name", ":", nameToString(trait.typeMultiname)])
                    break;
                case Constants.TRAIT_Const:
                    throw new UnsupportedConstructError("abcasm does not support TRAIT_Const: " + trait.multiname.toString());
                    break;
                case Constants.TRAIT_Method:
                case Constants.TRAIT_Getter:
                case Constants.TRAIT_Setter:
                case Constants.TRAIT_Function:
                    tablePrinter.addRow(["kind", ":", kindStr]);
                    tablePrinter.addRow(["name", ":", nameToString(trait.multiname)]);
                    tablePrinter.addRow(["method_id", ":", trait.method_info]);
                    if (trait.disp_id != 0)
                        tablePrinter.addRow(["disp_id", ":", trait.disp_id])
                    break;
                case Constants.TRAIT_Class:
                    throw new UnsupportedConstructError("abcasm does not support TRAIT_Class: " + trait.multiname.toString());
                    break;
                
                }
                tablePrinter.print(printer);
            }
            catch (e : UnsupportedConstructError)
            {
                printer.println("#error " + e.message);
            }
            printer.unindent();
            printer.println("}");
        }
        
        private function writeArray(p:Pool, writefunc:Function):void {
            printer.println("// "+p.name+" pool count: "+p.items.length+" start "+p.start_pos+", size: "+(p.end_pos - p.start_pos)+" bytes");
            var n:int = p.getNumItems()
            for(var i:int=0; i<n; i++) {
                var v : Object = p.get(i)
                writefunc.call(this, v, i)
            }
        }
        
        public static function writeFile(abcfile:ABCFile, file:String) : void {
            var w:AbcAsmWriter = new AbcAsmWriter(abcfile, file)
            w.write()
        }
    }
}

class UnsupportedConstructError extends Error
{
    public function UnsupportedConstructError(msg : String)
    {
        super(msg);
    }
}

