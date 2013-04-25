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

package core.memory.io
{
	import flash.xml.*;
	import flash.utils.*;
	import flash.errors.IllegalOperationError;
	import core.memory.io.IOHandlerInfo;
	import core.misc.Convert;
	import avmplus.*; // for describeType DS 12/17/2008

	public dynamic class IOHandler extends Proxy
	{
		protected var arr:ByteArray;
		protected var handlers:Array;
		protected var maskAddress:uint;
		protected var debug:Boolean;

		protected var cname:String;
		protected var cbase:String;

		static protected var props:Object;
	
		public function IOHandler(debugFlag:Boolean = false) {
			var info:XML = describeType(this,FLASH10_FLAGS);
			cname="VIC";
			cbase="IOHandler";
			props={};
/*
			cname = info.@name.toString().split("::")[1];
			cbase = info.@base.toString().split("::")[1];
			debug = debugFlag;
			if(props === null) {
			        arr=new ByteArray; // added 12/17/2008 
				var d:XMLDocument = new XMLDocument(describeType(arr,FLASH10_FLAGS).toXMLString());
				print("firstChild.firstChild");
				var n:XMLNode = d.firstChild.firstChild;
				print("node="+n);
				props = {};
				while(n) {
					if(props[n.nodeName] == undefined) {
						props[n.nodeName] = {};
					}
					props[n.nodeName][n.attributes.name] = {};
					n = n.nextSibling;
				}
			}
*/
			// initialize get/set handlers
			initHandlers();
			// create memory
			arr = new ByteArray();
			// determine address mask
			// registers of all c64 i/o chips are duplicated 
			// to fill the entire address space for the chip
			// (example: cia1's address space is $dc00-$dcff, 
			// but it only has 16 registers, so they are
			// mirrored in $dc10, $dc20, $dc30, ...)
			// (see get/set/callProperty methods)
			maskAddress = 0xffff;
			var hlen:uint = handlers.length - 1;
			while((hlen & (maskAddress >> 1)) == hlen) {
				maskAddress >>= 1;
			}
			// fill up handler array
			// to ensure we have handlers for all registers
			for(var i:uint = hlen + 1; i <= maskAddress; i++) {
				handlers.push(new IOHandlerInfo(getDefault, setDefault));
			}
			// set length of memory bytearray
			arr.length = maskAddress + 1;
		}
		
		public function setDebug(value:Boolean):void {
			debug = value;
		}

		public function getDebug():Boolean {
			return debug;
		}

		protected function debugMessage(message:String):void {
			trace(message);
		}

		/**
		* Initialize handlers table
		* (the table is an array of IOHandlerInfo objects)
		* This should be a "virtual" function (is that possible in AS3?).
		* It should be overwritten by subclasses.
		*/
		protected function initHandlers():void {
			handlers = [];
		}
		
		protected function getDefault(index:int):int {
			if(debug) debugMessage("[" + cname + "] get " + Convert.toHex(index) + ": #$" + Convert.toHex(arr[index]));
			return arr[index];
		}

		protected function setDefault(index:int, value:int):void {
			if(debug) debugMessage("[" + cname + "] set " + Convert.toHex(index) + ": #$" + Convert.toHex(value));
			arr[index] = value;
		}

		public override function setProperty(name:*, value:*):void {
//            print("setProperty name="+name+" value="+value);
			var index:int = parseInt(name) & maskAddress;
			if(!isNaN(index)) {
				IOHandlerInfo(handlers[index]).setter(index, value);
			} else if(props.accessor[name] != undefined) {
				arr[name.toString()] = value;
			} else {
				throw new IllegalOperationError("Error: Access of undefined property " + name.toString() + " through a reference with static type " + cname);
			}
		}

		public override function getProperty(name:*):* {
			var index:int = parseInt(name) & maskAddress;
			if(!isNaN(index)) {
				return IOHandlerInfo(handlers[index]).getter(index);
			} else if(props.accessor[name] != undefined) {
				return arr[name.toString()];
			} else {
				throw new IllegalOperationError("Error: Access of undefined property " + name.toString() + " through a reference with static type " + cname);
			}
		}

		public override function callProperty(name:*, ...rest):* {
			if(props.method[name] != undefined) {
				return arr[name.toString()].apply(null, rest);
			} else {
				throw new IllegalOperationError("Error: Call to a possibly undefined method " + name.toString() + " through a reference with static type " + cname);
			}
		}
	}
}
