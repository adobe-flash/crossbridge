/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc
{
    import avmplus.File
    
    import flash.utils.ByteArray
    import flash.utils.Dictionary
    
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
    import abc.Types.InstanceInfo
    import abc.Types.ClassInfo
    import abc.Types.MetadataInfo
    import IndentingPrinter;
    import ByteArrayPrinter;
    import TablePrinter;
    import IPrinter;
    
    public class AbcDumpWriter extends AbcAS3Traversal
    {
        public function AbcDumpWriter(abcfile:ABCFile)
        {
            super(abcfile)
            dumpedMethods = new Dictionary();
        }
        
        private var file:String
        private var data:ByteArray
        private var printer:IndentingPrinter
        private var dumpedMethods : Dictionary;
        
        private static var identifierRegEx : RegExp = /^[$_a-zA-Z][$_a-zA-Z0-9]*$/;
        
        private function qnameToString(ns : ConstantNamespace, n : String) : String
        {
            if (ns == null)
                return n
            
            if ((ns.kind == Constants.CONSTANT_PackageNs) && (ns.uriString.length > 0))
                return ns.uriString + "::" + n;
            
            var qual : String = nsQualifierForNamespace(ns)
            if (qual.length > 0)
                return n;
            
            if (ns.uriString.length == 0)
                return n
            
            return ns.uriString + "::" + n;
        }
        
        private function nssetToString(nsSet : ConstantNamespaceSet) : String
        {
            var s:String = ""
            try
            {
                for each (var ns:ConstantNamespace in nsSet.constantNamespaces) {
                    if (ns.kind != Constants.CONSTANT_PrivateNs)
                        s += (ns.uriString + ", ")
                    else
                        s += "private, "
                }
                return "{" + s + "}";
            }
            catch(e : *)
            {
                for each (var foo : * in nsSet.namespaces) {
                    trace(foo)
                }
                throw e;
            }
            return null;
        }
        
        private function nameToString(n : ConstantMultiname) : String
        {
            if (n.isAny)
                return "*"
            
            var nsset : ConstantNamespaceSet;
            
            switch(n.kind) {
                case Constants.CONSTANT_Qname:
                case Constants.CONSTANT_QnameA:
                    return qnameToString(abcfile.getNamespace(n.ns_index), abcfile.getString(n.name_index));
                    break
                case Constants.CONSTANT_Multiname:
                case Constants.CONSTANT_MultinameA:
                    nsset = abcfile.getNamespaceSet(n.nsset_index)
                    if (nsset.namespaces.length == 1)
                        return qnameToString(abcfile.getNamespace(nsset.namespaces[0]), abcfile.getString(n.name_index))
                    else
                        return (nsset == null ? "" : nssetToString(nsset) + "::") + abcfile.getString(n.name_index)
                    break
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                    return "<error> " + n.toString()
                    break
                case Constants.CONSTANT_MultinameL:
                    return "<error> " + n.toString()
                    break
                case Constants.CONSTANT_TypeName:
                    var typeStrings : Array = new Array();
                    for each (var typeId : int in n.types)
                        typeStrings.push(nameToString(abcfile.getMultiname(typeId)))
                    return nameToString(abcfile.getMultiname(n.name_index)) + ".<" + typeStrings.join(", ") + ">";
                    break
            }
            return "<error> " + n.toString()
        }
        
        private function nsQualifierForName(n : ConstantMultiname) : String
        {
            var nsset : ConstantNamespaceSet;
            switch(n.kind) {
                case Constants.CONSTANT_Qname:
                case Constants.CONSTANT_QnameA:
                    return nsQualifierForNamespace(abcfile.getNamespace(n.ns_index));
                    break
                case Constants.CONSTANT_Multiname:
                case Constants.CONSTANT_MultinameA:
                    nsset = abcfile.getNamespaceSet(n.nsset_index)
                    if (nsset.namespaces.length == 1)
                        return nsQualifierForNamespace(abcfile.getNamespace(nsset.namespaces[0]))
                    break
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                    break
                case Constants.CONSTANT_MultinameL:
                    break
                case Constants.CONSTANT_TypeName:
                    break
            }
            return ""
        }
        
        private function nsQualifierForNamespace(ns : ConstantNamespace) : String
        {
            switch (ns.kind)
            {
            case Constants.CONSTANT_PackageNs:
                return "public "
            case Constants.CONSTANT_ProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs2:
                return "protected "
            case Constants.CONSTANT_PackageInternalNs:
                return "internal "
            case Constants.CONSTANT_PrivateNs:
                return "private "
            }
            
            if (ns.kind == Constants.CONSTANT_Namespace)
            {
                if (ns.uriString == "http://adobe.com/AS3/2006/builtin")
                    return "AS3 ";
            }
            return ns.uriString + " "
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
        
        public function write(p : IPrinter):void {
            printer = new IndentingPrinter(p, 0, 2);
            
            traverse()
            
            writeArray(abcfile.methodInfos,    writeAnonMethodInfo);
        }
        
        protected override function traverseScript(id : uint, scriptInfo : ScriptInfo) : void
        {
            printer.println("// script " + id);
            super.traverseScript(id, scriptInfo)
            printer.println("")
        }
        
        protected override function traverseScriptSlotTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", trait, false);
        }
        
        protected override function traverseScriptConstTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", trait, false);
        }
        
        protected override function traverseScriptMethodTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, false);
        }
        
        protected override function traverseScriptGetterTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", trait, false);
        }
        
        protected override function traverseScriptSetterTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", trait, false);
        }
        
        protected override function traverseScriptFunctionTrait(trait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, false);
        }
        
        protected override function traverseScriptClassTrait(classId : uint, instanceInfo: InstanceInfo, classInfo : ClassInfo, trait : Trait, scriptInfo : ScriptInfo) : void
        {
            printer.println("")
            printer.println("// class_id=" + classId + " slot_id=" + trait.slot_id);
            var def : String;
            if (instanceInfo.isInterface)
            {
                def = "interface";
            }
            else
            {
                def = "class";
                if (instanceInfo.isDynamic)
                    def = "dynamic " + def;
                if (instanceInfo.isFinal)
                    def = "final " + def;
            }
            writeMetaData(trait)
            printer.println(nsQualifierForName(trait.multiname) + def + " " + nameToString(trait.multiname) + " extends " + nameToString(abcfile.getMultiname(instanceInfo.super_index)));
            if (instanceInfo.interfaces.length > 0)
            {
                printer.indent()
                var interfaceNames : Vector.<String> = new Vector.<String>()
                for each (var interfaceNameId : int in instanceInfo.interfaces) {
                    interfaceNames.push(nameToString(abcfile.getMultiname(interfaceNameId)))
                }
                var implementsStr : String = "implements " + interfaceNames.join(",");
                printer.println(implementsStr)
                printer.unindent()
            }
            
            printer.println("{");
            printer.indent();
            
            super.traverseScriptClassTrait(classId, instanceInfo, classInfo, trait, scriptInfo);
            
            printer.unindent();
            printer.println("}");
            
        }
        
        protected override function traverseScriptInit(init : MethodInfo, scriptInfo : ScriptInfo, scriptId : uint) : void
        {
            printer.println("")
            writeMethodInfo("", "script" + scriptId + "$init", "function", init, false, false, false);
        }
        
        protected override function traverseInstanceInit(init : MethodInfo, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            printer.println("")
            printer.println("// method_id=" + instanceInfo.iinit_index)
            writeMethodInfo("public ", nameToString(classTrait.multiname), "function", init, false, false, false);
        }
        
        protected override function traverseInstanceSlotTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", trait, false);
        }
        
        protected override function traverseInstanceConstTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", trait, false);
        }
        
        protected override function traverseInstanceMethodTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, false);
        }
        
        protected override function traverseInstanceGetterTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", trait, false);
        }
        
        protected override function traverseInstanceSetterTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", trait, false);
        }
        
        protected override function traverseInstanceFunctionTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, false);
        }
        
        protected override function traverseClassInit(init : MethodInfo, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            printer.println("")
            printer.println("// method_id=" + classInfo.init_index)
            writeMethodInfo("public ", nameToString(classTrait.multiname) + "$", "function", init, true, false, false);
        }
        
        protected override function traverseClassSlotTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", trait, true);
        }
        
        protected override function traverseClassConstTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", trait, true);
        }
        
        protected override function traverseClassMethodTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, true);
        }
        
        protected override function traverseClassGetterTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", trait, true);
        }
        
        protected override function traverseClassSetterTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", trait, true);
        }
        
        protected override function traverseClassFunctionTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, true);
        }
        
        private function writeMetaData(t : Trait) : void
        {
            if (!t.hasMetadata)
                return
            for each (var mid : uint in t.metadata)
            {
                var mi : MetadataInfo = MetadataInfo(abcfile.metadataInfos.get(mid));
                var entries : Vector.<String> = new Vector.<String>()
                for (var i : uint = 0; i < mi.keys.length; ++i)
                {
                    var key : String = abcfile.getString(mi.keys[i])
                    var value : String = abcfile.getString(mi.values[i])
                    if (key.length == 0)
                        entries.push("\"" + value + "\"")
                    else
                        entries.push(key + "=\"" + value + "\"")
                }
                printer.println("[" + abcfile.getString(mi.name_index) + "(" + entries.join(", ") + ")]" + " // metadata_id=" + mid)
            }
        }
        
        private function writeAnonMethodInfo(mi:MethodInfo, id:int) : void
        {
            if (mi in dumpedMethods)
                return
            printer.println("")
            printer.println("// " + id + " " + mi.derivedName);
            writeMethodInfo("", "", "function ", mi, false, false, false);
        }
        
        private function writeSlotTrait(kindStr : String, t : Trait, isStatic : Boolean) : void
        {
            printer.println("")
            var qual : String = nsQualifierForName(t.multiname)
            var nameStr : String = nameToString(t.multiname);
            var value : Object = t.valueObject
            var valueStr : String = "";
            if (t.value_kind == Constants.CONSTANT_Utf8)
                valueStr = " = \"" + value + "\""
            else if (value is ConstantNamespace)
                valueStr = " = " + value.uriString
            else if (value != null)
                valueStr = " = " + value.toString()
            else if (t.value_kind == Constants.CONSTANT_Null)
                valueStr = " = null"
            var staticStr : String = isStatic ? "static " : "";
            writeMetaData(t)
            printer.println("// name_id=" + t.name_index + " slot_id=" + t.slot_id)
            printer.println(qual + staticStr + kindStr + " " + nameStr + ":" + nameToString(abcfile.getMultiname(t.type_index)) + valueStr);
        }
        
        private function stringForLookupSwitch(inst : Instruction, mb : MethodBody) : String
        {
            var offset : uint = inst.getOffset(); // + cur.getImm(0)
            var defaultStr : String = "default: bb" + mb.blocks.indexOf(MethodBody.findBasicBlockWithStartOffset(offset + inst.getImm(0), inst.getBasicBlock().getSuccs()))
            var maxCase : uint = inst.getImm(1)
            var maxCaseStr : String = "maxcase: " + maxCase
            var result : Vector.<String> = new Vector.<String>()
            result.push(defaultStr)
            result.push(maxCaseStr)
            for (var i:int=2; i <= maxCase + 2; ++i)
            {
                result.push("bb" + mb.blocks.indexOf(MethodBody.findBasicBlockWithStartOffset(offset + inst.getImm(i), inst.getBasicBlock().getSuccs())))
            }
            return result.join(" ")
        }
        
        private function writeMethodInfo(qualStr : String, nameStr : String, kindStr : String, methodInfo : MethodInfo, isStatic : Boolean, isOverride : Boolean, isFinal : Boolean) : void
        {
            dumpedMethods[methodInfo] = true;
            var paramTypeStrings : Vector.<String> = new Vector.<String>();
            for each (var paramTypeNameId : int in methodInfo.param_types)
                paramTypeStrings.push(nameToString(abcfile.getMultiname(paramTypeNameId)))
            var staticStr : String = isStatic ? "static " : "";
            var overrideStr : String = isOverride ? "override " : "";
            var nativeStr : String = methodInfo.isNative ? "native " : "";
            var finalStr : String = isFinal ? "final " : "";
            printer.println(qualStr + staticStr + nativeStr + finalStr + overrideStr + kindStr + " " + nameStr + "(" + paramTypeStrings.join(",") + "):" + nameToString(abcfile.getMultiname(methodInfo.ret_type)))
            var mb : MethodBody = abcfile.methodBodyMap[methodInfo]
            if (mb)
            {
                printer.println("{")
                printer.indent()
                var tablePrinter : TablePrinter = new TablePrinter(3, 2);
                tablePrinter.addRow(["//", "derivedName", mb.derivedName])
                tablePrinter.addRow(["//", "method_info", mb.method_info]);
                tablePrinter.addRow(["//", "max_stack", mb.max_stack]);
                tablePrinter.addRow(["//", "max_regs", mb.max_regs]);
                tablePrinter.addRow(["//", "scope_depth", mb.scope_depth]);
                tablePrinter.addRow(["//", "max_scope", mb.max_scope]);
                tablePrinter.addRow(["//", "code_length", mb.code_length]);
                tablePrinter.addRow(["//", "code_offset", mb.code_offset]);
                tablePrinter.print(printer);
                if (mb.activation_traits.ts.length > 0)
                {
                    printer.println("activation_traits {")
                    printer.indent()
                    
                    for each (var trait : Trait in mb.activation_traits.ts)
                    {
                        var kindStr : String;
                        switch (trait.kind)
                        {
                        case Constants.TRAIT_Slot:
                            kindStr = "var"
                            break
                        case Constants.TRAIT_Const:
                            kindStr = "const"
                            break
                        default:
                            throw new Error("Illegal activation trait in " + mb.derivedName)
                        }
                        writeSlotTrait(kindStr, trait, false)
                    }
                    
                    printer.unindent()
                    printer.println("}")
                }
                var i : uint;
                for(i=0; i<mb.blocks.length; i++)
                {
                    printer.println("bb" + i + ":");
                    printer.indent();
                    printer.println("preds=[" + mb.blocks[i].getPredIds(mb.blocks).join(", ") + "]");
                    printer.println("succs=[" + mb.blocks[i].getSuccIds(mb.blocks).join(", ") + "]");
                    if(mb.blocks[i].state != null) {
                        printer.println("verification = " + (mb.blocks[i].state.verifyError == null ? "ok" : "failed: " + mb.blocks[i].state.verifyError));
                    }
                    tablePrinter = new TablePrinter(7, 2);
                    for(var j:int =0; j<mb.blocks[i].getInstrs().length; j++) {
                        var inst:Instruction = mb.blocks[i].getInstrs()[j]
                        var constantStr : String = ""
                        if (inst.hasName) {
                            constantStr = abcfile.getMultiname(inst.getImm(0)).toString();
                        }
                        else if (inst.isJump) {
                            constantStr = "bb" +  mb.blocks.indexOf(inst.jumpTarget);
                        }
                        else {
                            switch (inst.getOpcode())
                            {
                            case Constants.OP_debugfile:
                            case Constants.OP_pushstring:
                                constantStr = "\"" + stringToEscapedString(abcfile.getString(inst.getImm(0))) + "\""
                                break;
                            case Constants.OP_debugline:
                                constantStr = inst.getImm(0).toString()
                                break;
                            case Constants.OP_lookupswitch:
                                constantStr = stringForLookupSwitch(inst, mb)
                                break
                            }
                        }
                        tablePrinter.addRow([inst.getOffset() + "    ",
                                             inst.getOpcodeName(),
                                             constantStr,
                                             inst.getImmString(),
                                             //(inst.getStackDepth() == null ? "" : "// stack: " + inst.getStackDepth()),
                                             (inst.getState() == null ? "" : "// stack["+inst.getState().stackDepth+"]: " + inst.getState().stackTypeString()),
                                             (inst.getState() == null ? "" : "// locals: " + inst.getState().localsTypeString()),
                                             inst.getVerifyError()])
                    }
                    tablePrinter.print(printer);
                    printer.unindent();
                }
                
                printer.unindent()
                printer.println("}")
                if (mb.exceptions.length > 0)
                {
                    tablePrinter = new TablePrinter(9, 2);
                    tablePrinter.addRow(["//", "exception", "start", "end", "target", "type id", "type string", "name id", "name string"])
                    for(i=0; i<mb.exceptions.length; i++)
                        tablePrinter.addRow(["//", i, mb.exceptions[i].start, mb.exceptions[i].end, mb.exceptions[i].target, mb.exceptions[i].type_index, abcfile.getMultiname(mb.exceptions[i].type_index), mb.exceptions[i].name_index, abcfile.getMultiname(mb.exceptions[i].name_index)])
                    tablePrinter.print(printer);
                    printer.println("");
                }
            }
        }
        
        private function writeMethodTrait(kindStr : String, t : Trait, isStatic : Boolean) : void
        {
            var qual : String = nsQualifierForName(t.multiname)
            var nameStr : String = nameToString(t.multiname);
            var methodInfo : MethodInfo = abcfile.getMethodInfo(t.method_info);
            printer.println("")
            writeMetaData(t)
            printer.println("// name_id=" + t.name_index + " method_id=" + t.method_info + " disp_id=" + t.disp_id)
            writeMethodInfo(qual, nameStr, kindStr, methodInfo, isStatic, t.isOverride, t.isFinal);
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
        
        private function writeArray(p:Pool, writefunc:Function):void {
            var n:int = p.getNumItems()
            for(var i:int=0; i<n; i++) {
                var v : Object = p.get(i)
                writefunc.call(this, v, i)
            }
        }
        
        public static function print(abcfile : ABCFile, p : IPrinter) : void
        {
            var w : AbcDumpWriter = new AbcDumpWriter(abcfile)
            w.write(p);
        }
    }
}

