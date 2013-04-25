/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package abc.Types {

    import flash.utils.ByteArray
    
    import abc.Reader
    import abc.Constants
    import abc.Types.ValueKind
    import abc.Types.ABCObject
    import abc.Types.ABCFile
    
    public final class Trait extends ABCObject {
        public var name_index:int
        public var flagsAndKind:int
        public var slot_id:int
        public var type_index:int
        public var value_index:int
        public var value_kind:int
        public var disp_id:int
        public var class_info:int
        public var method_info:int
        public var metadata:Vector.<int>

        public function Trait(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function get multiname() : ConstantMultiname
        {
            return abcfile.getMultiname(name_index);
        }
        
        public function get flags() : int
        {
            return flagsAndKind >> 4;
        }
        
        public function get kind() : int
        {
            return flagsAndKind & 0x0F;
        }
        
        public function get hasMetadata() : Boolean
        {
            return (flags & Constants.ATTR_metadata) != 0;
        }
        
        public function get isFinal() : Boolean
        {
            return (flags & Constants.ATTR_final) != 0;
        }
        
        public function get isOverride() : Boolean
        {
            return (flags & Constants.ATTR_override) != 0;
        }
        
        public function get typeMultiname() : ConstantMultiname
        {
            return abcfile.getMultiname(type_index);
        }
        
        public function get valueObject() : Object
        {
            if (value_index == 0)
                return null;
            switch(value_kind)
            {
            case Constants.CONSTANT_Utf8:
                return abcfile.getString(value_index);
            case Constants.CONSTANT_Int:
                return abcfile.intPool.get(value_index);
            case Constants.CONSTANT_UInt:
                return abcfile.uintPool.get(value_index);
            case Constants.CONSTANT_Double:
                return abcfile.numberPool.get(value_index);
            case Constants.CONSTANT_Namespace:
                return abcfile.getNamespace(value_index);
            case Constants.CONSTANT_False:
                return false;
            case Constants.CONSTANT_True:
                return true;
            }
            return null;
        }
        
        public function get hasMethodID() : Boolean
        {
            switch (kind)
            {
            case Constants.TRAIT_Method:
            case Constants.TRAIT_Getter:
            case Constants.TRAIT_Setter:
            case Constants.TRAIT_Function:
                return true;
            }
            return false;
        }
        
    }
}