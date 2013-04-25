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
	import core.cpu.*;
	import core.memory.*;
	import core.misc.StringUtils;

	public class ASM6502
	{
		private static const ADDR_MODE_IMMEDIATE:uint = 0;
		private static const ADDR_MODE_ABSOLUTE:uint = 1;
		private static const ADDR_MODE_ABSOLUTE_X:uint = 2;
		private static const ADDR_MODE_ABSOLUTE_Y:uint = 3;
		private static const ADDR_MODE_INDIRECT:uint = 4;
		private static const ADDR_MODE_INDIRECT_X:uint = 5;
		private static const ADDR_MODE_INDIRECT_Y:uint = 6;
		private static const ADDR_MODE_ZEROPAGE:uint = 7;
		private static const ADDR_MODE_ZEROPAGE_X:uint = 8;
		private static const ADDR_MODE_ZEROPAGE_Y:uint = 9;

		protected var cpu:CPU6502;
		protected var mem:IMemoryManager;
		protected var addr:int;

		protected var opcodeTable:*;
		protected var opcodeTableRelative:*;
		protected var opcodeTableImmediate:*;
		protected var opcodeTableAccumulator:*;
		
		public function ASM6502(cpu:CPU6502, mem:IMemoryManager, addr:int) {
			this.cpu = cpu;
			this.mem = mem;
			this.addr = addr;
			init();
		}

		public function set address(addr:int):void {
			this.addr = addr;
		}
		public function get address():int {
			return this.addr;
		}
		
		public function compile(str:String):void {
			var src:Array = str.split("\r").join("").split("\n");
			for(var i:int = 0; i < src.length; i++) {
				parseLine(String(src[i]));
			}
		}
		
		protected function parseLine(str:String):void {
			var s:String = str;
			// strip comments
			var iComment:int = s.indexOf(";");
			if(iComment != -1) {
				s = s.substring(0, iComment - 1);
			}
			s = StringUtils.trim(s);
			if(s.length > 0) {
				var a:Array;
				if(s.charAt(0) == ".") {
					a = parseCommand(s.substr(1));
				} else {
					a = parseInstruction(s.toUpperCase());
				}
				// write results to memory
				for(var i:uint = 0; i < a.length; i++) {
					mem.write(addr++, a[i]);
				}
			}
		}
		
		protected function parseInstruction(s:String):Array {
			var a:Array = StringUtils.condense(s).split(" ");
			switch(a.length) {
				case 1:
					// immediate addressing
					if(opcodeTableImmediate.hasOwnProperty(a[0])) {
						return [opcodeTableImmediate[a[0]]];
					}
					break;
				case 2:
					var opcode:int;
					if(a[1] == "A") {
						// accumulator addressing
						if(opcodeTableAccumulator.hasOwnProperty(a[0])) {
							return [opcodeTableAccumulator[a[0]]];
						}
					} else {
						var isJump:Boolean = (a[0] == "JMP" || a[0] == "JSR");
						var o:Object = parseArgument(a[1], !isJump);
						if(opcodeTable.hasOwnProperty(a[0])) {
							var opcodes:Array = opcodeTable[a[0]];
							opcode = opcodes[o.mode];
							if(opcode >= 0) {
								var ret:Array = [opcode];
								if(o.value < 256 && !isJump) {
									ret.push(o.value);
								} else {
									ret.push(o.value & 0x00ff);
									ret.push((o.value & 0xff00) >> 8);
								}
								return ret;
							}
						} else {
							// relative addressing
							if(opcodeTableRelative.hasOwnProperty(a[0]) && (o.mode == ADDR_MODE_ABSOLUTE || o.mode == ADDR_MODE_ZEROPAGE)) {
								opcode = opcodeTableRelative[a[0]];
								var offset:int = o.value - (addr + 2);
								if(Math.abs(offset) < 128) {
									return [opcode, uint(offset) & 0xff];
								} else {
									// branch offset overflow
									// TODO: automagically create complement branch plus jmp
									throw Error("Branch offset out of range: " + offset);
								}
							}
						}
					}
					break;
			}
			throw Error("Syntax error: '" + s + "'");
		}

		protected function parseArgument(s:String, zeroPageFallback:Boolean = true):Object {
			var len:int = s.length;
			var c:String = s.charAt(0);
			var o:Object = { mode: -1, value: -1 };
			switch(c) {
				case "#":
					// immediate addressing
					s = s.substr(1);
					o.mode = ADDR_MODE_IMMEDIATE; // #ab
					o.value = parseValue(s, 0xff);
					break;
				case "(":
					// indirect adressing
					if(len > 4 && s.indexOf(",X)") == len - 3) {
						s = s.substr(1, len - 4);
						o.mode = ADDR_MODE_INDIRECT_X; // (ab,x)
						o.value = parseValue(s, 0xff);
					} else if(len > 4 && s.indexOf("),Y") == len - 3) {
						s = s.substr(1, len - 4);
						o.mode = ADDR_MODE_INDIRECT_Y; // (ab),y
						o.value = parseValue(s, 0xff);
					} else if(len > 2 && s.indexOf(")") == len - 1) {
						s = s.substr(1, len - 2);
						o.mode = ADDR_MODE_INDIRECT; // ($abcd)
						o.value = parseValue(s);
					} else {
						throw Error("Invalid argument: " + s);
					}
					break;
				default:
					// absolute addressing
					if(len > 2 && s.indexOf(",X") == len - 2) {
						s = s.substr(0, len - 2);
						o.mode = ADDR_MODE_ABSOLUTE_X; // abcd,x
						o.value = parseValue(s);
						if(o.value < 256) {
							o.mode = ADDR_MODE_ZEROPAGE_X; // ab,x
						}
					} else if(len > 2 && s.indexOf(",Y") == len - 2) {
						s = s.substr(0, len - 2);
						o.mode = ADDR_MODE_ABSOLUTE_Y; // abcd,y
						o.value = parseValue(s);
						if(o.value < 256) {
							o.mode = ADDR_MODE_ZEROPAGE_Y; // ab,y
						}
					} else {
						o.mode = ADDR_MODE_ABSOLUTE; // abcd
						o.value = parseValue(s);
						if(zeroPageFallback && o.value < 256) {
							o.mode = ADDR_MODE_ZEROPAGE; // ab
						}
					}
					break;
			}
			return o;
		}

		protected function parseValue(s:String, max:uint = 0xffff):int {
			var val:int;
			var sign:int = 0;
			var c:String = s.charAt(0);
			if(c == "$") {
				s = s.substr(1);
				if(StringUtils.isHexNumber(s)) {
					val = parseInt(s, 16);
				} else {
					throw Error("Hexadecimal value expected: " + s);
				}
			} else if(c == "%") {
				s = s.substr(1);
				if(StringUtils.isBinNumber(s)) {
					val = parseInt(s, 2);
				} else {
					throw Error("Binary value expected: " + s);
				}
			} else {
				if(StringUtils.isNumber(s)) {
					val = parseInt(s);
				} else {
					throw Error("Decimal value expected: " + s);
				}
			}
			if(val < 0 || val > max) {
				throw Error("Value out of range (" + val + ")");
			}
			return val;
		}

		protected function parseCommand(s:String):Array {
			var ret:Array = [];
			var valuesIndex:int = s.indexOf(" ");
			var command:String = s.substr(0, (valuesIndex > 0) ? valuesIndex : 0x7fffffff);
			var values:String = (valuesIndex > 0) ? s.substr(valuesIndex + 1) : "";
			switch(command) {
				case "a":
					addr = parseValue(StringUtils.condense(values));
					break;
				case "b":
					var valueArr:Array = values.split(",");
					for(var j:int = 0; j < valueArr.length; j++) {
						var value:String = String(valueArr[j]);
						if(value.charAt(0) == "\"") {
							for(var k:int = 1; k < value.length - 1; k++) {
								var t:int = value.charCodeAt(k);
								// TODO: PETSCII to ASCII conversion?
								ret.push(t);
							}
						} else {
							ret.push(parseInt(StringUtils.condense(value), 16));
						}
					}
					break;
				case "x":
					var type:int = parseInt(StringUtils.condense(values));
					if(values == "" || isNaN(type)) {
						type = 1;
					}
					cpu.setBreakpoint(addr, type);
					break;
			}
			return ret;
		}

		protected function init():void {
			opcodeTable = {
				//    imm   abs   absx  absy  ind   indx  indy  zp    zpx   zpy
				ADC: [0x69, 0x6d, 0x7d, 0x79, -1,   0x61, 0x71, 0x65, 0x75, -1  ],
				AND: [0x29, 0x2d, 0x3d, 0x39, -1,   0x21, 0x31, 0x25, 0x35, -1  ],
				ASL: [-1,   0x0e, 0x1e, -1,   -1,   -1,   -1,   0x06, 0x16, -1  ],
				BIT: [-1,   0x2c, -1,   -1,   -1,   -1,   -1,   0x24, -1  , -1  ],
				CMP: [0xc9, 0xcd, 0xdd, 0xd9, -1,   0xc1, 0xd1, 0xc5, 0xd5, -1  ],
				CPX: [0xe0, 0xec, -1,   -1,   -1,   -1,   -1,   0xe4, -1  , -1  ],
				CPY: [0xc0, 0xcc, -1,   -1,   -1,   -1,   -1,   0xc4, -1  , -1  ],
				DEC: [-1,   0xce, 0xde, -1,   -1,   -1,   -1,   0xc6, 0xd6, -1  ],
				EOR: [0x49, 0x4d, 0x5d, 0x59, -1,   0x41, 0x51, 0x45, 0x55, -1  ],
				INC: [-1,   0xee, 0xfe, -1,   -1,   -1,   -1,   0xe6, 0xf6, -1  ],
				JMP: [-1,   0x4c, -1,   -1,   0x6c, -1,   -1,   -1,   -1  , -1  ],
				JSR: [-1,   0x20, -1,   -1,   -1,   -1,   -1,   -1,   -1  , -1  ],
				LDA: [0xa9, 0xad, 0xbd, 0xb9, -1,   0xa1, 0xb1, 0xa5, 0xb5, -1  ],
				LDX: [0xa2, 0xae, -1,   0xbe, -1,   -1,   -1,   0xa6, -1  , 0xb6],
				LDY: [0xa0, 0xac, 0xbc, -1,   -1,   -1,   -1,   0xa4, 0xb4, -1  ],
				LSR: [-1,   0x4e, 0x5e, -1,   -1,   -1,   -1,   0x46, 0x56, -1  ],
				ORA: [0x09, 0x0d, 0x1d, 0x19, -1,   0x01, 0x11, 0x05, 0x15, -1  ],
				ROL: [-1,   0x2e, 0x3e, -1,   -1,   -1,   -1,   0x26, 0x36, -1  ],
				ROR: [-1,   0x6e, 0x7e, -1,   -1,   -1,   -1,   0x66, 0x76, -1  ],
				SBC: [0xe9, 0xed, 0xfd, 0xf9, -1,   0xe1, 0xf1, 0xe5, 0xf5, -1  ],
				STA: [-1,   0x8d, 0x9d, 0x99, -1,   0x81, 0x91, 0x85, 0x95, -1  ],
				STX: [-1,   0x8e, -1,   -1,   -1,   -1,   -1,   0x86, -1  , 0x96],
				STY: [-1,   0x8c, -1,   -1,   -1,   -1,   -1,   0x84, 0x94, -1  ]
			};
			opcodeTableRelative = {
				BCC: 0x90, BCS: 0xb0, BVC: 0x50, BVS: 0x70, BEQ: 0xf0, BNE: 0xd0, 
				BMI: 0x30, BPL: 0x10
			};
			opcodeTableImmediate = {
				BRK: 0x00, RTI: 0x40, RTS: 0x60, PHP: 0x08, CLC: 0x18, PLP: 0x28,
				SEC: 0x38, PHA: 0x48, CLI: 0x58, PLA: 0x68, SEI: 0x78, DEY: 0x88,
				TYA: 0x98, TAY: 0xa8, CLV: 0xb8, INY: 0xc8, CLD: 0xd8, INX: 0xe8,
				SED: 0xf8, TXA: 0x8a, TXS: 0x9a, TAX: 0xaa, TSX: 0xba, DEX: 0xca,
				NOP: 0xea
			};
			opcodeTableAccumulator = {
				ASL: 0x0a, ROL: 0x2a, LSR: 0x4a, ROR: 0x6a
			};
		}
	}
}