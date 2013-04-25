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
	import c64.memory.io.Keyboard;
	import core.misc.Convert;

	public dynamic class CIA1 extends IOHandler
	{
		public var keyboard:Keyboard;

		public var irqTriggered:Boolean = false;

		public static const TIMER_RUNMODE_ONESHOT:uint = 0;
		public static const TIMER_RUNMODE_CONTINUOUS:uint = 1;
		
		public static const TIMER_INPUTMODE_PROCESSOR:uint = 0;
		public static const TIMER_INPUTMODE_CNT:uint = 1;
		public static const TIMER_INPUTMODE_TIMERA:uint = 2;
		public static const TIMER_INPUTMODE_TIMERA_CNT:uint = 3;

		// Timer A
		public var taStarted:Boolean = false;
		public var taPortBOutput:Boolean = false;
		public var taPortBOutputToggle:Boolean = false;
		public var taRunMode:uint = TIMER_RUNMODE_CONTINUOUS;
		public var taInputMode:uint = TIMER_INPUTMODE_PROCESSOR;
		public var taCounter:int = 0;
		public var taLatch:int = 0;
		public var taIRQEnabled:Boolean = false;
		public var taIRQTriggered:Boolean = false;

		// Timer B
		public var tbStarted:Boolean = false;
		public var tbPortBOutput:Boolean = false;
		public var tbPortBOutputToggle:Boolean = false;
		public var tbRunMode:uint = TIMER_RUNMODE_CONTINUOUS;
		public var tbInputMode:uint = TIMER_INPUTMODE_PROCESSOR;
		public var tbCounter:int = 0;
		public var tbLatch:int = 0;
		public var tbIRQEnabled:Boolean = false;
		public var tbIRQTriggered:Boolean = false;

		// Time Of Day
		public var todFrequency:uint = 60;
		public var todSetAlarmOnWrite:Boolean = false;
		public var todTimeSet:Number = 0;
		public var todTimeStart:Number = 0;
		public var todTimeAlarm:Number = 0;
		public var todLatchRead:Number = 0;
		public var todLatchWrite:Number = 0;
		public var todLatchedRead:Boolean = false;
		public var todLatchedWrite:Boolean = false;
		public var todIRQEnabled:Boolean = false;
		public var todIRQTriggered:Boolean = false;

		// Serial Shift
		public var ssIRQEnabled:Boolean = false;
		public var ssIRQTriggered:Boolean = false;

		// Flag Line
		public var flIRQEnabled:Boolean = false;
		public var flIRQTriggered:Boolean = false;


		public function CIA1(debugFlag:Boolean = false) {
			super(debugFlag);
			var curDate:Date = new Date();
			todTimeStart = todTimeSet = curDate.time;
			keyboard = new Keyboard();
		}


		public function updateTimerA(cycles:uint):Boolean {
			if(taInputMode == TIMER_INPUTMODE_PROCESSOR) {
				taCounter -= cycles;
				if(taCounter <= 0) {
					arr[0x0d] |= 0x01;
					if(taIRQEnabled) {
						return true;
					}
					resetTimerA();
				}
			} else {
				// CNT not supported
			}
			return false;
		}
		public function resetTimerA():void {
			if(taIRQEnabled) {
				irqTriggered = true;
				taIRQTriggered = true;
				arr[0x0d] |= 0x80;
			}
			taCounter = taLatch;
			if(taRunMode == TIMER_RUNMODE_ONESHOT) {
				taStarted = false;
				arr[0x0e] &= 0x01;
			}
		}
		
		public function updateTimerB(cycles:uint, underflow:Boolean):Boolean {
			switch(tbInputMode) {
				case TIMER_INPUTMODE_PROCESSOR:
					tbCounter -= cycles;
					if(tbCounter <= 0) {
						arr[0x0d] |= 0x02;
						if(tbIRQEnabled) {
							return true;
						}
						resetTimerB();
					}
					break;
				case TIMER_INPUTMODE_TIMERA:
					if(underflow) {
						if(--tbCounter <= 0) {
							arr[0x0d] |= 0x02;
							if(tbIRQEnabled) {
								return true;
							}
							resetTimerB();
					}
					}
					break;
				case TIMER_INPUTMODE_TIMERA_CNT:
					// CNT not supported
					break;
				case TIMER_INPUTMODE_CNT:
					// CNT not supported
					break;
			}
			return false;
		}
		public function resetTimerB():void {
			if(tbIRQEnabled) {
				irqTriggered = true;
				tbIRQTriggered = true;
				arr[0x0d] |= 0x80;
			}
			tbCounter = tbLatch;
			if(tbRunMode == TIMER_RUNMODE_ONESHOT) {
				tbStarted = false;
				arr[0x0f] &= 0x01;
			}
		}
		
		/**
		* $DC00 - Data Port A
		*/
		protected function getDataPortA(index:int):int {
			if(debug) debugMessage("[CIA1] get 00: #$" + Convert.toHex(arr[index]));
			return keyboard.getJoystick2();
		}
		protected function setDataPortA(index:int, value:int):void {
			arr[index] = value;
			if(debug) debugMessage("[CIA1] set 00: #$" + Convert.toHex(value));
		}

		/**
		* $DC01 - Data Port B
		*/
		protected function getDataPortB(index:int):int {
			if(debug) debugMessage("[CIA1] get 01: #$" + Convert.toHex(keyboard.getRows(arr[0])));
			return keyboard.getRows(arr[0]);
		}
		protected function setDataPortB(index:int, value:int):void {
			arr[index] = value;
			if(debug) debugMessage("[CIA1] set 01: #$" + Convert.toHex(value));
		}

		/**
		* $DC04 - Timer A Low Byte
		*/
		protected function getLoTimerA(index:int):int {
			if(debug) debugMessage("[CIA1] get 04: #$" + Convert.toHex(taCounter & 0x00ff));
			return taCounter & 0x00ff;
		}
		protected function setLoTimerA(index:int, value:int):void {
			taLatch = (taLatch & 0xff00) | value;
			if(debug) debugMessage("[CIA1] set 04: #$" + Convert.toHex(value) + " (Timer A Latch: #$" + Convert.toHex(taLatch, 4) + ")");
		}

		/**
		* $DC05 - Timer A High Byte
		*/
		protected function getHiTimerA(index:int):int {
			if(debug) debugMessage("[CIA1] get 05: #$" + Convert.toHex((taCounter & 0xff00) >> 8));
			return (taCounter & 0xff00) >> 8;
		}
		protected function setHiTimerA(index:int, value:int):void {
			taLatch = (taLatch & 0x00ff) | (value * 256);
			if(debug) debugMessage("[CIA1] set 05: #$" + Convert.toHex(value) + " (Timer A Latch: #$" + Convert.toHex(taLatch, 4) + ")");
		}

		/**
		* $DC06 - Timer B Low Byte
		*/
		protected function getLoTimerB(index:int):int {
			if(debug) debugMessage("[CIA1] get 06: #$" + Convert.toHex(tbCounter & 0x00ff));
			return tbCounter & 0x00ff;
		}
		protected function setLoTimerB(index:int, value:int):void {
			tbLatch = (tbLatch & 0xff00) | value;
			if(debug) debugMessage("[CIA1] set 06: #$" + Convert.toHex(value) + " (Timer B Latch: #$" + Convert.toHex(tbLatch, 4) + ")");
		}

		/**
		* $DC07 - Timer B High Byte
		*/
		protected function getHiTimerB(index:int):int {
			if(debug) debugMessage("[CIA1] get 07: #$" + Convert.toHex((tbCounter & 0xff00) >> 8));
			return (tbCounter & 0xff00) >> 8;
		}
		protected function setHiTimerB(index:int, value:int):void {
			tbLatch = (tbLatch & 0x00ff) | (value * 256);
			if(debug) debugMessage("[CIA1] set 07: #$" + Convert.toHex(value) + " (Timer B Latch: #$" + Convert.toHex(tbLatch, 4) + ")");
		}
		
		/**
		* $DC08 - Time Of Day, Tenths Of Seconds
		*/
		protected function getTODSeconds10(index:int):int {
			var d:Date;
			if(todLatchedRead) {
				todLatchedRead = false;
				d = new Date(todLatchRead);
			} else {
				d = new Date();
				d = new Date(d.getTime() - todTimeStart + todTimeSet);
			}
			var value:int = Convert.toBCD(Math.floor(d.getMilliseconds() / 100));
			if(debug) debugMessage("[CIA1] get 08: #$" + Convert.toHex(value));
			return value;
		}
		protected function setTODSeconds10(index:int, value:int):void {
			if(todLatchedWrite) {
			} else {
			}
			if(debug) debugMessage("[CIA1] set 08: #$" + Convert.toHex(value));
		}

		/**
		* $DC09 - Time Of Day, Seconds
		*/
		protected function getTODSeconds(index:int):int {
			var d:Date;
			if(todLatchedRead) {
				d = new Date(todLatchRead);
			} else {
				d = new Date();
				d = new Date(d.getTime() - todTimeStart + todTimeSet);
			}
			var value:int = Convert.toBCD(d.getSeconds());
			if(debug) debugMessage("[CIA1] get 09: #$" + Convert.toHex(value));
			return value;
		}
		protected function setTODSeconds(index:int, value:int):void {
			if(todLatchedWrite) {
			} else {
			}
			if(debug) debugMessage("[CIA1] set 09: #$" + Convert.toHex(value));
		}

		/**
		* $DC0A - Time Of Day, Minutes
		*/
		protected function getTODMinutes(index:int):int {
			var d:Date;
			if(todLatchedRead) {
				d = new Date(todLatchRead);
			} else {
				d = new Date();
				d = new Date(d.getTime() - todTimeStart + todTimeSet);
			}
			var value:int = Convert.toBCD(d.getMinutes());
			if(debug) debugMessage("[CIA1] get 0A: #$" + Convert.toHex(value));
			return value;
		}
		protected function setTODMinutes(index:int, value:int):void {
			if(todLatchedWrite) {
			} else {
			}
			if(debug) debugMessage("[CIA1] set 0A: #$" + Convert.toHex(value));
		}

		/**
		* $DC0B - Time Of Day, Hours
		*/
		protected function getTODHours(index:int):int {
			var d:Date = new Date();
			todLatchRead = d.getTime() - todTimeStart + todTimeSet;
			todLatchedRead = true;
			d = new Date(todLatchRead);
			var value:int = Convert.toBCD(d.getHours());
			if(debug) debugMessage("[CIA1] get 0B: #$" + Convert.toHex(value));
			return value;
		}
		protected function setTODHours(index:int, value:int):void {
			if(todLatchedWrite) {
			} else {
			}
			if(debug) debugMessage("[CIA1] set 0B: #$" + Convert.toHex(value));
		}

		/**
		* $DC0D - Interrupt Control Register
		*/
		protected function getIRQControlReg(index:int):int {
			var value:int = 0;
			if(irqTriggered) {
				if(taIRQTriggered) { value |= 0x01; }
				if(tbIRQTriggered) { value |= 0x02; }
				if(todIRQTriggered) { value |= 0x04; }
				if(ssIRQTriggered) { value |= 0x08; }
				if(flIRQTriggered) { value |= 0x10; }
				value |= 0x80;
			}
			if(debug) { 
				debugMessage(
					"[CIA1] get 0D: #$" + Convert.toHex(value) + " (" +
					((irqTriggered) ? "" : "no irq ") +
					((taIRQTriggered) ? "TA " : "") +
					((tbIRQTriggered) ? "TB " : "") +
					((todIRQTriggered) ? "TOD " : "") +
					((ssIRQTriggered) ? "SS " : "") +
					((flIRQTriggered) ? "FL " : "") +
					"triggered)"
				);
			}
			return value;
		}
		protected function setIRQControlReg(index:int, value:int):void {
			// bit 7 decides if the other flags are cleared (0) or set (1)
			var newBitValue:Boolean = ((value & 0x80) != 0);
			// bits 0-4: if set, the corresponding flag is cleared or set (depending on bit 7)
			if((value & 0x01) != 0) { taIRQEnabled = newBitValue; }
			if((value & 0x02) != 0) { tbIRQEnabled = newBitValue; }
			if((value & 0x04) != 0) { todIRQEnabled = newBitValue; }
			if((value & 0x08) != 0) { ssIRQEnabled = newBitValue; }
			if((value & 0x10) != 0) { flIRQEnabled = newBitValue; }
			if(debug) { 
				debugMessage(
					"[CIA1] set 0D: #$" + Convert.toHex(value) + " (" +
					"TA: " + taIRQEnabled + ", " +
					"TB: " + tbIRQEnabled + ", " +
					"TOD: " + todIRQEnabled + ", " +
					"SS: " + ssIRQEnabled + ", " +
					"FL: " + flIRQEnabled + ")"
				);
			}
		}
		
		/**
		* $DC0E - Control Register A
		*/
		protected function getControlA(index:int):int {
			if(debug) debugMessage("[CIA1] get 0E: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setControlA(index:int, value:int):void {
			arr[index] = value;
			taStarted = (value & 0x01) != 0;
			taPortBOutput = (value & 0x02) != 0;
			taPortBOutputToggle = (value & 0x04) != 0;
			taRunMode = (value & 0x08) != 0 ? TIMER_RUNMODE_ONESHOT : TIMER_RUNMODE_CONTINUOUS;
			taInputMode = (value & 0x20) == 0 ? TIMER_INPUTMODE_PROCESSOR : TIMER_INPUTMODE_CNT;
			if((value & 0x10) != 0) {
				taCounter = taLatch;
			}
			if(debug) { 
				debugMessage(
					"[CIA1] set 0E: #$" + Convert.toHex(value) + " (" +
					(tbStarted ? "Timer A runs. " : "Timer A stopped. ") +
					"mode:" + (taRunMode == TIMER_RUNMODE_ONESHOT ? "oneshot" : "cont") + ", " +
					"input:" + (taInputMode == TIMER_INPUTMODE_PROCESSOR ? "int" : "ext") + ", " +
					"portb:" + taPortBOutput + "/" + (taPortBOutputToggle ? "toggle" : "tick") + ", " +
					(((value & 0x10) != 0) ? "counter set to #$" + Convert.toHex(taLatch, 4) : "counter not set") + ")"
				);
			}
		}
		
		/**
		* $DC0F - Control Register B
		*/
		protected function getControlB(index:int):int {
			if(debug) debugMessage("[CIA1] get 0F: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setControlB(index:int, value:int):void {
			arr[index] = value;
			tbStarted = (value & 0x01) != 0;
			tbPortBOutput = (value & 0x02) != 0;
			tbPortBOutputToggle = (value & 0x04) != 0;
			tbRunMode = (value & 0x08) != 0 ? TIMER_RUNMODE_ONESHOT : TIMER_RUNMODE_CONTINUOUS;
			tbInputMode = (value & 0x60) >> 5;
			todSetAlarmOnWrite = (value & 0x80) != 0;
			if((value & 0x10) != 0) {
				tbCounter = tbLatch;
			}
			if(debug) { 
				var dInputMode:String = "";
				switch(tbInputMode) {
					case TIMER_INPUTMODE_PROCESSOR: dInputMode = "int"; break;
					case TIMER_INPUTMODE_CNT: dInputMode = "ext"; break;
					case TIMER_INPUTMODE_TIMERA: dInputMode = "timera"; break;
					case TIMER_INPUTMODE_TIMERA_CNT: dInputMode = "timera+ext"; break;
				}
				debugMessage(
					"[CIA1] set 0F: #$" + Convert.toHex(value) + " (" +
					(tbStarted ? "Timer B runs. " : "Timer B stopped. ") +
					"mode:" + (tbRunMode == TIMER_RUNMODE_ONESHOT ? "oneshot" : "cont") + ", " +
					"input:" + dInputMode + ", " +
					"portb:" + tbPortBOutput + "/" + (tbPortBOutputToggle ? "toggle" : "tick") + ", " +
					"todwrite:" + (todSetAlarmOnWrite ? "setalarm" : "setclock") + ", " +
					(((value & 0x10) != 0) ? "counter set to #$" + Convert.toHex(tbLatch, 4) : "counter not set") + ")"
				);
			}
		}


		override protected function initHandlers():void {
			handlers = [
				// the CIA1 chip has 16 registers:
				new IOHandlerInfo(getDataPortA, setDataPortA), // 00
				new IOHandlerInfo(getDataPortB, setDataPortB), // 01
				new IOHandlerInfo(getDefault, setDefault), // 02
				new IOHandlerInfo(getDefault, setDefault), // 03
				new IOHandlerInfo(getDefault, setLoTimerA), // 04
				new IOHandlerInfo(getDefault, setHiTimerA), // 05
				new IOHandlerInfo(getDefault, setLoTimerB), // 06
				new IOHandlerInfo(getDefault, setHiTimerB), // 07
				new IOHandlerInfo(getTODSeconds10, setTODSeconds10), // 08
				new IOHandlerInfo(getTODSeconds, setTODSeconds), // 09
				new IOHandlerInfo(getTODMinutes, setTODMinutes), // 0a
				new IOHandlerInfo(getTODHours, setTODHours), // 0b
				new IOHandlerInfo(getDefault, setDefault), // 0c
				new IOHandlerInfo(getIRQControlReg, setIRQControlReg), // 0d
				new IOHandlerInfo(getControlA, setControlA), // 0e
				new IOHandlerInfo(getControlB, setControlB)  // 0f
			];
		}
	}
}
