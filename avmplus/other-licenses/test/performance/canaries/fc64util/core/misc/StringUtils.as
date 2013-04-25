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
	public class StringUtils
	{
		public static function trim(input:String):String {
			return StringUtils.ltrim(StringUtils.rtrim(input));
		}

		public static function ltrim(input:String):String {
			var size:Number = input.length;
			for(var i:Number = 0; i < size; i++) {
				if(input.charCodeAt(i) > 32) {
					return input.substring(i);
				}
			}
			return "";
		}

		public static function rtrim(input:String):String {
			var size:Number = input.length;
			for(var i:Number = size; i > 0; i--) {
				if(input.charCodeAt(i - 1) > 32) {
					return input.substring(0, i);
				}
			}
			return "";
		}
		
		public static function condense(input:String):String {
			input = trim(input);
			var output:String = "";
			var len:int = input.length;
			var white:Boolean = false;
			for(var i:int = 0; i < len; i++) {
				var c:int = input.charCodeAt(i);
				if(c <= 32) {
					if(!white) {
						white = true;
						output += String.fromCharCode(c);
					}
				} else {
					white = false;
					output += String.fromCharCode(c);
				}
			}
			return output;
		}
		
		public static function isNumber(s:String):Boolean {
			var len:uint = s.length;
			if(len == 0) { return false; }
			for(var i:uint = 0; i < len; i++) {
				var c:Number = s.charCodeAt(i);
				if(c < 48 || c > 57) {
					return false;
				}
			}
			return true;
		}

		public static function isHexNumber(s:String):Boolean {
			var len:uint = s.length;
			if(len == 0) { return false; }
			s = s.toUpperCase();
			for(var i:uint = 0; i < len; i++) {
				var c:Number = s.charCodeAt(i);
				if(c < 0x30 || c > 0x46 || (c >= 0x3a && c <= 0x40)) {
					return false;
				}
			}
			return true;
		}

		public static function isBinNumber(s:String):Boolean {
			var len:uint = s.length;
			if(len == 0) { return false; }
			for(var i:uint = 0; i < len; i++) {
				var c:Number = s.charCodeAt(i);
				if(c != 0x30 && c != 0x31) {
					return false;
				}
			}
			return true;
		}
	}
}