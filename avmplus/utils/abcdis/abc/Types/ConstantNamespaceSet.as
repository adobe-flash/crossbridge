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
    
    public final class ConstantNamespaceSet extends ABCObject {
        public var namespaces:Vector.<int> = new Vector.<int>()

        public function ConstantNamespaceSet(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function toString():String {
            return "{" + namespaces.join(",") + "}"
        }
        
        public function get namespaceIds() : String
        {
            return namespaces.join(", ")
        }
        
        public function get namespaceStrings() : String
        {
            var a : Array = new Array(namespaces.length)
            for (var i : uint = 0; i < namespaces.length; ++i)
                a[i] = abcfile.getNamespace(namespaces[i]).toString()
            return a.join(", ")
        }
        
        public function get constantNamespaces() : Vector.<ConstantNamespace>
        {
            var result : Vector.<ConstantNamespace> = new Vector.<ConstantNamespace>(namespaces.length, true);
            var i : uint = 0;
            for each (var nsId : int in namespaces)
            {
                result[i] = abcfile.getNamespace(nsId);
                ++i;
            }
            return result;
        }
        
        public static function emptySet(abcfile:ABCFile):ConstantNamespaceSet {
            return new ConstantNamespaceSet(abcfile)
        }
    }
}