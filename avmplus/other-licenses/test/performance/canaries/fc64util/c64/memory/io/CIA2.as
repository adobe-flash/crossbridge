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

package c64.memory.io
{
	import flash.utils.*;
	import core.memory.io.IOHandler;
	import core.memory.io.IOHandlerInfo;
	import core.misc.Convert;

	public dynamic class CIA2 extends IOHandler
	{
		public var vicBaseAddr:uint;
		public var timerLatchA:int;
		public var timerLatchB:int;
	
		public function CIA2(debugFlag:Boolean = false) {
			super(debugFlag);
			arr[2] = 0x3f; // init DRA
		}

		public function updateTimers(cycles:uint):void {
		}

		private function getDataPortA(index:int):int {
			if(debug) debugMessage("[CIA2] get PRA: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		private function setDataPortA(index:int, value:int):void {
			arr[index] = value;
			vicBaseAddr = (~value & 0x03) << 14;
			if(debug) debugMessage("[CIA2] set PRA: #$" + Convert.toHex(value) + " vicbase:" + Convert.toHex(vicBaseAddr, 4));
		}
		
		private function getDataPortB(index:int):int {
			if(debug) debugMessage("[CIA2] get PRB: #$" + Convert.toHex(arr[index]));
			return (~arr[3] & 0xff) | (arr[3] & arr[index]);
		}
		private function setDataPortB(index:int, value:int):void {
			arr[index] = value;
			if(debug) debugMessage("[CIA2] set PRB: #$" + Convert.toHex(value));
		}
		
		private function setLoTimerA(index:int, value:int):void {
			timerLatchA = (timerLatchA & 0xff00) | value;
			if(debug) debugMessage("[CIA2] set TAL: #$" + Convert.toHex(value) + " (Timer A Latch: #$" + Convert.toHex(timerLatchA, 4) + ")");
		}
		private function setHiTimerA(index:int, value:int):void {
			timerLatchA = (timerLatchA & 0x00ff) | (value * 256);
			if(debug) debugMessage("[CIA2] set TAH: #$" + Convert.toHex(value) + " (Timer A Latch: #$" + Convert.toHex(timerLatchA, 4) + ")");
		}
		private function setLoTimerB(index:int, value:int):void {
			timerLatchB = (timerLatchB & 0xff00) | value;
			if(debug) debugMessage("[CIA2] set TBL: #$" + Convert.toHex(value) + " (Timer B Latch: #$" + Convert.toHex(timerLatchB, 4) + ")");
		}
		private function setHiTimerB(index:int, value:int):void {
			timerLatchB = (timerLatchB & 0x00ff) | (value * 256);
			if(debug) debugMessage("[CIA2] set TBH: #$" + Convert.toHex(value) + " (Timer B Latch: #$" + Convert.toHex(timerLatchB, 4) + ")");
		}
		
		override protected function initHandlers():void {
			handlers = [
				// the CIA2 chip has 16 registers:
				new IOHandlerInfo(getDataPortA, setDataPortA), // 00
				new IOHandlerInfo(getDataPortB, setDataPortB), // 01
				new IOHandlerInfo(getDefault, setDefault), // 02
				new IOHandlerInfo(getDefault, setDefault), // 03
				new IOHandlerInfo(getDefault, setLoTimerA), // 04
				new IOHandlerInfo(getDefault, setHiTimerA), // 05
				new IOHandlerInfo(getDefault, setLoTimerB), // 06
				new IOHandlerInfo(getDefault, setHiTimerB), // 07
				new IOHandlerInfo(getDefault, setDefault), // 08
				new IOHandlerInfo(getDefault, setDefault), // 09
				new IOHandlerInfo(getDefault, setDefault), // 0a
				new IOHandlerInfo(getDefault, setDefault), // 0b
				new IOHandlerInfo(getDefault, setDefault), // 0c
				new IOHandlerInfo(getDefault, setDefault), // 0d
				new IOHandlerInfo(getDefault, setDefault), // 0e
				new IOHandlerInfo(getDefault, setDefault)  // 0f
			];
		}
	}
}
