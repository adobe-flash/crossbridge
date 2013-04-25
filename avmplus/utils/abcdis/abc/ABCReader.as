/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    
    import flash.utils.ByteArray
    import flash.utils.Endian

    import Utils
    import abc.Reader
    import abc.IReader
    import abc.MethodReference
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
    
    public final class ABCReader implements IReader {
        
        private var data:ByteArray
        private var abcfile:ABCFile
        
        public function ABCReader(data:ByteArray) {
            this.data = data
        }
        
        public function supportsContent():Boolean {
            data.endian = Endian.LITTLE_ENDIAN
            var isSupported:Boolean = data.readShort() == 16 && data.readShort() == 46
            data.position = 0
            return isSupported
        }
        
        public function readABC():Array {
            
            if(!supportsContent()) {
                Utils.error("ABCReader can't handle the supplied data")
            }
            
            abcfile = new ABCFile()
            
            data.endian = Endian.LITTLE_ENDIAN
            abcfile.minor_version = data.readShort()
            abcfile.major_version = data.readShort()
            
            var i:int, j:int

            abcfile.intPool.start(data, 0)
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.intPool.add(Reader.readS32(data))
            abcfile.intPool.end(data)
                
            abcfile.uintPool.start(data, 0)
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.uintPool.add(Reader.readU32(data))
            abcfile.uintPool.end(data)
                
            abcfile.numberPool.start(data, Number.NaN)
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.numberPool.add(data.readDouble())
            abcfile.numberPool.end(data)
                
            abcfile.stringPool.start(data, "")
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.stringPool.add(data.readUTFBytes(Reader.readU30(data)))
            abcfile.stringPool.end(data)
                
            abcfile.nsPool.start(data, ConstantNamespace.publicNS(abcfile))
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.nsPool.add(readConstantNamespace())
            abcfile.nsPool.end(data)
                
            abcfile.nssetPool.start(data, ConstantNamespaceSet.emptySet(abcfile))
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.nssetPool.add(readConstantNamespaceSet())
            abcfile.nssetPool.end(data)
                
            abcfile.namePool.start(data, ConstantMultiname.anyName(abcfile))
            i = Reader.readU30(data)
            while(--i > 0)
                abcfile.namePool.add(readConstantMultiname())
            abcfile.namePool.end(data)
                
            abcfile.methodInfos.start(data)
            i = Reader.readU30(data)
            while(--i >= 0)
                abcfile.methodInfos.add(readMethodInfo())
            abcfile.methodInfos.end(data)
            
            abcfile.metadataInfos.start(data)
            i = Reader.readU30(data)
            while(--i >= 0)
                abcfile.metadataInfos.add(readMetadataInfo())
            abcfile.metadataInfos.end(data)
                
            abcfile.instanceInfos.start(data)
            i = j = Reader.readU30(data)
            while(--i >= 0)
                abcfile.instanceInfos.add(readInstanceInfo())
            abcfile.instanceInfos.end(data)
                
            abcfile.classInfos.start(data)
            while(--j >= 0)
                abcfile.classInfos.add(readClassInfo())
            abcfile.classInfos.end(data)
                
            abcfile.scriptInfos.start(data)
            i = Reader.readU30(data)
            while(--i >= 0)
                abcfile.scriptInfos.add(readScriptInfo())
            abcfile.scriptInfos.end(data)
                
            abcfile.methodBodies.start(data)
            i = Reader.readU30(data)
            while(--i >= 0) {
                var mb : MethodBody = readMethodBody()
                abcfile.methodBodies.add(mb)
                abcfile.methodBodyMap[abcfile.methodInfos.get(mb.method_info)] = mb
            }
            abcfile.methodBodies.end(data)
            
            abcfile.bytesLeft = (data.length - data.position)
            
            return [abcfile];
        }
        
        public function readABCBytes() : Array
        {
            return [data];
        }

        
        public function readConstantNamespace():ConstantNamespace {
            var ns:ConstantNamespace = new ConstantNamespace(abcfile)
            ns.kind = data.readByte()
            
            switch(ns.kind) {
                case 5:
                case 8:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                    ns.name_index = Reader.readU30(data)
            }
            
            return ns
        }
        
        public function readConstantNamespaceSet():ConstantNamespaceSet {
            var nsset:ConstantNamespaceSet = new ConstantNamespaceSet(abcfile)
            
            var i:int = Reader.readU30(data)
            while(--i >= 0)
                nsset.namespaces.push(Reader.readS32(data))
                
            return nsset
        }
        
        public function readConstantMultiname():ConstantMultiname {
            var name:ConstantMultiname = new ConstantMultiname(abcfile)
            
            name.kind = data.readByte()
            switch(name.kind) {
                case Constants.CONSTANT_Qname:
                case Constants.CONSTANT_QnameA:
                    name.ns_index = Reader.readU30(data)
                    name.name_index = Reader.readU30(data)
                    break
                case Constants.CONSTANT_Multiname:
                case Constants.CONSTANT_MultinameA:
                    name.name_index = Reader.readU30(data)
                    name.nsset_index = Reader.readU30(data)
                    break
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                    name.name_index = Reader.readU30(data)
                    break
                case Constants.CONSTANT_MultinameL:
                case Constants.CONSTANT_MultinameLA:
                    name.nsset_index = Reader.readU30(data)
                    break
                case Constants.CONSTANT_TypeName:
                    name.name_index = Reader.readU30(data)
                    name.types = new Vector.<int>()
                    var c:int = Reader.readU30(data)
                    while(--c >= 0) {
                        name.types.push(Reader.readU30(data))
                    }
                    break
                case Constants.CONSTANT_RTQnameL:
                case Constants.CONSTANT_RTQnameLA:
                    break
                default:
                    Utils.error("unknown kind: " + name.kind)
                    break
            }
            return name
        }
        
        public function readClassInfo():ClassInfo {
            var ci:ClassInfo = new ClassInfo(abcfile)
            
            ci.init_index = Reader.readU30(data)
            abcfile.methodInfos.get(ci.init_index).reference = new MethodReference(MethodReference.CLASS_INIT, ci);
            ci.static_traits = readTraits()
            
            for each ( var t : Trait in ci.static_traits.ts )
            {
                if (t.hasMethodID)
                    abcfile.methodInfos.get(t.method_info).reference = new MethodReference(MethodReference.CLASS_METHOD, ci, t);
            }
            
            return ci
        }
        
        public function readException():Exception {
            var e:Exception = new Exception(abcfile)
            
            e.start         = Reader.readU30(data)
            e.end           = Reader.readU30(data)
            e.target        = Reader.readU30(data)
            e.type_index    = Reader.readU30(data)
            e.name_index    = Reader.readU30(data)
            
            return e
        }
        
        public function readInstanceInfo():InstanceInfo {
            var ii:InstanceInfo = new InstanceInfo(abcfile)
            
            ii.name_index = Reader.readU30(data)
            ii.super_index = Reader.readU30(data)
            ii.flags = data.readByte()
            
            if(ii.isProtected)
                ii.protectedNS = Reader.readU30(data)
                
            var i:int = Reader.readU30(data)
            while(--i >= 0)
                ii.interfaces.push(Reader.readU30(data))
                
            ii.iinit_index = Reader.readU30(data)
            abcfile.methodInfos.get(ii.iinit_index).reference = new MethodReference(MethodReference.INSTANCE_INIT, ii);
            ii.instance_traits = readTraits()
            
            for each ( var t : Trait in ii.instance_traits.ts )
            {
                if (t.hasMethodID)
                    abcfile.methodInfos.get(t.method_info).reference = new MethodReference(MethodReference.INSTANCE_METHOD, ii, t);
            }
            
            return ii
        }
        
        public function readMetadataInfo():MetadataInfo {
            var i : int;
            var mi:MetadataInfo = new MetadataInfo(abcfile)
            
            mi.name_index = Reader.readU30(data)
            
            var n:int = Reader.readU30(data)
            
            for(i=0; i<n; i++)
                mi.keys.push(Reader.readU30(data))
            
            for(i=0; i<n; i++)
                mi.values.push(Reader.readU30(data))
            
            return mi
        }
        
        public function readMethodInfo():MethodInfo {
            var mi:MethodInfo = new MethodInfo(abcfile)
            var i : int;
            
            mi.param_count = Reader.readU30(data)
            mi.ret_type = Reader.readU30(data)
            
            i = mi.param_count
            while(i-- > 0)
                mi.param_types.push(Reader.readU30(data))
                
            mi.name_index = Reader.readU30(data)
            mi.flags = data.readByte()
                
            if(mi.hasOptional) {
                mi.optional_count = Reader.readU30(data)
                mi.valuekinds = new Vector.<ValueKind>()
                i = mi.optional_count
                while(i-- > 0)
                    mi.valuekinds.push(readValueKind())
            }
            
            if(mi.hasParamNames) {
                mi.param_names = new Vector.<int>()
                i = mi.param_count
                while(i-- > 0)
                    mi.param_names.push(Reader.readU30(data))
            }
            
            return mi
        }
        
        private function updateFunctionLiteralReferences(instrs:Vector.<Instruction>) : void
        {
            for each (var instr : Instruction in instrs)
            {
                if (instr.getOpcode() == Constants.OP_newfunction)
                {
                    var methodID : int = instr.getImm(0);
                    abcfile.methodInfos.get(methodID).reference = new MethodReference(MethodReference.FUNCTION_LITERAL, instr);
                }
            }
        }
        
        public function readMethodBody():MethodBody {
            var mb:MethodBody = new MethodBody(abcfile)
            
            mb.method_info  = Reader.readU30(data)
            mb.max_stack    = Reader.readU30(data)
            mb.max_regs     = Reader.readU30(data)
            mb.scope_depth  = Reader.readU30(data)
            mb.max_scope    = Reader.readU30(data)
            mb.code_length  = Reader.readU30(data)
            mb.code         = new ByteArray()
            mb.code.endian  = Endian.LITTLE_ENDIAN
            mb.code_offset  = data.position;
            data.readBytes(mb.code, 0, mb.code_length)
            
            mb.instrs = Instruction.readAll(mb.code)
            var n:int = Reader.readU30(data)
            while(--n >= 0)
                mb.exceptions.push(readException())
            mb.activation_traits = readTraits()
            
            mb.blocks = BasicBlock.computeCFG(mb.instrs, mb.exceptions)
            updateFunctionLiteralReferences(mb.instrs)
                
            
            return mb
        }
        
        public function readScriptInfo():ScriptInfo {
            var si:ScriptInfo = new ScriptInfo(abcfile)
            
            si.init_index = Reader.readU30(data)
            abcfile.methodInfos.get(si.init_index).reference = new MethodReference(MethodReference.SCRIPT_INIT, si);
            si.traits = readTraits()
            
            for each ( var t : Trait in si.traits.ts )
            {
                if (t.hasMethodID)
                    abcfile.methodInfos.get(t.method_info).reference = new MethodReference(MethodReference.SCRIPT_METHOD, si, t);
            }
            
            return si
        }
        
        public function readTrait():Trait {
            var t:Trait = new Trait(abcfile)
            
            t.name_index = Reader.readU30(data)
            t.flagsAndKind = data.readUnsignedByte();
            switch(t.kind) {
                case Constants.TRAIT_Slot:
                case Constants.TRAIT_Const:
                    t.slot_id = Reader.readU30(data)
                    t.type_index = Reader.readU30(data)
                    t.value_index = Reader.readU30(data)
                    if(t.value_index != 0)
                        t.value_kind = data.readByte()
                    break
                case Constants.TRAIT_Method:
                case Constants.TRAIT_Getter:
                case Constants.TRAIT_Setter:
                    t.disp_id = Reader.readU30(data)
                    t.method_info = Reader.readU30(data)
                    break
                case Constants.TRAIT_Class:
                    t.slot_id = Reader.readU30(data)
                    t.class_info = Reader.readU30(data)
                    break
                case Constants.TRAIT_Function:
                    t.slot_id = Reader.readU30(data)
                    t.method_info = Reader.readU30(data)
                    break
            }
            
            if (t.hasMetadata) {
                var i:int = Reader.readU30(data)
                t.metadata = new Vector.<int>()
                while(--i >= 0)
                    t.metadata.push(Reader.readU30(data))
            }
            
            return t
        }

        public function readTraits():Traits {
            var t:Traits = new Traits(abcfile)
            var i:int = Reader.readU30(data)
            
            while(--i >= 0)
                t.ts.push(readTrait())
                
            return t
        }
        
        public function readValueKind():ValueKind {
            var vk:ValueKind = new ValueKind(abcfile)
            
            vk.value_index = Reader.readU30(data)
            vk.value_kind = data.readUnsignedByte()
            
            return vk
        }
    }
}