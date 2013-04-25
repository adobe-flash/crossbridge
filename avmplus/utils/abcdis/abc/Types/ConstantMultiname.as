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
    
    public final class ConstantMultiname extends ABCObject {
        public var kind:int        = 0
        public var name_index:int  = 0
        public var ns_index:int    = 0
        public var nsset_index:int = 0
        
        public var types:Vector.<int>
        
        public function ConstantMultiname(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function isRTname():Boolean {
            switch(kind) {
                case Constants.CONSTANT_MultinameL:
                case Constants.CONSTANT_RTQnameL:
                case Constants.CONSTANT_RTQnameLA:
                    return true
            }
            return false
        }
        
        public function isRTns():Boolean {
            switch(kind) {
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                case Constants.CONSTANT_RTQnameL:
                case Constants.CONSTANT_RTQnameLA:
                    return true
            }
            return false
        }

        public function toString(compact:Boolean = false) : String {
            if (this.isAny)
                return "*";
            var k:String = Constants.constantKinds[kind]
            var s:String = ""
            var nsset : String;
            switch(kind) {
                case Constants.CONSTANT_Qname:
                case Constants.CONSTANT_QnameA:
                    var ns:String = abcfile.getNamespace(ns_index).toString(compact)
                    s = (ns == null || ns == "" ? "" : ns + "::") + abcfile.getString(name_index)
                    break
                case Constants.CONSTANT_Multiname:
                case Constants.CONSTANT_MultinameA:
                    nsset = abcfile.getNamespaceSet(nsset_index).toString()
                    s = (nsset == null || nsset == "" ? "" : nsset + "::") + abcfile.getString(name_index)
                    break
                case Constants.CONSTANT_RTQname:
                case Constants.CONSTANT_RTQnameA:
                    s = abcfile.getString(name_index)
                    break
                case Constants.CONSTANT_MultinameL:
                    s = abcfile.getNamespaceSet(nsset_index).toString()
                    break
                case Constants.CONSTANT_TypeName:
                    var typeStrings : Array = new Array();
                    for each (var typeId : int in types)
                        typeStrings.push(abcfile.getMultiname(typeId).toString())
                    s = abcfile.getMultiname(name_index).toString() + ".<" + typeStrings.join(", ") + ">";
                    break
            }
            
            return compact ? s : (k + "(" + s + ")");
        }
        
        public function get kindString() : String
        {
            return Constants.constantKinds[kind]
        }
        
        public function get typeIds() : String
        {
            if (types != null)
                return "["+types.join(", ")+"]";
            return "[]";
        }
        
        public function get asString() : String
        {
            return toString();
        }
        
        public function get nameString() : String
        {
            return abcfile.getString(name_index);
        }
        
        public function get ns() : ConstantNamespace
        {
            return abcfile.getNamespace(ns_index)
        }
        
        public function get nsSet() : ConstantNamespaceSet
        {
            return abcfile.getNamespaceSet(nsset_index)
        }
        
        public function get isAny() : Boolean
        {
            return abcfile.getMultiname(0) === this;
        }
        
        public static function anyName(abcfile:ABCFile) : ConstantMultiname
        {
            return new ConstantMultiname(abcfile)
        }
    }
}