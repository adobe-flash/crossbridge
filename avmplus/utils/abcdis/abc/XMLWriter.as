/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    
    import flash.utils.ByteArray
    import flash.utils.Endian
    
    import Utils;
    import abc.Reader
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
    
    public final class XMLWriter /*implements IWriter*/ {
        private var abcfile:ABCFile
        private var file:String
        
        public function XMLWriter(abcfile:ABCFile, file:String) {
            this.abcfile = abcfile
            this.file = file
        }

        private function writePool(root:XMLList, p:Pool, skipFirst:Boolean = true, writefunc:Function = null):void {
            var n:int = p.getNumItems()
            root.@count = n
            for(var i:int=(skipFirst ? 1 : 0); i<n; i++) {
                var v : Object = p.get(i)
                root.appendChild((writefunc == null ? <item>{v}</item> : writefunc.call(this, v)))
            }
        }
        
        public static function writeFile(abcfile:ABCFile, file:String) : void {
            var xw:XMLWriter = new XMLWriter(abcfile, file)
            xw.write()
        }
        
        public function write():void {
            var root:XML = <abc/>
            
            root.@minor_version = abcfile.minor_version
            root.@major_version = abcfile.major_version
            var n:int
            
            writePool(root.intPool,         abcfile.intPool)
            writePool(root.uintPool,        abcfile.uintPool)
            writePool(root.numberPool,      abcfile.numberPool)
            writePool(root.stringPool,      abcfile.stringPool)
            writePool(root.nsPool,          abcfile.nsPool,         true, writeConstantNamespace)
            writePool(root.nssetPool,       abcfile.nssetPool,      true, writeConstantNamespaceSet)
            writePool(root.namePool,        abcfile.namePool,       true, writeConstantMultiname)
            writePool(root.methodInfos,     abcfile.methodInfos,    false, writeMethodInfo)
            writePool(root.metadataInfos,   abcfile.metadataInfos,  false, writeMetadataInfo)
            writePool(root.instanceInfos,   abcfile.instanceInfos,  false, writeInstanceInfo)
            writePool(root.classInfos,      abcfile.classInfos,     false, writeClassInfo)
            writePool(root.methodBodies,    abcfile.methodBodies,   false, writeMethodBody)
            
            Utils.writeStringToFile(root.toXMLString() + "\n", file)
        }

        private function writeConstantNamespace(ns:ConstantNamespace):XML {
            return <ConstantNamespace kind={ns.kind} name_index={ns.name_index}/>
        }
        
        private function writeConstantNamespaceSet(nsset:ConstantNamespaceSet):XML {
            var root:XML = <ConstantNamespaceSet count={nsset.namespaces.length}/>
            for each(var i:int in nsset.namespaces) {
                root.appendChild(<ns id={i}/>)
            }
            return root
        }
        
        private function writeConstantMultiname(n:ConstantMultiname):XML {
            var root:XML = <ConstantMultiname kind={n.kind} name_index={n.name_index} ns_index={n.ns_index} nsset_index={n.nsset_index}/>
            if(n.types != null && n.types.length > 0) {
                for each(var i:int in n.types) {
                    root.appendChild(<type id={i}/>)
                }
            }
            return root
        }

        private function writeMethodInfo(o:Object):XML {
            return <methodInfo/>
        }
        
        private function writeMetadataInfo(o:Object):XML {
            return <MetadataInfo/>
        }
        
        private function writeInstanceInfo(o:Object):XML {
            return <InstanceInfo/>
        }
        
        private function writeClassInfo(ci:ClassInfo):XML {
            return <ClassInfo init_index={ci.init_index}/>
        }
        
        private function writeScriptInfo(si:ScriptInfo):XML {
            return <ScriptInfo init_index={si.init_index}/>
        }
        
        private function writeMethodBody(o:Object):XML {
            return <MethodBody/>
        }
    }
}