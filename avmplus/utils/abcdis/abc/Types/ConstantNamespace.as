/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {
    
    import flash.utils.ByteArray
    
    import abc.Constants
    import abc.Reader
    import abc.Types.ABCObject
    import abc.Types.ABCFile
    
    public final class ConstantNamespace extends ABCObject {
        public var kind:int
        public var name_index : int = 0

        public function ConstantNamespace(abcfile:ABCFile) {
            super(abcfile)
        }
        
        private function stripVersioningChars(s : String, versions : Vector.<int>) : String
        {
            if (s.length == 0)
                return s;
            var i : uint = s.length - 1;
            var c : int = s.charCodeAt(i);
            while ((i > 0) && (c > 0xE000))
            {
                if ((c > 0xE294) && (versions != null))
                    versions.push(c - 0xE000);
                --i;
                c = s.charCodeAt(i)
            }
            if (c <= 0xE000)
                ++i;
            return s.substring(0, i);
        }
        
        public function toString(compact:Boolean = false):String {
            var k:String = Constants.constantKinds[kind]
            return (compact ? "" : k + " ") + uriString;
        }
        
        public function get uriString() : String
        {
            switch(kind) {
                case Constants.CONSTANT_PrivateNs:
                case Constants.CONSTANT_Namespace:
                case Constants.CONSTANT_PackageNs:
                case Constants.CONSTANT_PackageInternalNs:
                case Constants.CONSTANT_ProtectedNs:
                case Constants.CONSTANT_StaticProtectedNs:
                case Constants.CONSTANT_StaticProtectedNs2:
                    return stripVersioningChars(abcfile.getString(name_index), null)
                default:
                    return ""
            }
        }
        
        public function get apiVersions() : Vector.<int>
        {
            var result : Vector.<int> = new Vector.<int>();
            switch(kind) {
                case Constants.CONSTANT_PrivateNs:
                case Constants.CONSTANT_Namespace:
                case Constants.CONSTANT_PackageNs:
                case Constants.CONSTANT_PackageInternalNs:
                case Constants.CONSTANT_ProtectedNs:
                case Constants.CONSTANT_StaticProtectedNs:
                case Constants.CONSTANT_StaticProtectedNs2:
                    stripVersioningChars(abcfile.getString(name_index), result);
            }
            return result;
        }
        
        public function get apiVersionsString() : String
        {
            var versions : Vector.<int> = apiVersions;
            var n : int = versions.length;
            var v : Vector.<String> = new Vector.<String>(versions.length, true);
            for (var i : int = 0; i < n ; ++i)
                v[i] = Constants.versionStrings[versions[i] - Constants.firstVersion]
            return "[" + v.join(", ") + "]"
        }
        
        public function get kindString() : String
        {
            return Constants.constantKinds[kind];
        }
        
        public static function publicNS(abcfile:ABCFile):ConstantNamespace {
            var ns:ConstantNamespace = new ConstantNamespace(abcfile)
            ns.kind = Constants.CONSTANT_Namespace
            ns.name_index = 0
            return ns
        }
    }
}