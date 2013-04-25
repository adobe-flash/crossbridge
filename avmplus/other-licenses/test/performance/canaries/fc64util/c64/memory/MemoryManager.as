/*
 * Copyright notice
 *
 * (c) 2005-2006 Darron Schall, Claus Wahlers.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package c64.memory
{
	import core.misc.Convert;
	
	import core.memory.*;
	import core.memory.io.*;
	
	import c64.memory.*;
	import c64.memory.io.*;
	
	import flash.net.*;
	import flash.events.*;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
    
    import avmplus.*;
	
	public class MemoryManager implements IMemoryManager, IEventDispatcher
	{
		public static const BASEADDR_STACK:uint = 0x0100;

		public static const MEMBANK_RAM:int = 0; // "ram";
		public static const MEMBANK_VIC:int = 1; // "vic";
		public static const MEMBANK_CIA1:int = 2; // "cia1";
		public static const MEMBANK_CIA2:int = 3; // "cia2";
		public static const MEMBANK_SID:int = 4; // "sid";
		public static const MEMBANK_KERNAL:int = 5; // "kernal";
		public static const MEMBANK_BASIC:int = 6; // "basic";
		public static const MEMBANK_CHARACTER:int = 7; // "character";

		public static const debugMemBanks:Array = [
			"RAM", "VIC", "CIA1", "CIA2", "SID", "KERNAL", "BASIC", "CHAR"
		];

		public var ram:ByteArray;

		public var vic:VIC;
		public var cia1:CIA1;
		public var cia2:CIA2;
		public var sid:SID;

		private var memoryMapRead:Array;
		private var memoryMapWrite:Array;
		private var memoryBankInfo:Array;
		private var dispatcher:EventDispatcher;
		
		public function MemoryManager() {
			init();
		}
		
		/**
		* Initialize memory manager
		*/
		private function init():void {
			// initialize memory bank info table
			memoryBankInfo = [];
			// initialize ram image
			ram = new ByteArray();
			ram.length = 0x10000;
			memoryBankInfo[MEMBANK_RAM] = new MemoryBankInfo(ram);
			// initialize i/o images
			vic = new VIC();
			cia1 = new CIA1();
			cia2 = new CIA2();
			sid = new SID();
			memoryBankInfo[MEMBANK_VIC] = new MemoryBankInfo(vic, 0xd000, 0x0400);
			memoryBankInfo[MEMBANK_CIA1] = new MemoryBankInfo(cia1, 0xdc00, 0x0100);
			memoryBankInfo[MEMBANK_CIA2] = new MemoryBankInfo(cia2, 0xdd00, 0x0100);
			memoryBankInfo[MEMBANK_SID] = new MemoryBankInfo(sid, 0xd400, 0x0400);
			// initialize memory maps.
			// the two memory map arrays are used to map each page of the 
			// 64k address space (256 pages à 256 bytes) to the appropriate
			// ram, rom (basic, kernal) or i/o (vic, sid, cia1, cia2) 
			// memory images, separately for both read and write access.
			memoryMapRead = [];
			memoryMapWrite = [];
			for(var i:uint = 0; i < 256; i++) {
				// initially set full ram access (read and write)
				memoryMapRead.push(MEMBANK_RAM);
				memoryMapWrite.push(MEMBANK_RAM);
			}
			// initialize event dispatcher
			dispatcher = new EventDispatcher();
		}

		/**
		* Read a byte from memory
		*/
		public function read(address:uint):int {
			var bank:uint = address >> 8;
			var f:uint = memoryMapRead[bank];
            if (getQualifiedSuperclassName(memoryBankInfo[f].image)=="core.memory.io::IOHandler") {
                val=MemoryBankInfo(memoryBankInfo[f]).image.getProperty(""+address);
//                print("read: "+address+"="+val);
            } else {
                val=MemoryBankInfo(memoryBankInfo[f]).image[address];
            }
            return val;
		}
		
		/**
		* Read a word from memory
		*/
		public function readWord(address:uint):int {
			return read(address) + (read(address + 1) << 8);
		}
		
		/**
		* Read a byte from stack
		*/
		public function readStack(sp:uint):int {
			return ram[sp + BASEADDR_STACK];
		}
		
		/**
		 * Read a byte from character data.
		 * 
		 * We need an extra method for the VIC because it sees the character
		 * data ROM as shadows at $1000 and $9000 (VIC banks 0 and 2) only, 
		 * but the ROM is physically present at $D000. The VIC reads from RAM
		 * for all other locations.
		 * 
		 * Note that this method is only called by the VIC. The 6502 sees
		 * the character data ROM only at it's physical address $D000, and
		 * only if it's explicitly banked in.
		 * 
		 * The address calculation is done externally by the VIC to save time
		 * (it only needs to be calculated once at bad lines)
		 */
		public function readCharacterData(address:uint, accessRom:Boolean = true):int {
			if(accessRom) {
				return MemoryBankInfo(memoryBankInfo[MEMBANK_CHARACTER]).image[address];
			} else {
				return MemoryBankInfo(memoryBankInfo[MEMBANK_RAM]).image[address];
			}
		}
		
		/**
		* Copy bytes from RAM
		*/
		public function copyRam(ba:ByteArray, address:uint, len:uint):void {
			ba.writeBytes(ram, address, len);
		}
		
		/**
		* Write a byte to memory
		*/
		public function write(address:uint, value:int):void {
			if(address == 0x0001) {
				// write processor port (address 0x0001)
				// banks memory images in or out of the address space
				// according to bits 0-2:
				// %x00: ram visible everywhere
				// %x01: ram visible at $a000-$bfff and $e000-$ffff
				// %x10: ram visible at $a000-$bfff, kernal-rom visible at $e000-$ffff
				// %x11: basic-rom visible at $a000-$bfff, kernal-rom visible at $e000-$ffff
				// %0xx: character-rom visible at $d000-$dfff (except for %000, see above)
				// %1xx: i/o visible at $d000-$dfff (except for %000, see above)
				// a write access to a visible rom-area always writes to ram
				var enableKernal:Boolean = (value & 2) == 2;
				var enableBasic:Boolean = (value & 3) == 3;
				var enableCharacter:Boolean = ((value & 4) == 0) && ((value & 3) != 0);
				var enableIO:Boolean = (value & 4) == 4 && ((value & 3) != 0);
				enableMemoryBank(MEMBANK_KERNAL, enableKernal, false);
				enableMemoryBank(MEMBANK_BASIC, enableBasic, false);
				enableMemoryBank(MEMBANK_CHARACTER, enableCharacter, false);
				enableMemoryBank(MEMBANK_VIC, enableIO, enableIO);
				enableMemoryBank(MEMBANK_CIA1, enableIO, enableIO);
				enableMemoryBank(MEMBANK_CIA2, enableIO, enableIO);
				enableMemoryBank(MEMBANK_SID, enableIO, enableIO);
			}
			var bank:uint = address >> 8;
			var f:uint = memoryMapWrite[bank];
//            print("MemoryBankInfo: "+getType(memoryBankInfo[f].image)+" "+address+" "+value);
			
            if (getQualifiedSuperclassName(memoryBankInfo[f].image)=="core.memory.io::IOHandler")
                MemoryBankInfo(memoryBankInfo[f]).image.setProperty(""+address,value);
            else
                MemoryBankInfo(memoryBankInfo[f]).image[address] = value;
		}
		
		/**
		* Write a word to memory
		*/
        public function getType(obj):String {
            return getQualifiedClassName(obj)+" "+getQualifiedSuperclassName(obj);
//              return "foo";
        }
		public function writeWord(address:uint, value:int):void {
			var f:uint = memoryMapWrite[address >> 8];
			var image:ByteArray = MemoryBankInfo(memoryBankInfo[f]).image as ByteArray;
			image[address++] = value & 0xff;
			image[address] = (value & 0xff00) >> 8;
		}
		
		/**
		* Write a byte to stack
		*/
		public function writeStack(sp:uint, value:int):void {
			ram[sp + BASEADDR_STACK] = value;
		}
		
		/**
		* Add ROM or RAM memory banks to the memory manager 
		* and initialize the corresponding image
		*/
		public function setMemoryBank(bankid:int, baseAddress:uint = 0, length:uint = 0, data:ByteArray = null):void {
			var image:ByteArray = new ByteArray();
			image.length = 0x10000; // always 64k
			if(length != 0 && data !== null) {
				image.position = baseAddress;
				image.writeBytes(data, 0, length);
				memoryBankInfo[bankid] = new MemoryBankInfo(image, baseAddress, length);
			}
		}
		
		/**
		* Enable/disable read and write access for a ROM or I/O memory bank.
		*/
		public function enableMemoryBank(bankid:int, enableRead:Boolean = true, enableWrite:Boolean = false):void {
			// get memory bank info (image, address range, status)
			var bank:MemoryBankInfo = MemoryBankInfo(memoryBankInfo[bankid]);
			// check if memory is already mapped right
			if(bank.readAccess !== enableRead || bank.writeAccess !== enableWrite) {
				// map pages to the bank (visible), or to ram (not visible)
				var valueRead:int = enableRead ? bankid : MEMBANK_RAM;
				var valueWrite:int = enableWrite ? bankid : MEMBANK_RAM;
				var pageStart:uint = bank.baseAddress >> 8;
				var pageEnd:uint = (bank.length >> 8) + pageStart;
				for(var i:uint = pageStart; i < pageEnd; i++) {
					memoryMapRead[i] = valueRead;
					memoryMapWrite[i] = valueWrite;
				}
				// remember state
				bank.readAccess = enableRead;
				bank.writeAccess = enableWrite;
			}
		}

		/**
		* Dump memory to console
		*/
		public function dump(adr:uint, len:uint, bytesPerLine:uint = 8):String {
			var ret:String = "";
			for(var i:uint = 0; i < len; i++) {
				if(i % bytesPerLine == 0) {
					if(i > 0) {
						ret += "\n";
					}
					ret += Convert.toHex(adr, 4) + ": ";
				}
				ret += Convert.toHex(read(adr++)) + " ";
			}
			return ret;
		}

		/**
		* Load .PRG file
		*/
		public function loadPRG(url:String):void {
			var request:URLRequest = new URLRequest(url);
			var loader:URLLoader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadPRG);
			loader.load(request);
		}
		
		/**
		 * Load .PRG file is complete.
		 * Copy contents to memory.
		 */
		protected function onLoadPRG(event:Event):void {
			var ba:ByteArray = ByteArray(event.target.data);
			trace("PRG file loaded");
			// get start address
			ba.endian = Endian.LITTLE_ENDIAN;
			var startAddress:int = ba.readShort();
			trace("Start address: $" + Convert.toHex(startAddress, 4));
			// copy contents
			for(var i:uint = 0x02; i < ba.length; i++) {
				write(startAddress++, ba[i]);
			}
			writeWord(0x002b, 0x0801);
			writeWord(0x002d, startAddress);
			writeWord(0x002f, startAddress);
			writeWord(0x0031, startAddress);
			dispatchEvent(new Event("complete"));
		}

		/**
		* Load .PXX file
		*/
		public function loadPXX(url:String):void {
			var request:URLRequest = new URLRequest(url);
			var loader:URLLoader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadPXX);
			loader.load(request);
		}
		
		/**
		 * Load .PXX file is complete.
		 * Check if file is valid and copy contents to memory.
		 */
		protected function onLoadPXX(event:Event):void {
			var ba:ByteArray = ByteArray(event.target.data);
			// check if valid PRG file
			var intro:Array = [ 0x43, 0x36, 0x34, 0x46, 0x69, 0x6C, 0x65, 0x00 ];
			for(var i:uint = 0; i < intro.length; i++) {
				if(ba[i] != intro[i]) {
					// todo: error, not a PRG file
					return;
				}
			}
			// get filename
			var fileName:String = "";
			for(i = 8; i < 24; i++) {
				if(ba[i] == 0) {
					break;
				}
				// todo: this is actually PETSCII
				fileName += String.fromCharCode(ba[i]);
			}
			trace("PXX file '" + fileName + "' loaded");
			// get start address
			ba.endian = Endian.LITTLE_ENDIAN;
			ba.position = 0x1a;
			var startAddress:int = ba.readShort();
			trace("Start address: $" + Convert.toHex(startAddress, 4));
			// copy contents
			write(1, read(1) & 0xf8);
			for(i = 0x1c; i < ba.length; i++) {
				write(startAddress++, ba[i]);
			}
			write(1, read(1) | 0x07);
			dispatchEvent(new Event("complete"));
		}
		
		
		public function addEventListener(type:String, listener:Function, useCapture:Boolean = false, priority:int = 0, useWeakReference:Boolean = false):void{
			dispatcher.addEventListener(type, listener, useCapture, priority);
		}
		
		public function dispatchEvent(event:Event):Boolean{
			return dispatcher.dispatchEvent(event);
		}
		
		public function hasEventListener(type:String):Boolean{
			return dispatcher.hasEventListener(type);
		}
		
		public function removeEventListener(type:String, listener:Function, useCapture:Boolean = false):void{
			dispatcher.removeEventListener(type, listener, useCapture);
		}
		
		public function willTrigger(type:String):Boolean {
			return dispatcher.willTrigger(type);
		}
	}
}
