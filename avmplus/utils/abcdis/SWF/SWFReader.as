/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SWF
{
    import flash.utils.ByteArray
    import flash.utils.Endian

    import abc.IReader
    import abc.ABCReader
    import abc.Types.ABCFile
    
    
    public final class SWFReader implements IReader
    {
        private var bitPos:int
        private var bitBuf:int
    
        private var originalData : ByteArray;
        private var data : ByteArray;
        
        public function SWFReader(data:ByteArray)
        {
            this.originalData = data;
        }
        
        public function readABC() : Array {
            var swf : SWF = decodeSWFHeader();
            var abcs : Array = new Array();
            decodeTags(swf, function (abcData : ByteArray, abcName : String) : void {
                var a : ABCFile = new ABCReader(abcData).readABC()[0]
                a.abcName = abcName
                abcs.push(a)
            });
            
            return abcs;
        }
        
        public function readABCBytes() : Array
        {
            var swf : SWF = decodeSWFHeader();
            var abcs : Array = new Array();
            decodeTags(swf, function (abcData : ByteArray, abcName : String) : void {
                abcs.push(abcData)
            });
            return abcs;
        }
        
        public function readSWF() : SWF
        {
            var swf : SWF = decodeSWFHeader();
            decodeTags(swf, function (...rest) : void {});
            return swf;
        }
        
        public function getDecompressedBytes() : ByteArray
        {
            decodeSWFHeader()
            return data;
        }
        
        public function getOriginalBytes() : ByteArray
        {
            return originalData;
        }
        
        private function decodeSWFHeader() : SWF
        {
            decompressIfNeededAndSkipHeader();
            var swf : SWF = new SWF(data);
            swf.rect = decodeRect();
            swf.frameRate = (data.readUnsignedByte()<<8|data.readUnsignedByte())
            swf.frameCount = data.readUnsignedShort()
            return swf;
        }
        
        public static function decodeABCTag(tag : Tag, swfBytes : ByteArray, abcHandler : Function) : void
        {
            var abcLength : uint = tag.length;
            var abcName : String = "<unamed>"
            swfBytes.position = tag.payloadPos
            
            switch (tag.type)
            {
            case Constants.TAG_DoABC2:
                swfBytes.readInt()
                abcName = readStringFromBytes(swfBytes)
                abcLength -= (swfBytes.position-tag.payloadPos)
                // fall through
            case Constants.TAG_DoABC:
                var abcBytes : ByteArray = new ByteArray()
                abcBytes.endian = "littleEndian"
                swfBytes.readBytes(abcBytes,0,abcLength)
                abcHandler(abcBytes, abcName)
                break
            }
        }
        
        private function decodeTags(swf : SWF, abcHandler : Function) : void
        {
            var type:int, h:int, length:int
            var offset:int
            var abcName : String = "<unamed>";
            while (data.position < data.length)
            {
                var currTag : Tag = new Tag(swf);
                var currTagPos : uint = data.position;
                h = data.readUnsignedShort();
                var currTagType : uint = h >> 6;
                
                currTag.pos = data.position;
                
                currTag.type = h >> 6;
                length = h & 0x3F;
                
                if (length == 0x3F)
                    length = data.readInt();
                currTag.length = length;
                currTag.payloadPos = data.position
                
                swf.tags.push(currTag)
                
                switch (currTag.type)
                {
                case 0: return
                case Constants.TAG_DoABC2:
                case Constants.TAG_DoABC:
                    decodeABCTag(currTag, data, abcHandler)
                    break
                default:
                    data.position += length
                }
            }
        }

        private function readString():String
        {
            return readStringFromBytes(data)
        }
        
        private static function readStringFromBytes(bytes : ByteArray) : String
        {
            var s:String = ""
            var c:int

            while (c=bytes.readUnsignedByte())
                s += String.fromCharCode(c)
            return s
        }

        private function syncBits() : void
        {
            bitPos = 0
        }

        private function decodeRect():Rect
        {
            syncBits();

            var rect:Rect = new Rect();

            var nBits:int = readUBits(5)
            rect.xMin = readSBits(nBits);
            rect.xMax = readSBits(nBits);
            rect.yMin = readSBits(nBits);
            rect.yMax = readSBits(nBits);

            return rect;
        }

        private function readSBits(numBits:int):int
        {
            if (numBits > 32)
                throw new Error("Number of bits > 32");

            var num:int = readUBits(numBits);
            var shift:int = 32-numBits;
            // sign extension
            num = (num << shift) >> shift;
            return num;
        }

        private function readUBits(numBits:int):uint
        {
            if (numBits == 0)
                return 0

            var bitsLeft:int = numBits;
            var result:int = 0;

            if (bitPos == 0) //no value in the buffer - read a byte
            {
                bitBuf = data.readUnsignedByte()
                bitPos = 8;
            }

            while (true)
            {
                var shift:int = bitsLeft - bitPos;
                if (shift > 0)
                {
                    // Consume the entire buffer
                    result |= bitBuf << shift;
                    bitsLeft -= bitPos;

                    // Get the next byte from the input stream
                    bitBuf = data.readUnsignedByte();
                    bitPos = 8;
                }
                else
                {
                    // Consume a portion of the buffer
                    result |= bitBuf >> -shift;
                    bitPos -= bitsLeft;
                    bitBuf &= 0xff >> (8 - bitPos); // mask off the consumed bits

                    return result;
                }
            }

            // unreachable, but fixes a spurious compiler warning
            return result;
        }
        
        private function decompressIfNeededAndSkipHeader():void
        {
            originalData.position = 0
            originalData.endian = Endian.LITTLE_ENDIAN
            var version:uint = this.originalData.readUnsignedInt()
            switch (version&0xffffff) {
            case 67|87<<8|83<<16: // SWC
                data = new ByteArray()
                data.endian = Endian.LITTLE_ENDIAN
                originalData.position = 8
                originalData.readBytes(data,0,originalData.length-originalData.position)
                data.uncompress()
                break;
            case 70|87<<8|83<<16: // SWF
                data = originalData;
                data.position = 8;
                break;
            case 90|87<<8|83<<16: // SWZ
                data = new ByteArray;
                data.endian = "littleEndian"
                var ptr;
                // for swfs encoded in lzma: uncompressed length = bytes 4-7, lzma properties = bytes 8-12
                // also the swf-lzma length includes the ZWS+version (4 bytes) and the length(4 bytes) so we subtract 8
                // for 7z lzma files: lzma properties: bytes 0-4, uncompressed length: bytes 5-12
                // to correctly decompress the swf-lzma we have to change the headers

                // copy lzma properties into data[0-4]
                for (ptr=0;ptr<5;ptr++) {
                    data[ptr]=originalData[12+ptr]
                }
                // calculate uncompressed length, subtract 8 (4 for ZWS+version, 4 for this length field)
                var scriptlen:uint=originalData[4]+(originalData[5]<<8)+(originalData[6]<<16)+(originalData[7]<<24)-8;

                // copy uncompressed length in data[5-8] then write 0s into data[9-12] since 7z lzma expects length of 8 bytes
                data[5]=scriptlen&0xFF;
                data[6]=(scriptlen>>8) & 0xFF;
                data[7]=(scriptlen>>16) & 0xFF;
                data[8]=(scriptlen>>24) & 0xFF;

                for (ptr=0;ptr<4;ptr++) {
                    data[9+ptr]=0
                }

                // copy the compressed data into data[13] from originalData[17] after the header info
                originalData.position=17
                originalData.readBytes(data,13,originalData.length-17)
                data.position=0

                // uncompress should work now, Error #2038 or Error #1000 will occur if the lzma format is incorrect
                data.uncompress("lzma")
                break;
            default:
                data = null;
            }
        }
        
        public function supportsContent():Boolean {
            originalData.position = 0
            this.originalData.endian = Endian.LITTLE_ENDIAN
            var version:uint = this.originalData.readUnsignedInt()
            switch (version&0xffffff) {
            case 67|87<<8|83<<16: // SWC
            case 70|87<<8|83<<16: // SWF
            case 90|87<<8|83<<16: // SWZ
                return true;
            }
            return false;
        }
        
    }
}
