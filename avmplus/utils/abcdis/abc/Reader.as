/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {

    import avmplus.File

    import flash.utils.ByteArray
    
    import Utils
    import abc.Types.ABCFile
    import abc.ABCReader
    import SWF.SWFReader
    import SWC.SWCReader

    public final class Reader {

        public static function readS32(data:ByteArray):int {
            var result:int = data.readUnsignedByte();
            if (!(result & 0x00000080))
                return result;
            result = result & 0x0000007f | data.readUnsignedByte()<<7;
            if (!(result & 0x00004000))
                return result;
            result = result & 0x00003fff | data.readUnsignedByte()<<14;
            if (!(result & 0x00200000))
                return result;
            result = result & 0x001fffff | data.readUnsignedByte()<<21;
            if (!(result & 0x10000000))
                return result;
            return   result & 0x0fffffff | data.readUnsignedByte()<<28;
        }
        
        public static function readS24(data:ByteArray):int
        {
            var b:int = data.readUnsignedByte()
            b |= data.readUnsignedByte()<<8
            b |= data.readByte()<<16
            return b
        }

        public static function readU32(data:ByteArray):uint {
            return readS32(data)
        }
        
        public static function readU30(data:ByteArray):uint {
            return readS32(data)
        }

        public static function readS30(data:ByteArray):int {
            return readS32(data)
        }
        
        public static function createReader(file : String) : IReader
        {
            var data:ByteArray = File.readByteArray(file)
            var readers:Array = [ABCReader, SWFReader, SWCReader]
            
            for each(var ri : Class in readers) {
                var r:IReader = new ri(data)
                if(r.supportsContent())
                    return r
            }
            
            Utils.error("No readers are capable of handling '"+file+"'")
            return null
        }
        
        public static function readFile(file:String):Array
        {
            var r : IReader = createReader(file);
            if (r != null)
                return r.readABC()
            return []
        }
        
        public static function readABCBytes(file : String) : Array
        {
            var r : IReader = createReader(file);
            if (r != null)
                return r.readABCBytes()
            return []
        }
    }
}