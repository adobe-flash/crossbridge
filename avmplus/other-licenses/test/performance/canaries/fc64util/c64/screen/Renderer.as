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
 
package c64.screen
{
	import flash.display.*;
	import flash.utils.*;
	import flash.events.*;
	import flash.geom.*;
	import core.memory.*;
	import core.misc.*;
	import core.cpu.*;
	import c64.memory.*;
	import c64.events.*
	import core.exceptions.BreakpointException;
//	import c64.sid.SIDRenderer;
	import c64.memory.io.CIA1;
	import c64.memory.io.VICSpriteInfo;


	[Event(name="frameRateInfoInternal", type="c64.events.FrameRateInfoEvent")]
	[Event(name="rasterInternal", type="c64.events.RasterEvent")]
	[Event(name="stopInternal", type="c64.events.DebuggerEvent")]
	
	public class Renderer extends Sprite
	{
		private var cpu:CPU6502;
		private var mem:MemoryManager;

//		public var sidRenderer:SIDRenderer;
		
		private var displayBack:BitmapData;
		private var displayFore:BitmapData;
		private var displayBorder:BitmapData;
		
		private var colors:Array;

		private var raster:int;
		private var cycles:int;
		private var newRasterLine:Boolean;
		
		private var tmpRowColors:ByteArray;
		private var collisionBackground:ByteArray;
		private var collisionSprite:ByteArray;

		private var frameTimer:Timer;

		private var fpsSum:uint;
		private var fpsCount:uint;

		private var _frameRateInfoEventInterval:int;

		private var _rect:Rectangle = new Rectangle();

		public static const CYCLES_PER_STEP:uint = 63;
		
		public function Renderer(cpu:CPU6502, mem:MemoryManager)
		{
			init(cpu, mem);
		}

		
		public function init(cpu:CPU6502, mem:MemoryManager):void
		{
			this.cpu = cpu;
			this.mem = mem;

    		colors = [
    			0xff000000, 0xffffffff, 0xffe04040, 0xff60ffff, 
    			0xffe060e0, 0xff40e040, 0xff4040e0, 0xffffff40,
				0xffe0a040, 0xff9c7448, 0xffffa0a0, 0xff545454,
				0xff888888, 0xffa0ffa0, 0xffa0a0ff, 0xffc0c0c0
			];

			cycles = 0;
			raster = 0;
			newRasterLine = false;
			
			fpsSum = 0;
			fpsCount = 0;
			frameRateInfoEventInterval = 25;
			
			tmpRowColors = new ByteArray();
			tmpRowColors.length = 40;

			// background bitmap
			displayBack = new BitmapData(403, 284, false, 0x000000);
			addChild(new Bitmap(displayBack));
			// foreground bitmap
			displayFore = new BitmapData(403, 284, true, 0x00000000);
			addChild(new Bitmap(displayFore));
			// border bitmap
			displayBorder = new BitmapData(403, 284, true, 0x00000000);
			addChild(new Bitmap(displayBorder));
			
			// setup frame timer
			frameTimer = new Timer(1, 0);
                        frameTimer.addEventListener(TimerEvent.TIMER, frameLoop);
		}
		
		public function start():void
		{
			if(!frameTimer.running) {
	            frameTimer.start();
			}
		}
		
		public function stop(breakpointType:uint = 0):void
		{
			if(frameTimer.running) {
	            frameTimer.stop();
	            if(breakpointType != 0) {
					dispatchEvent(new DebuggerEvent("stopInternal", breakpointType));
	            }
			}
		}
		
		public function step():void
		{
			if(!frameTimer.running) {
				if(newRasterLine) {
					if(++raster == 312) {
						raster = 0;
					}
					checkRaster();
					newRasterLine = false;
				}
				executeInstruction(false);
				if(cycles >= CYCLES_PER_STEP) {
					cycles -= CYCLES_PER_STEP;
					drawRaster();
					newRasterLine = true;
				}
			}
		}
		
		private function frameLoop(event:TimerEvent):void
		{
			var t:int = getTimer();
			
			displayBack.lock();
			displayFore.lock();
			displayBorder.lock();
			
			dispatchEvent(new RasterEvent("rasterInternal", 0xffff));
			
			while(raster < 312)
			{
				// update raster (check irq condition, fire if met)
				var cyclesRasterIRQ:uint = checkRaster();
				if(cyclesRasterIRQ != 0) {
					dispatchEvent(new RasterEvent("rasterInternal", raster));
				}
				
				try {
					// execute 63 cycles minimum
					while(cycles < CYCLES_PER_STEP) {
						executeInstruction();
					}
				} catch(e:BreakpointException) {
					// CPU has hit a breakpoint:
					// update cycles and CIA timers
					cycles += e.cyclesConsumed;
					updateTimers(e.cyclesConsumed);
					// if needed, reset cycles and draw raster
					if(cycles >= CYCLES_PER_STEP) {
						cycles -= CYCLES_PER_STEP;
						drawRaster();
						raster++;
					}
					// stop timer
					stop(e.type);
					// break out of loop
					break;
				}
				
				// reset cycles
				cycles -= CYCLES_PER_STEP;
				
				// render current raster line
				drawRaster();
				
				// uncomment to enable sid
				// Update sid renderer
				// sidRenderer.getMem();
				
				// uncomment to visualize raster irq
				//if(cyclesRasterIRQ > 0) {
					//displayBorder.fillRect(new Rectangle(0, raster-16, 403, 1), 0xff00ff00);
				//}
				
				raster++;
			}
			
			displayBorder.unlock();
			displayFore.unlock();
			displayBack.unlock();
			
			if(frameTimer.running) {
				fpsSum += (getTimer() - t);
				if(++fpsCount == _frameRateInfoEventInterval) {
					dispatchEvent(new FrameRateInfoEvent("frameRateInfoInternal", fpsSum / _frameRateInfoEventInterval));
					fpsCount = 0;
					fpsSum = 0;
				}
				event.updateAfterEvent();
				raster = 0;
			}
		}

		private function checkRaster():uint
		{
			var cyclesRasterIRQ:uint = 0;
			// update VIC raster
			mem.vic.rasterPosition = raster;
			// check for raster IRQ
			if(mem.vic.rstIRQEnabled && mem.vic.rstIRQLatch) {
				cyclesRasterIRQ = cpu.IRQ();
				cycles += cyclesRasterIRQ + 10; // CW: the '+ 10' is a hack
			}
			return cyclesRasterIRQ;
		}

		private function executeInstruction(checkBreakpoints:Boolean = true):void
		{
			// execute instruction at pc
			// (throws BreakpointException)
			var cyclesConsumed:uint = cpu.exec(checkBreakpoints);
			// update cycles
			cycles += cyclesConsumed;
			// update timers
			updateTimers(cyclesConsumed);
		}

		private function updateTimers(cyclesConsumed:uint):void
		{
			var cia1:CIA1 = mem.cia1;
			// update CIA1 Timer A
			var underflow:Boolean = false;
			if(cia1.taStarted && cia1.updateTimerA(cyclesConsumed)) {
				cycles += cpu.IRQ();
				cia1.resetTimerA();
				underflow = true;
			}
			// update CIA1 Timer B
			if(cia1.tbStarted && cia1.updateTimerB(cyclesConsumed, underflow)) {
				cycles += cpu.IRQ();
				cia1.resetTimerB();
			}
		}

		private function drawRaster():void
		{
			if(raster > 15 && raster < 300)
			{
				var y:uint = raster - 16;
				var yOffs:int = mem.vic.verticalRasterScroll - 3;
				if(raster > 50 + yOffs && raster <= 250 + yOffs) {
					// draw raster line
					if(mem.vic.displayModeValid) {
						if(mem.vic.screenVisible) {
							if(mem.vic.bitmapMode) {
								drawRasterBitmap(y, mem.vic.multiColorMode);
							} else {
								drawRasterText(y, mem.vic.multiColorMode, mem.vic.extBackgroundMode);
							}
							if(mem.vic.spritesEnabled) {
								drawRasterSprites(y);
							}
						} else {
							displayFore.fillRect(rect(48, y, 320, 1), mem.vic.borderColor);
						}
					} else {
						// invalid display mode
						displayFore.fillRect(rect(48, y, 320, 1), 0xff000000);
					}
				}
				// draw border
				var bTop:uint = 50;
				var bBottom:uint = 250;
				if(mem.vic.screenHeight == 24) {
					// top border extends 4px down
					// bottom border extends 4px up
					bTop = 54;
					bBottom = 246;
				}
				if(raster <= bTop || raster > bBottom) {
					// draw vertical border
					displayBorder.fillRect(rect(0, y, 403, 1), mem.vic.borderColor);
				} else {
					// draw horizontal border
					var bLeft:uint = 48;
					var bRight:uint = 35;
					if(mem.vic.screenWidth == 38) {
						// left border extends 7px right
						// right border extends 9px left
						bLeft = 55;
						bRight = 44;
					}
					// left border
					displayBorder.fillRect(rect(0, y, bLeft, 1), mem.vic.borderColor);
					// right border
					displayBorder.fillRect(rect(403 - bRight, y, bRight, 1), mem.vic.borderColor);
					// make middle transparent
					displayBorder.fillRect(rect(bLeft, y, 403 - bLeft - bRight, 1), 0x00000000);
				}
			}
		}
        private function translateChar(ch) {
            var ret;
            if (ch==0)
                ret="";
            else if (ch>=1 && ch<=26)
                ret=String.fromCharCode(64+ch);
            else if (ch==32 || (ch>=48 && ch<=57))
                ret=String.fromCharCode(ch);
            else if (ch==42)
                ret="*";
            else if (ch==46)
                ret=".";
            else if (ch==160)
                ret="_";
            else
                ret="<"+ch+">";
            return ret;
        }
		private function drawRasterText(y:uint, isMultiColor:Boolean, isEnhancedColor:Boolean):void
		{
			var backgroundColors:Array = mem.vic.backgroundColors;
			// the relative y position
			var xOffs:uint = mem.vic.horizontalRasterScroll;
			var yDisp:uint = y - 32 - mem.vic.verticalRasterScroll;
			var yDispScreenOffset:uint = ((yDisp >> 3) * 40);
			// the vic base address (0x0000, 0x4000, 0x8000 or 0xc000)
			var vicBaseAddr:uint = mem.cia2.vicBaseAddr;
			// the character data base address
			var charDataBaseAddr:uint = mem.vic.characterMemoryAddr;
			// here we determine the actual address of the character data area
			// the vic reads from character *rom* at $d000 only if:
			// - character data base address is 0x1000 or 0x1800, and
			// - vic banks 0 or 2 are active (vic base address 0x0000 or 0x8000)
			// otherwise it reads from ram at (vic base address) + (character data base address)
			var getCharDataFromRom:Boolean = ((charDataBaseAddr == 0x1000 || charDataBaseAddr == 0x1800) && (vicBaseAddr & 0x4000) == 0);
			var charDataAddr:uint = charDataBaseAddr + (getCharDataFromRom ? 0xc000 : vicBaseAddr);
			// the screen base address
			var screenAddr:uint = mem.vic.screenMemoryAddr + vicBaseAddr;
			// draw background
			displayBack.fillRect(rect(48, y, 320, 1), backgroundColors[0]);
			displayFore.fillRect(rect(48, y, 320, 1), 0x00000000);
			var x:int;
			var foregroundColor:uint;
			var bm:BitmapData;
			// draw the line
            var screen:String="";
			for(var xDisp:int = 0; xDisp < 316; xDisp += 8) {
				var screenOffset:uint = yDispScreenOffset + (xDisp >> 3);
				var screenCode:uint = mem.read(screenAddr + screenOffset);
//                if (screenCode!=0 && screenCode!=32) print("screenCode: "+translateChar(screenCode));
                screen+=translateChar(screenCode);
				var c:uint = mem.read(0xd800 + screenOffset) & 0x0f;
				var b:uint = mem.readCharacterData(charDataAddr + (screenCode << 3) + (yDisp & 0x07), getCharDataFromRom);
				if(!isMultiColor || (c < 8)) {
					// monochrome text mode
					var colorIndex:int;
					if(b == 0xFF) {
						displayFore.fillRect(rect(xDisp + 48 + xOffs, y, 8, 1), colors[c]);
					} else if(b != 0x00) {
						x = xDisp + 48 + xOffs;
						if(isEnhancedColor && screenCode > 63) {
							colorIndex = (screenCode >> 6);
							bm = (colorIndex > 1) ? displayFore : displayBack;
							bm.fillRect(rect(x, y, 8, 1), backgroundColors[colorIndex]);
						}
						foregroundColor = colors[c];
						if(b & 0x1) { displayFore.setPixel32(x+7, y, foregroundColor); }
						if(b & 0x2) { displayFore.setPixel32(x+6, y, foregroundColor); }
						if(b & 0x4) { displayFore.setPixel32(x+5, y, foregroundColor); }
						if(b & 0x8) { displayFore.setPixel32(x+4, y, foregroundColor); }
						if(b & 0x10) { displayFore.setPixel32(x+3, y, foregroundColor); }
						if(b & 0x20) { displayFore.setPixel32(x+2, y, foregroundColor); }
						if(b & 0x40) { displayFore.setPixel32(x+1, y, foregroundColor); }
						if(b & 0x80) { displayFore.setPixel32(x+0, y, foregroundColor); }
					} else if(isEnhancedColor && screenCode > 63) {
						colorIndex = (screenCode >> 6);
						bm = (colorIndex > 1) ? displayFore : displayBack;
						bm.fillRect(rect(xDisp + 48 + xOffs, y, 8, 1), backgroundColors[colorIndex]);
					}
				} else {
					// multicolor text mode
					if(b == 0) { continue; }
					if(b == 0xFF) {
						displayFore.fillRect(rect(xDisp + 48 + xOffs, y, 8, 1), colors[c & 0x07]);
					} else if(b == 0xAA) {
						displayFore.fillRect(rect(xDisp + 48 + xOffs, y, 8, 1), backgroundColors[2]);
					} else if(b == 0x55) {
						displayBack.fillRect(rect(xDisp + 48 + xOffs, y, 8, 1), backgroundColors[1]);
					} else {
						x = xDisp + 48 + xOffs;
						c &= 0x07;
						var v:uint = b & 0x03;
						if(v != 0) {
							foregroundColor = (v == 3) ? colors[c] : backgroundColors[v];
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+7, y, foregroundColor);
							bm.setPixel32(x+6, y, foregroundColor);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							foregroundColor = (v == 3) ? colors[c] : backgroundColors[v];
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+5, y, foregroundColor);
							bm.setPixel32(x+4, y, foregroundColor);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							foregroundColor = (v == 3) ? colors[c] : backgroundColors[v];
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+3, y, foregroundColor);
							bm.setPixel32(x+2, y, foregroundColor);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							foregroundColor = (v == 3) ? colors[c] : backgroundColors[v];
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+1, y, foregroundColor);
							bm.setPixel32(x, y, foregroundColor);
						}
					}
				}
			}
            while (screen.length>0 && screen.substr(0,1)==' ')
               screen=screen.substr(1);
            while (screen.length>0 && screen.substr(screen.length-1)==' ')
               screen=screen.substr(0,screen.length-1);
            if (screen.length>0)
               Screen.logToScreen(screen);
		}

		private function drawRasterBitmap(y:uint, isMultiColor:Boolean):void
		{
            print("drawRasterBitmap");
			var bm:BitmapData;
			var backgroundColors:Array = mem.vic.backgroundColors;
			// the relative y position
			var yDisp:uint = y - 32 - mem.vic.verticalRasterScroll;
			var yDispScreenOffset:uint = ((yDisp >> 3) * 40);
			// the vic base address (0x0000, 0x4000, 0x8000 or 0xc000)
			var vicBaseAddr:uint = mem.cia2.vicBaseAddr;
			// the bitmap data base address
			var bitmapDataAddr:uint = vicBaseAddr + mem.vic.bitmapMemoryAddr;
			// the screen base address
			var screenAddr:uint = vicBaseAddr + mem.vic.screenMemoryAddr;
			// check bad line condition
			if((yDisp & 0x07) == 0) {
				if(!isMultiColor) {
					// get color data for current row
					tmpRowColors.position = 0;
					mem.copyRam(tmpRowColors, screenAddr + yDispScreenOffset, 40);
					// get background color for first 8x8 cell
					var bgColor:uint = tmpRowColors[0] & 0x0f;
					var startColumn:uint = 0;
					for(var column:uint = 1; column < 40; column++) {
						// check if background color of next 8x8 cell is the same 
						// as the background color of the previous cell(s)
						if(bgColor != (tmpRowColors[column] & 0x0f)) {
							// it's not the same:
							// draw background for previous cell(s)
							// CW: TODO: do we need to distinguish between fore and background here?
							displayBack.fillRect(rect((startColumn << 3) + 48, y, (column - startColumn) << 3, 8), colors[bgColor]);
							// remember current background color and cell index
							bgColor = tmpRowColors[column] & 0x0f;
							startColumn = column;
						}
					}
					// draw background for remaining cell(s) of current row
					// CW: TODO: do we need to distinguish between fore and background here?
					displayBack.fillRect(rect((startColumn << 3) + 48, y, (40 - startColumn) << 3, 8), colors[bgColor]);
				} else {
					displayBack.fillRect(rect(48, y, 320, 8), 0x00000000);
				}
			}
			displayFore.fillRect(rect(48, y, 320, 1), 0x00000000);
			// draw the line
			for(var xDisp:int = 0; xDisp < 316; xDisp += 8) {
				var b:uint = mem.read(bitmapDataAddr + (yDispScreenOffset << 3) + (yDisp & 0x07) + xDisp);
				if(!isMultiColor) {
					// monochrome bitmap mode
					if(b == 0xFF) {
						displayFore.fillRect(rect(xDisp + 48, y, 8, 1), colors[tmpRowColors[xDisp >> 3] >> 4]);
					} else if(b != 0x00) {
						var x:int = xDisp + 48;
						var foregroundColor:uint = colors[tmpRowColors[xDisp >> 3] >> 4];
						if(b & 0x1) { displayFore.setPixel32(x+7, y, foregroundColor); }
						if(b & 0x2) { displayFore.setPixel32(x+6, y, foregroundColor); }
						if(b & 0x4) { displayFore.setPixel32(x+5, y, foregroundColor); }
						if(b & 0x8) { displayFore.setPixel32(x+4, y, foregroundColor); }
						if(b & 0x10) { displayFore.setPixel32(x+3, y, foregroundColor); }
						if(b & 0x20) { displayFore.setPixel32(x+2, y, foregroundColor); }
						if(b & 0x40) { displayFore.setPixel32(x+1, y, foregroundColor); }
						if(b & 0x80) { displayFore.setPixel32(x+0, y, foregroundColor); }
					}
				} else {
					// multicolor bitmap mode
					var screenOffset:uint = yDispScreenOffset + (xDisp >> 3);
					var screenCode:uint = mem.read(screenAddr + screenOffset);
					var foregroundColors:Array = [
						0,
						colors[screenCode >> 4],
						colors[screenCode & 0x0f],
						colors[mem.read(0xd800 + screenOffset) & 0x0f]
					];
					if(b == 0) { continue; }
					if(b == 0xFF) {
						displayFore.fillRect(rect(xDisp + 48/* + xOffs*/, y, 8, 1), foregroundColors[3]);
					} else if(b == 0xAA) {
						displayFore.fillRect(rect(xDisp + 48/* + xOffs*/, y, 8, 1), foregroundColors[2]);
					} else if(b == 0x55) {
						displayBack.fillRect(rect(xDisp + 48/* + xOffs*/, y, 8, 1), foregroundColors[1]);
					} else {
						x = xDisp + 48/* + xOffs*/;
						var v:uint = b & 0x03;
						if(v != 0) {
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+7, y, foregroundColors[v]);
							bm.setPixel32(x+6, y, foregroundColors[v]);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+5, y, foregroundColors[v]);
							bm.setPixel32(x+4, y, foregroundColors[v]);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+3, y, foregroundColors[v]);
							bm.setPixel32(x+2, y, foregroundColors[v]);
						}
						b >>= 2;
						v = b & 0x03;
						if(v != 0) {
							bm = (v == 1) ? displayBack : displayFore;
							bm.setPixel32(x+1, y, foregroundColors[v]);
							bm.setPixel32(x, y, foregroundColors[v]);
						}
					}
				}
			}
		}

		private function drawRasterSprites(y:uint):void
		{
            print("drawRasterSprites");
			//var d:String = "";
			//y -= 16;
			var yDisp:uint = y + 32;
			var spritePointers:uint = mem.vic.screenMemoryAddr + mem.cia2.vicBaseAddr + 0x03f8;
			for(var i:int = 0; i < 8; i++) {
				var sprite:VICSpriteInfo = mem.vic.sprites[i] as VICSpriteInfo;
				// check if sprite is enabled at all
				if(sprite.enabled) {
					// check if sprite is visible in the current raster line
					var h:uint = (sprite.expandVertical) ? 42 : 21;
					var ySprite:uint = sprite.y - 16;
					if(y >= ySprite && y < ySprite + h) {
						var spriteRaster:int = y - ySprite;
						if(sprite.expandVertical) { spriteRaster >>= 1; }
						var addr:uint = mem.cia2.vicBaseAddr + (mem.read(spritePointers + i) << 6) + spriteRaster * 3;
						var v:uint = mem.read(addr) << 16 | mem.read(addr+1) << 8 | mem.read(addr+2);
						// all transparent?
						if(v != 0) {
							var c:uint;
							var xCur:uint;
							var x:uint = sprite.x + 24;
							var y1:uint = y;
							var bm:BitmapData = sprite.foreground ? displayFore : displayBack;
							if(sprite.multicolor) {
								var spriteColors:Array = [
									0, 
									mem.vic.spriteMulticolor0,
									sprite.color,
									mem.vic.spriteMulticolor1
								];
								if(sprite.expandHorizontal) {
									xCur = x + 44;
									while(v != 0) {
										c = v & 3;
										if(c != 0) { bm.fillRect(rect(xCur, y1, 4, 1), spriteColors[c]); }
										v >>= 2;
										xCur -= 4;
									}
								} else {
									xCur = x + 22;
									while(v != 0) {
										c = v & 3;
										if(c != 0) {
											bm.setPixel32(xCur, y1, spriteColors[c]);
											bm.setPixel32(xCur+1, y1, spriteColors[c]);
										}
										v >>= 2;
										xCur -= 2;
									}
								}
							} else {
								if(sprite.expandHorizontal) {
									xCur = x + 46;
									while(v != 0) {
										c = v & 1;
										if(c != 0) {
											bm.setPixel32(xCur, y1, sprite.color);
											bm.setPixel32(xCur+1, y1, sprite.color);
										}
										v >>= 1;
										xCur -= 2;
									}
								} else {
									xCur = x + 23;
									while(v != 0) {
										c = v & 1;
										if(c != 0) { bm.setPixel32(xCur, y1, sprite.color); }
										v >>= 1;
										xCur--;
									}
								}
							}
							//if(i == 0) {
							//	trace(mem.vic.toStringSprite(i));
							//}
							//d += i + " " + spriteRaster + ", ";
						}
					}
				}
			}
			//if(d != "") {
			//	trace(d);
			//}
		}

		public function set frameRateInfoEventInterval(val:int):void {
			if(val < 5) {
				val = 5;
			}
			_frameRateInfoEventInterval = val;
		}

		public function get frameRateInfoEventInterval():int {
			return _frameRateInfoEventInterval;
		}
		
		protected function rect(x:Number, y:Number, width:Number, height:Number):Rectangle {
			_rect.x = x;
			_rect.y = y;
			_rect.width = width;
			_rect.height = height;
			return _rect;
		}
	}
}
