/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.ByteArray
    
    import abc.Reader
    import abc.Constants
    import abc.MethodReference;
    import abc.Types.ValueKind
    import abc.Types.ABCObject
    import abc.Types.ABCFile
    import abc.Instruction;
    
    public final class MethodInfo extends ABCObject {
        public var param_count:int
        public var ret_type:int
        public var param_types:Vector.<int> = new Vector.<int>()
        public var name_index:int
        public var flags:int
        public var optional_count:int
        public var valuekinds:Vector.<ValueKind>
        public var param_names:Vector.<int>
        
        public var reference : MethodReference;
        
        public function get hasOptional():Boolean {
            return (flags & Constants.HAS_OPTIONAL) != 0;
        }
        
        public function get hasParamNames():Boolean {
            return (flags & Constants.HAS_ParamNames) != 0;
        }
        
        public function get derivedName() : String
        {
            if (reference != null)
            {
                return reference.toString();
            }
            return "";
        }
        
        public function get needsArguments() : Boolean
        {
            return (flags & Constants.NEED_ARGUMENTS) != 0;
        }
        
        public function get needsActivation() : Boolean
        {
            return (flags & Constants.NEED_ACTIVATION) != 0;
        }
        
        public function get needsRest() : Boolean
        {
            return (flags & Constants.NEED_REST) != 0;
        }
        
        public function get ignoreRest() : Boolean
        {
            return (flags & Constants.IGNORE_REST) != 0;
        }
        
        public function get isNative() : Boolean
        {
            return (flags & Constants.NATIVE) != 0;
        }
        
        public function get setDXNS() : Boolean
        {
            return (flags & Constants.SET_DXNS) != 0;
        }

        public function MethodInfo(abcfile:ABCFile) {
            super(abcfile)
        }
    }
}
