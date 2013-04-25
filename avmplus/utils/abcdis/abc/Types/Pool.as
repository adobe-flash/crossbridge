/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.ByteArray
    import flash.utils.Endian
    
    import abc.Reader
    
    public final class Pool {
        public var start_pos:uint
        public var end_pos:uint
        public var name:String
        
        public var items:Array = []
        public var refcounts:Vector.<uint> = new Vector.<uint>()
        
        public function Pool(n:String = "") {
            name = n
        }
        
        public function add(item:Object):void {
            items.push(item)
            refcounts.push(0)
        }
        
        public function get(i:uint):Object {
            return items[i]
        }
        
        public function getItems():Array {
            return items
        }
        
        public function getNumItems():int {
            return items.length
        }
        
        public function start(data:ByteArray, def:Object = null):void {
            if(def != null)
                add(def)
            start_pos = data.position
        }

        public function end(data:ByteArray):void {
            end_pos = data.position
        }
        
        public function indexOf(item : Object) : int
        {
            return items.indexOf(item);
        }
        
        public function getSizeInBytes() : uint
        {
            return end_pos - start_pos;
        }
            
        /*public function dump(name:String):void {
            print("// " + name + " pool count: " + items.length + " start " + start_pos + ", size: " + (end_pos - start_pos) + " bytes")
            
            var pos:uint = 0
            for each(var i in items)
                print("[" + (pos++) + "] = " + i)
        }
        
        public function dumpLarge(name:String):void {
            print("// " + name + " pool count: " + items.length + " start " + start_pos + ", size: " + (end_pos - start_pos) + " bytes")
            
            var pos:uint = 0
            for each(var i in items) {
                print(i.toString(pos++))
                print("")
            }
        }*/
        
        /*
        public static function readPool(data:ByteArray, abcfile:ABCFile, n:String, d:Object, c:IReadABCObject):Pool {
            var p:Pool = new Pool()
            var i:int = Reader.readU30(data)
            
            p.add(d);
            while(--i > 0)
                p.add(c.read(data, abcfile))
            
            return p
        }
        */
    }
}