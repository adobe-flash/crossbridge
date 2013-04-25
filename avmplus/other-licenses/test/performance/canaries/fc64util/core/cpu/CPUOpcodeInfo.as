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
	public class CPUOpcodeInfo
	{
		public var cycles:uint;
		public var len:uint;
		public var handler:Function;
		public var addr:Function;
		public var mnemo:String;

		public function CPUOpcodeInfo(cycles:uint, len:uint, handler:Function, addr:Function, mnemo:String)
		{
			this.cycles = cycles;
			this.len = len;
			this.handler = handler;
			this.addr = addr;
			this.mnemo = mnemo;
		}
	}
}