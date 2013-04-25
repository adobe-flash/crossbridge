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

	public dynamic class VIC extends IOHandler
	{
		public var colors:Array;
		
		public var characterMemoryAddr:int;
		public var bitmapMemoryAddr:int;
		public var screenMemoryAddr:int;

		public var verticalRasterScroll:int;
		public var horizontalRasterScroll:int;
		public var screenHeight:int;
		public var screenWidth:int;
		public var screenVisible:Boolean;
		public var rasterTrigger:int;

		/**
		 * Display mode
		 * 000: Standard text mode
		 * 001: Multicolor text mode
		 * 010: Standard bitmap mode
		 * 011: Multicolor bitmap mode
		 * 100: ECM text mode
		 * 101: (invalid)
		 * 110: (invalid)
		 * 111: (invalid)
		 */
		public var displayMode:uint;
		public var displayModeValid:Boolean;

		public var bitmapMode:Boolean;
		public var multiColorMode:Boolean;
		public var extBackgroundMode:Boolean;

		public var borderColor:uint;
		public var backgroundColors:Array;

		public var rstIRQEnabled:Boolean;
		public var mbcIRQEnabled:Boolean;
		public var mmcIRQEnabled:Boolean;
		public var lpIRQEnabled:Boolean;

		public var rstIRQLatch:Boolean;
		public var mbcIRQLatch:Boolean;
		public var mmcIRQLatch:Boolean;
		public var lpIRQLatch:Boolean;

		public var sprites:Array;
		public var spritesEnabled:Boolean;
		public var spriteMulticolor0:uint;
		public var spriteMulticolor1:uint;

		private var rasterPositionValue:int;


		public function VIC(debugFlag:Boolean = false) {
			super(debugFlag);
			displayMode = 0;
			displayModeValid = true;
			backgroundColors = [0, 0, 0, 0];
    		colors = [
    			0xff000000, 0xffffffff, 0xffe04040, 0xff60ffff, 
    			0xffe060e0, 0xff40e040, 0xff4040e0, 0xffffff40,
				0xffe0a040, 0xff9c7448, 0xffffa0a0, 0xff545454,
				0xff888888, 0xffa0ffa0, 0xffa0a0ff, 0xffc0c0c0
			];
			sprites = [
				new VICSpriteInfo(colors[1]),
				new VICSpriteInfo(colors[2]),
				new VICSpriteInfo(colors[3]),
				new VICSpriteInfo(colors[4]),
				new VICSpriteInfo(colors[5]),
				new VICSpriteInfo(colors[6]),
				new VICSpriteInfo(colors[7]),
				new VICSpriteInfo(colors[12])
			];
			spritesEnabled = false;
			spriteMulticolor0 = colors[4];
			spriteMulticolor1 = colors[0];
			rasterPositionValue = 0;
		}

		
		public function set rasterPosition(value:int):void {
			rasterPositionValue = value;
			if(rstIRQEnabled && value == rasterTrigger) {
				rstIRQLatch = true;
			}
		}

		// $d000: Sprite #0 x-position
		protected function getSprite0X(index:int):int {
			if(debug) debugMessage("[VIC] get S0X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite0X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[0] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S0X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d001: Sprite #0 y-position
		protected function getSprite0Y(index:int):int {
			if(debug) debugMessage("[VIC] get S0Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite0Y(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S0Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d002: Sprite #1 x-position
		protected function getSprite1X(index:int):int {
			if(debug) debugMessage("[VIC] get S1X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite1X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[1] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S1X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d003: Sprite #1 y-position
		protected function getSprite1Y(index:int):int {
			if(debug) debugMessage("[VIC] get S1Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite1Y(index:int, value:int):void {
			VICSpriteInfo(sprites[1]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S1Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d004: Sprite #2 x-position
		protected function getSprite2X(index:int):int {
			if(debug) debugMessage("[VIC] get S2X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite2X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[2] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S2X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d005: Sprite #2 y-position
		protected function getSprite2Y(index:int):int {
			if(debug) debugMessage("[VIC] get S2Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite2Y(index:int, value:int):void {
			VICSpriteInfo(sprites[2]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S2Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d006: Sprite #3 x-position
		protected function getSprite3X(index:int):int {
			if(debug) debugMessage("[VIC] get S3X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite3X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[3] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S3X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d007: Sprite #3 y-position
		protected function getSprite3Y(index:int):int {
			if(debug) debugMessage("[VIC] get S3Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite3Y(index:int, value:int):void {
			VICSpriteInfo(sprites[3]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S3Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d008: Sprite #4 x-position
		protected function getSprite4X(index:int):int {
			if(debug) debugMessage("[VIC] get S4X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite4X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[4] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S4X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d009: Sprite #4 y-position
		protected function getSprite4Y(index:int):int {
			if(debug) debugMessage("[VIC] get S4Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite4Y(index:int, value:int):void {
			VICSpriteInfo(sprites[4]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S4Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d00a: Sprite #5 x-position
		protected function getSprite5X(index:int):int {
			if(debug) debugMessage("[VIC] get S5X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite5X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[5] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S5X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d00b: Sprite #5 y-position
		protected function getSprite5Y(index:int):int {
			if(debug) debugMessage("[VIC] get S5Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite5Y(index:int, value:int):void {
			VICSpriteInfo(sprites[5]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S5Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d00c: Sprite #6 x-position
		protected function getSprite6X(index:int):int {
			if(debug) debugMessage("[VIC] get S6X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite6X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[6] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S6X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d00d: Sprite #6 y-position
		protected function getSprite6Y(index:int):int {
			if(debug) debugMessage("[VIC] get S6Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite6Y(index:int, value:int):void {
			VICSpriteInfo(sprites[6]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S6Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d00e: Sprite #7 x-position
		protected function getSprite7X(index:int):int {
			if(debug) debugMessage("[VIC] get S7X: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite7X(index:int, value:int):void {
			var sprite:VICSpriteInfo = sprites[7] as VICSpriteInfo;
			sprite.x = (sprite.x & 0x0100) | value;
			if(debug) { debugMessage("[VIC] set S7X: #$" + Convert.toHex(sprite.x, 3)); }
			arr[index] = value;
		}

		// $d00f: Sprite #7 y-position
		protected function getSprite7Y(index:int):int {
			if(debug) debugMessage("[VIC] get S7Y: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite7Y(index:int, value:int):void {
			VICSpriteInfo(sprites[7]).y = arr[index] = value;
			if(debug) { debugMessage("[VIC] set S7Y: #$" + Convert.toHex(arr[index])); }
		}

		// $d010: Sprite 0-7 x-position MSB
		protected function getSpritesXMSB(index:int):int {
			if(debug) debugMessage("[VIC] get SMX: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesXMSB(index:int, value:int):void {
			if(value & 0x01) { VICSpriteInfo(sprites[0]).x |= 0x100; } else { VICSpriteInfo(sprites[0]).x &= 0xff; }
			if(value & 0x02) { VICSpriteInfo(sprites[1]).x |= 0x100; } else { VICSpriteInfo(sprites[1]).x &= 0xff; }
			if(value & 0x04) { VICSpriteInfo(sprites[2]).x |= 0x100; } else { VICSpriteInfo(sprites[2]).x &= 0xff; }
			if(value & 0x08) { VICSpriteInfo(sprites[3]).x |= 0x100; } else { VICSpriteInfo(sprites[3]).x &= 0xff; }
			if(value & 0x10) { VICSpriteInfo(sprites[4]).x |= 0x100; } else { VICSpriteInfo(sprites[4]).x &= 0xff; }
			if(value & 0x20) { VICSpriteInfo(sprites[5]).x |= 0x100; } else { VICSpriteInfo(sprites[5]).x &= 0xff; }
			if(value & 0x40) { VICSpriteInfo(sprites[6]).x |= 0x100; } else { VICSpriteInfo(sprites[6]).x &= 0xff; }
			if(value & 0x80) { VICSpriteInfo(sprites[7]).x |= 0x100; } else { VICSpriteInfo(sprites[7]).x &= 0xff; }
			if(debug) { debugMessage("[VIC] set SMX: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}

		// $d011: Screen Control Register #1
		protected function getScreenControlRegister1(index:int):int {
			if(debug) debugMessage("[VIC] get SC1: #$" + Convert.toHex((arr[index] & 0x7f) | ((rasterPositionValue & 0x100) >> 1)));
			return (arr[index] & 0x7f) | ((rasterPositionValue & 0x100) >> 1);
		}
		protected function setScreenControlRegister1(index:int, value:int):void {
			verticalRasterScroll = value & 0x07;
			screenHeight = ((value & 0x08) != 0) ? 25 : 24;
			screenVisible = (value & 0x10) != 0;
//            print("setScreenControlRegister1: screenVisible="+screenVisible);
			bitmapMode = (value & 0x20) != 0;
			extBackgroundMode = (value & 0x40) != 0;
			displayMode = (displayMode & 0x01) | ((value & 0x60) >> 4);
			displayModeValid = (displayMode <= 4);
			rasterTrigger = (rasterTrigger & 0xff) | ((value & 0x80) << 1)
			if(debug) {
				var d:String = "[VIC] set SC1: #$" + Convert.toHex(value) + " ";
				d += "rastertrigger:" + rasterTrigger + ", ";
				d += "vscroll:" + verticalRasterScroll + ", ";
				d += "scrheight:" + screenHeight + ", ";
				d += "scrvisible:" + screenVisible + ", ";
				d += "bitmapmode:" + bitmapMode + ", ";
				d += "extbgmode:" + extBackgroundMode;
				debugMessage(d);
			}
			arr[index] = value & 0x7f;
		}

		// $d012: Raster (low 8 bits)
		protected function getRaster(index:int):int {
			if(debug) debugMessage("[VIC] get RST: #$" + Convert.toHex(rasterPositionValue & 0xff));
			return rasterPositionValue & 0xff;
		}
		protected function setRaster(index:int, value:int):void {
			rasterTrigger = (rasterTrigger & 0x100) | value;
			if(debug) {
				var d:String = "[VIC] set RST: #$" + Convert.toHex(value) + " ";
				d += "rastertrigger:" + rasterTrigger;
				debugMessage(d);
			}
		}

		// $d013: Lightpen x-position
		// Not supported

		// $d014: Lightpen y-position
		// Not supported

		// $d015: Sprite 0-7 enable flags
		protected function getSpritesEnable(index:int):int {
			if(debug) debugMessage("[VIC] get SEN: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesEnable(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).enabled = ((value & 0x01) != 0);
			VICSpriteInfo(sprites[1]).enabled = ((value & 0x02) != 0);
			VICSpriteInfo(sprites[2]).enabled = ((value & 0x04) != 0);
			VICSpriteInfo(sprites[3]).enabled = ((value & 0x08) != 0);
			VICSpriteInfo(sprites[4]).enabled = ((value & 0x10) != 0);
			VICSpriteInfo(sprites[5]).enabled = ((value & 0x20) != 0);
			VICSpriteInfo(sprites[6]).enabled = ((value & 0x40) != 0);
			VICSpriteInfo(sprites[7]).enabled = ((value & 0x80) != 0);
			spritesEnabled = (value != 0);
			if(debug) { debugMessage("[VIC] set SEN: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}

		// $d016: Screen Control Register #2
		protected function getScreenControlRegister2(index:int):int {
			if(debug) debugMessage("[VIC] get SC2: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setScreenControlRegister2(index:int, value:int):void {
			horizontalRasterScroll = value & 0x07;
			screenWidth = ((value & 0x08) != 0) ? 40 : 38;
			multiColorMode = (value & 0x10) != 0;
			displayMode = (displayMode & 0x06) | (multiColorMode ? 0x01 : 0x00);
			displayModeValid = (displayMode <= 4);
			if(debug) {
				var d:String = "[VIC] set SC2: #$" + Convert.toHex(value) + " ";
				d += "hscroll:" + horizontalRasterScroll + ", ";
				d += "scrwidth:" + screenWidth + ", ";
				d += "multicolormode:" + multiColorMode;
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d017: Sprite 0-7 vertical expansion flags
		protected function getSpritesExpandVertical(index:int):int {
			if(debug) debugMessage("[VIC] get SVX: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesExpandVertical(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).expandVertical = ((value & 0x01) != 0);
			VICSpriteInfo(sprites[1]).expandVertical = ((value & 0x02) != 0);
			VICSpriteInfo(sprites[2]).expandVertical = ((value & 0x04) != 0);
			VICSpriteInfo(sprites[3]).expandVertical = ((value & 0x08) != 0);
			VICSpriteInfo(sprites[4]).expandVertical = ((value & 0x10) != 0);
			VICSpriteInfo(sprites[5]).expandVertical = ((value & 0x20) != 0);
			VICSpriteInfo(sprites[6]).expandVertical = ((value & 0x40) != 0);
			VICSpriteInfo(sprites[7]).expandVertical = ((value & 0x80) != 0);
			if(debug) { debugMessage("[VIC] set SVX: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}

		// $d018: Memory Control Register
		protected function getMemoryControlRegister(index:int):int {
			if(debug) debugMessage("[VIC] get MCR: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setMemoryControlRegister(index:int, value:int):void {
			characterMemoryAddr = (value & 0x0e) << 10;
			bitmapMemoryAddr = (value & 0x08) << 10;
			screenMemoryAddr = (value & 0xf0) << 6;
			if(debug) {
				var d:String = "[VIC] set MCR: #$" + Convert.toHex(value) + " ";
				d += "charmem:$" + Convert.toHex(characterMemoryAddr,4) + ", ";
				d += "bitmapmem:$" + Convert.toHex(bitmapMemoryAddr,4) + ", ";
				d += "screenmem:$" + Convert.toHex(screenMemoryAddr,4) + " ";
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d019: IRQ Status Register
		protected function getIRQStatusRegister(index:int):int {
			var value:int = 0x70;
			if(rstIRQLatch) { value |= 0x01; }
			if(mbcIRQLatch) { value |= 0x02; }
			if(mmcIRQLatch) { value |= 0x04; }
			if(lpIRQLatch) { value |= 0x08; }
			// set bit 7 if at least one irq is enabled and latched. otherwise clear it.
			if(((value & arr[0x0a]) & 0x0f ) != 0) {
				value |= 0x80;
			}
			if(debug) debugMessage("[VIC] get IQS: #$" + Convert.toHex(value));
			return value;
		}
		protected function setIRQStatusRegister(index:int, value:int):void {
			// if bit is set, reset corresponding latch
			if((value & 0x01) != 0) { rstIRQLatch = false; }
			if((value & 0x02) != 0) { mbcIRQLatch = false; }
			if((value & 0x04) != 0) { mmcIRQLatch = false; }
			if((value & 0x08) != 0) { lpIRQLatch = false; }
			if(debug) {
				var d:String = "[VIC] set IQS: #$" + Convert.toHex(value) + " ";
				d += "rst:" + (rstIRQLatch ? 1 : 0) + ", ";
				d += "mbc:" + (mbcIRQLatch ? 1 : 0) + ", ";
				d += "mmc:" + (mmcIRQLatch ? 1 : 0) + ", ";
				d += "lp:" + (lpIRQLatch ? 1 : 0);
				debugMessage(d);
			}
		}

		// $d01a: IRQ Mask Register
		protected function getIRQMaskRegister(index:int):int {
			if(debug) debugMessage("[VIC] get IQM: #$" + Convert.toHex(arr[index] | 0xf0));
			return arr[index] | 0xf0;
		}
		protected function setIRQMaskRegister(index:int, value:int):void {
			// if bit is set, enable corresponding irq
			rstIRQEnabled = ((value & 0x01) != 0);
			mbcIRQEnabled = ((value & 0x02) != 0);
			mmcIRQEnabled = ((value & 0x04) != 0);
			lpIRQEnabled = ((value & 0x08) != 0);
			arr[index] = value | 0xf0;
			if(debug) {
				var d:String = "[VIC] set IQM: #$" + Convert.toHex(value) + " ";
				d += "rst:" + (rstIRQEnabled ? 1 : 0) + ", ";
				d += "mbc:" + (mbcIRQEnabled ? 1 : 0) + ", ";
				d += "mmc:" + (mmcIRQEnabled ? 1 : 0) + ", ";
				d += "lp:" + (lpIRQEnabled ? 1 : 0);
				debugMessage(d);
			}
		}

		// $d01b: Sprite 0-7 foreground flags
		protected function getSpritesForeground(index:int):int {
			if(debug) debugMessage("[VIC] get SFG: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesForeground(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).foreground = ((value & 0x01) == 0);
			VICSpriteInfo(sprites[1]).foreground = ((value & 0x02) == 0);
			VICSpriteInfo(sprites[2]).foreground = ((value & 0x04) == 0);
			VICSpriteInfo(sprites[3]).foreground = ((value & 0x08) == 0);
			VICSpriteInfo(sprites[4]).foreground = ((value & 0x10) == 0);
			VICSpriteInfo(sprites[5]).foreground = ((value & 0x20) == 0);
			VICSpriteInfo(sprites[6]).foreground = ((value & 0x40) == 0);
			VICSpriteInfo(sprites[7]).foreground = ((value & 0x80) == 0);
			if(debug) { debugMessage("[VIC] set SFG: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}

		// $d01c: Sprite 0-7 multicolor mode flags
		protected function getSpritesMulticolor(index:int):int {
			if(debug) debugMessage("[VIC] get SMC: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesMulticolor(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).multicolor = ((value & 0x01) != 0);
			VICSpriteInfo(sprites[1]).multicolor = ((value & 0x02) != 0);
			VICSpriteInfo(sprites[2]).multicolor = ((value & 0x04) != 0);
			VICSpriteInfo(sprites[3]).multicolor = ((value & 0x08) != 0);
			VICSpriteInfo(sprites[4]).multicolor = ((value & 0x10) != 0);
			VICSpriteInfo(sprites[5]).multicolor = ((value & 0x20) != 0);
			VICSpriteInfo(sprites[6]).multicolor = ((value & 0x40) != 0);
			VICSpriteInfo(sprites[7]).multicolor = ((value & 0x80) != 0);
			if(debug) { debugMessage("[VIC] set SMC: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}

		// $d01d: Sprite 0-7 horizontal expansion flags
		protected function getSpritesExpandHorizontal(index:int):int {
			if(debug) debugMessage("[VIC] get SHX: #%" + Convert.toBin(arr[index]));
			return arr[index];
		}
		protected function setSpritesExpandHorizontal(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).expandHorizontal = ((value & 0x01) != 0);
			VICSpriteInfo(sprites[1]).expandHorizontal = ((value & 0x02) != 0);
			VICSpriteInfo(sprites[2]).expandHorizontal = ((value & 0x04) != 0);
			VICSpriteInfo(sprites[3]).expandHorizontal = ((value & 0x08) != 0);
			VICSpriteInfo(sprites[4]).expandHorizontal = ((value & 0x10) != 0);
			VICSpriteInfo(sprites[5]).expandHorizontal = ((value & 0x20) != 0);
			VICSpriteInfo(sprites[6]).expandHorizontal = ((value & 0x40) != 0);
			VICSpriteInfo(sprites[7]).expandHorizontal = ((value & 0x80) != 0);
			if(debug) { debugMessage("[VIC] set SHX: #%" + Convert.toBin(value)); }
			arr[index] = value;
		}
		
		// $d01e: Sprite to Sprite collision flags
		// TODO

		// $d01f: Sprite to Foreground collision flags
		// TODO

		// $d020: Border Color
		protected function getBorderColor(index:int):int {
			if(debug) debugMessage("[VIC] get BOC: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setBorderColor(index:int, value:int):void {
			borderColor = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set BOC: #$" + Convert.toHex(value) + " ";
				d += "bordercol:" + Convert.toHex(borderColor, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d021: Background Color
		protected function getBackgroundColor(index:int):int {
			if(debug) debugMessage("[VIC] get BG0: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setBackgroundColor(index:int, value:int):void {
			backgroundColors[0] = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set BG0: #$" + Convert.toHex(value) + " ";
				d += "backgroundcol0:" + Convert.toHex(backgroundColors[0], 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d022: Extended Background Color #1
		protected function getBackgroundColor1(index:int):int {
			if(debug) debugMessage("[VIC] get BG1: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setBackgroundColor1(index:int, value:int):void {
			backgroundColors[1] = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set BG1: #$" + Convert.toHex(value) + " ";
				d += "backgroundcol1:" + Convert.toHex(backgroundColors[1], 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d023: Extended Background Color #2
		protected function getBackgroundColor2(index:int):int {
			if(debug) debugMessage("[VIC] get BG2: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setBackgroundColor2(index:int, value:int):void {
			backgroundColors[2] = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set BG2: #$" + Convert.toHex(value) + " ";
				d += "backgroundcol2:" + Convert.toHex(backgroundColors[2], 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d024: Extended Background Color #3
		protected function getBackgroundColor3(index:int):int {
			if(debug) debugMessage("[VIC] get BG3: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setBackgroundColor3(index:int, value:int):void {
			backgroundColors[3] = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set BG3: #$" + Convert.toHex(value) + " ";
				d += "backgroundcol3:" + Convert.toHex(backgroundColors[3], 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d025: Sprite Multicolor #0
		protected function getSpriteMulticolor0(index:int):int {
			if(debug) debugMessage("[VIC] get SM0: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSpriteMulticolor0(index:int, value:int):void {
			spriteMulticolor0 = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set SM0: #$" + Convert.toHex(value) + " ";
				d += "spriteMulticolor0:" + Convert.toHex(spriteMulticolor0, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d026: Sprite Multicolor #1
		protected function getSpriteMulticolor1(index:int):int {
			if(debug) debugMessage("[VIC] get SM1: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSpriteMulticolor1(index:int, value:int):void {
			spriteMulticolor1 = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set SM1: #$" + Convert.toHex(value) + " ";
				d += "spriteMulticolor0:" + Convert.toHex(spriteMulticolor1, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d027: Sprite #0 color
		protected function getSprite0Color(index:int):int {
			if(debug) debugMessage("[VIC] get S0C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite0Color(index:int, value:int):void {
			VICSpriteInfo(sprites[0]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S0C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[0]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d028: Sprite #1 color
		protected function getSprite1Color(index:int):int {
			if(debug) debugMessage("[VIC] get S1C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite1Color(index:int, value:int):void {
			VICSpriteInfo(sprites[1]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S1C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[1]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d029: Sprite #2 color
		protected function getSprite2Color(index:int):int {
			if(debug) debugMessage("[VIC] get S2C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite2Color(index:int, value:int):void {
			VICSpriteInfo(sprites[2]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S2C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[2]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d02a: Sprite #3 color
		protected function getSprite3Color(index:int):int {
			if(debug) debugMessage("[VIC] get S3C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite3Color(index:int, value:int):void {
			VICSpriteInfo(sprites[3]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S3C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[3]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d02b: Sprite #4 color
		protected function getSprite4Color(index:int):int {
			if(debug) debugMessage("[VIC] get S4C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite4Color(index:int, value:int):void {
			VICSpriteInfo(sprites[4]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S4C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[4]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d02c: Sprite #5 color
		protected function getSprite5Color(index:int):int {
			if(debug) debugMessage("[VIC] get S5C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite5Color(index:int, value:int):void {
			VICSpriteInfo(sprites[5]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S5C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[5]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d02d: Sprite #6 color
		protected function getSprite6Color(index:int):int {
			if(debug) debugMessage("[VIC] get S6C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite6Color(index:int, value:int):void {
			VICSpriteInfo(sprites[6]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S6C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[6]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		// $d02e: Sprite #7 color
		protected function getSprite7Color(index:int):int {
			if(debug) debugMessage("[VIC] get S7C: #$" + Convert.toHex(arr[index]));
			return arr[index];
		}
		protected function setSprite7Color(index:int, value:int):void {
			VICSpriteInfo(sprites[7]).color = colors[value & 0x0f];
			if(debug) {
				var d:String = "[VIC] set S7C: #$" + Convert.toHex(value) + " ";
				d += "color:" + Convert.toHex(VICSpriteInfo(sprites[7]).color, 8);
				debugMessage(d);
			}
			arr[index] = value;
		}

		override protected function initHandlers():void {
			handlers = [
				// the VIC chip has 64 registers:
				new IOHandlerInfo(getSprite0X, setSprite0X), // 00
				new IOHandlerInfo(getSprite0Y, setSprite0Y), // 01
				new IOHandlerInfo(getSprite1X, setSprite1X), // 02
				new IOHandlerInfo(getSprite1Y, setSprite1Y), // 03
				new IOHandlerInfo(getSprite2X, setSprite2X), // 04
				new IOHandlerInfo(getSprite2Y, setSprite2Y), // 05
				new IOHandlerInfo(getSprite3X, setSprite3X), // 06
				new IOHandlerInfo(getSprite3Y, setSprite3Y), // 07
				new IOHandlerInfo(getSprite4X, setSprite4X), // 08
				new IOHandlerInfo(getSprite4Y, setSprite4Y), // 09
				new IOHandlerInfo(getSprite5X, setSprite5X), // 0a
				new IOHandlerInfo(getSprite5Y, setSprite5Y), // 0b
				new IOHandlerInfo(getSprite6X, setSprite6X), // 0c
				new IOHandlerInfo(getSprite6Y, setSprite6Y), // 0d
				new IOHandlerInfo(getSprite7X, setSprite7X), // 0e
				new IOHandlerInfo(getSprite7Y, setSprite7Y), // 0f
				new IOHandlerInfo(getSpritesXMSB, setSpritesXMSB), // 10
				new IOHandlerInfo(getScreenControlRegister1, setScreenControlRegister1), // 11
				new IOHandlerInfo(getRaster, setRaster), // 12
				new IOHandlerInfo(getDefault, setDefault), // 13
				new IOHandlerInfo(getDefault, setDefault), // 14
				new IOHandlerInfo(getSpritesEnable, setSpritesEnable), // 15
				new IOHandlerInfo(getScreenControlRegister2, setScreenControlRegister2), // 16
				new IOHandlerInfo(getSpritesExpandVertical, setSpritesExpandVertical), // 17
				new IOHandlerInfo(getMemoryControlRegister, setMemoryControlRegister), // 18
				new IOHandlerInfo(getIRQStatusRegister, setIRQStatusRegister), // 19
				new IOHandlerInfo(getIRQMaskRegister, setIRQMaskRegister), // 1a
				new IOHandlerInfo(getSpritesForeground, setSpritesForeground), // 1b
				new IOHandlerInfo(getSpritesMulticolor, setSpritesMulticolor), // 1c
				new IOHandlerInfo(getSpritesExpandHorizontal, setSpritesExpandHorizontal), // 1d
				new IOHandlerInfo(getDefault, setDefault), // 1e
				new IOHandlerInfo(getDefault, setDefault), // 1f
				new IOHandlerInfo(getBorderColor, setBorderColor), // 20
				new IOHandlerInfo(getBackgroundColor, setBackgroundColor), // 21
				new IOHandlerInfo(getBackgroundColor1, setBackgroundColor1), // 22
				new IOHandlerInfo(getBackgroundColor2, setBackgroundColor2), // 23
				new IOHandlerInfo(getBackgroundColor3, setBackgroundColor3), // 24
				new IOHandlerInfo(getSpriteMulticolor0, setSpriteMulticolor0), // 25
				new IOHandlerInfo(getSpriteMulticolor1, setSpriteMulticolor1), // 26
				new IOHandlerInfo(getSprite0Color, setSprite0Color), // 27
				new IOHandlerInfo(getSprite1Color, setSprite1Color), // 28
				new IOHandlerInfo(getSprite2Color, setSprite2Color), // 29
				new IOHandlerInfo(getSprite3Color, setSprite3Color), // 2a
				new IOHandlerInfo(getSprite4Color, setSprite4Color), // 2b
				new IOHandlerInfo(getSprite5Color, setSprite5Color), // 2c
				new IOHandlerInfo(getSprite6Color, setSprite6Color), // 2d
				new IOHandlerInfo(getSprite7Color, setSprite7Color), // 2e
				new IOHandlerInfo(getDefault, setDefault), // 2f
				new IOHandlerInfo(getDefault, setDefault), // 30
				new IOHandlerInfo(getDefault, setDefault), // 31
				new IOHandlerInfo(getDefault, setDefault), // 32
				new IOHandlerInfo(getDefault, setDefault), // 33
				new IOHandlerInfo(getDefault, setDefault), // 34
				new IOHandlerInfo(getDefault, setDefault), // 35
				new IOHandlerInfo(getDefault, setDefault), // 36
				new IOHandlerInfo(getDefault, setDefault), // 37
				new IOHandlerInfo(getDefault, setDefault), // 38
				new IOHandlerInfo(getDefault, setDefault), // 39
				new IOHandlerInfo(getDefault, setDefault), // 3a
				new IOHandlerInfo(getDefault, setDefault), // 3b
				new IOHandlerInfo(getDefault, setDefault), // 3c
				new IOHandlerInfo(getDefault, setDefault), // 3d
				new IOHandlerInfo(getDefault, setDefault), // 3e
				new IOHandlerInfo(getDefault, setDefault)  // 3f
			];
		}
		
		public function toString():String {
			var d:String = "";
			d += "Display mode: ";
			switch(displayMode) {
				case 0: d += "Standard Text"; break;
				case 1: d += "Multicolor Text"; break;
				case 2: d += "Standard Bitmap"; break;
				case 3: d += "Multicolor Bitmap"; break;
				case 4: d += "ECM Text"; break;
				default: d += "invalid"; break;
			}
			d += " (%" + Convert.toBin(displayMode, 3) + ")\n";
			d += "Screen memory: $" + Convert.toHex(screenMemoryAddr, 4) + "\n";
			d += "Bitmap memory: $" + Convert.toHex(bitmapMemoryAddr, 4) + "\n";
			d += "Characterdata memory: $" + Convert.toHex(characterMemoryAddr, 4) + "\n";
			d += "Screen: " + (screenVisible ? "visible": "invisible") + " (" + screenWidth + "/" + screenHeight + ")\n";
			for(var i:uint = 0; i < 8; i++) {
				d += toStringSprite(i);
			}
			return d;
		}
		
		public function toStringSprite(index:uint, displayDisabled:Boolean = true):String {
			var d:String = "";
			var sprite:VICSpriteInfo = sprites[index] as VICSpriteInfo;
			d += "Sprite " + index + ": " + (sprite.enabled ? "enabled" : "disabled") + ", ";
			d += sprite.x + "/" + sprite.y + " (" + (sprite.foreground ? "foreground" : "background") + ", ";
			d += (sprite.multicolor ? "multicolor" : "monochrome") + ", ";
			d += (sprite.expandHorizontal ? "200%" : "100%") + "/";
			d += (sprite.expandVertical ? "200%" : "100%") + ")\n";
			return d;
		}
	}
}
