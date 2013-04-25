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

package core.misc
{
	public class Convert
	{
		public static function toHex(value:uint, len:uint = 2):String {
			var hex:String = value.toString(16);
			if(hex.length < len) {
				var zeros:String = "0000000";
				hex = zeros.substr(0, len - hex.length) + hex;
			}
			return hex.toUpperCase();
		}

		public static function toBin(value:uint, len:uint = 8):String {
			var bin:String = value.toString(2);
			if(bin.length < len) {
				var zeros:String = "000000000000000";
				bin = zeros.substr(0, len - bin.length) + bin;
			}
			return bin;
		}

		public static function toBCD(value:uint):int {
			if(value < 10) {
				return value;
			} else {
				var s:String = value.toString();
				var i:int = s.length - 1;
				return(((s.charCodeAt(i-1) - 48) << 4) | (s.charCodeAt(i) - 48));
			}
		}
	}
}