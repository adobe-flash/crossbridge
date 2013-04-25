/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.Dictionary;

    import abc.Types.ConstantNamespace
    import abc.Types.ConstantMultiname
    import abc.Types.ConstantNamespaceSet
    import abc.Types.MethodInfo
    import abc.Types.MethodBody
    import abc.Types.MetadataInfo
    import abc.Types.ClassInfo
    import abc.Types.ScriptInfo
    import abc.Types.InstanceInfo
           
    public final class ABCFile {
        public var abcName : String;
        public var minor_version:int
        public var major_version:int
        
        public var intPool:Pool = new Pool("Int")
        public var uintPool:Pool = new Pool("Uint")
        public var numberPool:Pool = new Pool("Number")
        public var stringPool:Pool = new Pool("String")
        public var nsPool:Pool = new Pool("Namespace")
        public var nssetPool:Pool = new Pool("NamespaceSet")
        public var namePool:Pool = new Pool("Name")
        public var methodInfos:Pool = new Pool("MethodInfo")
        public var metadataInfos:Pool = new Pool("MetadataInfo")
        public var instanceInfos:Pool = new Pool("InstanceInfo")
        public var classInfos:Pool = new Pool("ClassInfo")
        public var scriptInfos:Pool = new Pool("ScriptInfo")
        public var methodBodies:Pool = new Pool("MethodBody")
        
        public var methodBodyMap:Dictionary = new Dictionary()
        
        public var bytesLeft:int = 0
        
        public function getString(i:uint):String {
            return stringPool.get(i) as String
        }
        
        public function getMultiname(i:uint):ConstantMultiname {
            return namePool.get(i) as ConstantMultiname
        }
        
        public function getNamespace(i:uint):ConstantNamespace {
            return nsPool.get(i) as ConstantNamespace
        }
        
        public function getNamespaceSet(i:uint):ConstantNamespaceSet {
            return nssetPool.get(i) as ConstantNamespaceSet
        }
        
        public function getMethodInfo(i:uint):MethodInfo {
            return methodInfos.get(i) as MethodInfo
        }
        
        public function ABCFile() {
        }
        
        public function getMethodBodies():Array {
            return methodBodies.getItems()
        }
        
        /*public function dump():void {
        
            print("// ABC minor_version: " + minor_version)
            print("// ABC major_version: " + major_version)
            
            intPool.dump("Int")
            uintPool.dump("Uint")
            numberPool.dump("Number")
            stringPool.dump("String")
            nsPool.dump("Namespace")
            nssetPool.dump("NamespaceSet")
            namePool.dump("Multiname")
            
            methodInfos.dumpLarge("MethodInfo")
            metadataInfos.dumpLarge("MetadataInfo")
            instanceInfos.dumpLarge("InstanceInfo")
            classInfos.dumpLarge("ClassInfo")
            scriptInfos.dumpLarge("ScriptInfo")
            methodBodies.dumpLarge("MethodBody")
            
            if(bytesLeft > 0)
                warning("found " + bytesLeft + " unused bytes at EOF")
        }*/
    }

    
}