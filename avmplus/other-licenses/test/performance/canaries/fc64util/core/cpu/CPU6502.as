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
 
package core.cpu
{
	import core.cpu.CPUOpcodeInfo;
	import core.memory.IMemoryManager;
	import core.misc.Convert;
	import core.events.*;
	import core.exceptions.*;
	import flash.utils.*;
	import flash.events.*;

	[Event(name="cpuResetInternal", type="c64.events.CPUResetEvent")]
	public class CPU6502 implements IEventDispatcher
	{
		/** 6502 accumulator (8 bits) */
		private var a:int;
		/** 6502 register x (8 bits) */
		private var x:int;
		/** 6502 register y (8 bits) */
		private var y:int;
		/** 6502 processor status register (8 bits) */
		private var p:int;
		/** 6502 stack index pointer (8 bits) */
		private var sp:int;
		/** 6502 program counter (16 bits) */
		public var pc:uint;
		
		/** Memory manager */
		private var memory:IMemoryManager;

		/** Opcode info array */
		private var opcodes:Array;

		/** Two complement hash table */
		private var znTable:Array;

		/** Number of consumed cycles per instruction */
		private var cyclesConsumed:uint;
		
		/** Should undocumented instructions be processed? */
		private var useUndocumentedOpcodes:Boolean;

		/** Event Dispatcher */
		private var dispatcher:EventDispatcher;
		
		/** Breakpoint lookup table */
		private var breakpointTable:ByteArray;


		public function CPU6502(mm:IMemoryManager)
		{
			breakpointTable = new ByteArray();
			breakpointTable.length = 0x10000;
			dispatcher = new EventDispatcher();
			memory = mm;
			useUndocumentedOpcodes = true;
			opcodes = getOpcodeTable();
			znTable = getTwoComplementTable();
			reset();
		}
		
		/**
		* Execute instruction at PC
		*
		* @return The number of cycles consumed
		* @throws core.exceptions.BreakpointException
		*/
		public function exec(checkBreakpoints:Boolean = true):uint {
			// read opcode
			var opcode:int = memory.read(pc++);
            Stats.instructions++;
			switch (opcode) {
				case 0x85: // opSTA, byZeroPage
					cyclesConsumed = 3;
					memory.write(memory.read(pc++), a);
					break;
				case 0x8d:	// opSTA, byAbsolute
					cyclesConsumed = 4;
					memory.write(memory.readWord(pc), a);
					pc += 2
					break;
				case 0x91: // opSTA, byIndirectY
					cyclesConsumed = 6;
					memory.write(byIndirectY(), a);
					break;
				case 0xA5: // opLDA, byZeroPage
					cyclesConsumed = 3;
					a = memory.read(memory.read(pc++));
					setStatusFlags(a);
					break;
				case 0xD0: // opBNE, byZeroPage
					cyclesConsumed = 2;
					branch(0x02, false);
					break;
				case 0xF0: // opBEQ, byZeroPage
					cyclesConsumed = 2;
					branch(0x02, true);
					break;
				default:
					var opcodeInfo:CPUOpcodeInfo = CPUOpcodeInfo(opcodes[opcode]);
					var opcodeHandler:Function = opcodeInfo.handler;
					cyclesConsumed = opcodeInfo.cycles;
					opcodeHandler(opcodeInfo.addr);
					break;
			}
			// check for breakpoint
			if(checkBreakpoints && breakpointTable[pc] > 0) {
				throw(new BreakpointException("break", pc, breakpointTable[pc], cyclesConsumed));
			}
			return cyclesConsumed;
		}

		/**
		* Perform a non maskable interrupt
		*
		* @return The number of cycles consumed
		*/
		public function NMI():int {
			pushWord(pc);
			push(p & 0xEF); // clear brk
			pc = memory.readWord(0xFFFA);
			return 7;
		}

		/**
		* Perform an IRQ/BRK interrupt
		*
		* @return The number of cycles consumed
		*/
		public function IRQ():int {
			if ((p & 0x04) == 0x00) {
				pushWord(pc);
				push(p & 0xEF); // clear brk
				pc = memory.readWord(0xFFFE);
				p |= 0x04;
				return 7;
			}
			return 0;
		}

		/**
		* Reset the internal CPU registers
		*/
		public function reset():void {
			var pcOld:uint = pc;
			a = 0x00;
			x = 0x00;
			y = 0x00;
			p = 0x04;
			sp = 0xFF;
			// bank rom/ram/io
			memory.write(0x0000, 0x2f);
			memory.write(0x0001, 0x37);
			// read the reset vector for pc address
			pc = memory.readWord(0xFFFC);
			// fire event
			dispatchEvent(new CPUResetEvent("cpuResetInternal", pcOld, pc));
		}

		/**
		* Get values of processor's registers
		*/
		public function getRegisters():CPURegisters {
			return new CPURegisters(a, x, y, p, sp, pc);
		}


		/**
		* Set Breakpoint
		*/
		public function setBreakpoint(address:uint, type:uint = 1):void {
			breakpointTable[address] = type;
		}

		/**
		* Clear Breakpoint
		*/
		public function clearBreakpoint(address:uint):void {
			breakpointTable[address] = 0;
		}

		/**
		* Get Breakpoint
		*/
		public function getBreakpoint(address:uint):Boolean {
			return breakpointTable[address] > 0;
		}


		public function setUseUndocumentedOpcodes(value:Boolean):void {
			useUndocumentedOpcodes = value;
		}

		public function getUseUndocumentedOpcodes():Boolean {
			return useUndocumentedOpcodes;
		}


		// ==========================================================
		//    O P C O D E   H A N D L E R S
		// ==========================================================

		private function opBRK(addr:Function):void {
			pushWord(pc + 1);
			push(p | 0x10);
			pc = memory.readWord(0xFFFE);
			p |= 0x04;
			p |= 0x10;
			// [CW] todo: dispatch "break" event here
		}

		private function opHLT(addr:Function):void {
			pc--;
		}
		
		private function opNOP(addr:Function):void {
		}
		
		private function opBCC(addr:Function):void {
			branch(0x01, false);
		}
	
		private function opBCS(addr:Function):void {
			branch(0x01, true);
		}
	
		private function opBNE(addr:Function):void {
			branch(0x02, false);
		}
	
		private function opBEQ(addr:Function):void {
			branch(0x02, true);
		}
	
		private function opBVC(addr:Function):void {
			branch(0x40, false);
		}
		
		private function opBVS(addr:Function):void {
			branch(0x40, true);
		}
	
		private function opBPL(addr:Function):void {
			branch(0x80, false);
		}
		
		private function opBMI(addr:Function):void {
			branch(0x80, true);
		}
		
		private function opSEC(addr:Function):void {
			p |= 0x01;
		}
	
		private function opSEI(addr:Function):void {
			p |= 0x04;
		}
	
		private function opSED(addr:Function):void {
			p |= 0x08;
		}

		private function opCLC(addr:Function):void {
			p &= 0xFE;
		}
		
		private function opCLV(addr:Function):void {
			p &= 0xBF;
		}
		
		private function opCLD(addr:Function):void {
			p &= 0xF7;
		}
		
		private function opCLI(addr:Function):void {
			p &= 0xFB;
		}
	
		private function opJSR(addr:Function):void {
			pushWord(pc + 1);
			pc = addr();
		}

		private function opJMP(addr:Function):void {
			pc = addr();
		}

		private function opRTS(addr:Function):void {
			pc = popWord() + 1;
		}
	
		private function opRTI(addr:Function):void {
			p = pop();
			pc = popWord();
		}
	
		private function opAND(addr:Function):void {
			a &= memory.read(addr());
			setStatusFlags(a);
		}
	
		private function opORA(addr:Function):void {
			a |= memory.read(addr());
			setStatusFlags(a);
		}
		
		private function opBIT(addr:Function):void {
			var i:int = memory.read(addr());
			p &= 0x3D;
			p |= i & 0xc0;
			p |= (a & i) != 0 ? 0 : 0x02;
		}
	
		private function opADC(addr:Function):void {
			operateAdd(memory.read(addr()));
		}
	
		private function opSBC(addr:Function):void {
			operateSub(memory.read(addr()));
		}
	
		private function opROL(addr:Function):void {
			var address:int = addr();
			memory.write(address, rol(memory.read(address)));
		}
	
		private function opROL_A(addr:Function):void {
			a = rol(a);
		}
	
		private function opROR(addr:Function):void {
			var address:int = addr();
			memory.write(address, ror(memory.read(address)));
		}
	
		private function opROR_A(addr:Function):void {
			a = ror(a);
		}
	
		private function opASL(addr:Function):void {
			var address:int = addr();
			memory.write(address, asl(memory.read(address)));
		}
		
		private function opASL_A(addr:Function):void {
			a = asl(a);
		}
		
		private function opLSR(addr:Function):void {
			var address:int = addr();
			memory.write(address, lsr(memory.read(address)));
		}
	
		private function opLSR_A(addr:Function):void {
			a = lsr(a);
		}
	
		private function opPLA(addr:Function):void {
			a = pop();
			setStatusFlags(a);
		}
	
		private function opPLP(addr:Function):void {
			p = pop();
		}
	
		private function opPHA(addr:Function):void {
			push(a);
		}
		
		private function opPHP(addr:Function):void {
			push(p | 0x10); // set brk
		}
		
		private function opEOR(addr:Function):void {
			a ^= memory.read(addr());
			setStatusFlags(a);
		}
	
		private function opTAX(addr:Function):void {
			x = a;
			setStatusFlags(x);
		}
	
		private function opTAY(addr:Function):void {
			y = a;
			setStatusFlags(y);
		}
	
		private function opTXA(addr:Function):void {
			a = x;
			setStatusFlags(a);
		}
	
		private function opTYA(addr:Function):void {
			a = y;
			setStatusFlags(a);
		}
	
		private function opTSX(addr:Function):void {
			x = sp & 0xFF;
			setStatusFlags(x);
		}
	
		private function opTXS(addr:Function):void {
			sp = x & 0xFF;
		}
	
		private function opLDA(addr:Function):void {
			a = memory.read(addr());
			setStatusFlags(a);
		}
	
		private function opLDX(addr:Function):void {
			x = memory.read(addr());
			setStatusFlags(x);
		}
	
		private function opLDY(addr:Function):void {
			y = memory.read(addr());
			setStatusFlags(y);
		}
	
		private function opSTA(addr:Function):void {
			memory.write(addr(), a);
		}
	
		private function opSTX(addr:Function):void {
			memory.write(addr(), x);
		}
	
		private function opSTY(addr:Function):void {
			memory.write(addr(), y);
		}
	
		private function opCMP(addr:Function):void {
			var ad:uint = addr();
			operateCmp(a, memory.read(ad));
		}
	
		private function opCPX(addr:Function):void {
			operateCmp(x, memory.read(addr()));
		}
	
		private function opCPY(addr:Function):void {
			operateCmp(y, memory.read(addr()));
		}
	
		private function opDEC(addr:Function):void {
			var address:int = addr();
			memory.write(address, decrement(memory.read(address)));
		}
	
		private function opDEX(addr:Function):void {
			x--;
			x &= 0xFF;
			setStatusFlags(x);
		}
	
		private function opDEY(addr:Function):void {
			y--;
			y &= 0xFF;
			setStatusFlags(y);
		}
	
		private function opINC(addr:Function):void {
			var address:int = addr();
			memory.write(address, increment(memory.read(address)));
		}
	
		private function opINX(addr:Function):void {
			x++;
			x &= 0xFF;
			setStatusFlags(x);
		}
	
		private function opINY(addr:Function):void {
			y++;
			y &= 0xFF;
			setStatusFlags(y);
		}

	
		// ============================================================
		//    O P C O D E   H A N D L E R S  ( U N O F F I C I A L )
		// ============================================================

		private function opASO(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = asl(memory.read(address));
				memory.write(address, value);
				a |= value;
				x = a;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
		
		private function opSKB(addr:Function):void {
			if (useUndocumentedOpcodes) {
				pc++;
			} else {
				usedUndocumentedOpcode();
			}
		}
		
		private function opSKW(addr:Function):void {
			if (useUndocumentedOpcodes) {
				pc += 2;
			} else {
				usedUndocumentedOpcode();
			}
		}
		
		private function opANC(addr:Function):void {
			if (useUndocumentedOpcodes) {
				a &= memory.read(pc++);
				setStatusFlags(a);
				p |= (p & 0x80) >> 7;
			} else {
				usedUndocumentedOpcode();
			}
		}
		
		private function opRLA(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = rol(memory.read(address));
				memory.write(address, value);
				a &= value;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opLSE(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = lsr(memory.read(address));
				memory.write(address, value);
				a ^= value;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opALR(addr:Function):void {
			if (useUndocumentedOpcodes) {
				a &= memory.read(pc++);
				setStatusFlags(a); // [CW] needed? unsure..
				a = lsr(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opRRA(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = ror(memory.read(address));
				memory.write(address, value);
				operateAdd(value); // [CW] was: operateAdd(address). bug?
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opARR(addr:Function):void {
			if (useUndocumentedOpcodes) {
				a &= memory.read(pc++);
				setStatusFlags(a); // [CW] needed? unsure..
				a = ror(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opAXS(addr:Function):void {
			if (useUndocumentedOpcodes) {
				memory.write(addr(), a & x);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opAXA(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = ((address & 0xFF00) >> 8) + 1;
				memory.write(address, a & x & value);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opTAS(addr:Function):void {
			if (useUndocumentedOpcodes) {
				sp = x & a;
				var address:int = addr();
				var value:int = ((address & 0xFF00) >> 8) + 1;
				memory.write(address, value & sp);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opXAA(addr:Function):void {
			// [CW] warning: this opcode seems have odd behaviour
			if (useUndocumentedOpcodes) {
				a = x;
				a &= memory.read(pc++);
				setStatusFlags(a); // [CW] was: setStatusFlags(s). bug?
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opXAS(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = ((address & 0xFF00) >> 8) + 1;
				memory.write(address, value & x);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opSAY(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = ((address & 0xFF00) >> 8) + 1;
				memory.write(address, value & y);
			} else {
				usedUndocumentedOpcode();
			}
		}

		private function opLAX(addr:Function):void {
			if (useUndocumentedOpcodes) {
				a = memory.read(addr());
				x = a;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opLAS(addr:Function):void {
			if (useUndocumentedOpcodes) {
				a = memory.read(addr()) & sp;
				x = a;
				sp = a;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opOAL(addr:Function):void {
			// [CW] warning: this opcode seems have odd behaviour
			if (useUndocumentedOpcodes) {
				a |= 0xEE;
				a &= memory.read(addr());
				x = a;
				setStatusFlags(a);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opDCM(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = (memory.read(address) - 1) & 0xFF;
				memory.write(address, value);
				operateCmp(a, value);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opSAX(addr:Function):void {
			if (useUndocumentedOpcodes) {
				x = (a & x) - memory.read(addr());
				p |= x < 0 ? 0 : 1;
				x &= 0xFF;
				setStatusFlags(x);
			} else {
				usedUndocumentedOpcode();
			}
		}
	
		private function opINS(addr:Function):void {
			if (useUndocumentedOpcodes) {
				var address:int = addr();
				var value:int = increment(memory.read(address));
				memory.write(address, value);
				operateSub(value);
			} else {
				usedUndocumentedOpcode();
			}
		}

	
		// ==========================================================
		//    A D D R E S S I N G   M O D E   M E T H O D S
		// ==========================================================

		/**
		* Get value by immediate mode addressing - #$00
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byImmediate():int {
			return pc++;
		}

		/**
		* Get value by zero page mode addressing - $aa
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byZeroPage():int {
			return memory.read(pc++);
		}

		/**
		* Get value by zero page x mode addressing - $aa,x
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byZeroPageX():int {
			return memory.read(pc++) + x & 0xFF;
		}

		/**
		* Get value by zero page y mode addressing - $aa,y
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		public function byZeroPageY():int {
			return memory.read(pc++) + y & 0xFF;
		}

		/**
		* Get value by absolute mode addressing - $aaaa
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byAbsolute():int {
			var address:int = memory.readWord(pc);
			pc += 2;
			return address;
		}

		/**
		* Get value by absolute x mode addressing - $aaaa,x
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byAbsoluteX():int {
			var i:int = memory.readWord(pc);
			var j:int = i + x;
			// CW: no cycle is added on bound cross for the opcodes 
			// ASL, DEC and INC (they all have 7 cycles fixed)
			if(/*cyclesConsumed != 7 &&*/ ((j ^ i) & 0x100) != 0) {
				cyclesConsumed++;
			}
			pc += 2;
			return j;
		}

		/**
		* Get value by absolute y mode addressing - $aaaa,y
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byAbsoluteY():int {
			var i:int = memory.readWord(pc);
			var j:int = i + y;
			if(((j ^ i) & 0x100) != 0) {
				cyclesConsumed++;
			}
			pc += 2;
			return j;
		}

		/**
		* Get value by indirect mode addressing - ($aaaa)
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byIndirect():int {
			var i:int = memory.readWord(pc);
			pc += 2;
			if ((i & 0x00FF) == 0xFF) {
				return (memory.read(i & 0xFF00) << 8) | memory.read(i);
			} else {
				return memory.readWord(i);
			}
		}

		/**
		* Get value by indirect x mode addressing - ($aa,x)
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byIndirectX():int {
			return memory.readWord((memory.read(pc++) + x) & 0xFF);
		}

		/**
		* Read value by indirect y mode addressing - ($aa),y
		*
		* @return The value by the specified addressing mode in relation to the current px
		*/
		private function byIndirectY():int {
			var i:int = memory.readWord(memory.read(pc++));
			var j:int = i + y;
			if(((j ^ i) & 0x100) != 0) {
				cyclesConsumed++;
			}
			return j;
		}


		// ==========================================================
		//    U T I L I T Y    M E T H O D S
		// ==========================================================

		/**
		* Set the zero and negative status flags
		*/
		private function setStatusFlags(value:uint):void {
			p &= 0x7D;
			p |= znTable[value];
		}

		/**
		* Perform arithmetic shift left
		*/
		private function asl(i:int):int {
			p &= 0x7C;
			p |= i >> 7;
			i <<= 1;
			i &= 0xFF;
			p |= znTable[i];
			return i;
		}

		/**
		* Perform logical shift right
		*/
		private function lsr(i:int):int {
			p &= 0x7C;
			p |= i & 0x01;
			i >>= 1;
			p |= znTable[i];
			return i;
		}

		/**
		* Perform rotate left
		*/
		private function rol(i:int):int {
			i <<= 1;
			i |= p & 0x01;
			p &= 0x7C;
			p |= i >> 8;
			i &= 0xFF;
			p |= znTable[i];
			return i;
		}

		/**
		* Perform rotate right
		*/
		private function ror(i:int):int {
			var j:int = p & 0x01;
			p &= 0x7C;
			p |= i & 0x01;
			i >>= 1;
			i |= j << 7;
			p |= znTable[i];
			return i;
		}

		/**
		* Perform increment
		*/
		private function increment(i:int):int {
			i = ++i & 0xFF;
			setStatusFlags(i);
			return i;
		}

		/**
		* Perform decrement
		*/
		private function decrement(i:int):int {
			i = --i & 0xFF;
			setStatusFlags(i);
			return i;
		}

		/**
		* Perform add with carry
		*/
		private function operateAdd(i:int):void {
			// store carry
			var k:int = p & 0x01;
			// store result
			var j:int = a + i + k;
			// turn off czn
			p &= 0x3c;
			// set overflow
			p |= (~(a ^ i) & (a ^ i) & 0x80) == 0 ? 0 : 0x40;
			// set carry
			p |= j <= 255 ? 0 : 0x01;
			a = j & 0xFF;
			// set zn in p
			p |= znTable[a];
		}

		/**
		* Perform subtract with carry
		*/
		private function operateSub(i:int):void {
			// store carry
			var k:int = ~p & 0x01;
			// store result
			var j:int = a - i - k;
			// turn off czn
			p &= 0x3C;
			// set overflow
			p |= (~(a ^ i) & (a ^ i) & 0x80) == 0 ? 0 : 0x40;
			// set carry
			p |= j < 0 ? 0 : 0x01;
			a = j & 0xFF;
			// set zn in p
			p |= znTable[a];
		}

		/**
		* Perform compare function
		*/
		private function operateCmp(i:int, j:int):void {
			var k:int = i - j;
			p &= 0x7C;
			p |= k < 0 ? 0 : 0x01;
			p |= znTable[k & 0xFF];
		}

		/**
		* Handle branch
		*/
		private function branch(flagNum:int, flagVal:Boolean):void {
			var offset:int = memory.read(pc++);
			if ( ((p & flagNum) != 0) == flagVal ) {
				if(offset & 0x80) {
					offset = -(~offset & 0xff) - 1;
				}
				if(((pc ^ (pc + offset)) & 0x100) != 0) {
					cyclesConsumed++;
				}
				pc += offset;
				cyclesConsumed += 1;
			}
		}

		/**
		* Consume (skip) an undocumented opcode
		* [CW] we should either _really_ skip it or halt the processor
		*/
		private function usedUndocumentedOpcode():void {
		}


		// ==========================================================
		//    S T A C K   A C C E S S   M E T H O D S
		// ==========================================================

		/**
		* Push a byte onto the stack
		*/
		private function push(value:int):void {
			memory.writeStack(sp, value);
			sp--;
			sp &= 0xFF;
		}

		/**
		* Push a word onto the stack
		*/
		private function pushWord(value:int):void {
			push((value >> 8) & 0xFF);
			push(value & 0xFF);
		}

		/**
		* Pop a byte from stack
		*/
		private function pop():int {
			sp++;
			sp &= 0xFF;
			return memory.readStack(sp);
		}

		/**
		* Pop a word from stack
		*/
		private function popWord():int {
			return pop() + pop() * 256;
		}


		// ==========================================================
		//    D I S A S S E M B L E R
		// ==========================================================

		public function disassemble(address:int, instructionCount:int = 1, dumpAdr:Boolean = true, dumpHex:Boolean = true):String {
			var ret:String = "";
			for(var i:int = 1; i <= instructionCount; i++) {
				var d:String = "";
				var argument:int = 0;
				var opcode:int = memory.read(address);
				var opcodeInfo:CPUOpcodeInfo = CPUOpcodeInfo(opcodes[opcode]);
				if(i > 1) {
					d += "\n";
				}
				if(dumpAdr) {
					d += Convert.toHex(address, 4) + ":  ";
				}
				address++;
				if(dumpHex) {
					var lo:int;
					var hex:String = "";
					d += Convert.toHex(opcode, 2) + " ";
					switch(opcodeInfo.len) {
						case 1:
							hex = "       ";
							break;
						case 2:
							lo = memory.read(address++);
							argument = lo;
							hex = Convert.toHex(lo, 2) + "     ";
							break;
						case 3:
							lo = memory.read(address++);
							var hi:int = memory.read(address++);
							argument = lo + hi * 256;
							hex = Convert.toHex(lo, 2) + " " + Convert.toHex(hi, 2) + "  ";
							break;
					}
					d += hex;
				}
				var mnemo:String = opcodeInfo.mnemo;
				if(opcodeInfo.len == 2) {
					mnemo = mnemo.split("aa").join(Convert.toHex(argument, 2));
				} else if(opcodeInfo.len == 3) {
					mnemo = mnemo.split("aaaa").join(Convert.toHex(argument, 4));
				}
				d += mnemo;
				ret += d.toUpperCase();
			}
			return ret;
		}


		// ==========================================================
		//    C L A S S   I N I T I A L I Z A T I O N
		// ==========================================================

		private function getOpcodeTable():Array {
			return [
				new CPUOpcodeInfo(7, 1, opBRK,   null,        "brk"),         // 00
				new CPUOpcodeInfo(6, 2, opORA,   byIndirectX, "ora ($aa,x)"), // 01
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 02
				new CPUOpcodeInfo(8, 2, opASO,   byIndirectX, "aso ($aa,x)"), // 03
				new CPUOpcodeInfo(3, 1, opSKB,   null,        "skb"),         // 04
				new CPUOpcodeInfo(3, 2, opORA,   byZeroPage,  "ora $aa"),     // 05
				new CPUOpcodeInfo(5, 2, opASL,   byZeroPage,  "asl $aa"),     // 06
				new CPUOpcodeInfo(5, 2, opASO,   byZeroPage,  "aso $aa"),     // 07
				new CPUOpcodeInfo(3, 1, opPHP,   null,        "php"),         // 08
				new CPUOpcodeInfo(2, 2, opORA,   byImmediate, "ora #$aa"),    // 09
				new CPUOpcodeInfo(2, 1, opASL_A, null,        "asl a"),       // 0a
				new CPUOpcodeInfo(2, 2, opANC,   byImmediate, "anc #$aa"),    // 0b
				new CPUOpcodeInfo(4, 1, opSKW,   null,        "skw"),         // 0c
				new CPUOpcodeInfo(4, 3, opORA,   byAbsolute,  "ora $aaaa"),   // 0d
				new CPUOpcodeInfo(6, 3, opASL,   byAbsolute,  "asl $aaaa"),   // 0e
				new CPUOpcodeInfo(6, 3, opASO,   byAbsolute,  "aso $aaaa"),   // 0f
				new CPUOpcodeInfo(2, 2, opBPL,   byZeroPage,  "bpl $aa"),     // 10
				new CPUOpcodeInfo(5, 2, opORA,   byIndirectY, "ora ($aa),y"), // 11
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 12
				new CPUOpcodeInfo(8, 2, opASO,   byIndirectY, "aso ($aa),y"), // 13
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // 14
				new CPUOpcodeInfo(4, 2, opORA,   byZeroPageX, "ora $aa,x"),   // 15
				new CPUOpcodeInfo(6, 2, opASL,   byZeroPageX, "asl $aa,x"),   // 16
				new CPUOpcodeInfo(6, 2, opASO,   byZeroPageX, "aso $aa,x"),   // 17
				new CPUOpcodeInfo(2, 1, opCLC,   null,        "clc"),         // 18
				new CPUOpcodeInfo(4, 3, opORA,   byAbsoluteY, "ora $aaaa,y"), // 19
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // 1a
				new CPUOpcodeInfo(7, 3, opASO,   byAbsoluteY, "aso $aaaa,y"), // 1b
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // 1c
				new CPUOpcodeInfo(4, 3, opORA,   byAbsoluteX, "ora $aaaa,x"), // 1d was 5
				new CPUOpcodeInfo(7, 3, opASL,   byAbsoluteX, "asl $aaaa,x"), // 1e
				new CPUOpcodeInfo(7, 3, opASO,   byAbsoluteX, "aso $aaaa,x"), // 1f
				new CPUOpcodeInfo(6, 3, opJSR,   byAbsolute,  "jsr $aaaa"),   // 20
				new CPUOpcodeInfo(6, 2, opAND,   byIndirectX, "and ($aa,x)"), // 21
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 22
				new CPUOpcodeInfo(8, 2, opRLA,   byIndirectX, "rla ($aa,x)"), // 23
				new CPUOpcodeInfo(3, 2, opBIT,   byZeroPage,  "bit $aa"),     // 24
				new CPUOpcodeInfo(3, 2, opAND,   byZeroPage,  "and $aa"),     // 25
				new CPUOpcodeInfo(5, 2, opROL,   byZeroPage,  "rol $aa"),     // 26
				new CPUOpcodeInfo(5, 2, opRLA,   byZeroPage,  "rla $aa"),     // 27
				new CPUOpcodeInfo(4, 1, opPLP,   null,        "plp"),         // 28
				new CPUOpcodeInfo(2, 2, opAND,   byImmediate, "and #$aa"),    // 29
				new CPUOpcodeInfo(2, 1, opROL_A, null,        "rol a"),       // 2a
				new CPUOpcodeInfo(2, 2, opANC,   byImmediate, "anc #$aa"),    // 2b
				new CPUOpcodeInfo(4, 3, opBIT,   byAbsolute,  "bit $aaaa"),   // 2c
				new CPUOpcodeInfo(4, 3, opAND,   byAbsolute,  "and $aaaa"),   // 2d
				new CPUOpcodeInfo(6, 3, opROL,   byAbsolute,  "rol $aaaa"),   // 2e
				new CPUOpcodeInfo(6, 3, opRLA,   byAbsolute,  "rla $aaaa"),   // 2f
				new CPUOpcodeInfo(2, 2, opBMI,   byZeroPage,  "bmi $aa"),     // 30
				new CPUOpcodeInfo(5, 2, opAND,   byIndirectY, "and ($aa),y"), // 31
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 32
				new CPUOpcodeInfo(8, 2, opRLA,   byIndirectY, "rla ($aa),y"), // 33
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // 34
				new CPUOpcodeInfo(4, 2, opAND,   byZeroPageX, "and $aa,x"),   // 35
				new CPUOpcodeInfo(6, 2, opROL,   byZeroPageX, "rol $aa,x"),   // 36
				new CPUOpcodeInfo(6, 2, opRLA,   byZeroPageX, "rla $aa,x"),   // 37
				new CPUOpcodeInfo(2, 1, opSEC,   null,        "sec"),         // 38
				new CPUOpcodeInfo(4, 3, opAND,   byAbsoluteY, "and $aaaa,y"), // 39
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // 3a
				new CPUOpcodeInfo(7, 3, opRLA,   byAbsoluteY, "rla $aaaa,y"), // 3b
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // 3c
				new CPUOpcodeInfo(4, 3, opAND,   byAbsoluteX, "and $aaaa,x"), // 3d was 5
				new CPUOpcodeInfo(7, 3, opROL,   byAbsoluteX, "rol $aaaa,x"), // 3e
				new CPUOpcodeInfo(7, 3, opRLA,   byAbsoluteX, "rla $aaaa,x"), // 3f
				new CPUOpcodeInfo(6, 1, opRTI,   null,        "rti"),         // 40 was 4
				new CPUOpcodeInfo(6, 2, opEOR,   byIndirectX, "eor ($aa,x)"), // 41
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 42
				new CPUOpcodeInfo(8, 2, opLSE,   byIndirectX, "lse ($aa,x)"), // 43
				new CPUOpcodeInfo(3, 1, opSKB,   null,        "skb"),         // 44
				new CPUOpcodeInfo(3, 2, opEOR,   byZeroPage,  "eor $aa"),     // 45
				new CPUOpcodeInfo(5, 2, opLSR,   byZeroPage,  "lsr $aa"),     // 46
				new CPUOpcodeInfo(5, 2, opLSE,   byZeroPage,  "lse $aa"),     // 47
				new CPUOpcodeInfo(3, 1, opPHA,   null,        "pha"),         // 48
				new CPUOpcodeInfo(2, 2, opEOR,   byImmediate, "eor #$aa"),    // 49
				new CPUOpcodeInfo(2, 1, opLSR_A, null,        "lsr a"),       // 4a
				new CPUOpcodeInfo(2, 2, opALR,   byImmediate, "alr #$aa"),    // 4b
				new CPUOpcodeInfo(3, 3, opJMP,   byAbsolute,  "jmp $aaaa"),   // 4c
				new CPUOpcodeInfo(4, 3, opEOR,   byAbsolute,  "eor $aaaa"),   // 4d was 6
				new CPUOpcodeInfo(6, 3, opLSR,   byAbsolute,  "lsr $aaaa"),   // 4e
				new CPUOpcodeInfo(6, 3, opLSE,   byAbsolute,  "lse $aaaa"),   // 4f
				new CPUOpcodeInfo(2, 2, opBVC,   byZeroPage,  "bvc $aa"),     // 50
				new CPUOpcodeInfo(5, 2, opEOR,   byIndirectY, "eor ($aa),y"), // 51
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 52
				new CPUOpcodeInfo(8, 2, opLSE,   byIndirectY, "lse ($aa),y"), // 53
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // 54
				new CPUOpcodeInfo(4, 2, opEOR,   byZeroPageX, "eor $aa,x"),   // 55
				new CPUOpcodeInfo(6, 2, opLSR,   byZeroPageX, "lsr $aa,x"),   // 56
				new CPUOpcodeInfo(6, 2, opLSE,   byZeroPageX, "lse $aa,x"),   // 57
				new CPUOpcodeInfo(2, 1, opCLI,   null,        "cli"),         // 58
				new CPUOpcodeInfo(4, 3, opEOR,   byAbsoluteY, "eor $aaaa,y"), // 59
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // 5a
				new CPUOpcodeInfo(7, 3, opLSE,   byAbsoluteY, "lse $aaaa,y"), // 5b
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // 5c
				new CPUOpcodeInfo(4, 3, opEOR,   byAbsoluteX, "eor $aaaa,x"), // 5d was 5
				new CPUOpcodeInfo(7, 3, opLSR,   byAbsoluteX, "lsr $aaaa,x"), // 5e
				new CPUOpcodeInfo(7, 3, opLSE,   byAbsoluteX, "lse $aaaa,x"), // 5f
				new CPUOpcodeInfo(6, 1, opRTS,   null,        "rts"),         // 60 was 4
				new CPUOpcodeInfo(6, 2, opADC,   byIndirectX, "adc ($aa,x)"), // 61
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 62
				new CPUOpcodeInfo(8, 2, opRRA,   byIndirectX, "rra ($aa,x)"), // 63
				new CPUOpcodeInfo(3, 1, opSKB,   null,        "skb"),         // 64
				new CPUOpcodeInfo(3, 2, opADC,   byZeroPage,  "adc $aa"),     // 65
				new CPUOpcodeInfo(5, 2, opROR,   byZeroPage,  "ror $aa"),     // 66
				new CPUOpcodeInfo(5, 2, opRRA,   byZeroPage,  "rra $aa"),     // 67
				new CPUOpcodeInfo(4, 1, opPLA,   null,        "pla"),         // 68
				new CPUOpcodeInfo(2, 2, opADC,   byImmediate, "adc #$aa"),    // 69
				new CPUOpcodeInfo(2, 1, opROR_A, null,        "ror a"),       // 6a
				new CPUOpcodeInfo(2, 2, opARR,   byImmediate, "arr #$aa"),    // 6b
				new CPUOpcodeInfo(5, 3, opJMP,   byIndirect,  "jmp ($aaaa)"), // 6c
				new CPUOpcodeInfo(4, 3, opADC,   byAbsolute,  "adc $aaaa"),   // 6d
				new CPUOpcodeInfo(6, 3, opROR,   byAbsolute,  "ror $aaaa"),   // 6e
				new CPUOpcodeInfo(6, 3, opRRA,   byAbsolute,  "rra $aaaa"),   // 6f
				new CPUOpcodeInfo(4, 2, opBVS,   byZeroPage,  "bvs $aa"),     // 70
				new CPUOpcodeInfo(5, 2, opADC,   byIndirectY, "adc ($aa),y"), // 71
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 72
				new CPUOpcodeInfo(3, 2, opRRA,   byIndirectY, "rra ($aa),y"), // 73
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // 74
				new CPUOpcodeInfo(4, 2, opADC,   byZeroPageX, "adc $aa,x"),   // 75
				new CPUOpcodeInfo(6, 2, opROR,   byZeroPageX, "ror $aa,x"),   // 76
				new CPUOpcodeInfo(6, 2, opRRA,   byZeroPageX, "rra $aa,x"),   // 77
				new CPUOpcodeInfo(2, 1, opSEI,   null,        "sei"),         // 78
				new CPUOpcodeInfo(4, 3, opADC,   byAbsoluteY, "adc $aaaa,y"), // 79
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // 7a
				new CPUOpcodeInfo(7, 3, opRRA,   byAbsoluteY, "rra $aaaa,y"), // 7b
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // 7c
				new CPUOpcodeInfo(4, 3, opADC,   byAbsoluteX, "adc $aaaa,x"), // 7d was 5
				new CPUOpcodeInfo(7, 3, opROR,   byAbsoluteX, "ror $aaaa,x"), // 7e
				new CPUOpcodeInfo(7, 3, opRRA,   byAbsoluteX, "rra $aaaa,x"), // 7f
				new CPUOpcodeInfo(2, 1, opSKB,   null,        "skb"),         // 80
				new CPUOpcodeInfo(6, 2, opSTA,   byIndirectX, "sta ($aa,x)"), // 81
				new CPUOpcodeInfo(2, 1, opSKB,   null,        "skb"),         // 82
				new CPUOpcodeInfo(6, 2, opAXS,   byIndirectX, "axs ($aa,x)"), // 83
				new CPUOpcodeInfo(3, 2, opSTY,   byZeroPage,  "sty $aa"),     // 84
				new CPUOpcodeInfo(3, 2, opSTA,   byZeroPage,  "sta $aa"),     // 85
				new CPUOpcodeInfo(3, 2, opSTX,   byZeroPage,  "stx $aa"),     // 86
				new CPUOpcodeInfo(3, 2, opAXS,   byZeroPage,  "axs $aa"),     // 87
				new CPUOpcodeInfo(2, 1, opDEY,   null,        "dey"),         // 88
				new CPUOpcodeInfo(2, 1, opSKB,   null,        "skb"),         // 89
				new CPUOpcodeInfo(2, 1, opTXA,   null,        "txa"),         // 8a
				new CPUOpcodeInfo(2, 2, opXAA,   byImmediate, "xaa #$aa"),    // 8b
				new CPUOpcodeInfo(4, 3, opSTY,   byAbsolute,  "sty $aaaa"),   // 8c
				new CPUOpcodeInfo(4, 3, opSTA,   byAbsolute,  "sta $aaaa"),   // 8d
				new CPUOpcodeInfo(4, 3, opSTX,   byAbsolute,  "stx $aaaa"),   // 8e
				new CPUOpcodeInfo(4, 3, opAXS,   byAbsolute,  "axs $aaaa"),   // 8f
				new CPUOpcodeInfo(2, 2, opBCC,   byZeroPage,  "bcc $aa"),     // 90
				new CPUOpcodeInfo(6, 2, opSTA,   byIndirectY, "sta ($aa),y"), // 91
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // 92
				new CPUOpcodeInfo(6, 2, opAXA,   byIndirectY, "axa ($aa),y"), // 93
				new CPUOpcodeInfo(4, 2, opSTY,   byZeroPageX, "sty $aa,x"),   // 94
				new CPUOpcodeInfo(4, 2, opSTA,   byZeroPageX, "sta $aa,x"),   // 95
				new CPUOpcodeInfo(4, 2, opSTX,   byZeroPageY, "stx $aa,y"),   // 96
				new CPUOpcodeInfo(4, 2, opAXS,   byZeroPageY, "axs $aa,y"),   // 97
				new CPUOpcodeInfo(2, 1, opTYA,   null,        "tya"),         // 98
				new CPUOpcodeInfo(5, 3, opSTA,   byAbsoluteY, "sta $aaaa,y"), // 99
				new CPUOpcodeInfo(2, 1, opTXS,   null,        "txs"),         // 9a
				new CPUOpcodeInfo(5, 3, opTAS,   byAbsoluteY, "tas $aaaa,y"), // 9b
				new CPUOpcodeInfo(5, 3, opSAY,   byAbsoluteX, "say $aaaa,x"), // 9c
				new CPUOpcodeInfo(5, 3, opSTA,   byAbsoluteX, "sta $aaaa,x"), // 9d
				new CPUOpcodeInfo(5, 3, opXAS,   byAbsoluteY, "xas $aaaa,y"), // 9e
				new CPUOpcodeInfo(5, 3, opAXA,   byAbsoluteY, "axa $aaaa,y"), // 9f
				new CPUOpcodeInfo(2, 2, opLDY,   byImmediate, "ldy #$aa"),    // a0
				new CPUOpcodeInfo(6, 2, opLDA,   byIndirectX, "lda ($aa,x)"), // a1
				new CPUOpcodeInfo(2, 2, opLDX,   byImmediate, "ldx #$aa"),    // a2
				new CPUOpcodeInfo(6, 2, opLAX,   byIndirectX, "lax ($aa,x)"), // a3
				new CPUOpcodeInfo(3, 2, opLDY,   byZeroPage,  "ldy $aa"),     // a4
				new CPUOpcodeInfo(3, 2, opLDA,   byZeroPage,  "lda $aa"),     // a5
				new CPUOpcodeInfo(3, 2, opLDX,   byZeroPage,  "ldx $aa"),     // a6
				new CPUOpcodeInfo(3, 2, opLAX,   byZeroPage,  "lax $aa"),     // a7
				new CPUOpcodeInfo(2, 1, opTAY,   null,        "tay"),         // a8
				new CPUOpcodeInfo(2, 2, opLDA,   byImmediate, "lda #$aa"),    // a9
				new CPUOpcodeInfo(2, 1, opTAX,   null,        "tax"),         // aa
				new CPUOpcodeInfo(2, 2, opOAL,   byImmediate, "oal #$aa"),    // ab
				new CPUOpcodeInfo(4, 3, opLDY,   byAbsolute,  "ldy $aaaa"),   // ac
				new CPUOpcodeInfo(4, 3, opLDA,   byAbsolute,  "lda $aaaa"),   // ad
				new CPUOpcodeInfo(4, 3, opLDX,   byAbsolute,  "ldx $aaaa"),   // ae
				new CPUOpcodeInfo(4, 3, opLAX,   byAbsolute,  "lax $aaaa"),   // af
				new CPUOpcodeInfo(2, 2, opBCS,   byZeroPage,  "bcs $aa"),     // b0
				new CPUOpcodeInfo(5, 2, opLDA,   byIndirectY, "lda ($aa),y"), // b1
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // b2
				new CPUOpcodeInfo(5, 2, opLAX,   byIndirectY, "lax ($aa),y"), // b3
				new CPUOpcodeInfo(4, 2, opLDY,   byZeroPageX, "ldy $aa,x"),   // b4
				new CPUOpcodeInfo(4, 2, opLDA,   byZeroPageX, "lda $aa,x"),   // b5
				new CPUOpcodeInfo(4, 2, opLDX,   byZeroPageY, "ldx $aa,y"),   // b6
				new CPUOpcodeInfo(4, 2, opLAX,   byZeroPageY, "lax $aa,y"),   // b7
				new CPUOpcodeInfo(2, 1, opCLV,   null,        "clv"),         // b8
				new CPUOpcodeInfo(4, 3, opLDA,   byAbsoluteY, "lda $aaaa,y"), // b9
				new CPUOpcodeInfo(2, 1, opTSX,   null,        "tsx"),         // ba
				new CPUOpcodeInfo(4, 3, opLAS,   byIndirectY, "las $aaaa,y"), // bb
				new CPUOpcodeInfo(4, 3, opLDY,   byAbsoluteX, "ldy $aaaa,x"), // bc
				new CPUOpcodeInfo(4, 3, opLDA,   byAbsoluteX, "lda $aaaa,x"), // bd
				new CPUOpcodeInfo(4, 3, opLDX,   byAbsoluteY, "ldx $aaaa,y"), // be
				new CPUOpcodeInfo(4, 3, opLAX,   byAbsoluteY, "lax $aaaa,y"), // bf
				new CPUOpcodeInfo(2, 2, opCPY,   byImmediate, "cpy #$aa"),    // c0
				new CPUOpcodeInfo(6, 2, opCMP,   byIndirectX, "cmp ($aa,x)"), // c1
				new CPUOpcodeInfo(2, 1, opSKB,   null,        "skb"),         // c2
				new CPUOpcodeInfo(8, 2, opDCM,   byIndirectX, "dcm ($aa,x)"), // c3
				new CPUOpcodeInfo(3, 2, opCPY,   byZeroPage,  "cpy $aa"),     // c4
				new CPUOpcodeInfo(3, 2, opCMP,   byZeroPage,  "cmp $aa"),     // c5
				new CPUOpcodeInfo(5, 2, opDEC,   byZeroPage,  "dec $aa"),     // c6
				new CPUOpcodeInfo(5, 2, opDCM,   byZeroPage,  "dcm $aa"),     // c7
				new CPUOpcodeInfo(2, 1, opINY,   null,        "iny"),         // c8
				new CPUOpcodeInfo(2, 2, opCMP,   byImmediate, "cmp #$aa"),    // c9
				new CPUOpcodeInfo(2, 1, opDEX,   null,        "dex"),         // ca
				new CPUOpcodeInfo(2, 2, opSAX,   byImmediate, "sax #$aa"),    // cb
				new CPUOpcodeInfo(4, 3, opCPY,   byAbsolute,  "cpy $aaaa"),   // cc
				new CPUOpcodeInfo(4, 3, opCMP,   byAbsolute,  "cmp $aaaa"),   // cd
				new CPUOpcodeInfo(6, 3, opDEC,   byAbsolute,  "dec $aaaa"),   // ce
				new CPUOpcodeInfo(6, 3, opDCM,   byAbsolute,  "dcm $aaaa"),   // cf
				new CPUOpcodeInfo(2, 2, opBNE,   byZeroPage,  "bne $aa"),     // d0
				new CPUOpcodeInfo(5, 2, opCMP,   byIndirectY, "cmp ($aa),y"), // d1
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // d2
				new CPUOpcodeInfo(8, 2, opDCM,   byIndirectY, "dcm ($aa),y"), // d3
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // d4
				new CPUOpcodeInfo(4, 2, opCMP,   byZeroPageX, "cmp $aa,x"),   // d5
				new CPUOpcodeInfo(6, 2, opDEC,   byZeroPageX, "dec $aa,x"),   // d6
				new CPUOpcodeInfo(6, 2, opDCM,   byZeroPageX, "dcm $aa,x"),   // d7
				new CPUOpcodeInfo(2, 1, opCLD,   null,        "cld"),         // d8
				new CPUOpcodeInfo(4, 3, opCMP,   byAbsoluteY, "cmp $aaaa,y"), // d9
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // da
				new CPUOpcodeInfo(7, 3, opDCM,   byAbsoluteY, "dcm $aaaa,y"), // db
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // dc
				new CPUOpcodeInfo(4, 3, opCMP,   byAbsoluteX, "cmp $aaaa,x"), // dd was 5
				new CPUOpcodeInfo(7, 3, opDEC,   byAbsoluteX, "dec $aaaa,x"), // de
				new CPUOpcodeInfo(7, 3, opDCM,   byAbsoluteX, "dcm $aaaa,x"), // df
				new CPUOpcodeInfo(2, 2, opCPX,   byImmediate, "cpx #$aa"),    // e0
				new CPUOpcodeInfo(6, 2, opSBC,   byIndirectX, "sbc ($aa,x)"), // e1
				new CPUOpcodeInfo(2, 1, opSKB,   null,        "skb"),         // e2
				new CPUOpcodeInfo(8, 2, opINS,   byIndirectX, "ins ($aa,x)"), // e3
				new CPUOpcodeInfo(3, 2, opCPX,   byZeroPage,  "cpx $aa"),     // e4
				new CPUOpcodeInfo(3, 2, opSBC,   byZeroPage,  "sbc $aa"),     // e5
				new CPUOpcodeInfo(5, 2, opINC,   byZeroPage,  "inc $aa"),     // e6
				new CPUOpcodeInfo(5, 2, opINS,   byZeroPage,  "ins $aa"),     // e7
				new CPUOpcodeInfo(2, 1, opINX,   null,        "inx"),         // e8
				new CPUOpcodeInfo(2, 2, opSBC,   byImmediate, "sbc #$aa"),    // e9
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // ea
				new CPUOpcodeInfo(2, 2, opSBC,   byImmediate, "sbc #$aa"),    // eb
				new CPUOpcodeInfo(4, 3, opCPX,   byAbsolute,  "cpx $aaaa"),   // ec
				new CPUOpcodeInfo(4, 3, opSBC,   byAbsolute,  "sbc $aaaa"),   // ed
				new CPUOpcodeInfo(6, 3, opINC,   byAbsolute,  "inc $aaaa"),   // ee
				new CPUOpcodeInfo(6, 3, opINS,   byAbsolute,  "ins $aaaa"),   // ef
				new CPUOpcodeInfo(2, 2, opBEQ,   byZeroPage,  "beq $aa"),     // f0
				new CPUOpcodeInfo(5, 2, opSBC,   byIndirectY, "sbc ($aa),y"), // f1
				new CPUOpcodeInfo(2, 1, opHLT,   null,        "hlt"),         // f2
				new CPUOpcodeInfo(8, 2, opINS,   byIndirectY, "ins ($aa),y"), // f3
				new CPUOpcodeInfo(4, 1, opSKB,   null,        "skb"),         // f4
				new CPUOpcodeInfo(4, 2, opSBC,   byZeroPageX, "sbc $aa,x"),   // f5
				new CPUOpcodeInfo(6, 2, opINC,   byZeroPageX, "inc $aa,x"),   // f6
				new CPUOpcodeInfo(6, 2, opINS,   byZeroPageX, "ins $aa,x"),   // f7
				new CPUOpcodeInfo(2, 1, opSED,   null,        "sed"),         // f8
				new CPUOpcodeInfo(4, 3, opSBC,   byAbsoluteY, "sbc $aaaa,y"), // f9
				new CPUOpcodeInfo(2, 1, opNOP,   null,        "nop"),         // fa
				new CPUOpcodeInfo(7, 3, opINS,   byAbsoluteY, "ins $aaaa,y"), // fb
				new CPUOpcodeInfo(5, 1, opSKW,   null,        "skw"),         // fc
				new CPUOpcodeInfo(4, 3, opSBC,   byAbsoluteX, "sbc $aaaa,x"), // fd was 5
				new CPUOpcodeInfo(7, 3, opINC,   byAbsoluteX, "inc $aaaa,x"), // fe
				new CPUOpcodeInfo(7, 3, opINS,   byAbsoluteX, "ins $aaaa,x")  // ff
			];
		}
		
		private function getTwoComplementTable():Array {
			return [
				002, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
				000, 000, 000, 000, 000, 000, 000, 000, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
				128, 128, 128, 128, 128, 128
			];
		}

	
		// ==========================================================
		//    E V E N T   D I S P A T C H E R   P R O X Y
		// ==========================================================

		public function addEventListener(type:String, listener:Function, useCapture:Boolean = false, priority:int = 0, useWeakReference:Boolean = false):void{
			dispatcher.addEventListener(type, listener, useCapture, priority);
		}
		
		public function dispatchEvent(evt:Event):Boolean{
			return dispatcher.dispatchEvent(evt);
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
