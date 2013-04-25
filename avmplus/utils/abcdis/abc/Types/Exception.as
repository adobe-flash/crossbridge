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
    import abc.Types.Traits
        
    public final class Exception extends ABCObject {
        public var start:int
        public var end:int
        public var target:int
        public var type_index:int
        public var name_index:int
    
        public function Exception(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function toString():String {
            return "{ start: " + start + " end: " + end + " target: " + target + " type: " + type_index + " name: " + name_index + " }"
        }
    }
}