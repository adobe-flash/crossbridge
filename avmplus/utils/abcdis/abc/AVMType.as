/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    
    import flash.utils.ByteArray
    
    import abc.Constants
    import abc.Types.ConstantMultiname
    
    public final class AVMType {
        public static const UNDEF:AVMType = new AVMType(0)
        public static const NULL:AVMType = new AVMType(1)
        public static const VOID:AVMType = new AVMType(2)
        public static const INT:AVMType = new AVMType(3)
        public static const UINT:AVMType = new AVMType(4)
        public static const NUMBER:AVMType = new AVMType(5)
        public static const STRING:AVMType = new AVMType(6)
        public static const NS:AVMType = new AVMType(7)
        public static const BOOL:AVMType = new AVMType(8)
        public static const ARRAY:AVMType = new AVMType(9)
        public static const OBJECT:AVMType = new AVMType(10)
        
        private var v:int
        public var notNull:Boolean
        private var n:ConstantMultiname
        
        public function AVMType(i:int = 0) {
            v = i
            n = null
            notNull = true
        }
        
        public function get isNumeric() : Boolean
        {
            if (this == INT || this == UINT || this == NUMBER || this == BOOL)
                return true;
            return false;
        }
        
        public static function typeFromName(n:ConstantMultiname, notNull:Boolean = true):AVMType {
            if (n.isRTname() || n.isRTns())
            {
                trace(n.toString() + " : " + n.isRTname() + " , " + n.isRTns())
                return NULL;
            }
            
            if(n.kind == Constants.CONSTANT_Qname && n.abcfile.getNamespace(n.ns_index).kind == Constants.CONSTANT_PackageNs) {
                var name:String = n.abcfile.getString(n.name_index)
                switch(name) {
                    case "null":
                        return NULL
                    case "void":
                        return VOID
                    case "int":
                        return INT
                    case "uint":
                        return UINT
                    case "Number":
                        return NUMBER
                    case "String":
                        return STRING
                    case "OBJECT":
                        return OBJECT
                    case "Array":
                        return ARRAY
                    case "Boolean":
                        return BOOL
                }
            }
            
            var t:AVMType = OBJECT.clone()
            t.n = n
            t.notNull = notNull
            return t
        }
        
        public function checkEquals(t:AVMType):Boolean {
            return (v === t.v && notNull === t.notNull)
        }
        
        public function clone():AVMType {
            var t:AVMType = new AVMType(v)
            t.n = n
            t.notNull = notNull
            return t
        }
        
        public function toString():String {
            var s:String = null
            switch(v) {
                case 0:
                    s = "undef"
                    break
                case 1:
                    s = "null"
                    break
                case 2:
                    s = "void"
                    break
                case 3:
                    s = "int"
                    break
                case 4:
                    s = "uint"
                    break
                case 5:
                    s = "number"
                    break
                case 6:
                    s = "string"
                    break
                case 7:
                    s = "ns"
                    break
                case 8:
                    s = "bool"
                    break
                case 9:
                    s = "array"
                    break
                case 10:
                    if(n == null)
                        s = "object"
                    else
                        s = n.toString(true)
                    break
                default:
                    s = "unknown("+v+")"
                    break
            }
            
            return s + (notNull ? "" : "?")
        }
    }
}