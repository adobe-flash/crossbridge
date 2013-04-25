/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.ByteArray
    import abc.Reader
    import abc.Constants
    import abc.Types.ABCObject
    import abc.Types.ABCFile
    
    public final class MetadataInfo extends ABCObject {
        public var name_index:int
        public var keys:Vector.<int> = new Vector.<int>()
        public var values:Vector.<int> = new Vector.<int>()
        
        public function MetadataInfo(abcfile:ABCFile) {
            super(abcfile)
        }
    }
}