/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {

    import avmplus.File
    
    import flash.utils.ByteArray
    
    import IPrinter;
    import IndentingPrinter;
    import ByteArrayPrinter;
    import TablePrinter;
    import abc.Reader
    //import abc.IWriter
    import abc.Types.Pool
    import abc.Types.ABCFile
    import abc.Types.ConstantNamespace
    import abc.Types.ConstantMultiname
    import abc.Types.ConstantNamespaceSet
    import abc.Types.Exception
    import abc.Types.Trait
    import abc.Types.Traits
    import abc.Types.ValueKind
    import abc.Types.MethodInfo
    import abc.Types.MethodBody
    import abc.Types.MetadataInfo
    import abc.Types.ClassInfo
    import abc.Types.ScriptInfo
    import abc.Types.InstanceInfo
    
    public final class AbcFormatWriter /*implements IWriter*/ {
        private var abcfile:ABCFile
        private var data:ByteArray
        private var printer : IndentingPrinter;
        
        public function AbcFormatWriter(abcfile:ABCFile) {
            this.abcfile = abcfile
        }

        private function writePrimitiveConstantPool(p:Pool):void {
            printer.println("// "+p.name+" pool count: "+p.items.length+" start "+p.start_pos+", size: "+(p.end_pos - p.start_pos)+" bytes")
            var tablePrinter : TablePrinter = new TablePrinter(2, 2)
            tablePrinter.addRow(["id", "value"])
            var n:int = p.getNumItems()
            for(var i:int=1; i<n; i++)
                tablePrinter.addRow([i, p.get(i).toString()])
            tablePrinter.print(printer);
        }

        private function writeConstantPool(p:Pool, fields : Array):void {
            printer.println("// "+p.name+" pool count: "+p.items.length+" start "+p.start_pos+", size: "+(p.end_pos - p.start_pos)+" bytes")
            var nCols : int = fields.length + 1
            var tablePrinter : TablePrinter = new TablePrinter(nCols, 2)
            tablePrinter.addRow(["id"].concat(fields))
            var n:int = p.getNumItems()
            var r : Array
            var c : uint
            for(var i:int=1; i<n; i++) {
                r = new Array(nCols)
                r[0] = i.toString()
                for (c=1 ; c < nCols; ++c)
                    r[c] = p.get(i)[fields[c - 1]].toString()
                tablePrinter.addRow(r)
            }
            tablePrinter.print(printer);
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
            var bytes : ByteArray = new ByteArray();
            var p : IPrinter = new ByteArrayPrinter(bytes);
            var w : AbcFormatWriter = new AbcFormatWriter(abcfile)
            w.write(p);
            File.writeByteArray(file, bytes);
        }
        
        public static function print(abcfile : ABCFile, p : IPrinter) : void
        {
            var w : AbcFormatWriter = new AbcFormatWriter(abcfile)
            w.write(p);
        }
        
        private function write(p : IPrinter) : void
        {
            printer = new IndentingPrinter(p, 0, 2);
            printer.println("// minor_version " + abcfile.minor_version);
            printer.println("// major_version " + abcfile.major_version);
            
            printer.println("");
            writePrimitiveConstantPool(abcfile.intPool)
            printer.println("");
            writePrimitiveConstantPool(abcfile.uintPool)
            printer.println("");
            writePrimitiveConstantPool(abcfile.numberPool)
            printer.println("");
            writePrimitiveConstantPool(abcfile.stringPool)
            printer.println("");
            writeConstantPool(abcfile.nsPool, ["kind", "name_index", "kindString", "uriString", "apiVersionsString"])
            printer.println("");
            writeConstantPool(abcfile.nssetPool, ["namespaceIds", "namespaceStrings"])
            printer.println("");
            writeConstantPool(abcfile.namePool, ["kind", "name_index", "ns_index", "nsset_index", "kindString", "typeIds", "asString"])
            
            printer.println("");
            writeArray(abcfile.methodInfos,    writeMethodInfo)
            printer.println("");
            writeArray(abcfile.metadataInfos,  writeMetadataInfo)
            printer.println("");
            writeArray(abcfile.instanceInfos,  writeInstanceInfo)
            printer.println("");
            writeArray(abcfile.classInfos,     writeClassInfo)
            printer.println("");
            writeArray(abcfile.scriptInfos,    writeScriptInfo)
            printer.println("");
            writeArray(abcfile.methodBodies,   writeMethodBody)
        }
        
        private function nameIdToString(nameId : int) : String
        {
            if (nameId == 0)
                return "*";
            return abcfile.getMultiname(nameId).toString();
        }

        private function writeMethodInfo(mi:MethodInfo, id:int):void {
            var i : int;
            printer.println("MethodInfo " + id + " {");
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(5, 2);
            tablePrinter.addRow(["derivedName", "", "", "", mi.derivedName])
            tablePrinter.addRow(["param_count", mi.param_count, "", "", ""])
            tablePrinter.addRow(["ret_type", mi.ret_type, "", "", nameIdToString(mi.ret_type)])
                        
            for(i=0; i<mi.param_count; i++)
                tablePrinter.addRow(["param_types["+i+"]", mi.param_types[i], "", "", nameIdToString(mi.param_types[i])])
            tablePrinter.addRow(["name_index", mi.name_index, "", "", abcfile.getString(mi.name_index)])
            tablePrinter.addRow(["flags", mi.flags, "", "", ""])
            if(mi.hasOptional) {
                tablePrinter.addRow(["", "", "", "", ""]);
                tablePrinter.addRow(["optional_count", mi.optional_count, "", "", ""])
                tablePrinter.addRow(["valueKind", "kind", "value_index", "kindString", "valueString"])
                for(i=0; i<mi.optional_count; i++) {
                    var valueKindObject : Object = mi.valuekinds[i].valueObject;
                    tablePrinter.addRow(["value_kind["+i+"]",
                        mi.valuekinds[i].value_kind,
                        mi.valuekinds[i].value_index,
                        Constants.constantKinds[mi.valuekinds[i].value_kind],
                        valueKindObject != null ? valueKindObject.toString() : "null"])
                }
                tablePrinter.addRow(["", "", "", "", ""]);
                
            }
            
            if(mi.hasParamNames)
                for(i=0; i<mi.param_count; i++)
                    tablePrinter.addRow(["param_name["+i+"]", mi.param_names[i], abcfile.getString(mi.param_names[i]), "", ""])
            //try
            //{
                tablePrinter.print(printer);
            //}
            //catch (e : *)
            //{
            //    for each (var v : ValueKind in mi.valuekinds)
            //    {
            //        trace(mi.derivedName);
            //        trace(v);
            //    }
            //}
            printer.unindent();
            printer.println("}");
        }

        private function writeMetadataInfo(mi:MetadataInfo, id:int):void {
            printer.println("MetadataInfo " + id + "{");
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(5, 2);
            tablePrinter.addRow(["name_index", mi.name_index, abcfile.getString(mi.name_index), "", ""])
            tablePrinter.addRow(["values_count", mi.keys, "", "", ""])
            tablePrinter.addRow(["", "", "", "", ""])
            tablePrinter.addRow(["tupleIndex", "key", "value", "keyString", "valueString"]);
            for(var i:int = 0; i<mi.keys.length; i++)
                tablePrinter.addRow(["tuple["+i+"]", mi.keys[i], mi.values[i], mi.keys[i] == 0 ? "" : abcfile.getString(mi.keys[i]), abcfile.getString(mi.values[i])])
            tablePrinter.print(printer);
            printer.unindent();
            printer.println("}");
        }
        
        private function writeTrait(trait : Trait) : void
        {
            printer.println("{");
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(3, 2);
            tablePrinter.addRow(["name_index", trait.name_index, trait.multiname]);
            tablePrinter.addRow(["kind", trait.kind, Constants.traitKinds[trait.kind]]);
            tablePrinter.addRow(["flags", trait.flags, ""]);
            switch(trait.kind)
            {
            case Constants.TRAIT_Slot:
            case Constants.TRAIT_Const:
                tablePrinter.addRow(["slot_id", trait.slot_id, ""]);
                tablePrinter.addRow(["type_index", trait.type_index, trait.typeMultiname]);
                var traitValue : String = trait.valueObject != null ? trait.valueObject.toString() : "";
                tablePrinter.addRow(["value_index", trait.value_index, traitValue]);
                if (trait.value_index != 0) {
                    tablePrinter.addRow(["value_kind", trait.value_kind, Constants.constantKinds[trait.value_kind]]);
                }
                break;
            case Constants.TRAIT_Method:
            case Constants.TRAIT_Getter:
            case Constants.TRAIT_Setter:
                tablePrinter.addRow(["disp_id", trait.disp_id, ""]);
                tablePrinter.addRow(["method_info", trait.method_info, ""]);
                break;
            case Constants.TRAIT_Class:
                tablePrinter.addRow(["slot_id", trait.slot_id, ""]);
                tablePrinter.addRow(["class_info", trait.class_info, ""]);
                break;
            case Constants.TRAIT_Function:
                tablePrinter.addRow(["slot_id", trait.slot_id, ""]);
                tablePrinter.addRow(["method_info", trait.method_info, ""]);
                break;
            
            }
            if (trait.metadata != null)
            {
                for (var i : uint = 0; i < trait.metadata.length; ++i)
                    tablePrinter.addRow(["tmetadata["+i+"]", trait.metadata[i], ""]);
            }
            tablePrinter.print(printer);
            printer.unindent();
            printer.println("}");
        }
        
        private function writeTraits(traits : Traits) : void
        {
            printer.println("traits {");
            printer.indent();
            for each (var trait : Trait in traits.ts) {
                writeTrait(trait);
            }
            printer.unindent();
            printer.println("}");
        }
        
        private function writeInstanceInfo(ii:InstanceInfo, id:int):void {
            printer.println("InstanceInfo " + id + " {");
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(3, 2);
            tablePrinter.addRow(["name_index", ii.name_index, ii.nameString]);
            tablePrinter.addRow(["super_index", ii.super_index, ii.superNameString]);
            tablePrinter.addRow(["flags", ii.flags, ""]);
            if(ii.isProtected)
                tablePrinter.addRow(["protectedNS", ii.protectedNS, abcfile.getNamespace(ii.protectedNS).toString()]);
                
            for(var i:int =0; i<ii.interfaces.length; i++)
                tablePrinter.addRow(["interfaces["+i+"]", ii.interfaces[i], abcfile.getMultiname(ii.interfaces[i]).toString()]);
            tablePrinter.addRow(["iinit_index", ii.iinit_index, ""]);
            tablePrinter.print(printer);
            writeTraits(ii.instance_traits);
            printer.unindent();
            printer.println("}")
        }
        
        private function writeClassInfo(ci:ClassInfo, id:int):void {
            printer.println("ClassInfo " + id + " {")
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(2, 2);
            tablePrinter.addRow(["class name", abcfile.getMultiname(abcfile.instanceInfos.items[id].name_index).toString()]);
            tablePrinter.addRow(["init_index", ci.init_index]);
            tablePrinter.print(printer);
            writeTraits(ci.static_traits)
            printer.unindent();
            printer.println("}");
        }
        
        private function writeScriptInfo(si:ScriptInfo, id:int):void
        {
            printer.println("ScriptInfo " + id + " {")
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(2, 2);
            tablePrinter.addRow(["init_index", si.init_index]);
            tablePrinter.print(printer);
            writeTraits(si.traits)
            printer.unindent();
            printer.println("}");
        }
        
        private function writeMethodBody(mb:MethodBody, id:int):void {
            var i : int;
            printer.println("MethodBody " + id + " {")
            printer.indent();
            var tablePrinter : TablePrinter = new TablePrinter(2, 2);
            tablePrinter.addRow(["derivedName", mb.derivedName])
            tablePrinter.addRow(["method_info", mb.method_info]);
            tablePrinter.addRow(["max_stack", mb.max_stack]);
            tablePrinter.addRow(["max_regs", mb.max_regs]);
            tablePrinter.addRow(["scope_depth", mb.scope_depth]);
            tablePrinter.addRow(["max_scope", mb.max_scope]);
            tablePrinter.addRow(["code_length", mb.code_length]);
            tablePrinter.addRow(["code_offset", mb.code_offset]);
            tablePrinter.print(printer);
            writeTraits(mb.activation_traits);
            printer.println("code {");
            printer.indent();
            
            for(i=0; i<mb.blocks.length; i++) {
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
                    else {
                        switch (inst.getOpcode())
                        {
                        case Constants.OP_debugfile:
                        case Constants.OP_pushstring:
                            constantStr = abcfile.getString(inst.getImm(0))
                            break;
                        case Constants.OP_debugline:
                            constantStr = inst.getImm(0).toString()
                            break;
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
            printer.unindent();
            printer.println("}");
            if (mb.exceptions.length > 0)
            {
                tablePrinter = new TablePrinter(8, 2);
                tablePrinter.addRow(["exception", "start", "end", "target", "type id", "type string", "name id", "name string"])
                for(i=0; i<mb.exceptions.length; i++)
                    tablePrinter.addRow([i, mb.exceptions[i].start, mb.exceptions[i].end, mb.exceptions[i].target, mb.exceptions[i].type_index, abcfile.getMultiname(mb.exceptions[i].type_index), mb.exceptions[i].name_index, abcfile.getMultiname(mb.exceptions[i].name_index)])
                tablePrinter.print(printer);
            }
            printer.unindent();
            printer.println("}");
        }
    }
}
