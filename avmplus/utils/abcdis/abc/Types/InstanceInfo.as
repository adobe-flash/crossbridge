/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.ByteArray
    
    import abc.Reader
    import abc.Types.ABCObject
    import abc.Types.ABCFile
    import abc.Constants;
    
    public final class InstanceInfo extends ABCObject {
        public var name_index:int
        public var super_index:int
        public var flags:int
        public var protectedNS:int
        public var interfaces:Vector.<int> = new Vector.<int>()
        public var iinit_index:int
        public var instance_traits:Traits
    
        public function InstanceInfo(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function get isProtected():Boolean {
            return (flags & Constants.CLASS_FLAG_protected) != 0;
        }
        
        public function get isInterface():Boolean {
            return (flags & Constants.CLASS_FLAG_interface) != 0;
        }
        
        public function get isFinal():Boolean {
            return (flags & Constants.CLASS_FLAG_final) != 0;
        }
        
        public function get isDynamic():Boolean {
            return (flags & Constants.CLASS_FLAG_sealed) == 0;
        }
        
        public function get nameString() : String
        {
            return abcfile.getMultiname(name_index).toString();
        }
        
        public function get superNameString() : String
        {
            return abcfile.getMultiname(super_index).toString();
        }
    }
}