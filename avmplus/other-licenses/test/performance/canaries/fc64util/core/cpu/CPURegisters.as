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
	import core.misc.Convert;

	public class CPURegisters
	{
		public var a:int;
		public var x:int;
		public var y:int;
		public var p:int;
		public var sp:int;
		public var pc:int;

		public var carryFlag:Boolean;
		public var zeroFlag:Boolean;
		public var irqDisableFlag:Boolean;
		public var decimalModeFlag:Boolean;
		public var breakFlag:Boolean;
		public var overflowFlag:Boolean;
		public var negativeFlag:Boolean;

		public function CPURegisters(a:int, x:int, y:int, p:int, sp:int, pc:int)
		{
			this.a = a;
			this.x = x;
			this.y = y;
			this.p = p;
			this.sp = sp;
			this.pc = pc;
			
			carryFlag = (p & 0x01) != 0;
			zeroFlag = (p & 0x02) != 0;
			irqDisableFlag = (p & 0x04) != 0;
			decimalModeFlag = (p & 0x08) != 0;
			breakFlag = (p & 0x10) != 0;
			overflowFlag = (p & 0x40) != 0;
			negativeFlag = (p & 0x80) != 0;
		}
		
		public function toString():String {
			return toStringRegisters() + "\n" + toStringFlags();
		}

		public function toStringRegisters():String {
			var d:String = "";
			d += "a:" + Convert.toHex(a) + " ";
			d += "x:" + Convert.toHex(x) + " ";
			d += "y:" + Convert.toHex(y) + " ";
			d += "p:" + Convert.toHex(p) + " ";
			d += "sp:" + Convert.toHex(sp) + " ";
			d += "pc:" + Convert.toHex(pc, 4);
			return "[" + d.toUpperCase() + "]";
		}

		public function toStringFlags():String {
			var d:String = "";
			d += "p:" + Convert.toBin(p) + " ";
			d += "n:" + (negativeFlag ? 1 : 0) + " ";
			d += "v:" + (overflowFlag ? 1 : 0) + " ";
			d += "b:" + (breakFlag ? 1 : 0) + " ";
			d += "d:" + (decimalModeFlag ? 1 : 0) + " ";
			d += "i:" + (irqDisableFlag ? 1 : 0) + " ";
			d += "z:" + (zeroFlag ? 1 : 0) + " ";
			d += "c:" + (carryFlag ? 1 : 0);
			return "[" + d.toUpperCase() + "]";
		}
	}
}