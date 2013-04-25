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
    
    public final class ValueKind extends ABCObject {
        public var value_index:int
        public var value_kind:int
        
        public function ValueKind(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function toString() : String
        {
            return "index=" + value_index + " value_kind=" + value_kind;
        }
        
        public function get valueObject() : Object
        {
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
    }
}