/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{

    import avmplus.System
    import avmplus.File
    import flash.utils.ByteArray

    public final class Utils
    {

        public static function warning(msg:String) : void {
            trace("// warning: " + msg)
        }
        
        public static function error(msg:String) : void {
            throw ("// ERROR: " + msg)
        }
        
        public static function padString(msg:String, len:int):String {
            if(msg.length < len)
                return padString(msg + " ", len)
            else
                return msg
        }
        
        // Sort a numeric array, then clone it ignoring duplicates
        public static function sortAndRemoveDuplicates(xs:Array):Array {
            xs.sort(Array.NUMERIC)
            var ys:Array = new Array()
            
            // handle empty arrays
            if(xs.length == 0)
                return ys
            
            // Always accept the first element
            var cur : Object = xs[0]
            ys.push(cur)
            
            // Accept all elements which aren't equal to their immediate predecessor
            for each(var x : Object in xs) {
                if(x != cur) {
                    cur = x
                    ys.push(cur)
                }
            }
            
            return ys
        }
        
        public static function getProps(p:String, xs:Array):Array {
            var ys:Array = new Array()
            for(var x : Object in xs) {
                ys.push(x[p])
            }
            return ys
        }
        
        public static function writeStringToFile(s:String, f:String):void {
            var ba:ByteArray = new ByteArray()
            ba.writeUTFBytes(s)
            File.writeByteArray(f, ba);
        }
        
        public static function escapeGraphvizLabel(s:String):String {
            var r:String = ""
            for(var i:int = 0; i<s.length; i++) {
                var x : String = s.charAt(i)
                switch(x) {
                    case "\n": r += "\\l"
                        break
                    case "\r": r += "\\r"
                        break
                    case "\t": r += "\\t"
                        break
                    case "\"": r += "\\\""
                        break
                    default:
                        r += x
                }
            }
            return r
        }
        
        public static function escapeGraphvizHTMLLabel(s:String):String {
            var r:String = ""
            for(var i:int = 0; i<s.length; i++) {
                var x : String = s.charAt(i)
                switch(x) {
                    case "\n": r += "<BR ALIGN='LEFT'/>"
                        break
                    case "\r": r += "\\r"
                        break
                    case "\t": r += "\\t"
                        break
                    case "\"": r += "\\\""
                        break
                    default:
                        r += x
                }
            }
            return r
        }
    }
}