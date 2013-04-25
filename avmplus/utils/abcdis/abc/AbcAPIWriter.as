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
    import IndentingPrinter;
    import ByteArrayPrinter;
    import TablePrinter;
    import IPrinter;
    
    public final class AbcAPIWriter extends AbcAS3Traversal
    {
        public function AbcAPIWriter(abcfile:ABCFile)
        {
            super(abcfile)
            dumpedMethods = new Dictionary();
            currentQualifier = "";
        }
        
        private var printer:IndentingPrinter
        private var dumpedMethods : Object;
        private var currentQualifier : String;
        
        static private var identifierRegEx : RegExp = /^[$_a-zA-Z][$_a-zA-Z0-9]*$/;
        
        private function isVisibleNamespace(ns : ConstantNamespace) : Boolean
        {
            return ns.kind != Constants.CONSTANT_PrivateNs;
        }
        
        private function qnameToString(ns : ConstantNamespace, n : String) : String
        {
            return ((ns == null) || (ns.uriString.length == 0) ? "" : ns.uriString + "::") + n;
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
        
        private function nsQualifierForNamespace(ns : ConstantNamespace) : String
        {
            switch (ns.kind)
            {
            case Constants.CONSTANT_PackageNs:
                return "public"
            case Constants.CONSTANT_ProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs:
            case Constants.CONSTANT_StaticProtectedNs2:
                return "protected"
            case Constants.CONSTANT_PackageInternalNs:
                return "internal"
            }
            
            if (ns.kind == Constants.CONSTANT_Namespace)
            {
                if (ns.uriString == "http://adobe.com/AS3/2006/builtin")
                    return "AS3";
            }
            return ns.uriString
        }
        
        public function write(printer : IPrinter):void {
            this.printer = new IndentingPrinter(printer, 0, 2);
            traverse()
            
            var n:int = abcfile.methodInfos.getNumItems()
            for(var i:int=0; i<n; i++) {
                var v : MethodInfo = abcfile.methodInfos.get(i) as MethodInfo
                writeAnonMethodInfo(v, i)
            }
            //writeArray(abcfile.methodInfos,    writeAnonMethodInfo);
        }
        
        protected override function traverseScriptInit(init : MethodInfo, si : ScriptInfo, scriptId : uint) : void
        {
            dumpedMethods[abcfile.getMethodInfo(si.init_index)] = true
        }
        
        private function writeAnonMethodInfo(mi:MethodInfo, id:int) : void
        {
            if (mi in dumpedMethods)
                return
            printer.println("// " + id + " " + mi.derivedName);
            writeMethodInfo("", "function ", mi, "");
        }
        
        protected override function traverseScriptSlotTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", t, "");
        }
        
        protected override function traverseScriptConstTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", t, "");
        }
        
        protected override function traverseScriptMethodTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", t, "");
        }
        
        protected override function traverseScriptGetterTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", t, "");
        }
        
        protected override function traverseScriptSetterTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", t, "");
        }
        
        protected override function traverseScriptFunctionTrait(t : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", t, "");
        }
        
        protected override function traverseScriptClassTrait(classIndex : uint, instanceInfo: InstanceInfo, classInfo : ClassInfo, trait : Trait, scriptInfo : ScriptInfo) : void
        {
            var initMethodInfo : MethodInfo = abcfile.getMethodInfo(instanceInfo.iinit_index)
            var cinitMethodInfo : MethodInfo = abcfile.getMethodInfo(classInfo.init_index)
            
            dumpedMethods[initMethodInfo] = true
            dumpedMethods[cinitMethodInfo] = true
            
            var f : Function = function (t : Trait, mi : MethodInfo) : void
            {
                dumpedMethods[mi] = true;
            }
            visitMethodTraits(instanceInfo.instance_traits, f);
            visitMethodTraits(classInfo.static_traits, f);
            
            if (!isVisibleNamespace(abcfile.getNamespace(trait.multiname.ns_index)))
                return;
            
            printer.println("")
            var def : String = classDeclarationString(instanceInfo, trait)
            printer.println(def + "extends " + nameToString(abcfile.getMultiname(instanceInfo.super_index)));
            printer.indent()
            if (instanceInfo.interfaces.length > 0)
            {
                var interfaceNames : Vector.<String> = new Vector.<String>()
                for each (var interfaceNameId : int in instanceInfo.interfaces) {
                    interfaceNames.push(nameToString(abcfile.getMultiname(interfaceNameId)))
                }
                var implementsStr : String = "implements " + interfaceNames.join(",");
                printer.println(implementsStr)
            }
            printer.println("");
            currentQualifier = classDeclarationString(instanceInfo, trait)
            super.traverseScriptClassTrait(classIndex, instanceInfo, classInfo, trait, scriptInfo)
            printer.unindent() // handles .indent call from traverseInstanceInit
            printer.unindent()
        }
        
        private function classDeclarationString(instanceInfo :  InstanceInfo, classTrait : Trait) : String
        {
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
            return def + " " + nameToString(classTrait.multiname) + " ";
        }
        
        protected override function traverseInstanceInit(init : MethodInfo, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodInfo(nameToString(classTrait.multiname), "function", init, currentQualifier)
            printer.indent()
        }
        
        protected override function traverseInstanceSlotTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", trait, currentQualifier);
        }
        
        protected override function traverseInstanceConstTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", trait, currentQualifier);
        }
        
        protected override function traverseInstanceMethodTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, currentQualifier);
        }
        
        protected override function traverseInstanceGetterTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", trait, currentQualifier);
        }
        
        protected override function traverseInstanceSetterTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", trait, currentQualifier);
        }
        
        protected override function traverseInstanceFunctionTrait(trait : Trait, instanceInfo : InstanceInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, currentQualifier);
        }
        
        protected override function traverseClassSlotTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("var", trait, currentQualifier + "static ");
        }
        
        protected override function traverseClassConstTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeSlotTrait("const", trait, currentQualifier + "static ");
        }
        
        protected override function traverseClassMethodTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, currentQualifier + "static ");
        }
        
        protected override function traverseClassGetterTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function get", trait, currentQualifier + "static ");
        }
        
        protected override function traverseClassSetterTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function set", trait, currentQualifier + "static ");
        }
        
        protected override function traverseClassFunctionTrait(trait : Trait, classInfo : ClassInfo, classTrait : Trait, scriptInfo : ScriptInfo) : void
        {
            writeMethodTrait("function", trait, currentQualifier + "static ");
        }
        
        private function writeSlotTrait(kindStr : String, t : Trait, containerQualifier : String) : void
        {
            
            if (!isVisibleNamespace(abcfile.getNamespace(t.multiname.ns_index)))
                return;
            var nameStr : String = nameToString(t.multiname)
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
            printer.println(containerQualifier + kindStr + " " + nameStr + ":" + nameToString(abcfile.getMultiname(t.type_index)) + valueStr);
        }
        
        private function writeMethodInfo(nameStr : String, kindStr : String, methodInfo : MethodInfo, containerQualifier : String) : void
        {
            var paramTypeStrings : Vector.<String> = new Vector.<String>();
            for each (var paramTypeNameId : int in methodInfo.param_types)
                paramTypeStrings.push(nameToString(abcfile.getMultiname(paramTypeNameId)))
            printer.println(containerQualifier + kindStr + " " + nameStr + "(" + paramTypeStrings.join(",") + "):" + nameToString(abcfile.getMultiname(methodInfo.ret_type)))
        }
        
        private function writeMethodTrait(kindStr : String, t : Trait, containerQualifier : String) : void
        {
            var methodInfo : MethodInfo = abcfile.getMethodInfo(t.method_info);
            dumpedMethods[methodInfo] = true;
            if (!isVisibleNamespace(abcfile.getNamespace(t.multiname.ns_index)))
                return;
            var nameStr : String = nameToString(t.multiname);
            printer.println("");
            writeMethodInfo(nameStr, kindStr, methodInfo, containerQualifier);
        }
        
        private function visitMethodTraits(traits : Traits, f : Function) : void
        {
            for each (var t : Trait in traits.ts) {
                switch(t.kind)
                {
                case Constants.TRAIT_Slot:
                case Constants.TRAIT_Const:
                    break;
                case Constants.TRAIT_Method:
                case Constants.TRAIT_Getter:
                case Constants.TRAIT_Setter:
                case Constants.TRAIT_Function:
                    f(t, abcfile.getMethodInfo(t.method_info));
                    break;
                case Constants.TRAIT_Class:
                    break;
                }
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
            throw new APIWriterUnsupportedConstructError("abcasm does not support this namespace: " + ns.kind + " " + ns.toString());
            return null;
        }
        
        private function writeArray(p:Pool, writefunc:Function):void {
            printer.println("// "+p.name+" pool count: "+p.items.length+" start "+p.start_pos+", size: "+(p.end_pos - p.start_pos)+" bytes");
            var n:int = p.getNumItems()
            for(var i:int=0; i<n; i++) {
                var v : Object = p.get(i)
                writefunc.call(this, v, i)
            }
        }
        
        public static function writeFile(abcfile:ABCFile, file:String) : void
        {
            var data : ByteArray = new ByteArray()
            
            var printer : IPrinter = new ByteArrayPrinter(data)
            print(abcfile, printer)
            
            File.writeByteArray(file, data)
        }
        
        public static function print(abcfile:ABCFile, printer : IPrinter) : void
        {
            var w:AbcAPIWriter = new AbcAPIWriter(abcfile)
            w.write(printer)
        }
    }
}

class APIWriterUnsupportedConstructError extends Error
{
    public function APIWriterUnsupportedConstructError(msg : String)
    {
        super(msg);
    }
}

